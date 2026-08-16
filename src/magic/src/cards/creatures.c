#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int get_card_display_pic_num(int card_id, int player, int card);
int check_duel_finished(void);
int card_sea_serpent(int player, int card, event_t event);

static __inline int give_pump_until_eot(int player, int card, int target_player, int target_card, int power, int toughness)
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

static __inline int poison_player_on_combat_damage(int player, int card, event_t event, int poison_counters)
{
  card_instance_t *damage;
  int poisoned_player;

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_card == -1 &&
        damage->damage_source_player == player &&
        damage->damage_source_card == card &&
        damage->info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)damage->damage_target_player;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = poison_counters;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      poisoned_player = (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
      (&DAT_007abce0)[poisoned_player] += PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      check_duel_finished();
    }
  }

  return 0;
}

static __inline int create_no_attack_legacy(int player, int card, int target_player, int target_card)
{
  int legacy_card;

  legacy_card = create_legacy_effect(player, card, g_damage_card_internal_card_id + 9, target_player, target_card);
  if (legacy_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0;
  }

  return legacy_card;
}

static __inline int random_magic_color(void)
{
  return internal_rand(5) + 1;
}

static __inline void set_card_to_random_color(int player, int card)
{
  int color;

  color = random_magic_color();
  PLAYER_CARD_INSTANCE(player, card).color = (char)(1 << (unsigned char)color);
}

static __inline int is_blue_land(int player, int card)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  return internal_card_id != -1 &&
         (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 &&
         ((unsigned char)global_cards_data[internal_card_id].color & (1 << COLOR_BLUE)) != 0;
}

static __inline int sacrifice_blue_land(int player)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) != 0 && is_blue_land(player, current_card) != 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(player, current_card, KILL_SACRIFICE);
      return 1;
    }
  }

  return 0;
}

static __inline int return_permanent_to_hand(int target_player, int target_card)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
  if ((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & STATUS_TOKEN) == 0)
  {
    add_card_to_hand(target_player, internal_card_id);
  }
  kill_card(target_player, target_card, KILL_REMOVE);

  return 0;
}

static __inline int permanent_is_enchanted(int target_player, int target_card)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
          (int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player &&
          PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
      {
        return 1;
      }
    }
  }

  return 0;
}

static __inline int hand_has_creature_card(int target_player)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_IN_PLAY) == 0 &&
        (global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

static __inline void discard_random_creature_card_from_hand(int target_player)
{
  int candidates[500];
  int candidate_count;
  int current_card;

  candidate_count = 0;
  for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_IN_PLAY) == 0 &&
        (global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      candidates[candidate_count] = current_card;
      ++candidate_count;
    }
  }

  if (candidate_count != 0)
  {
    discard_card_from_hand(target_player, candidates[internal_rand(candidate_count)]);
  }
}

static __inline void transfer_random_card_from_hand(int from_player, int to_player)
{
  int candidates[500];
  int candidate_count;
  int current_card;
  int selected_card;
  int internal_card_id;

  candidate_count = 0;
  for (current_card = 0; current_card < g_active_cards_count[from_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(from_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(from_player, current_card).state & STATE_IN_PLAY) == 0)
    {
      candidates[candidate_count] = current_card;
      ++candidate_count;
    }
  }

  if (candidate_count != 0)
  {
    selected_card = candidates[internal_rand(candidate_count)];
    internal_card_id = PLAYER_CARD_INSTANCE(from_player, selected_card).internal_card_id;
    add_card_to_hand(to_player, internal_card_id);
    discard_card_from_hand(from_player, selected_card);
  }
}

// FUNCTION: MAGIC 0x0053aa50
// FUNCTION: SHANDALAR 0x00488ca0
int card_dummy(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa62
// FUNCTION: SHANDALAR 0x00488cb2
int card_data_card(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa74
// FUNCTION: SHANDALAR 0x00488cc4
unsigned int get_protections_from(int player, int card)
{
  struct
  {
    unsigned int type;
    unsigned int illegal_abilities;
  } s;

  s.illegal_abilities = 0;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 ||
      PLAYER_CARD_INSTANCE(player, card).internal_card_id == g_stack_proxy_internal_card_id)
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type;
  }
  else
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
  }

  if ((s.type & TYPE_SORCERY) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_SORCERIES;
  }
  if ((s.type & TYPE_INSTANT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INSTANTS;
  }
  if ((s.type & TYPE_INTERRUPT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INTERRUPTS;
  }
  if ((s.type & TYPE_ENCHANTMENT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ENCHANTMENTS;
  }
  if ((s.type & TYPE_ARTIFACT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ARTIFACTS;
  }

  return (0x800 << (unsigned char)(single_color_test_bit_to_color_t(
                                       PLAYER_CARD_INSTANCE(player, card).color) -
                                   1)) |
         s.illegal_abilities;
}

// FUNCTION: MAGIC 0x0053ac18
// FUNCTION: SHANDALAR 0x00488e68
int card_prismatic_dragon(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_FLYING;
    return 0;
  }

  if ((g_trigger_condition == TRIGGER_UPKEEP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      set_card_to_random_color(player, card);
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_COLORLESS, 2);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    set_card_to_random_color(player, card);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053b155
// FUNCTION: SHANDALAR 0x004893a9
int card_rainbow_knights(int player, int card, event_t event)
{
  int selected_ability;
  int pump_amount;

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = random_magic_color();
    return 0;
  }

  if (event == EVENT_ABILITIES &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) != 0)
    {
      g_event_result |= KEYWORD_PROT_BLACK << ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) - 1);
    }
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0x100) != 0)
    {
      g_event_result |= KEYWORD_FIRST_STRIKE;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_COLORLESS, 1) != 0 ||
           has_mana(player, COLOR_WHITE, 2) != 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    selected_ability = 0;
    if (has_mana(player, COLOR_COLORLESS, 1) != 0 &&
        has_mana(player, COLOR_WHITE, 2) != 0)
    {
      if (g_duel_ai_mode_state == 1)
      {
        selected_ability = 1;
      }
      else
      {
        selected_ability = do_dialog(player, player, card, -1, -1, " First strike\n Random power", 0);
      }
    }
    else if (has_mana(player, COLOR_WHITE, 2) != 0)
    {
      selected_ability = 1;
    }

    if (selected_ability == 0)
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = 1;
    }
    else
    {
      charge_mana(player, COLOR_WHITE, 2);
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = 2;
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = 0;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).damage_source_card == 1)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot |= 0x100;
      }
      else if (PLAYER_CARD_INSTANCE(player, card).damage_source_card == 2)
      {
        pump_amount = internal_rand(3);
        if (pump_amount != 0)
        {
          give_pump_until_eot(g_card_on_stack_controller,
                              g_card_on_stack,
                              PLAYER_CARD_INSTANCE(player, card).parent_controller,
                              PLAYER_CARD_INSTANCE(player, card).parent_card,
                              pump_amount,
                              0);
        }
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~0x100;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053c256
// FUNCTION: SHANDALAR 0x0048a4aa
int card_sindbad(int player, int card, event_t event)
{
  int drawn_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           global_library[player][0] != -1;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
        global_library[player][0] != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (global_library[player][0] != -1)
    {
      drawn_card = global_library[player][0];
      draw_card_for_player(player);
      if ((global_cards_data[drawn_card].type & TYPE_LAND) == 0 &&
          g_duel_summary.hand_counts[player] > 0)
      {
        discard_card_from_hand(player, g_duel_summary.hand_counts[player] - 1);
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053c406
// FUNCTION: SHANDALAR 0x0048a65a
int card_tempest_efreet(int player, int card, event_t event)
{
  int opponent;
  int opponent_pays;

  opponent = 1 - player;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = opponent;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = -1;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    opponent = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    opponent_pays = 0;
    if (g_life[opponent] >= 10)
    {
      if (g_duel_ai_mode_state == 1)
      {
        opponent_pays = g_duel_summary.hand_counts[opponent] != 0;
      }
      else
      {
        opponent_pays = do_dialog(opponent, player, card, -1, -1, " Pay 10 life\n Exchange card", 0) == 0;
      }
    }
    if (opponent_pays != 0)
    {
      g_life[opponent] -= 10;
    }
    else
    {
      transfer_random_card_from_hand(opponent, player);
    }
    kill_card(player, card, KILL_SACRIFICE);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053cbde
// FUNCTION: SHANDALAR 0x0048ae32
int card_xenic_poltergeist(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int animated_internal_card_id;

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
                                 TYPE_ARTIFACT,
                                 TYPE_CREATURE,
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
                                 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((instance->state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "XENIC_POLTERGEIST");
      }
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
                               TYPE_CREATURE,
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
                               &target) == 0)
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
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
                               TYPE_CREATURE,
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
                               0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)target.player;
      instance->damage_target_card = target.card;
      instance->eot_toughness = PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id;
      animated_internal_card_id = create_a_card_type(instance->eot_toughness);
      if (animated_internal_card_id != -1)
      {
        global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
        global_cards_data[animated_internal_card_id].power =
            global_cards_data[instance->eot_toughness].cc[1];
        global_cards_data[animated_internal_card_id].toughness =
            global_cards_data[instance->eot_toughness].cc[1];
        instance->dummy3 = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(target.player, target.card).regen_status |= 0x1000000;
      }
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CHANGE_TYPE &&
      instance->damage_target_card == g_affected_card &&
      (int)instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      instance->dummy3 != 0 &&
      is_in_play((int)instance->damage_target_player, instance->damage_target_card) != 0)
  {
    g_event_result = instance->dummy3;
    PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card).token_status |= 0x40;
    return 0;
  }

  if (event == EVENT_CLEANUP && instance->dummy3 != 0)
  {
    if (instance->damage_target_card != -1 &&
        instance->damage_target_player != -1 &&
        is_in_play((int)instance->damage_target_player, instance->damage_target_card) != 0 &&
        PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card).internal_card_id == instance->dummy3)
    {
      PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card).internal_card_id =
          instance->eot_toughness;
    }
    invalidate_dynamic_card_type(instance->dummy3);
    instance->dummy3 = 0;
    instance->damage_target_player = -1;
    instance->damage_target_card = -1;
    instance->eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d06d
// FUNCTION: SHANDALAR 0x0048b2c1
int card_uncle_istvan(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_PREVENT_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_player == player &&
        damage->damage_target_card == card &&
        damage->info_slot != 0 &&
        (damage->eot_toughness & TYPE_CREATURE) != 0)
    {
      damage->unknown0x37 = (char)damage->info_slot;
      damage->info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d355
// FUNCTION: SHANDALAR 0x0048b5a9
int card_ironclaw_orcs(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && g_affected_card == card && g_affected_card_controller == player)
  {
    if (C_get_abilities(g_attacking_card_controller, g_attacking_card, EVENT_POWER, card) > 1)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d3bc
// FUNCTION: SHANDALAR 0x0048b610
int card_amrou_kithkin(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && card == g_attacking_card && player == g_attacking_card_controller)
  {
    if (C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_POWER, -1) > 2)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d41f
// FUNCTION: SHANDALAR 0x0048b673
int card_elven_riders(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && card == g_attacking_card && player == g_attacking_card_controller)
  {
    if (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype != 0 &&
        (C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) == 0)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d4c9
// FUNCTION: SHANDALAR 0x0048b71c
int card_hypnotic_specter(int player, int card, event_t event)
{
  if ((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id))
  {
    if ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }

    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_DEAL_DAMAGE) && (g_affected_card == card)) && (g_affected_card_controller == player) && (PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1)) && (player == g_current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      discard((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, 1, 0);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d778
// FUNCTION: SHANDALAR 0x0048b9cb
int card_marsh_viper(int player, int card, event_t event)
{
  return poison_player_on_combat_damage(player, card, event, 2);
}

// FUNCTION: MAGIC 0x0053da26
// FUNCTION: SHANDALAR 0x0048bc78
int card_pit_scorpion(int player, int card, event_t event)
{
  return poison_player_on_combat_damage(player, card, event, 1);
}

// FUNCTION: MAGIC 0x0053dcd4
// FUNCTION: SHANDALAR 0x0048bf26
int card_nafs_asp(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_card == -1 &&
        damage->damage_source_player == player &&
        damage->damage_source_card == card &&
        damage->info_slot != 0)
    {
      create_legacy_effect(player,
                           card,
                           g_damage_card_internal_card_id + 17,
                           (int)damage->damage_target_player,
                           -1);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053dfb5
// FUNCTION: SHANDALAR 0x0048c204
int create_damage_effect_copy_for_target_player(int player, int card)
{
  struct
  {
    int target_player;
    int effect_card;
  } s;

  s.target_player = (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  s.effect_card = add_card_to_hand(s.target_player, g_damage_effect_internal_card_id);
  if (s.effect_card != -1)
  {
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).state |= STATE_IN_PLAY;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).token_status |= STATUS_OBLITERATED;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).info_slot = 1;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).color = PLAYER_CARD_INSTANCE(player, card).color;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).display_pic_csv_id =
        (get_card_display_pic_num(0x1bc, player, card) << 16) | 0x1bc;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_source_card = card;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_target_player = (char)s.target_player;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_target_card = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053e216
// FUNCTION: SHANDALAR 0x0048c465
int card_giant_tortoise(int player, int card, event_t event)
{
  if (event == EVENT_TOUGHNESS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_BLOCKING) != 0)
  {
    g_event_result += 3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053e28a
// FUNCTION: SHANDALAR 0x0048c4db
int card_personal_incarnation(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *selected;
  target_t target;
  int damage_source_player;
  int max_damage;
  int damage_to_deal;
  int damage_card;
  int replacement_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == 0x21)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)
    {
      instance->info_slot |= 1;
    }
    return 0;
  }

  if (event == 0x73)
  {
    if ((instance->info_slot & 1) != 0 && (g_land_can_be_played & 4) != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->info_slot & 1) != 0 && (g_land_can_be_played & 4) != 0)
  {
    damage_source_player = (instance->state & 0x1000) != 0;
    do
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "PERSONAL_INCARNATION");
      }

      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id, ~SUB_WALL,
                                -1, -1, 0, 0, 0, g_text_lines[0], 1, &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        selected = &PLAYER_CARD_INSTANCE(target.player, target.card);
        if (selected->damage_target_player == player && selected->damage_target_card == card)
        {
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "PERSONAL_INCARNATION");
          }
          if (selected->info_slot + instance->toughness < 6)
          {
            max_damage = 0;
          }
          else
          {
            max_damage = selected->info_slot - (5 - instance->toughness);
          }
          damage_to_deal = choose_a_number(player, g_text_lines[1], max_damage);
          if (damage_to_deal > selected->info_slot)
          {
            damage_to_deal = selected->info_slot;
          }
          damage_card = damage_creature(damage_source_player, -1, damage_to_deal,
                                        selected->damage_source_player, selected->damage_source_card);
          if (damage_card != -1)
          {
            *(int *)((char *)&PLAYER_CARD_INSTANCE(player, damage_card) + 0x60) =
                *(int *)((char *)selected + 0x60);
            instance->info_slot &= ~1;
            selected->info_slot -= damage_to_deal;
          }
        }
      }
    } while (!g_spell_fizzled && (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player != player || PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card != card));
  }

  if (event == EVENT_CLEANUP)
  {
    instance->info_slot &= ~1;
    return 0;
  }

  if (event == 0x77 && g_affected_card == card && g_affected_card_controller == player && instance->kill_code != 4 && is_in_play(player, card))
  {
    replacement_card = add_card_to_hand(player, unk_00939348);
    if (replacement_card != -1)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, replacement_card) + 0x38) = instance->internal_card_id;
      PLAYER_CARD_INSTANCE(player, replacement_card).state |= ((instance->state >> 8) & 0x10) | 2;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, replacement_card) + 0x60) = 0xb5;
      TENTATIVE_set_timestamps(player, replacement_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053ea3b
// FUNCTION: SHANDALAR 0x0048cc8c
int card_ali_from_cairo(int player, int card, event_t event)
{
  card_instance_t *damage;
  int damage_to_prevent;

  if (event == EVENT_PREVENT_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_player == player &&
        damage->damage_target_card == -1 &&
        damage->info_slot >= g_life[player] &&
        g_life[player] > 0)
    {
      damage_to_prevent = damage->info_slot - g_life[player] + 1;
      damage->unknown0x37 = (char)damage_to_prevent;
      damage->info_slot -= damage_to_prevent;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053f22d
// FUNCTION: SHANDALAR 0x0048d477
int card_fungusaur(int player, int card, event_t event)
{
  if (event == EVENT_UNKNOWN80)
  {
    if (((((g_land_can_be_played & 0x200) != 0) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player)) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }

    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_DEAL_DAMAGE) && (card == g_affected_card)) && (player == g_affected_card_controller)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }
  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && ((card == g_affected_card) && ((player == g_affected_card_controller) && (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)))) && (player == g_current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if ((event == EVENT_RESOLVE_TRIGGER) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).counter_power++;

      PLAYER_CARD_INSTANCE(player, card).counter_toughness++;

      add_special_counters(player, card, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053f521
// FUNCTION: SHANDALAR 0x0048d76b
int card_carrion_ants(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_COLORLESS];
    return 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_COLORLESS, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_COLORLESS, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_COLORLESS, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_COLORLESS, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_COLORLESS] != 0)
  {
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053fe1c
// FUNCTION: SHANDALAR 0x0048e066
int card_shivan_dragon(int player, int card, event_t event)
{
  int legacy;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    return 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      if (g_current_player == player)
      {
        charge_mana(player, 4, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        legacy = create_legacy_effect(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      g_card_on_stack_controller,
                                      g_card_on_stack);
        if (legacy != -1)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy).info_slot |= 0x80000;
        }
      }
    }

    return 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_RED] != 0)
  {
    g_event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (g_other_player == player)
      {
        g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
      }
      else
      {
        g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
      }
    }

    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054060c
// FUNCTION: SHANDALAR 0x0048e853
int card_dragon_whelp(int player, int card, event_t event)
{
  int result;
  int saved_max_x_value;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    result = 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
    saved_max_x_value = g_max_x_value;
    if (result != 0)
    {
      g_x_value = 0;
      if (player == g_current_player)
      {
        if ((((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) == 0x30000)) || unk_00715fb0 != 1)
        {
          g_max_x_value = -1;
        }
        else
        {
          g_max_x_value = 3 - ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) >> 16);
        }
        charge_mana(player, COLOR_RED, -1);
        g_max_x_value = saved_max_x_value;
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_RED, 1);
        g_x_value = 1;
      }

      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).eot_toughness &= 0xff0000;
      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += g_x_value * 0x10001;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        result = create_legacy_effect(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      g_card_on_stack_controller,
                                      g_card_on_stack);
        if (result != -1)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, result).info_slot |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    result = has_mana(player, COLOR_RED, 1);
    result = ClampIntToRange(result, 0, 3);
    result -= PLAYER_CARD_INSTANCE(player, card).info_slot;
  }
  else
  {
    if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && (g_affected_card == card) && (g_affected_card_controller == player) && ((char)PLAYER_CARD_INSTANCE(player, card).info_slot != 0)) && player == g_current_turn)
    {
      if (PLAYER_CARD_INSTANCE(player, card).info_slot < 4)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
      else
      {
        if (event == EVENT_TRIGGER)
        {
          g_event_result |= 2;
        }
        if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
        {
          kill_card(player, card, KILL_DESTROY);
        }
      }
    }
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == g_other_player)
      {
        g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_RED] * 0xc;
      }
      else
      {
        g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_RED] * -0xc;
      }
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00540fc8
// FUNCTION: SHANDALAR 0x0048f20f
int card_goblin_balloon_brigade(int player, int card, event_t event)
{
  int legacy_card;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x20;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot == 0) || (has_mana(player, COLOR_RED, 1) == 0))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      charge_mana(player, COLOR_RED, 1);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((g_other_player == player) && ((g_duel_network_flags & 2) == 0))
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot = 0x20;
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         unk_00896534,
                                         g_card_on_stack_controller,
                                         g_card_on_stack);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541405
// FUNCTION: SHANDALAR 0x0048f64a
int card_whirling_dervish(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_BLACK) - 1);
    return 0;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_card == -1 &&
        damage->damage_source_player == player &&
        damage->damage_source_card == card &&
        damage->damage_target_player == 1 - player &&
        damage->info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) &&
       g_affected_card == card &&
       g_affected_card_controller == player &&
       PLAYER_CARD_INSTANCE(player, card).info_slot != 0) &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      ++PLAYER_CARD_INSTANCE(player, card).counter_power;
      ++PLAYER_CARD_INSTANCE(player, card).counter_toughness;
      add_special_counters(player, card, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541740
// FUNCTION: SHANDALAR 0x0048f983
int card_nightmare(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  int legacy_effect_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_effect_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot = 1;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status = 0x10020;
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status;
      *state_ptr |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_effect_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005418b4
// FUNCTION: SHANDALAR 0x0048faf7
int card_angry_mob(int player, int card, event_t event)
{
  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result += g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLACK)];
  }

  if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    unk_00925d3c += g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLACK)] * 0xc;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541b88
// FUNCTION: SHANDALAR 0x0048fdcb
int card_gaea_s_liege(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_RESOLVE_SPELL)
  {
    effect_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 3;
      PLAYER_CARD_INSTANCE(player, effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x2000000;
      instance->damage_source_player = (char)player;
      instance->damage_source_card = effect_card;
    }
  }
  else if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((instance->state & STATE_ATTACKING) == 0)
    {
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness |= 1;
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness &= 0xfffffffd;
    }
    else
    {
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness |= 2;
      PLAYER_CARD_INSTANCE((int)instance->damage_source_player, instance->damage_source_card)
          .eot_toughness &= 0xfffffffe;
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
                                                                  TARGET_SCAN_DIRECT,
                                                                  player,
                                                                  2,
                                                                  2,
                                                                  TARGET_ZONE_IN_PLAY,
                                                                  TYPE_LAND,
                                                                  TYPE_NONE,
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
                                                                  0) != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "GAEAS_LIEGE");
    }

    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_LAND,
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
      instance->targets[0].player = selected_target.player;
      instance->targets[0].card = selected_target.card;
      instance->number_of_targets = 1;
      instance->state |= 0x10;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = instance->targets[0].player;
    selected_target.card = instance->targets[0].card;

    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
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
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 3;
      effect_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         unk_008b49c4,
                                         selected_target.player,
                                         selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(g_card_on_stack_controller, effect_card).token_status |= 0x11020;
      }
    }

    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }
  else if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) && card == g_affected_card && player == g_affected_card_controller && (instance->info_slot & 0x4000) != 0)
  {
    instance->info_slot &= 0xffffbfff;
    C_get_abilities(player, card, EVENT_POWER, -1);
    C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005424a4
// FUNCTION: SHANDALAR 0x004906e7
int card_plague_rats(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x20f;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = STATUS_INVISIBLE_FX | STATUS_PERMANENT;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (unsigned char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542610
// FUNCTION: SHANDALAR 0x00490853
int card_keldon_warlord(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x80d;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542720
// FUNCTION: SHANDALAR 0x00490963
int card_ghost_ship(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 2, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054276d
// FUNCTION: SHANDALAR 0x004909b0
int generic_regeneration_ability(int player, int card, event_t event, unsigned int color, int amount)
{
  int can_activate;

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & 0x200) != 0 && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    can_activate = 1;
    if (PLAYER_CARD_INSTANCE(player, card).kill_code != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x800002) != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).regen_status & 0x200) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && has_mana(player, color, amount) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0)
    {
      return 99;
    }
    return 0;
  }
  else if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    charge_mana(player, color, amount);
    if (g_spell_fizzled != 1)
    {
      unk_007a7c1c = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot += 1;
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & 0x200) != 0)
  {
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).info_slot = 0;
    regenerate_card(g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542a2a
// FUNCTION: SHANDALAR 0x00490c6d
void regenerate_card(int player, int card)
{
  struct
  {
    int found_linked_card; /* [ebp-0xc] */
    int test_player;       /* [ebp-0x8] */
    int test_card;         /* [ebp-0x4] */
  } s;

  s.test_player = 0;
  s.found_linked_card = 0;

  while (s.test_player < 2 && s.found_linked_card == 0)
  {
    s.test_card = 0;
    while (s.test_card < g_active_cards_count[s.test_player] && s.found_linked_card == 0)
    {
      if (is_in_play(s.test_player, s.test_card) != 0 &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card &&
          ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_008b49c4 &&
            (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).token_status & 0x00800000) != 0) ||
           PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_008b3d10))
      {
        s.found_linked_card = 1;
      }

      ++s.test_card;
    }

    ++s.test_player;
  }

  if (s.found_linked_card == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
    tap_card_and_dispatch_event(player, card);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_REGEN);
    }
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~0x80u;
    PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
    PLAYER_CARD_INSTANCE(player, card).state &= ~0x0c;
  }
}

// FUNCTION: MAGIC 0x00542d29
// FUNCTION: SHANDALAR 0x00490f6c
int card_uthden_troll(int player, int card, event_t event)
{
  unsigned int color;

  color = single_color_test_bit_to_color_t(
      *(char *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color);
  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][color] += 2;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542de2
// FUNCTION: SHANDALAR 0x00491025
int card_vampire_bats(int player, int card, event_t event)
{
  int legacy_card;
  int saved_max_x_value;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLACK];
    return 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) < 2 &&
           has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) < 2 &&
        has_mana(player, COLOR_BLACK, 1))
    {
      if (player == g_current_player)
      {
        saved_max_x_value = g_max_x_value;
        g_max_x_value = 2 - (PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff);
        charge_mana(player, COLOR_BLACK, -1);
        g_max_x_value = saved_max_x_value;
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return (2 - (PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff)) *
           (has_mana(player, COLOR_BLACK, 1) != 0);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_BLACK] != 0)
  {
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543640
// FUNCTION: SHANDALAR 0x00491883
int card_frozen_shade(int player, int card, event_t event)
{
  union
  {
    int legacy_effect_card;
    card_instance_t *instance;
  } u;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    u.instance = &PLAYER_CARD_INSTANCE(player, card);
    u.instance->eot_toughness = 0;
    u.instance->info_slot = u.instance->eot_toughness;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLACK];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLACK, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_BLACK, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        u.legacy_effect_card = create_legacy_effect(g_card_on_stack_controller,
                                                    g_card_on_stack,
                                                    LEGACY_EFFECT_PUMP,
                                                    g_card_on_stack_controller,
                                                    g_card_on_stack);
        if (u.legacy_effect_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).counter_toughness = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, u.legacy_effect_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && (g_raw_mana_available[player][COLOR_BLACK] != 0))
  {
    g_event_result |= 1;
    return 0;
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    u.instance = &PLAYER_CARD_INSTANCE(player, card);
    u.instance->eot_toughness = 0;
    u.instance->info_slot = u.instance->eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543f11
// FUNCTION: SHANDALAR 0x00492155
int card_killer_bees(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_GREEN];
    return 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_GREEN, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_GREEN, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_GREEN, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_GREEN] != 0)
  {
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005447da
// FUNCTION: SHANDALAR 0x00492a1e
int card_wall_of_water(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLUE];
    return 0;
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
        return 0;
      }
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLUE, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_BLUE, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLUE, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .info_slot &
            0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_BLUE] != 0)
  {
    g_event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == g_other_player)
      {
        g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
      else
      {
        g_ai_modifier -= g_basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
    }

    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00544ff5
// FUNCTION: SHANDALAR 0x00493239
int card_rag_man(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][COLOR_BLACK] += 3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_BLACK, 3) &&
           hand_has_creature_card(1 - player) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 1 - player,
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
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_BLACK, 3);
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
                                "Select target opponent.",
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
    discard_random_creature_card_from_hand(instance->targets[0].player);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005455e2
// FUNCTION: SHANDALAR 0x00493829
int card_time_elemental(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_BLUE, 2) != 0 &&
           has_mana(player, COLOR_ANY, 2) != 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
                                 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
        has_mana(player, COLOR_BLUE, 2) != 0 &&
        has_mana(player, COLOR_ANY, 2) != 0)
    {
      charge_mana(player, COLOR_BLUE, 2);
      if (g_spell_fizzled != 1)
      {
        charge_mana(player, COLOR_COLORLESS, 2);
      }
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "TIME_ELEMENTAL");
        }
        if (C_real_select_target(player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
                                 &target) == 0)
        {
          g_spell_fizzled = 1;
        }
        else if (permanent_is_enchanted(target.player, target.card) != 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
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
    target = PLAYER_CARD_INSTANCE(player, card).targets[0];
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
                               0) == 0 ||
        permanent_is_enchanted(target.player, target.card) != 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      return_permanent_to_hand(target.player, target.card);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_DECLARE_ATTACKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    return 0;
  }

  if (event == EVENT_DECLARE_BLOCKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player != g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).blocking != -1)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    return 0;
  }

  if ((g_trigger_condition == TRIGGER_EOT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      damage_player(player, 5, player, card);
      kill_card(player, card, KILL_SACRIFICE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00545c2e
// FUNCTION: SHANDALAR 0x00493e75
int card_northern_paladin(int player, int card, event_t event)
{
  target_t target;
  int result;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_WHITE];
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) != 0)
    {
      result = 0;
    }
    if (result != 0 && has_mana(player, COLOR_WHITE, 2) == 0)
    {
      result = 0;
    }
    if (result != 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE,
                                     0,
                                     get_protections_from(player, card),
                                     1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                                     0,
                                     -1,
                                     -1,
                                     0xffffffff,
                                     0xffffffff,
                                     0,
                                     0,
                                     0);
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_WHITE, 2);
    if (result != 0 && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      charge_mana(player, COLOR_WHITE, 2);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "NORTHERN_PALADIN");
        }

        if (C_real_select_target(player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
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
                                 &target) == 0)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x005460cb
// FUNCTION: SHANDALAR 0x00494312
int card_royal_assassin(int player, int card, event_t event)
{
  int result;
  target_t selected_target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     0x200,
                                     2,
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
                                     1,
                                     0);
    }
    return result;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ROYAL_ASSASSIN");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             0,
                             0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             TARGET_STATE_TAPPED,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
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
                               0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }
  else if (((event == EVENT_ATTACK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    unk_00925d3c += 0x30;
    return 0;
  }
  else if (((event == EVENT_BLOCK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    unk_00925d3c += -0x30;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005466d0
// FUNCTION: SHANDALAR 0x00494917
int card_osai_vultures(int player, int card, event_t event)
{
  if ((g_trigger_condition == TRIGGER_EOT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_duel_summary.creatures_died != 0 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      add_special_counter(player, card);
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return C_get_special_counters(player, card) >= 2;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    remove_special_counters(player, card, 2);
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      give_pump_until_eot(g_card_on_stack_controller,
                          g_card_on_stack,
                          PLAYER_CARD_INSTANCE(player, card).parent_controller,
                          PLAYER_CARD_INSTANCE(player, card).parent_card,
                          1,
                          1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00546d3b
// FUNCTION: SHANDALAR 0x00494f82
int card_murk_dwellers(int player, int card, event_t event)
{
  if (event == EVENT_DECLARE_BLOCKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0 &&
      PLAYER_CARD_INSTANCE(player, card).blocking == -1)
  {
    give_pump_until_eot(player, card, player, card, 2, 0);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00546eca
// FUNCTION: SHANDALAR 0x00495111
int card_sorceress_queen(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int current_power;
  int current_toughness;

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
      current_power = C_get_abilities(target.player, target.card, EVENT_POWER, -1);
      current_toughness = C_get_abilities(target.player, target.card, EVENT_TOUGHNESS, -1);
      give_pump_until_eot(g_card_on_stack_controller,
                          g_card_on_stack,
                          target.player,
                          target.card,
                          -current_power,
                          2 - current_toughness);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054760a
// FUNCTION: SHANDALAR 0x00495851
int card_stone_giant(int player, int card, event_t event)
{
  unsigned int target_result;
  unsigned int illegal_abilities;
  int legacy_card;
  unsigned char player_byte;
  target_t selected_target;

  player_byte = (unsigned char)player;
  if (event == EVENT_CAN_ACTIVATE)
  {
    legacy_card = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      illegal_abilities = get_protections_from(player, card);
      legacy_card = real_target_available((int *)0,
                                          TARGET_SCAN_DIRECT,
                                          player,
                                          player_byte,
                                          player_byte,
                                          TARGET_ZONE_IN_PLAY,
                                          TYPE_CREATURE,
                                          0,
                                          0,
                                          illegal_abilities,
                                          0,
                                          0,
                                          -1,
                                          -1,
                                          (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | 0x2000,
                                          0xffffffff,
                                          0,
                                          0,
                                          0);
    }
    return legacy_card;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "STONE_GIANT");
    }
    legacy_card = C_real_select_target(player,
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
                                       ~SUB_WALL,
                                       -1,
                                       (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | 0x2000,
                                       0,
                                       0,
                                       0,
                                       g_text_lines[0],
                                       1,
                                       &selected_target);
    if (legacy_card == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((g_other_player == player) && ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status & 0x20) != 0))
      {
        g_ai_modifier += -0x60;
      }
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    target_result = C_real_validate_target(selected_target.player,
                                           selected_target.card,
                                           (char *)0,
                                           player,
                                           player_byte,
                                           player_byte,
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
                                           (int)*(short *)&PLAYER_CARD_INSTANCE(player, card).upkeep_colorless - 1U | TARGET_PT_LESSER_OR_EQUAL,
                                           0,
                                           0,
                                           0);
    if (target_result == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         unk_00896534,
                                         selected_target.player,
                                         selected_target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).mana_color = 5;
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
        PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status = 0x8000000;
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547bb3
// FUNCTION: SHANDALAR 0x00495df9
int card_dwarven_warriors(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
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
                                                                  0x2002,
                                                                  0xffffffff,
                                                                  0,
                                                                  0,
                                                                  0))
    {
      return 1;
    }
  }
  else if (event == 0x90)
  {
    load_recorded_action_target(0);
  }
  else if ((event == EVENT_ACTIVATE) && ((instance->state & 0x20010) == 0))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DWARVEN_WARRIORS");
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
                              0x2002,
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
  else if (event == EVENT_RESOLVE_ACTIVATION)
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
                                0x2002,
                                -1,
                                0,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, unk_007abc7c, target.player, target.card);
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547f99
// FUNCTION: SHANDALAR 0x004961df
int card_cave_people(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int legacy_card;
  int mountainwalk_mask;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  mountainwalk_mask = 1 << (unsigned char)(get_hacked_color(player, card, COLOR_RED) - 1);

  if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    unk_00925d3c -= 0x18;
    return 0;
  }

  if (g_trigger_condition == TRIGGER_ATTACKER_CHOSEN &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card &&
      g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      give_pump_until_eot(player, card, player, card, 1, -2);
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & 0x20010) == 0 &&
           has_mana(player, COLOR_COLORLESS, 1) &&
           has_mana(player, COLOR_RED, 2) &&
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
    if ((instance->state & 0x20010) == 0 &&
        has_mana(player, COLOR_COLORLESS, 1) &&
        has_mana(player, COLOR_RED, 2))
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      if (g_spell_fizzled != 1)
      {
        charge_mana(player, COLOR_RED, 2);
      }
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "CAVE_PEOPLE");
        }
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
                                  g_text_lines[0],
                                  1,
                                  &target))
        {
          g_spell_fizzled = 1;
        }
        else
        {
          instance->state |= STATE_TAPPED;
          instance->targets[0] = target;
          instance->number_of_targets = 1;
        }
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
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = mountainwalk_mask;
      }
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005485e9
// FUNCTION: SHANDALAR 0x0049682f
int card_pradesh_gypsies(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & 0x20010) == 0 &&
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
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PRADESH_GYPSIES");
    }

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
                              g_text_lines[0],
                              1,
                              &target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      instance->targets[0] = target;
      instance->number_of_targets = 1;
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
      give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack, target.player, target.card, -2, 0);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00548bd9
// FUNCTION: SHANDALAR 0x00496e1d
int card_samite_healer(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    int result = 1;
    if ((g_land_can_be_played & 4) == 0)
    {
      result = 0;
    }
    if (PLAYER_CARD_INSTANCE(player, card).state & 0x20010)
    {
      result = 0;
    }
    if (result != 0)
    {
      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                2,
                                2,
                                0x200,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                g_damage_card_internal_card_id,
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
      {
        result = 99;
      }
      else
      {
        result = 0;
      }
    }
    return result;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SAMITE_HEALER");
    }
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
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target.player,
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
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == 0x3b && (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
  {
    ++unk_007a7d88[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549014
// FUNCTION: SHANDALAR 0x00497258
int card_verduran_enchantress(int player, int card, event_t event)
{
  int library_position;

  if ((((((g_trigger_condition == TRIGGER_SPELL_CAST) && (g_affected_card == card)) && (g_affected_card_controller == player)) && ((g_current_turn == player) && (g_current_player == player))) && (((PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0) && ((g_affected_card_controller == player) && (PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1)))) && ((global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id]
                                                                                                                                                                                                                                                                                                                                                                                                .type &
                                                                                                                                                                                                                                                                                                                                                                                            TYPE_ENCHANTMENT) != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      if ((g_active_player == player) || ((g_duel_network_flags & 2) != 0))
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
      else
      {
        library_position = 0;
        while ((library_position < 500) && (global_library[player][library_position] != -1))
        {
          ++library_position;
        }

        if ((g_duel_summary.hand_counts[player] < 8) && (5 < library_position))
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
      draw_card_for_player(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054920c
// FUNCTION: SHANDALAR 0x0049744a
int card_serra_angel(int player, int card, event_t event)
{
  if (g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054926e
// FUNCTION: SHANDALAR 0x004974ab
int card_giant_spider(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        if (g_current_player == player)
        {
          g_event_result &= ~KEYWORD_FLYING;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005492be
// FUNCTION: SHANDALAR 0x004974fb
int card_zombie_master(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_ABILITIES)
  {
    if (is_in_play(player, card))
    {
      if (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card)
                                .internal_card_id]
              .subtype == SUB_ZOMBIE)
      {
        color = get_hacked_color(player, card, COLOR_BLACK);
        g_event_result |= (1 << (((char)color - 1U) & 0x1f)) + KEYWORD_REGENERATION;
      }
    }
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    dispatch_three_arg_callback_to_cards_in_play(mark_zombie_as_nonregenerable, -1);
    TENTATIVE_reassess_all_cards(0, 0xff);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005493a6
int mark_zombie_as_nonregenerable(int player, int card, int internal_card_id)
{
  if (global_cards_data[internal_card_id].subtype == SUB_ZOMBIE)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x8000000;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00549404
// FUNCTION: SHANDALAR 0x00497640
int card_scathe_zombies(int player, int card, event_t event)
{
  if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) || (event == EVENT_RESOLVE_ACTIVATION))
  {
    return generic_regeneration_ability(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549451
// FUNCTION: SHANDALAR 0x0049768d
int card_goblin_king(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 3)
  {
    if (event == EVENT_ABILITIES)
    {
      g_event_result |= 1 << (get_hacked_color(player, card, COLOR_RED) - 1);
    }
    if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549524
// FUNCTION: SHANDALAR 0x00497760
int card_erg_raiders(int player, int card, event_t event)
{
  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) &&
       g_affected_card == card &&
       g_affected_card_controller == player &&
       (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKED) == 0 &&
       (PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_NOATTACK) == 0) &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      damage_player(player, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549726
// FUNCTION: SHANDALAR 0x00497962
int card_el_hajjaj(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    return 0;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_card == -1 &&
        damage->damage_source_player == player &&
        damage->damage_source_card == card &&
        damage->info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness += damage->info_slot;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      gain_life(player, PLAYER_CARD_INSTANCE(player, card).eot_toughness, player, card);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549c89
// FUNCTION: SHANDALAR 0x00497ec5
int card_leviathan(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_current_player == player && player == unk_00742f60)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_basiclandtypes_controlled[player][COLOR_BLUE] < 2 ||
        sacrifice_blue_land(player) == 0 ||
        sacrifice_blue_land(player) == 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
           has_mana(player, COLOR_BLUE, 2) != 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
        has_mana(player, COLOR_BLUE, 2) != 0)
    {
      charge_mana(player, COLOR_BLUE, 2);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
        dispatch_event_to_single_card(player, card, EVENT_UNTAP_PHASE, -1, -1);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_ATTACK_LEGALITY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_basiclandtypes_controlled[player][COLOR_BLUE] < 2)
  {
    g_event_result = 1;
    return 0;
  }

  if (event == EVENT_DECLARE_ATTACKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    if (sacrifice_blue_land(player) == 0 || sacrifice_blue_land(player) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_ATTACKING;
    }
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_BLUE] < 2)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a397
// FUNCTION: SHANDALAR 0x004985d4
int card_brothers_of_fire(int player, int card, event_t event)
{
  unsigned int *state_ptr;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           has_mana(player, COLOR_COLORLESS, 1) &&
           has_mana(player, COLOR_RED, 2);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
        has_mana(player, COLOR_COLORLESS, 1) &&
        has_mana(player, COLOR_RED, 2))
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      if (g_spell_fizzled != 1)
      {
        charge_mana(player, COLOR_RED, 2);
      }
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "PRODIGAL_SORCERER");
        }
        select_damage_target(player, card, 1);
      }
      if (g_spell_fizzled != 1)
      {
        state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
        *state_ptr |= 0x10;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 1) != 0)
    {
      damage_player(player, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a549
// FUNCTION: SHANDALAR 0x00498786
int card_crimson_manticore(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_RED, 1) &&
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
                                 TARGET_STATE_IN_COMBAT,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_RED, 1);
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
                                TARGET_STATE_IN_COMBAT,
                                0,
                                "Select target attacking or blocking creature.",
                                1,
                                &target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = PLAYER_CARD_INSTANCE(player, card).targets[0];
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
                                TARGET_STATE_IN_COMBAT,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a96d
// FUNCTION: SHANDALAR 0x00498baa
int card_prodigal_sorcerer(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PRODIGAL_SORCERER");
    }
    select_damage_target(player, card, 1);
    if (g_spell_fizzled != 1)
    {
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
      *state_ptr |= 0x10;
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else if ((event == EVENT_CHECK_PUMP) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20014) == 0))
  {
    --unk_007a7d88[1 - player];
    result = 0;
  }
  else
  {
    if ((event == EVENT_SHOULD_AI_PLAY) && (player == g_current_player) && (g_other_player == player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
    {
      g_ai_modifier += 0x18;
    }

    if (((event == EVENT_ATTACK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
    {
      unk_00925d3c += 0x30;
      result = 0;
    }
    else if (((event == EVENT_BLOCK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
    {
      unk_00925d3c += -0x30;
      result = 0;
    }
    else
    {
      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054ac4d
// FUNCTION: SHANDALAR 0x00498e8a
int select_damage_target(int player, int card, int damage_unused)
{
  struct
  {
    char *prompt;
    int target_card;
    target_t target;
    int unused;
  } s;

  s.unused = 0;
  if (g_active_player == player || (g_duel_network_flags & 2) != 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      if (!C_real_select_target(player,
                                2,
                                1 - player,
                                TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
                                &s.target) == 0)
      {
        s.target_card = s.target.card;
        unk_00742fcc = s.target.player;
      }
      else
      {
        g_spell_fizzled = 1;
        s.target_card = -1;
        unk_00742fcc = -1;
      }
    }
    else
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
    }
  }
  else
  {
    if (g_duel_ai_mode_state == 1)
    {
      if (internal_rand(3) != 0)
        g_ai_recorded_choice = 0;
      else
        g_ai_recorded_choice = 1;

      record_ai_action_selection();
    }
    else
    {
      replay_ai_action_selection();
    }

    if (g_ai_recorded_choice == 0)
    {
      if (g_duel_ai_mode_state == 1)
        s.prompt = "";
      else
        s.prompt = g_text_lines[0];

      C_real_select_target(player,
                           2,
                           1 - player,
                           TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
                           s.prompt,
                           1,
                           &s.target);
      s.target_card = s.target.card;
      unk_00742fcc = s.target.player;
    }
    else
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
      if (g_duel_ai_mode_state == 1)
      {
        g_ai_recorded_choice = 0;
        g_ai_recorded_action = ((unk_00742fcc == 0) ? 0 : 0x100) | 0xff;
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }
    }
  }

  if (g_spell_fizzled != 1)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target_card;
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_00742fcc;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x0054af10
// FUNCTION: SHANDALAR 0x0049914d
int deal_damage_to_selected_target(int player, int card, event_t event, int amount)
{
  int source_player;
  int source_card;

  if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 0)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    source_player = g_card_on_stack_controller;
    source_card = g_card_on_stack;
  }
  else
  {
    source_player = player;
    source_card = card;
  }

  if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == -1 && PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
    return 0;

  if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                              PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                              NULL,
                              player,
                              2,
                              2,
                              TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
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
    return 0;
  }

  if (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
  {
    damage_player(PLAYER_CARD_INSTANCE(player, card).targets[0].player, amount, source_player, source_card);
  }
  else
  {
    damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                    PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                    amount,
                    source_player,
                    source_card);
  }

  return 1;
}

// FUNCTION: MAGIC 0x0054b1ad
// FUNCTION: SHANDALAR 0x004993ea
int card_pirate_ship(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PIRATE_SHIP");
    }
    select_damage_target(player, card, 1);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  card_sea_serpent(player, card, event);

  return 0;
}

// FUNCTION: MAGIC 0x0054b360
// FUNCTION: SHANDALAR 0x0049959d
int card_sea_serpent(int player, int card, event_t event)
{
  if (is_in_play(player, card))
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b401
// FUNCTION: SHANDALAR 0x0049963e
int card_elder_land_wurm(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_DECLARE_BLOCKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player != g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).blocking != -1)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b4ed
// FUNCTION: SHANDALAR 0x00499729
int card_island_fish_jasconius(int player, int card, event_t event)
{
  card_sea_serpent(player, card, event);

  return 0;
}

// FUNCTION: MAGIC 0x0054b8ad
// FUNCTION: SHANDALAR 0x00499ae9
int card_lord_of_atlantis(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 1)
  {
    if (event == EVENT_ABILITIES)
    {
      g_event_result |= 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
    }
    else if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY && (PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).regen_status |= 0xe000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ba0b
// FUNCTION: SHANDALAR 0x00499c47
int card_goblin_rock_sled(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_RED)] == 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_DECLARE_ATTACKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  if (event == EVENT_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054baf9
// FUNCTION: SHANDALAR 0x00499d35
int card_orcish_artillery(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ORCISH_ARTILLERY");
    }
    select_damage_target(player, card, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 2) != 0)
    {
      damage_player(player, 3, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bcc8
// FUNCTION: SHANDALAR 0x00499f04
int card_psionic_entity(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ORCISH_ARTILLERY");
    }
    select_damage_target(player, card, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 2) != 0)
    {
      damage_creature(player, card, 3, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bfaf
// FUNCTION: SHANDALAR 0x0049a1eb
int card_scavenging_ghoul(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    result = generic_regeneration_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0);
    if (C_get_special_counters(player, card) == 0)
    {
      result = 0;
    }
    return result;
  }

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    result = generic_regeneration_ability(player, card, EVENT_ACTIVATE, 0, 0);
    remove_special_counters(player, card, 1);
    return result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & 0x200) != 0)
  {
    return generic_regeneration_ability(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
  }

  if ((g_trigger_condition == 0xcd || event == 199) && g_affected_card == card && g_affected_card_controller == player && g_duel_summary.creatures_died != 0 && player == g_current_turn)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      add_special_counters(player, card, g_duel_summary.creatures_died);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054c131
// FUNCTION: SHANDALAR 0x0049a36e
int card_sengir_vampire(int player, int card, event_t event)
{
  struct
  {
    int dead_internal_id;   /* [ebp-0x10] */
    int count_triggered;   /* [ebp-0x0c] */
    int shift_index;       /* [ebp-0x08] */
    int tracked;           /* [ebp-0x04] */
  } s;

  if (event == 0x6e && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card != -1)
    {
      s.shift_index = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player;
      if (PLAYER_CARD_INSTANCE(
              s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
              .internal_card_id == -1)
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
                .original_internal_card_id;
      }
      else
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
                .internal_card_id;
      }

      if ((global_cards_data[s.dead_internal_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(player, card).number_of_targets < 0x13)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].card =
            PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].player =
            s.shift_index;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (event == 0x77)
  {
    s.count_triggered = 0;
    for (s.tracked = 0; s.tracked < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++s.tracked)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].card == g_affected_card && PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].player == g_affected_card_controller && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status & 0x4000000) == 0)
      {
        s.shift_index = s.tracked;
        if (!s.count_triggered)
        {
          ++PLAYER_CARD_INSTANCE(player, card).info_slot;
          s.count_triggered = 1;
        }

        while (++s.shift_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].player =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].card =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].card;
        }
        --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (g_trigger_condition == 0xd5 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && g_current_turn == player && g_affected_card == card && g_affected_card_controller == player)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      add_special_counters(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
      PLAYER_CARD_INSTANCE(player, card).counter_power += (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).counter_toughness +=
          (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }
  else if (event == 0x22 || event == 199)
  {
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cb2b
// FUNCTION: SHANDALAR 0x0049ad68
int card_junun_efreet(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_current_player == player && player == unk_00742f60)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_black += 2;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_BLACK] < 2)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cc76
// FUNCTION: SHANDALAR 0x0049aeb3
int card_phantasmal_forces(int player, int card, event_t event)
{
  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if ((g_current_player == player) && (player == unk_00742f60))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_blue += 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && (g_basiclandtypes_controlled[player][COLOR_BLUE] < 1))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cdc0
// FUNCTION: SHANDALAR 0x0049affe
int card_force_of_nature(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        if (g_current_player == player)
        {
          if (unk_00742f60 == player)
          {
            PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
            PLAYER_CARD_INSTANCE(player, card).upkeep_green =
                PLAYER_CARD_INSTANCE(player, card).upkeep_green + '\x04';
          }
        }
      }
    }
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    load_text("prompts.txt", "FORCE_OF_NATURE");
    do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
    damage_player(player, 8, g_card_on_stack_controller, g_card_on_stack);
  }
  else if ((event == EVENT_SHOULD_AI_PLAY) && (g_basiclandtypes_controlled[player][COLOR_GREEN] < 4))
  {
    damage_player(player, 8, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cf44
// FUNCTION: SHANDALAR 0x0049b183
int card_llanowar_elves(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    undeclare_mana_available_and_produce_it(player, COLOR_GREEN, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    g_produced_mana_color = COLOR_GREEN;
    return 0;
  }

  if ((event == EVENT_COUNT_MANA) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_GREEN, 1);
    }
    return 0;
  }

  if ((event == EVENT_ATTACK_RATING) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    unk_00925d3c += 0x18 / (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  if ((event == EVENT_BLOCK_RATING) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    unk_00925d3c -= 0x60 / (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d153
// FUNCTION: SHANDALAR 0x0049b392
int card_birds_of_paradise(int player, int card, event_t event)
{
  unsigned int available_colors;
  color_t choice_hint;
  color_t color;
  int current_color;
  card_instance_t *instance;
  char message[300];

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & 0x20010) == 0;
  }
  else if ((event == EVENT_ACTIVATE) && ((instance->state & STATE_TAPPED) == 0))
  {
    available_colors = (unsigned int)(unsigned char)instance->damage_source_card;
    if ((((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1)) || (g_duel_network_state != 0))
    {
      choice_hint = -1;
      current_color = 1;
      while ((current_color < 6) && (choice_hint == -1))
      {
        if (((unk_00938e2c & (1 << (((unsigned char)current_color) & 0x1f))) != 0) && ((available_colors & (1 << (((unsigned char)current_color) & 0x1f))) != 0))
        {
          choice_hint = current_color;
        }
        ++current_color;
      }
      if ((choice_hint == -1) && ((unk_00938e2c & 1) != 0))
      {
        choice_hint = COLOR_BLACK;
      }
      if ((choice_hint == -1) && ((unk_00938e2c & 0x40) != 0))
      {
        choice_hint = COLOR_BLACK;
      }
      if (choice_hint == -1)
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      choice_hint = -1;
    }
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "BIRDS_OF_PARADISE");
      }
      color = choose_a_color(player, g_text_lines[0], 1, choice_hint, (unsigned char)instance->damage_source_card);
      if (color == ~COLOR_COLORLESS)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, color, 1);
        undeclare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
        instance->state |= STATE_TAPPED;
        g_produced_mana_color = color;
        if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
        {
          load_text("prompts.txt", "BIRDS_OF_PARADISE2");
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
  else if ((event == EVENT_COUNT_MANA) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if ((instance->state & 0x20010) == 0)
    {
      declare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
    }
  }
  else if ((event == 0x8a) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    unk_00925d3c += 0x60 / (g_basiclandtypes_controlled[player][7] + 2);
  }
  else if ((event == 0x8b) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    unk_00925d3c -= 0x60 / (g_basiclandtypes_controlled[player][7] + 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d69d
// FUNCTION: SHANDALAR 0x0049b8dc
int card_cosmic_horror(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_current_player == player && player == unk_00742f60)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_colorless += 3;
      PLAYER_CARD_INSTANCE(player, card).upkeep_black += 3;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    damage_player(player, 7, g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_BLACK] < 3)
  {
    damage_player(player, 7, player, card);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d8ca
// FUNCTION: SHANDALAR 0x0049bb0a
int card_lord_of_the_pit(int player, int card, event_t event)
{
  int creature_to_sacrifice;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_summary.creature_counts[player] < 2)
    {
      g_ai_modifier -= 0xa8;
    }
    return 0;
  }

  if (event == EVENT_CHECK_UPK_PAYMENT)
  {
    if (has_other_creature_in_play(player, card) == 0)
    {
      g_event_result |= 1;
    }
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS && g_affected_card == card && g_affected_card_controller == player)
  {
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (has_other_creature_in_play(player, card) == 0)
    {
      ++g_ai_modifier;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (has_other_creature_in_play(player, card) == 0)
    {
      g_event_result |= 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x100000;
      TENTATIVE_reassess_all_cards(0, 0x20);
      load_text("prompts.txt", "LORD_OF_THE_PIT");

      creature_to_sacrifice = choose_creature_to_sacrifice(player);

      PLAYER_CARD_INSTANCE(player, card).state &= 0xffefffff;
      if (creature_to_sacrifice != -1)
      {
        kill_card(player, creature_to_sacrifice, KILL_SACRIFICE);
      }
      else
      {
        g_event_result |= 1;
      }
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(player, 7, g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    unk_00925d3c -= 0x30;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    unk_00925d3c += 0x30;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dccd
// FUNCTION: SHANDALAR 0x0049bf0e
int has_other_creature_in_play(int player, int card)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (current_card != card && is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dd89
// FUNCTION: SHANDALAR 0x0049bfca
int card_ball_lightning(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= ~(STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP);
    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) &&
       g_affected_card == card &&
       g_affected_card_controller == player) &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dee6
// FUNCTION: SHANDALAR 0x0049c127
int card_cyclopean_mummy(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = KILL_REMOVE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054df54
// FUNCTION: SHANDALAR 0x0049c195
int card_nether_shadow(int player, int card, event_t event)
{
  int hand_card;
  int hand_player;

  if (card == g_affected_card)
  {
    if (player == g_affected_card_controller)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;
    }
  }

  if (event == 0x8d ||
      (event == 0x77 &&
       card == g_affected_card &&
       player == g_affected_card_controller &&
       (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
       PLAYER_CARD_INSTANCE(player, card).kill_code != 4))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0)
    {
      hand_player = g_other_player;
    }
    else
    {
      hand_player = g_active_player;
    }

    hand_card = add_card_to_hand(hand_player, unk_008a9194);
    if (hand_card != -1)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(hand_player, hand_card).state |= 2;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(hand_player, hand_card) + 0x64) =
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e160
// FUNCTION: SHANDALAR 0x0049c3a1
int card_nether_link(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage;
  int result;

  result = generic_creature_ability_aura(player, card, event, 0);
  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    instance->damage_source_player = -1;
    instance->eot_toughness = 0;
    return result;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if ((int)instance->damage_target_player == damage->damage_source_player &&
        instance->damage_target_card == damage->damage_source_card &&
        damage->info_slot != 0)
    {
      instance->damage_source_player = (char)g_affected_card_controller;
      instance->eot_toughness += damage->info_slot;
    }
    return result;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      instance->damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      gain_life(player, instance->eot_toughness, player, card);
      instance->damage_source_player = -1;
      instance->eot_toughness = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054e36e
// FUNCTION: SHANDALAR 0x0049c5b0
int card_white_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << ((get_sleighted_color(player, card, COLOR_BLACK) - 1U));
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_BLACK);
    g_event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e3ef
// FUNCTION: SHANDALAR 0x0049c630
int card_black_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if ((event == 0x34) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_event_result |= 0x800 << (get_sleighted_color(player, card, 5) - 1U);
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, 5);
    g_event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e470
// FUNCTION: SHANDALAR 0x0049c6b1
void destroy_attached_auras_of_color(int player, int card, int color)
{
  int test_player;
  int test_card;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    for (test_card = 0; test_card < g_active_cards_count[test_player]; ++test_card)
    {
      if (is_in_play(test_player, test_card))
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == card)
        {
          if (((1 << (unsigned char)get_sleighted_color(player, card, color)) &
               (char)PLAYER_CARD_INSTANCE(test_player, test_card).color) != 0 &&
              (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(test_player, test_card).internal_card_id].type & 4) != 0))
          {
            kill_card(test_player, test_card, 1);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0054e5f3
// FUNCTION: SHANDALAR 0x0049c834
int card_ali_baba(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1) &&
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
                                 TARGET_SPECIAL_WALL,
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
    if (has_mana(player, COLOR_RED, 1))
    {
      charge_mana(player, COLOR_RED, 1);
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
                                  TARGET_SPECIAL_WALL,
                                  0,
                                  0,
                                  "",
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
                                TARGET_SPECIAL_WALL,
                                0,
                                0))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state |= STATE_TAPPED;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e986
// FUNCTION: SHANDALAR 0x0049cbc7
int card_ley_druid(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && real_target_available((int *)0,
                                                                  TARGET_SCAN_DIRECT,
                                                                  player,
                                                                  2,
                                                                  2,
                                                                  0x200,
                                                                  TYPE_LAND,
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
                                                                  0) != 0)
    {
      return 1;
    }
    return 0;
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text(0, "LEY DRUID");
    }

    if (C_real_select_target(player,
                             2,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
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
                             &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      instance->targets[0] = target;
      instance->number_of_targets = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
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
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
      dispatch_event_to_single_card(target.player, target.card, EVENT_UNTAP_PHASE, -1, -1);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ed44
// FUNCTION: SHANDALAR 0x0049cf85
int card_venom(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int enchanted_player;
  int enchanted_card;
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;

  result = generic_creature_ability_aura(player, card, event, 0);

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    instance = &PLAYER_CARD_INSTANCE(player, card);
    enchanted_player = (int)instance->damage_target_player;
    enchanted_card = instance->damage_target_card;

    if (enchanted_player != -1 &&
        enchanted_card != -1 &&
        is_in_play(enchanted_player, enchanted_card) != 0)
    {
      opponent = 1 - enchanted_player;
      if (enchanted_player == g_current_player &&
          (PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).state & (STATE_ATTACKING | STATE_ATTACKED)) != 0)
      {
        if (PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking == -1)
        {
          blocking_card = enchanted_card;
        }
        else
        {
          blocking_card = (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking;
        }

        for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if (blocking_iid != -1 &&
              PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card &&
              (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
              global_cards_data[blocking_iid].subtype != SUB_WALL)
          {
            create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
          }
        }
      }

      if (enchanted_player != g_current_player &&
          PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking != -1)
      {
        grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking).blocking;
        if (grouped_blocker == -1)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking).internal_card_id;
          if (blocking_iid != -1 &&
              (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
              global_cards_data[blocking_iid].subtype != SUB_WALL)
          {
            create_legacy_effect(player, card, unk_008b3104, opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking);
          }
        }
        else
        {
          for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
          {
            blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
            if (is_in_play(opponent, current_card) != 0 &&
                blocking_iid != -1 &&
                PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker &&
                (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
                global_cards_data[blocking_iid].subtype != SUB_WALL)
            {
              create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
            }
          }
        }
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054f7fe
// FUNCTION: SHANDALAR 0x0049da3f
int card_thicket_basilisk(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;

  if (event == 0x3c)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked = PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked | 0x3f;
  }
  else if (event == 0x1a)
  {
    opponent = 1 - player;
    if ((player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x44) != 0))
    {
      if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = card;
      }
      else
      {
        blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }

      for (current_card = 0; current_card < g_active_cards_count[opponent]; current_card = current_card + 1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (((PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card) && (blocking_iid != -1)) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
        }
      }
    }

    if ((player != g_current_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
    {
      grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
      if (grouped_blocker == -1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id;
        if ((blocking_iid != -1) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        }
      }
      else
      {
        for (current_card = 0; current_card < g_active_cards_count[opponent]; current_card = current_card + 1)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if ((is_in_play(opponent, current_card) != 0) && (PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
          {
            create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054fcbc
// FUNCTION: SHANDALAR 0x0049deff
int card_wall_of_dust(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_iid;
  char grouped_blocker;

  if (event == EVENT_DECLARE_BLOCKERS &&
      player != g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).blocking != -1)
  {
    opponent = 1 - player;
    grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
    if (grouped_blocker == -1)
    {
      blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id;
      if (blocking_iid != -1 &&
          (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)
      {
        create_no_attack_legacy(player, card, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
      }
    }
    else
    {
      for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (is_in_play(opponent, current_card) != 0 &&
            blocking_iid != -1 &&
            PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker &&
            (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)
        {
          create_no_attack_legacy(player, card, opponent, current_card);
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ffb0
// FUNCTION: SHANDALAR 0x0049e1f3
int card_sisters_of_the_flame(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_RED, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_RED;
    return 0;
  }

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_RED, 1);
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0055011b
// FUNCTION: SHANDALAR 0x0049e35e
int card_apprentice_wizard(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           has_mana(player, COLOR_BLUE, 1);
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    charge_mana(player, COLOR_BLUE, 1);
    if (g_spell_fizzled != 1)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      g_produced_mana_color = COLOR_COLORLESS;
    }
    return 0;
  }

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
        g_raw_mana_available[player][COLOR_BLUE] != 0)
    {
      declare_mana_available(player, COLOR_COLORLESS, 3);
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0055025c
// FUNCTION: SHANDALAR 0x0049e49f
int card_abomination(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;
  color_test_t affected_colors;

  affected_colors = (1 << (unsigned char)get_sleighted_color(player, card, COLOR_GREEN)) |
                    (1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE));

  if (event == 0x1a)
  {
    opponent = 1 - player;
    if ((player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0))
    {
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
        if (blocking_iid != -1 &&
            PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card &&
            (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(opponent, current_card).color & affected_colors) != 0)
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
        }
      }
    }

    if ((player != g_current_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
    {
      grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
      if (grouped_blocker == -1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id;
        if (blocking_iid != -1 &&
            (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).color & affected_colors) != 0)
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        }
      }
      else
      {
        for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if (is_in_play(opponent, current_card) != 0 &&
              blocking_iid != -1 &&
              PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker &&
              (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
              (PLAYER_CARD_INSTANCE(opponent, current_card).color & affected_colors) != 0)
          {
            create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00550761
// FUNCTION: SHANDALAR 0x0049e9a4
int card_radjan_spirit(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int legacy_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & 0x20010) == 0 &&
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
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "RADJAN_SPIRIT");
    }

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
                              g_text_lines[0],
                              1,
                              &target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      instance->targets[0] = target;
      instance->number_of_targets = 1;
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
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00550c4b
// FUNCTION: SHANDALAR 0x0049ee8e
int card_hurr_jackal(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (instance->state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 &&
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
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "HURR_JACKAL");
    }

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
                              g_text_lines[0],
                              1,
                              &target))
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      instance->targets[0] = target;
      instance->number_of_targets = 1;
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
      PLAYER_CARD_INSTANCE(target.player, target.card).regen_status |= 0x200;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}
