#include <stdio.h>
#include <string.h>
#include "../cards.h"
#include "../game_support.h"
#include "../global_strings.h"
#include "../network.h"

static __inline int artifact_give_pump_until_eot(int player, int card, int target_player, int target_card, int power, int toughness)
{
  int legacy_card;

  legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, target_player, target_card);
  if (legacy_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = power;
    PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = toughness;
  }

  return legacy_card;
}

int mana_battery(int player, int card, event_t event, color_t color);
int count_tetravites_attached_to_tetravus(int player, int card);
int tetravus_remove_counter_to_make_tetravite(int player, int card);
int tetravus_create_tetravite(int player);
int tetravus_reabsorb_tetravite(int player, int card);
int resolve_pandoras_box(int player, int card);
int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off);
int AddCardToDeckSorted(int card_id);
int GetCardRarity(int card_id);
int choose_best_revealed_card_index(int player, int controller, unsigned int required_type, int *card_ids);
int select_revealed_card_index_for_aladdins_lamp(int player, int *card_ids, int card_count, char *title, int require_card_click, int *out_selection);
int helper_lucky_charm(int player, int card, int event, int color);
int helper_mox(int player, int card, event_t event, int color);
void remove_card_from_shandalar_deck_by_id(unsigned int card_id);

static __inline void put_top_library_card_into_graveyard(int target_player)
{
  int graveyard_index;
  int internal_card_id;

  if (global_library[target_player][0] == -1)
  {
    return;
  }

  internal_card_id = global_library[target_player][0];
  graveyard_index = 0;
  while (graveyard_index < 500 && global_graveyard_slots[target_player][graveyard_index] != -1)
  {
    ++graveyard_index;
  }

  if (graveyard_index < 500)
  {
    global_graveyard_slots[target_player][graveyard_index] = internal_card_id;
  }
  remove_card_from_deck(target_player, 0);
}

static __inline int artifact_pay_to_untap_during_upkeep(int player, int card, event_t event, int cost)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->untap_status &= ~3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return g_current_phase == PHASE_UPKEEP &&
           g_current_player == player &&
           g_current_turn == player &&
           (instance->state & STATE_TAPPED) != 0 &&
           has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, cost);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, cost);
    if (g_spell_fizzled != 1)
    {
      instance->info_slot = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card);
    if (instance->internal_card_id != -1 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      instance->state &= ~STATE_TAPPED;
      instance->info_slot = 0;
    }
  }

  return 0;
}

static __inline unsigned int get_fellwar_stone_colors(int player)
{
  unsigned int colors;
  int opponent;
  int current_card;

  colors = 0;
  opponent = 1 - player;
  for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
  {
    if (is_in_play(opponent, current_card) &&
        (global_cards_data[PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id].type & TYPE_LAND) != 0)
    {
      colors |= (unsigned char)PLAYER_CARD_INSTANCE(opponent, current_card).mana_color;
    }
  }

  return colors & 0x3e;
}

static __inline int artifact_tapped_pump(int player,
                                         int card,
                                         event_t event,
                                         int power,
                                         int toughness,
                                         unsigned int allowed_controller,
                                         unsigned int preferred_controller)
{
  card_instance_t *instance;
  card_instance_t *parent;
  card_instance_t *legacy;
  target_t target;
  int legacy_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->info_slot = -1;
    instance->damage_source_player = -1;
  }

  if (event == EVENT_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->untap_status &= ~UNTAP_STATUS_WILL_UNTAP;
  }

  if (g_current_phase == PHASE_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER &&
        (instance->untap_status & UNTAP_STATUS_COULD_UNTAP) != 0 &&
        (instance->untap_status & UNTAP_STATUS_WILL_UNTAP) == 0 &&
        (unk_00925d38 & global_cards_data[instance->internal_card_id].type) == 0)
    {
      if ((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
          g_duel_ai_mode_state == 1 ||
          g_duel_network_state != 0)
      {
        if (instance->info_slot == -1 ||
            ((PLAYER_CARD_INSTANCE(player, instance->info_slot).untap_status & UNTAP_STATUS_COULD_UNTAP) == 0 &&
             (PLAYER_CARD_INSTANCE(player, instance->info_slot).state & STATE_TAPPED) != 0))
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->untap_status |= UNTAP_STATUS_WILL_UNTAP;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2) &&
           !is_animated_and_sick(player, card) &&
           (instance->state & STATE_TAPPED) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 allowed_controller,
                                 preferred_controller,
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

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      if (!C_real_select_target(player,
                                allowed_controller,
                                preferred_controller,
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
                                "Select target creature.",
                                1,
                                &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = target;
        instance->number_of_targets = 1;
        instance->info_slot = 0;
        instance->state |= STATE_TAPPED;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    if (parent->internal_card_id != -1)
    {
      target = instance->targets[0];
      parent->number_of_targets = 0;
      if (!C_real_validate_target(target.player,
                                  target.card,
                                  (char *)0,
                                  player,
                                  allowed_controller,
                                  preferred_controller,
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
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->token_status |= STATUS_PERMANENT;
          legacy->counter_power = (short)power;
          legacy->counter_toughness = (short)toughness;
          parent->damage_source_player = (char)player;
          parent->info_slot = legacy_card;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (instance->info_slot != -1 &&
        PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->info_slot).damage_target_player ==
            g_affected_card_controller &&
        PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->info_slot).damage_target_card ==
            g_affected_card)
    {
      instance->info_slot = -1;
      instance->damage_source_player = -1;
    }

    if (card == g_affected_card &&
        player == g_affected_card_controller &&
        instance->info_slot != -1)
    {
      kill_card((int)instance->damage_source_player, instance->info_slot, KILL_BURY);
      instance->info_slot = -1;
      instance->damage_source_player = -1;
    }
  }

  if (instance->info_slot != -1 && (instance->state & STATE_TAPPED) == 0)
  {
    kill_card((int)instance->damage_source_player, instance->info_slot, KILL_BURY);
    instance->info_slot = -1;
  }

  if (event == EVENT_CHECK_PUMP &&
      (instance->state & (STATE_TAPPED | STATE_SUMMONSICK_NOATTACK)) == 0 &&
      has_mana(player, COLOR_COLORLESS, 2))
  {
    ++unk_007a7d80[player];
    ++unk_007a7d88[player];
  }

  return 0;
}

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
    g_ai_modifier -= 0x24;
    s.available_colors = PLAYER_CARD_INSTANCE(player, card).mana_color;
    if ((((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1)) || (g_duel_network_state != 0))
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
        g_spell_fizzled = 1;
      }
    }
    else
    {
      s.choice_hint = ~COLOR_COLORLESS;
    }

    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "BLACK_LOTUS");
      }
      s.choice_hint = choose_a_color(player, g_text_lines[0], 1, s.choice_hint, PLAYER_CARD_INSTANCE(player, card).mana_color);
      if (s.choice_hint == ~COLOR_COLORLESS)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, s.choice_hint, 3);
        g_produced_mana_color = s.choice_hint;
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
        if (((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
        {
          char message[300];
          load_text("prompts.txt", "BLACK_LOTUS");
          if (s.choice_hint == COLOR_BLACK)
          {
            strcpy(message, g_text_lines[1]);
          }
          else if (s.choice_hint == COLOR_BLUE)
          {
            strcpy(message, g_text_lines[2]);
          }
          else if (s.choice_hint == COLOR_GREEN)
          {
            strcpy(message, g_text_lines[3]);
          }
          else if (s.choice_hint == COLOR_RED)
          {
            strcpy(message, g_text_lines[4]);
          }
          else
          {
            strcpy(message, g_text_lines[5]);
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

  if ((event == EVENT_COUNT_MANA) && (card == g_affected_card) && (player == g_affected_card_controller) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  if (event == 0x6a && current_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 && C_get_special_counters(player, card) == 0)
  {
    load_text("prompts.txt", "TIME_VAULT");
    sprintf(prompt, " %s\n %s", g_text_lines, g_text_lines + 0x12c);
    if (do_dialog(player, player, card, -1, -1, prompt, internal_rand(5) < 1) != 0)
    {
      g_land_can_be_played |= 0x8000;
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
      g_ai_modifier += 0x30;
    }

    if (g_duel_extra_turn_player == -1)
    {
      effect_found = 0;
      for (current_player = 0; current_player < 2 && !effect_found; ++current_player)
      {
        for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
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

    remove_special_counter(g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 1;
    effect_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, unk_008b49c4, -1, -1);
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
        s.damaged_player = g_other_player;
      }
      else
      {
        s.damaged_player = g_active_player;
      }

      s.damage = g_duel_summary.hand_counts[s.damaged_player] - 4;
      if (s.damage < 1)
      {
        s.damage = 0;
      }

      if (s.damage != 0)
      {
        s.tmp = g_life[s.damaged_player] / s.damage;
        s.damage = 0x18 - s.tmp;
        if (s.damage < 2)
        {
          s.damage = 1;
        }
        if (s.damaged_player == 0)
        {
          g_ai_modifier += (s.damage * 3) << 3;
        }
        else
        {
          g_ai_modifier -= (s.damage * 3) << 3;
        }
      }
    }
  }

  if (g_trigger_condition == TRIGGER_END_UPKEEP && card == g_affected_card && player == g_affected_card_controller &&
      player == g_current_turn)
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
      s.damaged_player = g_other_player;
    }
    else
    {
      s.damaged_player = g_active_player;
    }

    if (g_current_player == s.damaged_player && g_duel_summary.hand_counts[s.damaged_player] > 4)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        damage_player(s.damaged_player, g_duel_summary.hand_counts[s.damaged_player] - 4, player, card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040dd78
// FUNCTION: SHANDALAR 0x00510594
int card_the_rack(int player, int card, event_t event)
{
  int damaged_player;
  int damage;
  int internal_card_id;

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (is_in_play(player, card))
    {
      if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) == 0)
      {
        damaged_player = g_other_player;
      }
      else
      {
        damaged_player = g_active_player;
      }

      damage = 3 - g_duel_summary.hand_counts[damaged_player];
      if (damage < 1)
      {
        damage = 0;
      }
      if (damage != 0)
      {
        damage = 0x18 - g_life[damaged_player] / damage;
        if (damage < 2)
        {
          damage = 1;
        }
        if (damaged_player == 0)
        {
          g_ai_modifier += (damage * 3) << 3;
        }
        else
        {
          g_ai_modifier -= (damage * 3) << 3;
        }
      }
    }
  }

  if (g_trigger_condition == TRIGGER_END_UPKEEP &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      player == g_current_turn)
  {
    if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) != 0)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0)
      {
        return 0;
      }
    }

    if ((((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).state)[1] & 0x10) == 0)
    {
      damaged_player = g_other_player;
    }
    else
    {
      damaged_player = g_active_player;
    }

    damage = 3 - g_duel_summary.hand_counts[damaged_player];
    if (g_current_player == damaged_player && damage > 0)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        damage_player(damaged_player, damage, player, card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040dfe6
// FUNCTION: SHANDALAR 0x00510802
int card_ivory_tower(int player, int card, event_t event)
{
  int life_gain;

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (is_in_play(player, card) && g_duel_summary.hand_counts[player] > 4)
    {
      life_gain = g_duel_summary.hand_counts[player] - 4;
      if (player == 0)
      {
        g_ai_modifier += life_gain * 0x18;
      }
      else
      {
        g_ai_modifier -= life_gain * 0x18;
      }
    }
  }

  if (g_trigger_condition == TRIGGER_END_UPKEEP &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      player == g_current_turn &&
      g_current_player == player &&
      g_duel_summary.hand_counts[player] > 4)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      gain_life(player, g_duel_summary.hand_counts[player] - 4, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040e1a7
// FUNCTION: SHANDALAR 0x005109c7
int card_cursed_rack(int player, int card, event_t event)
{
  int affected_player;

  if (event == EVENT_MAX_HAND_SIZE &&
      ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 ||
       (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OWNED_BY_OPPONENT) == 0)
    {
      affected_player = g_other_player;
    }
    else
    {
      affected_player = g_active_player;
    }

    if (affected_player == g_current_player && g_duel_summary.hand_counts[g_current_player] > 4)
    {
      g_event_result |= 1;
      while (g_duel_summary.hand_counts[g_current_player] > 4)
      {
        discard(g_current_player, 0, 0);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040e2db
// FUNCTION: SHANDALAR 0x00510afa
int card_aladdin_s_lamp(int player, int card, event_t event)
{
  card_instance_t *instance;
  int revealed[150];
  int paid_mana;
  int reveal_index;
  int revealed_count;
  int chosen_index;
  int saved_x_value;
  int saved_max_x_value;
  int playable_count;
  int untapped_playable_count;
  int tries;
  int done;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (g_trigger_condition == TRIGGER_REPLACE_CARD_DRAW &&
      (instance->state & STATE_TAPPED) == 0 &&
      has_mana(player, COLOR_ANY, 1) &&
      (instance->state & STATE_INVISIBLE) == 0 &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        if (instance->info_slot != 0 && (paid_mana = has_mana(player, COLOR_ANY, 3)) != 0)
        {
          g_event_result |= 2;
          if (instance->eot_toughness == 0)
          {
            instance->eot_toughness = internal_rand(paid_mana - 2) + 2;
            g_loaded_recorded_action_code = instance->eot_toughness;
          }
        }
      }
      else
      {
        g_event_result |= 1;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->info_slot = 0;
      saved_x_value = g_x_value;
      saved_max_x_value = g_max_x_value;
      g_max_x_value = -1;

      if ((player == g_active_player || (g_duel_network_flags & 2) != 0) &&
          g_duel_ai_mode_state != 1 &&
          g_duel_network_state == 0)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, COLOR_COLORLESS, -1);
        obliterate_top_card_of_stack();
        paid_mana = g_x_value;
      }
      else
      {
        paid_mana = charge_mana(player, COLOR_COLORLESS, instance->eot_toughness);
        if (paid_mana == 0)
        {
          g_spell_fizzled = 1;
        }
        instance->eot_toughness = 0;
      }

      g_max_x_value = saved_max_x_value;
      g_x_value = saved_x_value;

      if (g_spell_fizzled == 1 || paid_mana < 1)
      {
        g_spell_fizzled = -1;
      }
      else
      {
        instance->state |= STATE_TAPPED;
        revealed_count = 0;
        for (reveal_index = 0; reveal_index < paid_mana; ++reveal_index)
        {
          if (global_library[player][reveal_index] != -1)
          {
            revealed[revealed_count] = global_library[player][reveal_index];
            ++revealed_count;
          }
        }
        revealed[revealed_count] = -1;

        if ((player == g_active_player || (g_duel_network_flags & 2) != 0) &&
            g_duel_ai_mode_state != 1 &&
            g_duel_network_state == 0)
        {
          if (player == g_active_player)
          {
            load_text("prompts.txt", "ALADDINS_LAMP");
          }
          chosen_index = select_revealed_card_index_for_aladdins_lamp(player, revealed, revealed_count, g_text_lines[0], 1, (int *)gs_done_008b40e0);
        }
        else
        {
          chosen_index = choose_best_revealed_card_index(player, player, TYPE_CREATURE, revealed);
          if (chosen_index == -1)
          {
            chosen_index = 0;
          }
        }

        if (revealed_count != 0)
        {
          add_card_to_hand(player, revealed[chosen_index]);
          for (reveal_index = 0; reveal_index < revealed_count; ++reveal_index)
          {
            remove_card_from_deck(player, 0);
          }
          revealed[chosen_index] = -1;

          done = 0;
          while (done == 0)
          {
            if ((g_duel_network_flags & 2) == 0 || player != g_other_player)
            {
              tries = 0;
              do
              {
                chosen_index = internal_rand(paid_mana);
                if (revealed[chosen_index] != -1)
                {
                  break;
                }
                ++tries;
              } while (tries < 1000);

              if (revealed[chosen_index] == -1)
              {
                for (reveal_index = 0; reveal_index < paid_mana; ++reveal_index)
                {
                  if (revealed[reveal_index] != -1)
                  {
                    chosen_index = reveal_index;
                  }
                }
              }

              if ((g_duel_network_flags & 2) != 0 && player == g_active_player)
              {
                g_network_result_value = chosen_index;
                g_network_result_packet_type = 0x14;
                TENTATIVE_send_network_result(player, 0x14);
              }
            }
            else
            {
              TENTATIVE_wait_for_network_result(player, 0x14);
              chosen_index = g_network_result_value;
            }

            if (revealed[chosen_index] == -1)
            {
              done = 1;
            }
            else
            {
              put_card_on_bottom_of_library(player, revealed[chosen_index]);
              revealed[chosen_index] = -1;
            }
          }
        }
        unk_0091c4fc = 1;
      }
    }
  }

  if (event == EVENT_SHOULD_AI_PLAY && player == g_other_player && (g_duel_network_flags & 2) == 0)
  {
    instance->eot_toughness = 0;
    untapped_playable_count = 0;
    playable_count = 0;
    for (reveal_index = 0; reveal_index < g_active_cards_count[g_other_player]; ++reveal_index)
    {
      if (is_in_play(g_other_player, reveal_index) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, reveal_index).internal_card_id].extra_ability & 0x1000) != 0)
      {
        ++playable_count;
        if ((PLAYER_CARD_INSTANCE(g_other_player, reveal_index).state & STATE_TAPPED) == 0)
        {
          ++untapped_playable_count;
        }
      }
    }
    if ((untapped_playable_count * 100) / playable_count > 0x50)
    {
      instance->info_slot = 2;
    }
  }

  return 0;
}

int select_revealed_card_index_for_aladdins_lamp(int player, int *card_ids, int card_count, char *title, int require_card_click, int *out_selection)
{
  int result;

  if (player == g_other_player && (g_duel_network_flags & 2) != 0 && g_duel_active != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    return g_network_result_value;
  }

  if (player == g_other_player && (g_duel_network_flags & 2) == 0)
  {
    result = internal_rand(card_count);
  }
  else
  {
    result = (int)show_cardlist(card_ids, (int *)0, (int *)0, card_count, title, require_card_click, (char *)out_selection);
  }

  if ((g_duel_network_flags & 2) != 0 && player == g_active_player)
  {
    g_network_result_value = result;
    g_network_result_packet_type = 0x19;
    TENTATIVE_send_network_result(player, 0x19);
  }

  return result;
}

// FUNCTION: SHANDALAR 0x0044e72f
int choose_best_revealed_card_index(int player, int controller, unsigned int required_type, int *card_ids)
{
  int current_index;
  int best_score;
  int best_index;
  int score;
  int internal_card_id;
  int colored_cost;

  best_score = -99;
  best_index = -1;
  if (controller == -1)
  {
    return -1;
  }

  for (current_index = 0; card_ids[current_index] != -1 && current_index < 150; ++current_index)
  {
    internal_card_id = card_ids[current_index];
    if (required_type == 0xffffffff || (required_type & global_cards_data[internal_card_id].type) != 0)
    {
      colored_cost = (int)global_cards_data[internal_card_id].cc[1];
      if (colored_cost < 0)
      {
        colored_cost = -colored_cost;
      }
      score = colored_cost + global_cards_data[internal_card_id].cc[0] * 2;
      if (g_basiclandtypes_controlled[player][COLOR_ANY] < score)
      {
        score = -score;
      }
      if (score == 0)
      {
        score = 99;
      }
      score += GetCardRarity(internal_card_id) * 2;
      if (best_score < score)
      {
        best_index = current_index;
        best_score = score;
      }
    }
  }

  return best_index;
}

// FUNCTION: MAGIC 0x0040eb7d
// FUNCTION: SHANDALAR 0x00511395
int card_feldon_s_cane(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && global_graveyard_slots[player][0] != -1;
  }

  if (event == EVENT_ACTIVATE)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    kill_card(player, card, KILL_REMOVE);
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    while (global_graveyard_slots[player][0] != -1)
    {
      put_card_on_bottom_of_library(player, global_graveyard_slots[player][0]);
      remove_card_from_graveyard(player, 0);
    }
    shuffle_duel_library(player, player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040ed84
// FUNCTION: SHANDALAR 0x0051159c
int card_mishra_s_war_machine(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_player &&
      unk_00742f60 == player)
  {
    instance->upkeep_flags |= 1;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_summary.hand_counts[player] < 1)
    {
      g_event_result |= 1;
    }
    else
    {
      discard(player, 0, 1);
    }
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    LoadTextSectionLines("prompts.txt", "MISHRAS_WARMACHINE");
    do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).state |= STATE_TAPPED;
    damage_player(player, 3, g_card_on_stack_controller, g_card_on_stack);
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    instance->info_slot = 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_duel_summary.hand_counts[player] != 0)
  {
    damage_player(player, 3, player, card);
    g_ai_modifier += 0x60;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040efdc
// FUNCTION: SHANDALAR 0x005117f9
int card_primal_clay(int player, int card, event_t event)
{
  card_instance_t *instance;
  char prompt[900];
  int choice;
  int dynamic_internal_card_id;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    prompt[0] = '\0';
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PRIMAL_CLAY");
      sprintf(prompt, "%s\n%s\n%s\n%s", g_text_lines[0], g_text_lines[1], g_text_lines[2], g_text_lines[3]);
    }
    choice = do_dialog(player, player, card, -1, -1, prompt, 1);
    dynamic_internal_card_id = create_a_card_type(instance->internal_card_id);
    if (dynamic_internal_card_id != -1)
    {
      if (choice == 0)
      {
        global_cards_data[dynamic_internal_card_id].power = 1;
        global_cards_data[dynamic_internal_card_id].toughness = 6;
        global_cards_data[dynamic_internal_card_id].subtype = 0;
        global_cards_data[dynamic_internal_card_id].static_ability = 0;
      }
      else if (choice == 1)
      {
        global_cards_data[dynamic_internal_card_id].power = 2;
        global_cards_data[dynamic_internal_card_id].toughness = 2;
        global_cards_data[dynamic_internal_card_id].static_ability = KEYWORD_FLYING;
      }
      else if (choice == 2)
      {
        global_cards_data[dynamic_internal_card_id].power = 3;
        global_cards_data[dynamic_internal_card_id].toughness = 3;
        global_cards_data[dynamic_internal_card_id].static_ability = 0;
      }
      instance->dummy3 = dynamic_internal_card_id;
      instance->internal_card_id = instance->dummy3;
      instance->regen_status |= 0xf000000;
    }
  }

  if (event == EVENT_CHANGE_TYPE &&
      (g_land_can_be_played & 0x20000) == 0 &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (is_in_play(player, card) != 0 || (instance->state & STATE_INVISIBLE) != 0))
  {
    g_event_result = instance->dummy3;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    invalidate_dynamic_card_type(instance->dummy3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040f3a6
// FUNCTION: SHANDALAR 0x00511bc3
int card_shapeshifter(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  int choice;
  int dynamic_internal_card_id;
  char prompt[600];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      if (g_duel_ai_mode_state == 1)
      {
        g_ai_recorded_choice = internal_rand(7);
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }
    }
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SHAPESHIFTER");
      sprintf(prompt,
              "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
              g_text_lines[0],
              g_text_lines[1],
              g_text_lines[2],
              g_text_lines[3],
              g_text_lines[4],
              g_text_lines[5],
              g_text_lines[6],
              g_text_lines[7],
              g_text_lines[8]);
    }
    choice = do_dialog(player, player, card, -1, -1, prompt, g_ai_recorded_choice);
    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      choice = g_ai_recorded_choice;
    }

    dynamic_internal_card_id = create_a_card_type(instance->internal_card_id);
    if (dynamic_internal_card_id != -1)
    {
      global_cards_data[dynamic_internal_card_id].power = (short)choice;
      global_cards_data[dynamic_internal_card_id].toughness = 7 - (short)choice;
      instance->dummy3 = dynamic_internal_card_id;
      instance->internal_card_id = instance->dummy3;
      instance->regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
    }
  }

  if (event == EVENT_CHANGE_TYPE &&
      (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (is_in_play(player, card) != 0 || (instance->state & STATE_INVISIBLE) != 0))
  {
    g_event_result = instance->dummy3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return g_current_phase == PHASE_UPKEEP &&
           player == g_current_player &&
           instance->eot_toughness == 0 &&
           player == unk_00742f60;
  }

  if (event == EVENT_ACTIVATE && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->targets[0].player = player;
    instance->targets[0].card = card;
    instance->number_of_targets = 1;
    ++instance->eot_toughness;
  }

  if (event == EVENT_RESOLVE_ACTIVATION &&
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).internal_card_id != -1)
  {
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
    load_text("prompts.txt", "SHAPESHIFTER");
    sprintf(prompt,
            "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
            g_text_lines[0],
            g_text_lines[1],
            g_text_lines[2],
            g_text_lines[3],
            g_text_lines[4],
            g_text_lines[5],
            g_text_lines[6],
            g_text_lines[7],
            g_text_lines[8]);
    choice = do_dialog(player, player, card, -1, -1, prompt, 4);
    global_cards_data[parent->internal_card_id].power = (short)choice;
    global_cards_data[parent->internal_card_id].toughness = 7 - (short)choice;
  }

  if (event == EVENT_CLEANUP)
  {
    instance->eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040fb86
// FUNCTION: SHANDALAR 0x0051239f
int card_tetravus(int player, int card, event_t event)
{
  card_instance_t *instance;
  int can_activate;
  int counters_removed;
  int attached_tetravites;
  char prompt[600];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    set_special_counters(player, card, 3);
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result += C_get_special_counters(player, card);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 0;
    if (g_current_phase == PHASE_UPKEEP && player == g_current_player && player == unk_00742f60)
    {
      if (instance->info_slot < (int)C_get_special_counters(player, card))
      {
        can_activate = 1;
      }
      else if (count_tetravites_attached_to_tetravus(player, card) != 0)
      {
        can_activate = 1;
      }
    }

    if (can_activate == 0)
    {
      return 0;
    }

    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && unk_00939330 > 0)
    {
      unk_008b3270 |= 3;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE && g_affected_card == card && g_affected_card_controller == player)
  {
    counters_removed = C_get_special_counters(player, card) - instance->info_slot;
    attached_tetravites = count_tetravites_attached_to_tetravus(player, card);
    if (counters_removed == 3 || (counters_removed != 0 && attached_tetravites == 0))
    {
      tetravus_remove_counter_to_make_tetravite(player, card);
    }
    else if (counters_removed == 0 && attached_tetravites != 0)
    {
      instance->eot_toughness |= 0x100;
    }
    else if (counters_removed != 0 && attached_tetravites != 0)
    {
      load_text("prompts.txt", "TETRAVUS");
      sprintf(prompt, "%s\n%s", g_text_lines[0], g_text_lines[1]);
      if (do_dialog(player, player, card, -1, -1, prompt, 0) == 0)
      {
        tetravus_remove_counter_to_make_tetravite(player, card);
      }
      else
      {
        instance->eot_toughness |= 0x100;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) == 0)
    {
      tetravus_create_tetravite(player);
    }
    else if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                  PLAYER_CARD_INSTANCE(player, card).parent_card)
                 .internal_card_id != -1)
    {
      tetravus_reabsorb_tetravite(player, card);
    }
  }

  if ((event == EVENT_CLEANUP || event == 199) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    instance->eot_toughness = 0;
    instance->info_slot = instance->eot_toughness;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00512898
int count_tetravites_attached_to_tetravus(int player, int card)
{
  int current_player;
  int current_card;
  int count;
  int internal_card_id;

  count = 0;
  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
      if (is_in_play(current_player, current_card) != 0 &&
          global_cards_data[internal_card_id].id == 0x37b &&
          PLAYER_CARD_INSTANCE(current_player, current_card).damage_source_player == player &&
          PLAYER_CARD_INSTANCE(current_player, current_card).damage_source_card == card &&
          PLAYER_CARD_INSTANCE(current_player, current_card).info_slot == 0)
      {
        ++count;
      }
    }
  }

  return count;
}

// FUNCTION: SHANDALAR 0x00512a01
int tetravus_remove_counter_to_make_tetravite(int player, int card)
{
  remove_special_counters(player, card, 1);
  PLAYER_CARD_INSTANCE(player, card).regen_status |= KEYWORD_RECALC_TOUGHNESS;
  if (C_get_abilities(player, card, EVENT_TOUGHNESS, -1) > 0)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |= KEYWORD_RECALC_POWER;
    C_get_abilities(player, card, EVENT_POWER, -1);
  }
  if (unk_00939330 > 0)
  {
    --unk_00939330;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00512b06
int tetravus_create_tetravite(int player)
{
  int tetravite_card;

  tetravite_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(0x37b));
  if (tetravite_card != -1)
  {
    process_card_enters_play(player, tetravite_card);
    PLAYER_CARD_INSTANCE(player, tetravite_card).damage_source_player = (char)g_card_on_stack_controller;
    PLAYER_CARD_INSTANCE(player, tetravite_card).damage_source_card = g_card_on_stack;
    PLAYER_CARD_INSTANCE(player, tetravite_card).token_status |= 0x10;
    PLAYER_CARD_INSTANCE(player, tetravite_card).info_slot = 1;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00512c19
int tetravus_reabsorb_tetravite(int player, int card)
{
  target_t target;
  char invalid_prompt[300];
  int absorbed;
  int invalid_selection;

  absorbed = 0;
  do
  {
    load_text("prompts.txt", "TETRAVITE");
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
                              find_internal_card_id_by_csv_id(0x37b),
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
      invalid_selection = 1;
      load_text("prompts.txt", "TETRAVITE");
      strcpy(invalid_prompt, g_text_lines[1]);
      if (PLAYER_CARD_INSTANCE(target.player, target.card).damage_source_player == g_card_on_stack_controller &&
          PLAYER_CARD_INSTANCE(target.player, target.card).damage_source_card == g_card_on_stack)
      {
        strcpy(invalid_prompt, g_text_lines[2]);
        if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot == 0)
        {
          add_special_counter(g_card_on_stack_controller, g_card_on_stack);
          kill_card(target.player, target.card, KILL_REMOVE);
          ++PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .info_slot;
          ++absorbed;
          invalid_selection = 0;
        }
      }
      if (invalid_selection != 0 && g_duel_ai_mode_state != 1)
      {
        set_duel_prompt_text(invalid_prompt);
        Sleep(2000);
        set_duel_prompt_text("");
      }
    }
  } while (g_spell_fizzled != 1 && absorbed == 0);

  strcpy(g_ui_message_buffer, "");
  PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                       PLAYER_CARD_INSTANCE(player, card).parent_card)
      .eot_toughness = 0;

  return 0;
}

// FUNCTION: MAGIC 0x00410756
// FUNCTION: SHANDALAR 0x00512f6e
int card_tetravite(int player, int card, event_t event)
{
  if ((event == EVENT_CLEANUP || event == 199) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_ABILITIES &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result |= 0x20000;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004107ff
// FUNCTION: SHANDALAR 0x00513019
int card_triskelion(int player, int card, event_t event)
{
  unsigned int counters;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    set_special_counters(player, card, 3);
  }

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result += C_get_special_counters(player, card);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    counters = C_get_special_counters(player, card);
    return counters > 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE && C_get_special_counters(player, card) > 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "TRISKELION");
    }
    if (select_damage_target(player, card, 1) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x6000000;
      remove_special_counter(player, card);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, EVENT_RESOLVE_ACTIVATION, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00410a22
// FUNCTION: SHANDALAR 0x0051323c
int card_urza_s_avenger(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  int legacy_card;
  char prompt[600];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (player == g_other_player &&
        (g_duel_network_flags & 2) == 0 &&
        instance->eot_toughness != 0 &&
        (instance->regen_status & 0x20) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "URZAS_AVENGER");
      sprintf(prompt,
              "%s\n%s\n%s\n%s\n%s\n%s",
              g_text_lines[0],
              g_text_lines[1],
              g_text_lines[2],
              g_text_lines[3],
              g_text_lines[4],
              g_text_lines[5]);
    }
    instance->eot_toughness = do_dialog(player, player, card, -1, -1, prompt, 0) + 1;
    if (instance->eot_toughness == 5)
    {
      g_spell_fizzled = 1;
      instance->eot_toughness = 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    if (parent->internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      if (instance->damage_source_player == -1 && instance->damage_source_card == -1)
      {
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           unk_00896534,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).regen_status = 0;
          parent->damage_source_player = (char)player;
          parent->damage_source_card = legacy_card;
        }
      }
      else
      {
        legacy_card = instance->damage_source_card;
      }

      if (legacy_card != -1)
      {
        switch (instance->eot_toughness)
        {
        case 1:
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x20;
          break;
        case 2:
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x40;
          break;
        case 3:
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x100;
          break;
        case 4:
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80;
          break;
        }
        ++PLAYER_CARD_INSTANCE(player, legacy_card).counter_power;
        ++PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness;
      }

      parent->regen_status = KEYWORD_RECALC_ABILITIES;
      --parent->counter_power;
      --parent->counter_toughness;
      ++parent->info_slot;
      parent->eot_toughness = 0;
    }
  }

  if (event == EVENT_CLEANUP || event == 199)
  {
    instance->counter_power += (short)instance->info_slot;
    instance->counter_toughness += (short)instance->info_slot;
    instance->info_slot = 0;
    instance->damage_source_card = -1;
    instance->damage_source_player = (char)-1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00411509
// FUNCTION: SHANDALAR 0x00513d22
int card_millstone(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2) &&
        !is_animated_and_sick(player, card) &&
        (instance->state & STATE_TAPPED) == 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
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
                                "Select target player.",
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
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    put_top_library_card_into_graveyard(instance->targets[0].player);
    put_top_library_card_into_graveyard(instance->targets[0].player);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
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
    g_ai_modifier -= 0x18;
    charge_mana(player, 0, 2);
    if (g_spell_fizzled != 1)
    {
      if ((g_other_player == player) && ((g_duel_network_flags & 2) == 0))
      {
        if (g_duel_ai_mode_state == 1)
        {
          g_ai_recorded_choice = g_ai_search_try_count % 5 + 1;
          color = g_ai_recorded_choice;
          record_ai_action_selection();
        }
        else
        {
          replay_ai_action_selection();
          if (g_ai_recorded_choice < 6)
          {
            color = g_ai_recorded_choice;
          }
          else
          {
            g_spell_fizzled = 1;
            color = ~COLOR_COLORLESS;
          }
        }
      }
      else
      {
        color = -1;
      }

      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "CELESTIAL_PRISM");
        }
        color = choose_a_color(player, g_text_lines[0], 1, color, PLAYER_CARD_INSTANCE(player, card).damage_source_card);
        if (color == ~COLOR_COLORLESS)
        {
          g_spell_fizzled = 1;
        }
        else if (g_spell_fizzled != 1)
        {
          produce_mana(player, color, 1);
          undeclare_mana_available_hex(player, PLAYER_CARD_INSTANCE(player, card).damage_source_card, 1);
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
          g_produced_mana_color = color;
          if (((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
          {
            load_text("prompts.txt", "CELESTIAL_PRISM2");
            if (color == COLOR_BLACK)
            {
              strcpy(message, g_text_lines[1]);
            }
            else if (color == COLOR_BLUE)
            {
              strcpy(message, g_text_lines[2]);
            }
            else if (color == COLOR_GREEN)
            {
              strcpy(message, g_text_lines[3]);
            }
            else if (color == COLOR_RED)
            {
              strcpy(message, g_text_lines[4]);
            }
            else
            {
              strcpy(message, g_text_lines[5]);
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
  unsigned int available_colors;
  color_t color;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return !is_animated_and_sick(player, card) &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
           get_fellwar_stone_colors(player) != 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    available_colors = get_fellwar_stone_colors(player);
    if (available_colors == 0)
    {
      g_spell_fizzled = 1;
      return 0;
    }

    color = choose_a_color(player, "Choose a color.", 1, ~COLOR_COLORLESS, available_colors);
    if (color == ~COLOR_COLORLESS)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      produce_mana(player, color, 1);
      g_produced_mana_color = color;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_COUNT_MANA &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      !is_animated_and_sick(player, card))
  {
    available_colors = get_fellwar_stone_colors(player);
    if (available_colors != 0)
    {
      declare_mana_available_hex(player, available_colors, 1);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041240b
// FUNCTION: SHANDALAR 0x00514c20
int card_ashnod_s_battle_gear(int player, int card, event_t event)
{
  return artifact_tapped_pump(player, card, event, 2, -2, player, player);
}

// FUNCTION: MAGIC 0x00412e9c
// FUNCTION: SHANDALAR 0x005156b0
int card_tawnos_s_weaponry(int player, int card, event_t event)
{
  return artifact_tapped_pump(player, card, event, 1, 1, 2, player);
}

// FUNCTION: MAGIC 0x00413c87
// FUNCTION: SHANDALAR 0x0051649a
int card_battering_ram(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;

  if (event == EVENT_CHANGE_TYPE)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked |= 0x40;
  }

  if (event == EVENT_DECLARE_BLOCKERS &&
      player == g_current_player &&
      (PLAYER_CARD_INSTANCE(player, card).state & (STATE_ATTACKING | STATE_ATTACKED)) != 0)
  {
    opponent = 1 - player;
    if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
    {
      blocking_card = card;
    }
    else
    {
      blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
    }

    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
      if (PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card &&
          blocking_iid != -1 &&
          global_cards_data[blocking_iid].subtype == SUB_WALL &&
          (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)
      {
        create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00413eb8
// FUNCTION: SHANDALAR 0x005166cc
int card_candelabra_of_tawnos(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int target_index;
  int old_max_x_value;
  int tapped_lands;
  int current_player;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (is_animated_and_sick(player, card) || (instance->state & STATE_TAPPED) != 0)
    {
      return 0;
    }
    tapped_lands = 0;
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (PLAYER_CARD_INSTANCE(current_player, current_card).state & STATE_TAPPED) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ++tapped_lands;
        }
      }
    }
    return tapped_lands != 0 && has_mana(player, COLOR_COLORLESS, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    tapped_lands = 0;
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (PLAYER_CARD_INSTANCE(current_player, current_card).state & STATE_TAPPED) != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ++tapped_lands;
        }
      }
    }

    old_max_x_value = g_max_x_value;
    g_max_x_value = tapped_lands;
    instance->info_slot = charge_mana(player, COLOR_COLORLESS, -1);
    g_max_x_value = old_max_x_value;

    if (g_spell_fizzled != 1 && instance->info_slot > 0)
    {
      instance->number_of_targets = 0;
      target_index = 0;
      while (target_index < instance->info_slot && g_spell_fizzled != 1)
      {
        if (!C_real_select_target(player,
                                  2,
                                  2,
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
                                  TARGET_STATE_TAPPED,
                                  0,
                                  "Select target tapped land.",
                                  1,
                                  &target))
        {
          g_spell_fizzled = 1;
        }
        else
        {
          instance->targets[instance->number_of_targets] = target;
          ++instance->number_of_targets;
          PLAYER_CARD_INSTANCE(target.player, target.card).state |= 0x200000;
          TENTATIVE_reassess_all_cards(0, 0xff);
        }
        ++target_index;
      }

      for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(instance->targets[target_index].player, instance->targets[target_index].card).state &= ~0x200000;
      }

      if (g_spell_fizzled != 1)
      {
        instance->state |= STATE_TAPPED;
      }
      else
      {
        instance->number_of_targets = 0;
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
    for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
    {
      target = instance->targets[target_index];
      if (C_real_validate_target(target.player,
                                 target.card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
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
                                 TARGET_STATE_TAPPED,
                                 0))
      {
        PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
      }
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041465a
// FUNCTION: SHANDALAR 0x00516e6e
int card_clay_statue(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 0, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004146a7
// FUNCTION: SHANDALAR 0x00516ebb
int card_diabolic_machine(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 0, 3);
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
    if (((instance->state & 0x10) != 0 && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) || (g_land_can_be_played & 4) == 0 || (g_current_phase != 0x19 && g_current_phase != 0x1a) || has_mana(player, COLOR_COLORLESS, 1) == 0)
    {
      return 0;
    }
    return 99;
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_COLORLESS, 1) != 0 && (g_land_can_be_played & 4) != 0 && (g_current_phase == 0x19 || g_current_phase == 0x1a))
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
                                g_damage_card_internal_card_id,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_DAMAGE_PLAYER,
                                0,
                                0,
                                g_text_lines[0],
                                1,
                                &target))
      {
        g_spell_fizzled = 1;
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
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    *((char *)&PLAYER_CARD_INSTANCE(g_card_on_stack_controller, g_card_on_stack) + 0x32) = 0;
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
                               g_damage_card_internal_card_id,
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
      g_spell_fizzled = 1;
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
    if (has_mana(player, 7, 3) && player == g_current_player && CAN_TAP(player, card))
    {
      return 1;
    }
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) && has_mana(player, 7, 3) && player == g_current_player)
  {
    charge_mana(player, 0, 3);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "DISRUPTING_SCEPTER");
      }
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE,
                                0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, g_text_lines[0], 1, &target))
      {
        g_spell_fizzled = 1;
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
    ++g_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041510c
// FUNCTION: SHANDALAR 0x00517922
int card_blue_mana_battery(int player, int card, event_t event)
{
  return mana_battery(player, card, event, COLOR_BLUE);
}

// FUNCTION: MAGIC 0x00415132
// FUNCTION: SHANDALAR 0x00517948
int card_red_mana_battery(int player, int card, event_t event)
{
  return mana_battery(player, card, event, COLOR_RED);
}

// FUNCTION: MAGIC 0x00415158
// FUNCTION: SHANDALAR 0x0051796e
int card_green_mana_battery(int player, int card, event_t event)
{
  return mana_battery(player, card, event, COLOR_GREEN);
}

// FUNCTION: MAGIC 0x0041517e
// FUNCTION: SHANDALAR 0x00517994
int card_white_mana_battery(int player, int card, event_t event)
{
  return mana_battery(player, card, event, COLOR_WHITE);
}

// FUNCTION: MAGIC 0x004151a4
// FUNCTION: SHANDALAR 0x005179ba
int card_black_mana_battery(int player, int card, event_t event)
{
  return mana_battery(player, card, event, COLOR_BLACK);
}

// FUNCTION: MAGIC 0x004151ca
// FUNCTION: SHANDALAR 0x005179e0
int mana_battery(int player, int card, event_t event, color_t color)
{
  card_instance_t *instance;
  unsigned int counters;
  unsigned int counters_to_use;
  int choice;
  int default_choice;
  char prompt[900];
  char number_prompt[400];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((instance->untap_status & 3) == 0) ||
         (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) &&
        (instance->state & STATE_TAPPED) == 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->state & STATE_TAPPED) == 0)
  {
    counters = C_get_special_counters(player, card);
    if (unk_00938e2c == 0 && has_mana(player, COLOR_ANY, counters + 3) != 0)
    {
      prompt[0] = '\0';
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "MANABATTERY");
        sprintf(prompt, "%s\n%s\n%s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      }
      if (g_current_phase == EVENT_MAX_HAND_SIZE && 1 - player == g_current_player)
      {
        default_choice = 1;
      }
      else
      {
        default_choice = 0;
      }
      choice = do_dialog(player, player, card, -1, -1, prompt, default_choice);
    }
    else
    {
      choice = 0;
    }

    instance->info_slot = 0;
    if (choice == 0)
    {
      produce_mana(player, color, 1);
      counters = C_get_special_counters(player, card);
      if (counters != 0)
      {
        if (((player == g_other_player && (g_duel_network_flags & 2) == 0) ||
             g_duel_ai_mode_state == 1) ||
            g_duel_network_state != 0)
        {
          if (g_duel_ai_mode_state == 1)
          {
            counters_to_use = internal_rand(counters + 1);
            g_ai_recorded_choice = counters_to_use;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            counters_to_use = g_ai_recorded_choice;
          }
        }
        else
        {
          if (g_duel_ai_mode_state != 1)
          {
            LoadTextSectionLines("prompts.txt", "MANABATTERY");
            sprintf(number_prompt, "Use how many counters from %s? (0-%u)", global_cards_data[instance->internal_card_id].name, counters);
          }
          counters_to_use = choose_a_number(player, number_prompt, 0);
        }
        if (counters_to_use == 0xffffffff)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          if (counters < counters_to_use)
          {
            counters_to_use = counters;
          }
          undeclare_mana_available_and_produce_it(player, color, counters_to_use);
          remove_special_counters(player, card, counters_to_use);
        }
      }
      if (g_spell_fizzled == 1)
      {
        unproduce_mana(player, color, 1);
      }
      else
      {
        instance->info_slot = 0;
        instance->state |= STATE_TAPPED;
        g_duel_special_land_card_ids[6] = color;
      }
    }
    else if (choice == 1)
    {
      instance->state |= STATE_TAPPED;
      charge_mana(player, COLOR_COLORLESS, 2);
      if (g_spell_fizzled == 1)
      {
        instance->state &= ~STATE_TAPPED;
      }
      if (g_spell_fizzled != 1)
      {
        instance->info_slot = 1;
        g_duel_special_land_card_ids[6] = -1;
      }
    }
    else if (choice == 2)
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && instance->info_slot == 1)
  {
    add_special_counter(g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 0;
  }

  if (event == EVENT_COUNT_MANA &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      (instance->state & STATE_TAPPED) == 0)
  {
    declare_mana_available(player, color, 1);
    declare_mana_available(player, color, C_get_special_counters(player, card));
  }

  if (event == EVENT_CAN_WASTE_MANA &&
      g_raw_mana_available[player][COLOR_WHITE] != 0 &&
      (instance->state & STATE_TAPPED) == 0)
  {
    g_event_result |= 1;
  }

  if (event == EVENT_SHOULD_AI_PLAY && player == g_other_player)
  {
    g_ai_modifier += C_get_special_counters(player, card) * 0xc;
  }

  return 0;
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

  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_basiclandtypes_controlled[player][7] * 6;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) == 0 || is_animated_and_sick(player, card) || (instance->state & STATE_TAPPED) || !has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3) || !real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id, ~SUB_WALL, -1, -1, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->state & STATE_TAPPED) == 0)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      instance->number_of_targets = 0;
      target_index = 0;
      stop_selecting = 0;
      while (target_index < 2 && stop_selecting == 0 && g_spell_fizzled != 1)
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
                                  g_damage_card_internal_card_id,
                                  ~SUB_WALL,
                                  -1,
                                  -1,
                                  TARGET_SPECIAL_DAMAGE_PLAYER,
                                  0,
                                  0,
                                  g_text_lines[0],
                                  3,
                                  &selected_target))
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
        else
        {
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= 0x200000;
          TENTATIVE_reassess_all_cards(0, 0xff);
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

      if (g_spell_fizzled == 1)
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
                                  g_damage_card_internal_card_id,
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
      g_spell_fizzled = 1;
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
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_basiclandtypes_controlled[g_other_player][color] * 0xc;
  }

  if (g_trigger_condition == TRIGGER_SPELL_CAST && g_affected_card == card && g_affected_card_controller == player && player == g_current_turn && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) && is_in_play(player, card) && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
  {
    if (((1 << ((unsigned char)get_sleighted_color(player, card, color) & 0x1f)) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).color) != 0 && PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 && global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type != TYPE_LAND)
    {
      if (event == EVENT_TRIGGER)
      {
        if (g_other_player == player && (g_duel_network_flags & 2) == 0)
        {
          g_event_result |= 2;
        }
        else
        {
          g_event_result |= 1;
        }
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, COLOR_COLORLESS, 1);
        obliterate_top_card_of_stack();
        if (g_spell_fizzled != 1)
        {
          dispatch_event(player, card, EVENT_PLAY_ABILITY);
          gain_life(player, 1, player, card);
          if (g_other_player == player)
          {
            g_ai_modifier -= 0x18;
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
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_basiclandtypes_controlled[g_other_player][7] - g_basiclandtypes_controlled[g_active_player][7]) * 0xc;
  }

  if ((g_trigger_condition == 0xdb || g_trigger_condition == 0xd3) && g_affected_card == card && g_affected_card_controller == player && g_current_player == g_current_turn && PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      damage_player(g_trigger_cause_controller, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041674e
// FUNCTION: SHANDALAR 0x00518f64
int card_armageddon_clock(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int counters;
  int parent_player;
  int parent_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player && g_current_player == player)
  {
    add_special_counters(player, card, 1);
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    counters = C_get_special_counters(player, card);
    if (counters != 0)
    {
      damage_player(g_current_player, counters, player, card);
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return g_current_phase == PHASE_UPKEEP &&
           C_get_special_counters(player, card) != 0 &&
           has_mana(g_current_player, COLOR_COLORLESS, 4);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(g_current_player, COLOR_COLORLESS, 4);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    parent_player = instance->parent_controller;
    parent_card = instance->parent_card;
    if (PLAYER_CARD_INSTANCE(parent_player, parent_card).internal_card_id != -1 &&
        C_get_special_counters(parent_player, parent_card) != 0)
    {
      remove_special_counter(parent_player, parent_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00416dd5
// FUNCTION: SHANDALAR 0x005195e9
int card_dingus_egg(int player, int card, event_t event)
{
  card_instance_t *instance;
  int current_player;
  int damage_count;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_GRAVEYARD_FROM_PLAY && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_LAND) && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 4 && (instance->state & 0x20) == 0 && (((instance->state & STATE_TAPPED) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if (g_affected_card_controller == 0)
    {
      ++instance->info_slot;
    }
    else
    {
      instance->info_slot += 0x100;
    }
  }

  if (g_trigger_condition == 0xd5 && g_affected_card == card && g_affected_card_controller == player && (instance->info_slot & 0xffff) != 0 && player == g_current_turn && (instance->state & 0x20) == 0 && (((instance->state & STATE_TAPPED) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
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
  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x90;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status & 0x10) == 0 && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).state & 0x20) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
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

  dummy = 0;
  if (g_trigger_condition == 0xd5 && g_affected_card == card && g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && player == g_current_turn)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = (int)((unsigned int)PLAYER_CARD_INSTANCE(player, card).info_slot | 0x100);
    dummy = ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ||
            (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE);
  }

  if (dummy)
  {
    if (event == EVENT_TRIGGER)
    {
      if (!has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
      else if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        g_event_result |= 2;
      }
      else
      {
        g_event_result |= 1;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
      charge_mana(player, COLOR_COLORLESS, 1);
      obliterate_top_card_of_stack();
      if (g_spell_fizzled == 1)
      {
        g_spell_fizzled = -1;
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
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += g_creature_power_by_color[g_other_player][7] * 3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((instance->untap_status & 3) == 0) ||
         (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) &&
        (instance->state & STATE_TAPPED) == 0 &&
        has_mana(player, COLOR_COLORLESS, 2) &&
        real_target_available((int *)0,
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
                              -1,
                              -1,
                              -1,
                              0,
                              TARGET_STATE_ATTACKING,
                              0))
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE &&
        (instance->state & STATE_TAPPED) == 0 &&
        has_mana(player, COLOR_COLORLESS, 2) &&
        player == g_current_player &&
        g_attacking_creature_count != 0)
    {
      charge_mana(player, COLOR_COLORLESS, 2);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "EBONYHORSE");
        }
        if (!C_real_select_target(player,
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
                                  TARGET_STATE_ATTACKING,
                                  0,
                                  g_text_lines[0],
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
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      target = instance->targets[0];
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
      if (!C_real_validate_target(target.player,
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
        PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
        create_legacy_effect(g_card_on_stack_controller,
                             g_card_on_stack,
                             DAT_007ab2c8,
                             target.player,
                             target.card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00417d1f
// FUNCTION: SHANDALAR 0x0051a531
int card_jayemdae_tome(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player && g_other_player == player && (g_duel_network_flags & 2) == 0)
  {
    int artifact_count;

    artifact_count = count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player);
    if (artifact_count != 0 && g_basiclandtypes_controlled[g_other_player][7] / artifact_count > 3)
    {
      g_ai_modifier += 0x30;
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
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00417f48
// FUNCTION: SHANDALAR 0x0051a759
int card_mana_vault(int player, int card, event_t event)
{
  int can_activate;

  if (event == EVENT_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 1;
    if (IS_SICK(player, card))
    {
      can_activate = 0;
    }
    else if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
             ((has_mana(player, COLOR_ANY, 4) == 0 || g_current_phase != 4) || player != g_current_player))
    {
      can_activate = 0;
    }
    else if (player == g_other_player && (g_duel_network_flags & 2) == 0 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
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
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      g_produced_mana_color = 0;
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

  if (((g_trigger_condition == 0xcb) || event == EVENT_SHOULD_AI_PLAY) && g_affected_card == card && g_affected_card_controller == player && player == g_current_player && g_current_turn == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
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

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_ANY] < 4 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    g_life[player] -= 4 - g_basiclandtypes_controlled[player][COLOR_ANY];
  }

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)))
  {
    declare_mana_available(player, COLOR_COLORLESS, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004186d8
// FUNCTION: SHANDALAR 0x0051aee6
int card_sol_ring(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0xc0 / (g_basiclandtypes_controlled[g_other_player][7] + 1);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (!is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    g_ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 2);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = 0;
  }

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
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

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player && count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) != 0)
  {
    g_ai_modifier -= 0xf0;
  }

  if (((event == 0x82) && ((global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_CREATURE) != 0)) && ((2 < C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_POWER, -1)) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) && ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))))
  {
    *(unsigned int *)&PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).dummy3 &= 0xfffffffc;
  }

  if (event == 199)
  {
    max_cards = g_active_cards_count[g_other_player];
    if (max_cards <= g_active_cards_count[g_active_player])
    {
      max_cards = g_active_cards_count[g_active_player];
    }
    ai_delta = 0;
    for (current_card = 0; current_card < max_cards; ++current_card)
    {
      if (is_in_play(g_active_player, current_card) && PLAYER_CARD_INSTANCE(g_active_player, current_card).power > 2)
      {
        if ((PLAYER_CARD_INSTANCE(g_active_player, current_card).state & STATE_TAPPED) == 0)
        {
          if (has_vigilance(g_active_player, current_card) == 0)
          {
            ai_delta += PLAYER_CARD_INSTANCE(g_active_player, current_card).power;
          }
        }
        else if (PLAYER_CARD_INSTANCE(g_active_player, current_card).untap_status == 0)
        {
          ai_delta += PLAYER_CARD_INSTANCE(g_active_player, current_card).power * 2;
        }
      }

      if (is_in_play(g_other_player, current_card) && PLAYER_CARD_INSTANCE(g_other_player, current_card).power > 2)
      {
        if ((PLAYER_CARD_INSTANCE(g_other_player, current_card).state & STATE_TAPPED) == 0)
        {
          if (has_vigilance(g_other_player, current_card) == 0)
          {
            ai_delta -= PLAYER_CARD_INSTANCE(g_other_player, current_card).power;
          }
        }
        else if (PLAYER_CARD_INSTANCE(g_other_player, current_card).untap_status == 0)
        {
          ai_delta += PLAYER_CARD_INSTANCE(g_other_player, current_card).power * -2;
        }
      }
    }
    g_ai_modifier += ai_delta * 0xc;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00418d7c
// FUNCTION: SHANDALAR 0x0051b58d
int card_jandor_s_saddlebags(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3) &&
           !is_animated_and_sick(player, card) &&
           (instance->state & STATE_TAPPED) == 0 &&
           real_target_available((int *)0,
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
                                 TARGET_STATE_TAPPED,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      if (!C_real_select_target(player,
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
                                TARGET_STATE_TAPPED,
                                0,
                                "Select target tapped creature.",
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
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                TARGET_STATE_TAPPED,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00419295
// FUNCTION: SHANDALAR 0x0051baa5
int card_jade_monolith(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_life[g_other_player] - g_life[g_active_player]) * 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) != 0 && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && real_target_available((int *)0, TARGET_SCAN_DAMAGE_TARGET, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_DAMAGED, 0))
    {
      return 99;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
      if (g_spell_fizzled != 1)
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
                                  g_text_lines[0],
                                  1,
                                  &selected_target))
        {
          g_spell_fizzled = 1;
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
        g_spell_fizzled = 1;
      }
      else
      {
        redirect_pending_damage_to_player(selected_target.player, selected_target.card, player);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00419667
void redirect_pending_damage_to_player(int target_player, int target_card, int damage_target_player)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == g_damage_card_internal_card_id && (PLAYER_CARD_INSTANCE(current_player, current_card).state & 0x800002) == 2 && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
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
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != 0)
  {
    gain_life(player, 2, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00419ae1
// FUNCTION: SHANDALAR 0x0051c2f1
int card_amulet_of_kroog(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) == 0 ||
        is_animated_and_sick(player, card) ||
        (instance->state & STATE_TAPPED) != 0 ||
        !has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2) ||
        !real_target_available((int *)0,
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

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
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
                                "Select damage to prevent.",
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
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (!C_real_validate_target(target.player,
                                target.card,
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
    else if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
    {
      --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00419fc3
// FUNCTION: SHANDALAR 0x0051c7d3
int card_grapeshot_catapult(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
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
                                 ~SUB_WALL,
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
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              KEYWORD_FLYING,
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
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041a389
// FUNCTION: SHANDALAR 0x0051cb99
int card_bronze_tablet(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;
  char prompt[24];
  int choice;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->state |= STATE_TAPPED;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((instance->state & STATE_SUMMONSICK_BOTH) == 0) ||
         (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) &&
        (instance->state & STATE_TAPPED) == 0 &&
        has_mana(player, COLOR_ANY, 4) &&
        real_target_available((int *)0,
                              TARGET_SCAN_DIRECT,
                              player,
                              1 - player,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              0x7f,
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
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE && has_mana(player, COLOR_ANY, 4))
    {
      charge_mana(player, COLOR_COLORLESS, 4);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "BRONZE_TABLET");
        }
        if (!C_real_select_target(player,
                                  1 - player,
                                  1 - player,
                                  TARGET_ZONE_IN_PLAY,
                                  0x7f,
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
          g_spell_fizzled = 1;
        }
        else
        {
          instance->targets[0] = target;
          instance->number_of_targets = 1;
          instance->state |= STATE_TAPPED;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      target = instance->targets[0];
      if (!C_real_validate_target(target.player,
                                  target.card,
                                  (char *)0,
                                  player,
                                  1 - player,
                                  1 - player,
                                  TARGET_ZONE_IN_PLAY,
                                  0x7f,
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
        g_spell_fizzled = 1;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "BRONZE_TABLET");
          if (g_life[1 - player] < 10)
          {
            sprintf(prompt, " %s\n _%s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
          }
          else
          {
            sprintf(prompt, " %s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
          }
        }

        instance->info_slot = do_dialog(1 - player, player, card, -1, -1, prompt, 0);
        choice = instance->info_slot;
        parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);

        if (choice == 0)
        {
          if (parent->internal_card_id != -1)
          {
            if (player == g_active_player && g_duel_ai_mode_state != 1)
            {
              remove_card_from_shandalar_deck_by_id(parent->internal_card_id);
            }
            parent->state ^= STATE_OWNED_BY_OPPONENT;
            kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_REMOVE);
          }

          if (player == g_active_player && g_duel_ai_mode_state != 1)
          {
            AddCardToDeckSorted(PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id);
          }
          PLAYER_CARD_INSTANCE(target.player, target.card).state ^= STATE_OWNED_BY_OPPONENT;
          kill_card(target.player, target.card, KILL_REMOVE);
        }
        else if (choice == 1)
        {
          g_life[1 - player] -= 10;
          if (parent->internal_card_id != -1)
          {
            kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_DESTROY);
          }
        }
        else if (choice == 2)
        {
          if (g_duel_ai_mode_state == 1)
          {
            g_life[1 - player] = -99;
          }
          else
          {
            exit_duel_thread(1 - player);
          }
          if (parent->internal_card_id != -1)
          {
            kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_DESTROY);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0056bf4c
void remove_card_from_shandalar_deck_by_id(unsigned int card_id)
{
  int deck_slot;
  int shift_slot;

  for (deck_slot = 0; deck_slot < 500; ++deck_slot)
  {
    if (((unsigned int)deck[deck_slot] & 0xfff) == card_id)
    {
      for (shift_slot = deck_slot + 1; shift_slot < 500; ++shift_slot)
      {
        deck[shift_slot - 1] = deck[shift_slot];
      }
      deck[499] = -1;
      return;
    }
  }
}

// FUNCTION: MAGIC 0x0041ae2a
// FUNCTION: SHANDALAR 0x0051d63a
int card_nevinyrral_s_disk(int player, int card, event_t event)
{
  int result;
  int current_card;
  int guardian_beast_survives[2];
  int max_active_cards;

  if (event == 0x6c && g_affected_card == card && g_affected_card_controller == player)
  {
    result = count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1);
    if (result == 0)
    {
      g_ai_modifier += (g_creature_power_by_color[1 - player][7] - g_creature_power_by_color[player][7]) *
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
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      guardian_beast_survives[g_other_player] = 1;
      guardian_beast_survives[g_active_player] = guardian_beast_survives[g_other_player];

      current_card = 0;
      while (1)
      {
        max_active_cards = g_active_cards_count[g_other_player];
        if (max_active_cards <= g_active_cards_count[g_active_player])
        {
          max_active_cards = g_active_cards_count[g_active_player];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(g_active_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          if (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id]
                      .code_pointer ==
                  card_guardian_beast &&
              (PLAYER_CARD_INSTANCE(g_active_player, current_card).state & 0x10) == 0)
          {
            guardian_beast_survives[g_active_player] = 0;
          }
          kill_card(g_active_player, current_card, KILL_DESTROY);
        }

        result = is_in_play(g_other_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id].type &
             TYPE_CREATURE) != 0)
        {
          if (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id]
                      .code_pointer ==
                  card_guardian_beast &&
              (PLAYER_CARD_INSTANCE(g_other_player, current_card).state & 0x10) == 0)
          {
            guardian_beast_survives[g_other_player] = 0;
          }
          kill_card(g_other_player, current_card, KILL_DESTROY);
        }

        ++current_card;
      }

      regenerate_or_graveyard_triggers();

      current_card = 0;
      while (1)
      {
        max_active_cards = g_active_cards_count[g_other_player];
        if (max_active_cards <= g_active_cards_count[g_active_player])
        {
          max_active_cards = g_active_cards_count[g_active_player];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(g_active_player, current_card);
        if (result != 0 && guardian_beast_survives[g_active_player] != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id].type &
             TYPE_ARTIFACT) != 0)
        {
          kill_card(g_active_player, current_card, KILL_DESTROY);
        }

        result = is_in_play(g_other_player, current_card);
        if (result != 0 && guardian_beast_survives[g_other_player] != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id].type &
             TYPE_ARTIFACT) != 0)
        {
          kill_card(g_other_player, current_card, KILL_DESTROY);
        }

        ++current_card;
      }

      regenerate_or_graveyard_triggers();

      current_card = 0;
      while (1)
      {
        max_active_cards = g_active_cards_count[g_other_player];
        if (max_active_cards <= g_active_cards_count[g_active_player])
        {
          max_active_cards = g_active_cards_count[g_active_player];
        }
        if (max_active_cards <= current_card)
        {
          break;
        }

        result = is_in_play(g_active_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          kill_card(g_active_player, current_card, KILL_DESTROY);
        }

        result = is_in_play(g_other_player, current_card);
        if (result != 0 &&
            (global_cards_data[PLAYER_CARD_INSTANCE(g_other_player, current_card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          kill_card(g_other_player, current_card, KILL_DESTROY);
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
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 8) &&
        !is_animated_and_sick(player, card) &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
  }
  else if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 8);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ALADDINS_RING");
      }
      select_damage_target(player, card, 4);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, EVENT_RESOLVE_ACTIVATION, 4);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041b801
// FUNCTION: SHANDALAR 0x0051e01b
int card_rod_of_ruin(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_special_mana_pool[g_other_player][COLOR_ANY] * 3 - 6) * 4;
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
    load_recorded_action_target(1);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "ROD_OF_RUIN");
        }
        select_damage_target(player, card, 1);
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      deal_damage_to_selected_target(player, card, EVENT_RESOLVE_ACTIVATION, 1);
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

  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player && count_permanents_by_internal_card_id(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
  {
    g_ai_modifier +=
        (g_creature_power_by_color[g_other_player][7] - g_creature_power_by_color[g_active_player][7]) * 0xc;
  }

  if (event == 0x82 && (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).dummy3 &= ~2;
    unk_00925d3c |= 1;
  }

  if (g_current_phase == PHASE_UPKEEP && g_affected_card == card && g_affected_card_controller == player && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER && !real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_COULD_UNTAP, 0) && real_target_available((int *)0, TARGET_SCAN_DIRECT, g_current_player, g_current_player, g_current_player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, TARGET_STATE_TAPPED, 0))
    {
      g_event_result |= 2;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (g_other_player == g_current_player && (g_duel_network_flags & 2) == 0)
      {
        selected_target.player = g_current_player;
        selected_target.card = choose_best_tapped_permanent_for_ai(g_current_player, 1);
        load_text("prompts.txt", "WINTERORB");
        do_dialog(player, player, card, selected_target.player, selected_target.card, g_text_lines[1], 0);
      }
      else
      {
        load_text("prompts.txt", "WINTERORB");
        C_real_select_target(g_current_player,
                             g_current_player,
                             g_current_player,
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
                             g_text_lines[0],
                             0,
                             &selected_target);
      }

      PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).dummy3 |= 2;
      for (current_card = 0; current_card < g_active_cards_count[g_current_player]; ++current_card)
      {
        if (is_in_play(g_current_player, current_card) && (PLAYER_CARD_INSTANCE(g_current_player, current_card).state & STATE_TAPPED) && (global_cards_data[PLAYER_CARD_INSTANCE(g_current_player, current_card).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(g_current_player, current_card).dummy3 & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(g_current_player, current_card).dummy3 &= ~1;
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
  return artifact_pay_to_untap_during_upkeep(player, card, event, 1);
}

// FUNCTION: MAGIC 0x0041c1ef
// FUNCTION: SHANDALAR 0x0051ea11
int card_dragon_engine(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_COLORLESS];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_COLORLESS, 2);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      artifact_give_pump_until_eot(g_card_on_stack_controller,
                                   g_card_on_stack,
                                   g_card_on_stack_controller,
                                   g_card_on_stack,
                                   1,
                                   0);
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_COLORLESS, 2);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_COLORLESS] >= 2)
  {
    g_event_result |= 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041c706
// FUNCTION: SHANDALAR 0x0051ef24
int card_clockwork_beast(int player, int card, event_t event)
{
  return generic_clockwork_creature(player, card, event, 7);
}

// FUNCTION: MAGIC 0x0041c72c
// FUNCTION: SHANDALAR 0x0051ef4a
int card_clockwork_avian(int player, int card, event_t event)
{
  return generic_clockwork_creature(player, card, event, 4);
}

// FUNCTION: MAGIC 0x0041c752
// FUNCTION: SHANDALAR 0x0051ef70
int generic_clockwork_creature(int player, int card, int event, int amount)
{
  unsigned int special_counters;
  int old_max_x_value;
  int mana_result;
  int zero;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    set_special_counters(player, card, amount);
  }

  if (g_trigger_condition == 0xcc && (special_counters = C_get_special_counters(player, card)) != 0 && g_affected_card == card && g_affected_card_controller == player && player == g_current_turn && ((PLAYER_CARD_INSTANCE(player, card).state & 4) != 0 || (PLAYER_CARD_INSTANCE(player, card).untap_status != -1 && g_current_player != player)))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      remove_special_counter(player, card);
    }
  }

  if (event == EVENT_POWER && g_affected_card == card && g_affected_card_controller == player)
  {
    special_counters = C_get_special_counters(player, card);
    g_event_result += special_counters;
  }

  old_max_x_value = g_max_x_value;
  if (event == EVENT_CAN_ACTIVATE && g_current_phase == PHASE_MAIN1 && g_current_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && unk_00742f60 == player)
  {
    special_counters = C_get_special_counters(player, card);
    if ((int)special_counters < amount && (mana_result = has_mana(player, COLOR_ANY, 1)) != 0)
    {
      if (g_other_player == player && (g_duel_network_flags & 2) == 0 && PLAYER_CARD_INSTANCE(player, card).toughness < g_life[player])
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    mana_result = has_mana(player, COLOR_ANY, 1);
    zero = 0;
    special_counters = C_get_special_counters(player, card);
    g_loaded_recorded_action_code = ClampIntToRange(amount - special_counters, zero, mana_result);
  }
  else
  {
    if (event == EVENT_ACTIVATE && g_affected_card == card && g_affected_card_controller == player)
    {
      special_counters = C_get_special_counters(player, card);
      g_max_x_value = amount - special_counters;
      if (g_active_player == player || (g_duel_network_flags & 2) != 0)
      {
        mana_result = charge_mana(player, 0, -1);
      }
      else
      {
        mana_result = charge_mana(player, 0, g_loaded_recorded_action_code);
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = mana_result;
      g_max_x_value = old_max_x_value;
      if (g_spell_fizzled == 1)
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
      zero = 0;
      mana_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
      special_counters = C_get_special_counters(g_card_on_stack_controller, g_card_on_stack);
      mana_result = ClampIntToRange(mana_result + special_counters, zero, amount);
      set_special_counters(g_card_on_stack_controller, g_card_on_stack, mana_result);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041cc95
// FUNCTION: SHANDALAR 0x0051f4af
int card_colossus_of_sardia(int player, int card, event_t event)
{
  return artifact_pay_to_untap_during_upkeep(player, card, event, 9);
}

// FUNCTION: MAGIC 0x0041ce96
// FUNCTION: SHANDALAR 0x0051f6af
int card_flying_carpet(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int legacy_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2) &&
           !is_animated_and_sick(player, card) &&
           (instance->state & STATE_TAPPED) == 0 &&
           real_target_available((int *)0,
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

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      if (!C_real_select_target(player,
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
                                0,
                                "Select target creature.",
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
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         unk_00896534,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = KEYWORD_FLYING;
      }
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041d4f9
// FUNCTION: SHANDALAR 0x0051fd11
int card_helm_of_chatzuk(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_SHOULD_AI_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += (g_duel_summary.artifact_counts[player] * 0xc) / 2;
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
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
      if (g_spell_fizzled != 1)
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
                                  g_text_lines[0],
                                  1,
                                  &selected_target))
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0] = selected_target;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (g_other_player == player && (((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status & 0x40) != 0) || selected_target.player == g_active_player))
          {
            g_ai_modifier -= 0x30;
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
        g_spell_fizzled = 1;
      }
      else
      {
        int legacy_card;

        legacy_card =
            create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, unk_00896534, selected_target.player, selected_target.card);
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
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3) &&
           g_duel_summary.hand_counts[player] > 0 &&
           real_target_available((int *)0,
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

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      discard(player, 1, 0);
      if (!C_real_select_target(player,
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
                                0,
                                "Select target creature.",
                                1,
                                &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = target;
        instance->number_of_targets = 1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
      g_spell_fizzled = 1;
    }
    else
    {
      artifact_give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack, target.player, target.card, 2, 2);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041df71
// FUNCTION: SHANDALAR 0x00520787
int card_tawnos_s_wand(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2) &&
           !is_animated_and_sick(player, card) &&
           (instance->state & STATE_TAPPED) == 0 &&
           real_target_available((int *)0,
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
                                 TARGET_PT_LESSER_OR_EQUAL | 2,
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
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      if (!C_real_select_target(player,
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
                                TARGET_PT_LESSER_OR_EQUAL | 2,
                                -1,
                                0,
                                0,
                                0,
                                "Select target creature with power 2 or less.",
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
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                TARGET_PT_LESSER_OR_EQUAL | 2,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, unk_008b3d10, target.player, target.card);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
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
      if (g_other_player == player && (g_duel_network_flags & 2) == 0 && unk_00939330 > 0)
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
      if (g_spell_fizzled != 1)
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
    int tmp = find_internal_card_id_by_csv_id(0x375);
    bee_card = add_card_to_hand(player, tmp);
    if (bee_card != -1)
    {
      process_card_enters_play(player, bee_card);
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
  int djinn_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1);
    if (g_spell_fizzled != 1)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (internal_rand(2) != 0)
    {
      djinn_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_DJINN));
      if (djinn_card != -1)
      {
        process_card_enters_play(player, djinn_card);
        PLAYER_CARD_INSTANCE(player, djinn_card).token_status |= STATUS_TOKEN;
      }
    }
    else
    {
      damage_player(player, 5, g_card_on_stack_controller, g_card_on_stack);
    }
  }

  return 0;
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
      instance->state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (player == g_current_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1 && g_duel_network_state == 0)
  {
    target_player = instance->targets[0].player;
    count = 0;

    if (player == g_current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
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

    show_deck(player, internal_card_ids, count, (char (*)[300])0x89684c, 0, gs_cancel_008a8c20);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041edf5
// FUNCTION: SHANDALAR 0x0052160a
int card_library_of_leng(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    unk_007a79b0[player] |= 2;
  }

  if (event == 0x1f && g_current_player == player && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    ++g_event_result;
  }

  if (event == 0x77 && card == g_affected_card && player == g_affected_card_controller)
  {
    unk_007a79b0[player] &= ~2;
  }

  if (g_trigger_condition == 0xd4 && card == g_affected_card && player == g_affected_card_controller && player == g_trigger_cause_controller && card == g_trigger_cause && player == g_current_turn)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
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
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, COLOR_COLORLESS, 3) == 0 ||
        (((instance->untap_status & 3) != 0 &&
          (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0) ||
         (instance->state & STATE_TAPPED) != 0))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 3);
    if (g_spell_fizzled != 1)
    {
      instance->state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    resolve_pandoras_box(player, card);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x005218f3
int resolve_pandoras_box(int player, int card)
{
  int current_player;
  int current_card;
  int chosen_internal_card_id;
  int random_index;
  int created_card;
  int candidates[1000];
  int candidate_count;

  (void)card;

  current_player = player;
  candidate_count = 0;
  while (current_player >= 0 && current_player < 2)
  {
    current_card = 0;
    while (current_card < 500)
    {
      candidates[candidate_count] = global_library[current_player][current_card];
      if (global_library[current_player][current_card] == -1)
      {
        break;
      }
      if ((global_cards_data[global_library[current_player][current_card]].type &
           (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_CREATURE)
      {
        ++candidate_count;
      }
      ++current_card;
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

  if ((g_duel_network_flags & 2) == 0)
  {
    random_index = internal_rand(candidate_count);
  }
  else
  {
    random_index = network_random(g_card_on_stack_controller, candidate_count);
  }
  chosen_internal_card_id = candidates[random_index];

  if (chosen_internal_card_id == -1)
  {
    g_spell_fizzled = 1;
  }
  else
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "PANDORAS_BOX");
    }
    if (coin_flip(player, g_text_lines[player], 1) == 0)
    {
      created_card = add_card_to_hand(player, chosen_internal_card_id);
      if (created_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, created_card).token_status |= STATUS_TOKEN;
        process_card_enters_play(player, created_card);
      }
    }

    if (g_duel_ai_mode_state != 1)
    {
      load_text("promptsX2.txt", "PANDORAS_BOX");
    }
    if (coin_flip(player, g_text_lines[1 - player], 1) == 0)
    {
      created_card = add_card_to_hand(1 - player, chosen_internal_card_id);
      if (created_card != -1)
      {
        PLAYER_CARD_INSTANCE(1 - player, created_card).token_status |= STATUS_TOKEN;
        process_card_enters_play(1 - player, created_card);
      }
    }

    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_PANDORA);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041f3d3
// FUNCTION: SHANDALAR 0x00521be8
int card_sunglasses_of_urza(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    declare_mana_color_conversion(player, get_sleighted_color(player, card, 5), get_sleighted_color(player, card, 4));
  }

  return 0;
}

// FUNCTION: MOK 0x00427740
// FUNCTION: MAGIC 0x0041f4c0
// FUNCTION: SHANDALAR 0x00521cd0
int helper_mox(int player, int card, event_t event, int color)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) ? 1 : 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    g_ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, color, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = color;
  }

  if ((event == EVENT_COUNT_MANA) && (card == g_card_on_stack) && (player == g_card_on_stack_controller) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    declare_mana_available(player, color, 1);
  }

  return 0;
}
