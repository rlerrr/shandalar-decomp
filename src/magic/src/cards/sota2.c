#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int gain_control(int player, int card);

static __inline int select_own_artifact_for_cost(int player, int card, target_t *target, char *prompt)
{
  return C_real_select_target(player,
                              player,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT,
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
                              prompt,
                              1,
                              target);
}

static __inline int own_artifact_available(int player)
{
  return real_target_available((int *)0,
                               TARGET_SCAN_DIRECT,
                               player,
                               player,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
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

static __inline int count_artifacts_in_play(int player)
{
  int count;
  int current_card;

  count = 0;
  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) &&
        (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_ARTIFACT) != 0)
    {
      ++count;
    }
  }
  return count;
}

static __inline int select_own_land_for_cost(int player, target_t *target, char *prompt)
{
  return C_real_select_target(player,
                              player,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_LAND,
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
                              prompt,
                              1,
                              target);
}

static __inline int own_land_available(int player)
{
  return real_target_available((int *)0,
                               TARGET_SCAN_DIRECT,
                               player,
                               player,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_LAND,
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

static __inline int is_card_id(int player, int card, int csvid)
{
  return PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1 &&
         global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id == csvid;
}

static __inline int controls_card_id(int player, int csvid)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) && is_card_id(player, current_card, csvid))
    {
      return 1;
    }
  }
  return 0;
}

static __inline int damage_card_source_is_artifact(card_instance_t *damage)
{
  return damage->internal_card_id == g_damage_card_internal_card_id &&
         (damage->eot_toughness & TYPE_ARTIFACT) != 0;
}

static __inline int damage_card_targets_artifact_creature(int damage_player, int damage_card)
{
  card_instance_t *damage;
  int target_player;
  int target_card;
  int target_type;

  damage = &PLAYER_CARD_INSTANCE(damage_player, damage_card);
  target_player = damage->damage_target_player;
  target_card = damage->damage_target_card;
  if (damage->internal_card_id != g_damage_card_internal_card_id ||
      target_player == -1 ||
      target_card == -1 ||
      PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id == -1)
  {
    return 0;
  }

  target_type = global_cards_data[PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id].type;
  return (target_type & (TYPE_ARTIFACT | TYPE_CREATURE)) == (TYPE_ARTIFACT | TYPE_CREATURE);
}

static __inline int artifact_creature_damage_available(void)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_in_play(current_player, current_card) &&
          damage_card_targets_artifact_creature(current_player, current_card) &&
          PLAYER_CARD_INSTANCE(current_player, current_card).info_slot != 0)
      {
        return 1;
      }
    }
  }
  return 0;
}

static __inline int is_noncreature_artifact(int player, int card)
{
  int type;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
  return (type & TYPE_ARTIFACT) != 0 && (type & TYPE_CREATURE) == 0;
}

// FUNCTION: MAGIC 0x0046d000
// FUNCTION: SHANDALAR 0x00470b00
int card_abu_ja_far(int player, int card, event_t event)
{
  int current_card;
  int opponent;

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    opponent = 1 - player;
    if ((int)(char)PLAYER_CARD_INSTANCE(player, card).blocking != -1 &&
        PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id != -1)
    {
      kill_card(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking, KILL_BURY);
    }
    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      if (is_in_play(opponent, current_card) &&
          (int)(char)PLAYER_CARD_INSTANCE(opponent, current_card).blocking == card)
      {
        kill_card(opponent, current_card, KILL_BURY);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046d478
// FUNCTION: SHANDALAR 0x00470f7a
int card_aladdin(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           has_mana(player, COLOR_RED, 2) &&
           has_mana(player, COLOR_ANY, 3) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_RED, 2);
    if (g_spell_fizzled != 1)
    {
      charge_mana(player, COLOR_COLORLESS, 1);
    }
    if (g_spell_fizzled != 1)
    {
      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0,
                               "Select target artifact.", 1, &target) == 0)
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
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0 &&
        target.player != player)
    {
      gain_control(target.player, target.card);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046e478
// FUNCTION: SHANDALAR 0x00471f68
int card_argivian_archaeologist(int player, int card, event_t event)
{
  struct
  {
    int can_select;
    int selectable[500];
    int graveyard_index;
    int current_card;
    int hand_card;
    char (*prompt)[300];
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && has_mana(player, COLOR_WHITE, 2) &&
           graveyard_has_type(player, TYPE_ARTIFACT);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_WHITE, 2);
    if (g_spell_fizzled != 1)
    {
      if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1))
      {
        s.can_select = 1;
        s.graveyard_index = find_highest_value_graveyard_card_by_type(player, TYPE_ARTIFACT);
      }
      else
      {
        for (s.current_card = 0;
             s.current_card < 500 && global_graveyard_slots[player][s.current_card] != -1;
             ++s.current_card)
        {
          s.selectable[s.current_card] =
              (global_cards_data[global_graveyard_slots[player][s.current_card]].type & TYPE_ARTIFACT) != 0;
        }
        load_text("promptsX1.txt", "ARGIVIAN_ARCH");
        s.prompt = g_text_lines;
        s.can_select = select_from_graveyard_with_dialog(player,
                                                         global_graveyard_slots[player],
                                                         s.selectable,
                                                         500,
                                                         (int)&s.prompt,
                                                         1,
                                                         (int)&s.graveyard_index,
                                                         0,
                                                         1);
      }

      if (s.can_select != 0 && s.graveyard_index != -1 &&
          global_graveyard_slots[player][s.graveyard_index] != -1 &&
          (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_ARTIFACT) != 0)
      {
        s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][s.graveyard_index]);
        if (s.hand_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.hand_card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id == global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] &&
        (global_cards_data[global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]].type & TYPE_ARTIFACT) != 0)
    {
      remove_card_from_graveyard(player, PLAYER_CARD_INSTANCE(player, card).info_slot);
    }
    else
    {
      kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                KILL_REMOVE);
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046e8d0
// FUNCTION: SHANDALAR 0x004723c1
int card_argivian_blacksmith(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) != 0 &&
        CAN_TAP(player, card) &&
        artifact_creature_damage_available())
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0,
                             0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                             ~SUB_WALL, -1, -1, 0, 0, 0,
                             "Select damage to prevent.", 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else if (damage_card_targets_artifact_creature(target.player, target.card) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (damage_card_targets_artifact_creature(target.player, target.card) != 0)
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).unknown0x37 =
          (char)PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      PLAYER_CARD_INSTANCE(target.player, target.card).info_slot = 0;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f265
// FUNCTION: SHANDALAR 0x00472d56
int card_argothian_pixies(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_ARTIFACTS;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f305
// FUNCTION: SHANDALAR 0x00472df4
int card_argothian_treefolk(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_ARTIFACTS;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f47c
// FUNCTION: SHANDALAR 0x00472f6b
int card_atog(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return own_artifact_available(player) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (select_own_artifact_for_cost(player, card, &target, "Select an artifact to sacrifice.") == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(target.player, target.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack,
                        g_card_on_stack_controller, g_card_on_stack, 2, 2);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046fb1e
// FUNCTION: SHANDALAR 0x0047360e
int card_camel(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_BANDING | KEYWORD_PROT_LANDS;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046ffa0
// FUNCTION: SHANDALAR 0x00473a91
int card_citanul_druid(int player, int card, event_t event)
{
  if (g_trigger_condition == TRIGGER_SPELL_CAST &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn &&
      g_trigger_cause_controller == 1 - player &&
      PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type &
       TYPE_ARTIFACT) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      ++PLAYER_CARD_INSTANCE(player, card).counter_power;
      ++PLAYER_CARD_INSTANCE(player, card).counter_toughness;
      add_special_counters(player, card, 1);
    }
  }

  return 0;
}

static int clone_find_data_card_callback(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id);

// FUNCTION: MAGIC 0x00470140
// FUNCTION: SHANDALAR 0x00473c2e
int card_clone(int player, int card, event_t event)
{
  target_t selected_target;
  int data_card_slot;
  int source_internal_card_id;
  int cloned_internal_card_id;
  int data_card_controller;
  unsigned int data_card_location;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0x200,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "CLONE");
    if (C_real_select_target(player,
                             2,
                             2,
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
                             TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                             0,
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
                               TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                               0,
                               0) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_DESTROY);
    }
    else if (global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id]
                 .id == CARD_ID_VESUVAN_DOPPELGANGER)
    {
      data_card_location = dispatch_function_to_all_cards_in_play(selected_target.player,
                                                                  selected_target.card,
                                                                  clone_find_data_card_callback,
                                                                  -1);
      if (data_card_location == -1)
      {
        source_internal_card_id = 0;
      }
      else
      {
        source_internal_card_id = PLAYER_CARD_INSTANCE((data_card_location >> 8) & 0xff,
                                                       data_card_location & 0xff)
                                      .info_slot;
      }
      data_card_controller = player;
      data_card_slot = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_DATA_CARD));
      if (data_card_slot != -1)
      {
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).state =
            ((((unsigned int)(((player == 0) - 1) >> 8)) & 0x1000) | 2);
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).token_status |= 0x10008;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_player =
            (char)player;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_card = card;
        cloned_internal_card_id = create_a_card_type(source_internal_card_id);
        if (cloned_internal_card_id != -1)
        {
          PLAYER_CARD_INSTANCE(player, card).internal_card_id = cloned_internal_card_id;
          PLAYER_CARD_INSTANCE(player, card).dummy3 = cloned_internal_card_id;
          PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
          PLAYER_CARD_INSTANCE(player, card).state &= 0xffffdfff;
          PLAYER_CARD_INSTANCE(player, card).color = global_cards_data[cloned_internal_card_id].color;
          if ((global_cards_data[cloned_internal_card_id].type & TYPE_ARTIFACT) != 0)
          {
            ++g_duel_summary.artifact_counts[player];
          }
          if ((global_cards_data[cloned_internal_card_id].type & TYPE_ENCHANTMENT) != 0)
          {
            ++g_duel_summary.enchantment_counts[player];
          }
          g_card_types_in_play[player] |= global_cards_data[cloned_internal_card_id].type;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot =
              source_internal_card_id;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |=
              global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
          dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
          global_cards_data[cloned_internal_card_id].code_pointer =
              global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                    .internal_card_id]
                  .code_pointer;
          global_cards_data[cloned_internal_card_id].extra_ability |= 1;
        }
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).dummy3 =
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3;
      PLAYER_CARD_INSTANCE(player, card).color =
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).color;
      process_card_enters_play(player, card);
    }
  }

  if (((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & 0x20000) == 0)) && ((card == g_affected_card) && (player == g_affected_card_controller)))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047095d
// FUNCTION: SHANDALAR 0x0047444f
int card_cuombajj_witches(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
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
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0,
                             "Select target creature or player.", 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      if (C_real_select_target(1 - player, 2, 2, TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0,
                               "Select target creature or player.", 1, &target) == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->targets[1] = target;
        instance->number_of_targets = 2;
        instance->state |= STATE_TAPPED;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (target.card == -1)
    {
      damage_player(target.player, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    else if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                    TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                    get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                    -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }

    target = instance->targets[1];
    if (target.card == -1)
    {
      damage_player(target.player, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    else if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                                    TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                    get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                    -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00470e7e
// FUNCTION: SHANDALAR 0x00474970
int card_demonic_hordes(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
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
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX1.txt", "DEMONIC_HORDES");
      if (!select_target_land_and_store(player, 2, card))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->state |= 0x10;
        g_ai_modifier += instance->targets[0].player == player ? -24 : 36;
      }

      if (g_spell_fizzled == 1)
      {
        instance->number_of_targets = 0;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (!C_real_validate_target(instance->targets[0].player,
                                  instance->targets[0].card,
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
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }

      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    }

    if (event == EVENT_SETUP_UPKEEP_COSTS && card == g_affected_card && player == g_affected_card_controller && player == g_current_player && unk_00742f60 == player)
    {
      instance->upkeep_flags |= 1;
      instance->upkeep_black += 3;
    }

    if (event == EVENT_UPKEEP_COSTS_UNPAID)
    {
      selected_target.player = -1;
      selected_target.card = -1;

      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                player,
                                player,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
                                TYPE_NONE,
                                0,
                                0,
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
        load_text("promptsX1.txt", "DEMONIC_HORDES_2");
        if (g_current_player == player || (g_duel_network_flags & 2) != 0)
        {
          C_real_select_target(player,
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
                               0,
                               0,
                               g_text_lines[0],
                               0,
                               &selected_target);
        }
        else if (select_best_land_target_by_score(1 - player, player, (int *)&selected_target))
        {
          do_dialog(player,
                    player,
                    0,
                    selected_target.player,
                    selected_target.card,
                    g_text_lines[1],
                    0);
        }

        if (selected_target.player != -1)
        {
          if (g_duel_ai_mode_state != 1)
          {
            play_sound_effect(WAV_SACRFICE);
          }
          kill_card(selected_target.player, selected_target.card, KILL_SACRIFICE);
        }
      }

      instance->state |= 0x10;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047146f
// FUNCTION: SHANDALAR 0x00474f5d
int card_desert_nomads(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY &&
      g_attacking_card == card &&
      g_attacking_card_controller == player &&
      controls_card_id(1 - player, CARD_ID_DESERT))
  {
    g_event_result = 1;
  }

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_LANDS;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00471838
// FUNCTION: SHANDALAR 0x00475328
int card_dwarven_demolition_team(int player, int card, event_t event)
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
                                                                  0xffffffff,
                                                                  0xffffffff,
                                                                  1,
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
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX1.txt", "DWARVEN_DTEAM");
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
        kill_card(target.player, target.card, KILL_DESTROY);
      }
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00471ba6
// FUNCTION: SHANDALAR 0x00475696
int card_dwarven_weaponsmith(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  target_t artifact;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           own_artifact_available(player) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (select_own_artifact_for_cost(player, card, &artifact, "Select an artifact to sacrifice.") == 0)
    {
      g_spell_fizzled = 1;
    }
    else if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                  get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                  -1, -1, -1, -1, 0, 0, 0,
                                  "Select target creature.", 1, &target) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      instance->number_of_targets = 1;
      instance->state |= STATE_TAPPED;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack, target.player, target.card, 1, 1);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004722ff
// FUNCTION: SHANDALAR 0x00475df0
int card_erhnam_djinn(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;
  int forestwalk_mask;

  forestwalk_mask = 1 << (unsigned char)(get_hacked_color(player, card, COLOR_GREEN) - 1);

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (C_real_select_target(player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_NON_WALL, 0, 0,
                             "Select target non-Wall creature.", 1, &target) != 0)
    {
      legacy_card = create_legacy_effect(player, card, unk_00896534, target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = forestwalk_mask;
        PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004729c8
// FUNCTION: SHANDALAR 0x004764bc
int card_gaea_s_avenger(int player, int card, event_t event)
{
  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    g_event_result += count_artifacts_in_play(1 - player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472b0a
// FUNCTION: SHANDALAR 0x004765fe
int card_ghazban_ogre(int player, int card, event_t event)
{
  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_life[1 - player] > g_life[player])
    {
      gain_control(player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472df0
// FUNCTION: SHANDALAR 0x004768e5
int card_giant_badger(int player, int card, event_t event)
{
  if (event == EVENT_DECLARE_BLOCKERS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player != g_current_player &&
      (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking != -1)
  {
    give_pump_until_eot(player, card, player, card, 2, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472f31
// FUNCTION: SHANDALAR 0x00476a27
int card_goblin_artisans(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (CAN_TAP(player, card) &&
        g_card_on_stack_controller == player &&
        g_card_on_stack != -1 &&
        C_real_validate_target(g_card_on_stack_controller, g_card_on_stack, (char *)0, player,
                               player, player, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                               TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) != 0)
    {
      return 1;
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    instance->targets[0].player = g_card_on_stack_controller;
    instance->targets[0].card = g_card_on_stack;
    instance->number_of_targets = 1;
    instance->state |= STATE_TAPPED;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                               (char *)0, player, player, player, 0,
                               TYPE_ARTIFACT, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) == 0)
    {
      g_spell_fizzled = 1;
    }
    else if (internal_rand(2) != 0)
    {
      draw_card_for_player(player);
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004732e4
// FUNCTION: SHANDALAR 0x00476ddc
int card_granite_gargoyle(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    result = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      result = has_mana(player, COLOR_RED, 1);
      if (result != 0)
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
        *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
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
    }

    if (event == EVENT_TOU_BOOST)
    {
      result = has_mana(player, COLOR_RED, 1);
    }
    else
    {
      if (event == EVENT_CAN_WASTE_MANA && g_raw_mana_available[player][COLOR_RED] != 0)
      {
        g_event_result |= 1;
      }

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

      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }

      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00473ab4
// FUNCTION: SHANDALAR 0x004775a8
int card_guardian_beast(int player, int card, event_t event)
{
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      is_in_play(player, card))
  {
    if (event == EVENT_ABILITIES &&
        g_affected_card_controller == player &&
        is_noncreature_artifact(player, g_affected_card))
    {
      g_event_result |= KEYWORD_PROT_ENCHANTMENTS;
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY &&
        g_affected_card_controller == player &&
        is_noncreature_artifact(player, g_affected_card) &&
        PLAYER_CARD_INSTANCE(player, g_affected_card).kill_code == KILL_DESTROY)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00474179
// FUNCTION: SHANDALAR 0x00477c68
int card_hasran_ogress(int player, int card, event_t event)
{
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
      if (has_mana(player, COLOR_COLORLESS, 2))
      {
        charge_mana(player, COLOR_COLORLESS, 2);
      }
      if (g_spell_fizzled == 1)
      {
        g_spell_fizzled = 0;
        damage_player(player, 3, player, card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004743a9
// FUNCTION: SHANDALAR 0x00477e97
int card_ifh_biff_efreet(int player, int card, event_t event)
{
  int current_player;
  int current_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    damage_player(0, 1, g_card_on_stack_controller, g_card_on_stack);
    damage_player(1, 1, g_card_on_stack_controller, g_card_on_stack);
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
            (C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) != 0)
        {
          damage_creature(current_player, current_card, 1, g_card_on_stack_controller, g_card_on_stack);
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00474f4d
// FUNCTION: SHANDALAR 0x00478a33
int card_juggernaut(int player, int card, event_t event)
{
  if ((event == EVENT_BLOCK_LEGALITY) && g_attacking_card == card && player == g_attacking_card_controller && global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 0)
  {
    g_event_result = 1;
  }
  if ((event == EVENT_MUST_ATTACK) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_UNKNOWN8000) == 0))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_UNKNOWN8000;
    if (can_attack(player, card))
    {
      g_attacking_creature_count = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047506c
// FUNCTION: SHANDALAR 0x00478b53
int card_khabal_ghoul(int player, int card, event_t event)
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
      PLAYER_CARD_INSTANCE(player, card).counter_power += g_duel_summary.creatures_died;
      PLAYER_CARD_INSTANCE(player, card).counter_toughness += g_duel_summary.creatures_died;
      add_special_counters(player, card, g_duel_summary.creatures_died);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004751d9
// FUNCTION: SHANDALAR 0x00478cc1
int card_king_suleiman(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0,
                             "Select target Djinn or Efreet.", 1, &target) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0) != 0)
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475555
// FUNCTION: SHANDALAR 0x0047903d
int card_kird_ape(int player, int card, event_t event)
{
  if (g_affected_card == card &&
      g_affected_card_controller == player &&
      g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_GREEN)] > 0)
  {
    if (event == EVENT_POWER)
    {
      ++g_event_result;
    }
    if (event == EVENT_TOUGHNESS)
    {
      g_event_result += 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004755ca
// FUNCTION: SHANDALAR 0x004790b4
int card_martyrs_of_korlis(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (event == EVENT_PREVENT_DAMAGE &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_target_player == player &&
        damage->damage_target_card == -1 &&
        damage->info_slot != 0 &&
        damage_card_source_is_artifact(damage))
    {
      damage_creature(player, card, damage->info_slot, damage->damage_source_player, damage->damage_source_card);
      damage->unknown0x37 = (char)damage->info_slot;
      damage->info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004757d7
// FUNCTION: SHANDALAR 0x004792c1
int card_merchant_ship(int player, int card, event_t event)
{
  card_instance_t *damage;

  if (is_in_play(player, card) &&
      g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
  {
    kill_card(player, card, KILL_SACRIFICE);
  }

  if (event == EVENT_ATTACK_LEGALITY &&
      g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    damage = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card);
    if (damage->damage_source_player == player &&
        damage->damage_source_card == card &&
        damage->damage_target_card == -1 &&
        damage->damage_target_player == 1 - player &&
        damage->info_slot != 0)
    {
      gain_life(player, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475953
// FUNCTION: SHANDALAR 0x0047943c
int card_mijae_djinn(int player, int card, event_t event)
{
  if (g_trigger_condition == TRIGGER_ATTACKER_CHOSEN &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card &&
      g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER && internal_rand(2) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_ATTACKING;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_ATTACKED | STATE_TAPPED;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475b66
// FUNCTION: SHANDALAR 0x0047964f
int card_nettling_imp(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((instance->state & 0x20010) == 0 && player != g_current_player && g_current_phase < 0x1a && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, 0xffffffff, 0xffffffff, 0x80, 0, 0x1000))
    {
      return 1;
    }
  }
  else if (event == 0x90)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX1.txt", "NETTLING_IMP");
      if (!C_real_select_target(player,
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
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_NON_WALL,
                                0,
                                TARGET_STATE_SUMMONING_SICK,
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
        if (player == g_other_player)
        {
          g_ai_modifier += 0x30;
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
                                  TARGET_SPECIAL_NON_WALL,
                                  0,
                                  TARGET_STATE_SUMMONING_SICK))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, unk_0079140c, target.player, target.card);
      }
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475f9a
// FUNCTION: SHANDALAR 0x00479a85
int card_old_man_of_the_sea(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int max_power;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  max_power = C_get_abilities(player, card, EVENT_POWER, -1);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, TARGET_PT_LESSER_OR_EQUAL | max_power, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, TARGET_PT_LESSER_OR_EQUAL | max_power, -1, 0, 0, 0,
                             "Select target creature.", 1, &target) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, TARGET_PT_LESSER_OR_EQUAL | max_power, -1, 0, 0, 0) != 0 &&
        target.player != player)
    {
      gain_control(target.player, target.card);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00477791
// FUNCTION: SHANDALAR 0x0047b286
int card_orcish_mechanics(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t artifact;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && own_artifact_available(player);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (select_own_artifact_for_cost(player, card, &artifact, "Select an artifact to sacrifice.") == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      instance->state |= STATE_TAPPED;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
      select_damage_target(player, card, 2);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 2);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004779f6
// FUNCTION: SHANDALAR 0x0047b4eb
int card_phyrexian_gremlins(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0,
                             "Select target artifact.", 1, &target) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      tap_card_and_dispatch_event(target.player, target.card);
      PLAYER_CARD_INSTANCE(target.player, target.card).untap_status |= 4;
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  if (event == EVENT_UNTAP_CARD && g_affected_card == card && g_affected_card_controller == player)
  {
    target = instance->targets[0];
    if (target.card != -1 && is_in_play(target.player, target.card))
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).untap_status &= ~4;
    }
    instance->number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00478606
// FUNCTION: SHANDALAR 0x0047c106
int card_priest_of_yawgmoth(int player, int card, event_t event)
{
  target_t artifact;
  int internal_card_id;
  int mana_value;

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    if (CAN_TAP(player, card) && own_artifact_available(player))
    {
      declare_mana_available(player, COLOR_COLORLESS, 1);
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && own_artifact_available(player);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (select_own_artifact_for_cost(player, card, &artifact, "Select an artifact to sacrifice.") == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      internal_card_id = PLAYER_CARD_INSTANCE(artifact.player, artifact.card).internal_card_id;
      mana_value = (int)global_cards_data[internal_card_id].cc[0] +
                   (int)global_cards_data[internal_card_id].cc[1];
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
      produce_mana(player, COLOR_COLORLESS, mana_value);
      g_produced_mana_color = COLOR_COLORLESS;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047894c
// FUNCTION: SHANDALAR 0x0047c44c
int card_rock_hydra(int player, int card, event_t event)
{
  int *piVar1;
  unsigned int uVar3;
  int iVar4;
  unsigned int damage_prevented;
  target_t selected_damage;
  int legacy_card;
  int valid_target_selected;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    set_special_counters(player, card, g_x_value);
    if (g_other_player == player)
    {
      g_ai_modifier += (g_x_value + g_x_value * 2 - 9) << 3;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (legacy_card != -1)
    {
      uVar3 = C_get_special_counters(player, card);
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = (uVar3 << 8) | uVar3;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x40c;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((g_land_can_be_played & 4) == 0) || ((iVar4 = has_mana(player, 4, 1)) == 0)) || (((uVar3 = C_get_special_counters(player, card)) == 0) || ((iVar4 = has_effect_source_type(player, card, 0xffffffff)) == 0)))
    {
      if (((g_current_phase == 4) && (player == g_current_player) && (player == unk_00742f60)) && ((iVar4 = has_mana(player, 4, 3)) != 0))
      {
        if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_summary.hand_counts[player] - *(int *)((char *)g_basiclandtypes_controlled + player * 0x20 + 0x10) == 3))
        {
          unk_008b3270 |= 3;
        }
        iVar4 = 1;
      }
      else
      {
        iVar4 = 0;
      }
    }
    else
    {
      iVar4 = 99;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    iVar4 = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if ((g_land_can_be_played & 4) == 0)
      {
        charge_mana(player, 4, 3);
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        if (g_spell_fizzled != 1)
        {
          valid_target_selected = 0;
          do
          {
            load_text("promptsX1.txt", "ROCK_HYDRA");
            iVar4 = C_real_select_target(player,
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
                                         0,
                                         g_text_lines[0],
                                         1,
                                         &selected_damage);
            if (iVar4 == 0)
            {
              g_spell_fizzled = 1;
            }
            else if (PLAYER_CARD_INSTANCE(selected_damage.player, selected_damage.card).damage_source_player == player && PLAYER_CARD_INSTANCE(selected_damage.player, selected_damage.card).damage_source_card == card)
            {
              PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_damage.player;
              PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_damage.card;
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              valid_target_selected = 1;
            }
            else if (g_duel_ai_mode_state == 1)
            {
              rewind_recorded_action();
            }
            else
            {
              set_duel_prompt_text(g_text_lines[1]);
              Sleep(0x9c4);
              set_duel_prompt_text("");
            }
          } while ((g_spell_fizzled != 1) && (valid_target_selected == 0));
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id != -1)
      {
        if ((g_land_can_be_played & 4) == 0)
        {
          add_special_counter(g_card_on_stack_controller, g_card_on_stack);
          piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                         PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                        .info_slot;
          *piVar1 += 0x101;
        }
        else
        {
          uVar3 = C_real_validate_target(
              PLAYER_CARD_INSTANCE(player, card).targets[0].player,
              PLAYER_CARD_INSTANCE(player, card).targets[0].card,
              (char *)0,
              player,
              ANYBODY,
              ANYBODY,
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
              0);
          if (uVar3 == 0)
          {
            g_spell_fizzled = 1;
          }
          else if (0 < PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                            PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                           .info_slot)
          {
            piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                          .info_slot;
            *piVar1 += -1;
          }
        }
      }

      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }

    if ((((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card)) && ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)))
    {
      uVar3 = C_get_special_counters(player, card);
      if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot < (int)uVar3)
      {
        damage_prevented = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      }
      else
      {
        damage_prevented = C_get_special_counters(player, card);
      }
      piVar1 = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      *piVar1 -= damage_prevented;
      remove_special_counters(player, card, damage_prevented);
      piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                    .info_slot;
      *piVar1 -= (damage_prevented << 8) | damage_prevented;
    }
    iVar4 = 0;
  }
  return iVar4;
}

// FUNCTION: MAGIC 0x004794d8
// FUNCTION: SHANDALAR 0x0047cfdc
int card_rukh_egg(int player, int card, event_t event)
{
  int rukh_card;

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE)
  {
    rukh_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_RUKH));
    if (rukh_card != -1)
    {
      process_card_enters_play(player, rukh_card);
      PLAYER_CARD_INSTANCE(player, rukh_card).token_status |= STATUS_TOKEN;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004795a4
// FUNCTION: SHANDALAR 0x0047d0aa
int card_sage_of_lat_nam(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && own_artifact_available(player);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (select_own_artifact_for_cost(player, card, &target, "Select an artifact to sacrifice.") == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(target.player, target.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(g_card_on_stack_controller);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004796cc
// FUNCTION: SHANDALAR 0x0047d1d2
int card_sedge_troll(int player, int card, event_t event)
{
  int color;

  if (card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLACK)] > 0)
    {
      if (event == EVENT_POWER)
      {
        ++g_event_result;
      }
      if (event == EVENT_TOUGHNESS)
      {
        ++g_event_result;
      }
    }
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    color = get_hacked_color(player, card, COLOR_BLACK);
    ((int *)((char *)g_ai_mana_demand_by_color + player * 0x20))[color] += 2;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004797ae
// FUNCTION: SHANDALAR 0x0047d2b6
int card_serendib_djinn(int player, int card, event_t event)
{
  target_t land;

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (own_land_available(player) == 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    else if (select_own_land_for_cost(player, &land, "Select a land to sacrifice.") == 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    else
    {
      if (is_card_id(land.player, land.card, CARD_ID_ISLAND))
      {
        damage_player(player, 3, player, card);
      }
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(land.player, land.card, KILL_SACRIFICE);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00479c7b
// FUNCTION: SHANDALAR 0x0047d770
int card_serendib_efreet(int player, int card, event_t event)
{
  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    damage_player(player, 1, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00479e4c
// FUNCTION: SHANDALAR 0x0047d940
int card_singing_tree(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int power;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0,
                             "Select target attacking creature.", 1, &target) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0) != 0)
    {
      power = C_get_abilities(target.player, target.card, EVENT_POWER, -1);
      give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack, target.player, target.card, -power, 0);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047a22a
// FUNCTION: SHANDALAR 0x0047dd1e
int card_two_headed_giant_of_foriys(int player, int card, event_t event)
{
  card_instance_t *instance;
  int giant_copy_internal_id;
  int giant_copy_card;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (g_trigger_condition == 0xdf && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player != g_current_player && player == g_trigger_cause_controller && card == g_trigger_cause)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      giant_copy_internal_id = create_a_card_type(instance->internal_card_id);
      if (giant_copy_internal_id != -1)
      {
        global_cards_data[giant_copy_internal_id].code_pointer = card_two_headed_giant_of_foriys_legacy;
        global_cards_data[giant_copy_internal_id].extra_ability = 0;
        global_cards_data[giant_copy_internal_id].id = (unsigned short)unk_008cf1ac;

        giant_copy_card = add_card_to_hand(player, giant_copy_internal_id);
        if (giant_copy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, giant_copy_card).state = instance->state & 0xfffffff7;
          *((int *)((char *)&PLAYER_CARD_INSTANCE(player, giant_copy_card) + 0x24)) =
              *((int *)((char *)instance + 0x24));
          *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(player, giant_copy_card) + 0x14)) = 0x8000008;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).info_slot = 2;
          *((char *)&PLAYER_CARD_INSTANCE(player, giant_copy_card) + 8) = player;
          *((int *)((char *)&PLAYER_CARD_INSTANCE(player, giant_copy_card) + 0x44)) = card;
          *((int *)((char *)&PLAYER_CARD_INSTANCE(player, giant_copy_card) + 0x60)) =
              global_cards_data[instance->internal_card_id].id;
        }
      }
    }
  }

  if (g_trigger_condition == 0xd4 && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player == g_trigger_cause_controller && card == g_trigger_cause && *((char *)instance + 0x20) != -1)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
      {
        if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy && *((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 8) == player && *((int *)((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 0x44)) == card)
        {
          kill_card(player, current_card, KILL_DESTROY);
        }
      }
    }
  }

  if (event == 0x77 && g_affected_card == card && g_affected_card_controller == player && *((char *)instance + 0x20) != -1)
  {
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy && *((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 8) == player && *((int *)((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 0x44)) == card)
      {
        kill_card(player, current_card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

static int C_vesuvan_doppelganger_helper(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id);

// FUNCTION: MAGIC 0x0047a847
// FUNCTION: SHANDALAR 0x0047e344
int card_vesuvan_doppelganger(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  card_instance_t *data_card;
  card_instance_t *target_instance;
  target_t selected_target;
  int data_card_location;
  int data_card_controller;
  int data_card_slot;
  int target_data_card_location;
  int source_internal_card_id;
  int copied_internal_id;
  int lookup_player;
  int lookup_card;
  unsigned int result;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  parent = instance;
  data_card = (card_instance_t *)0;
  data_card_location = -1;
  data_card_controller = -1;
  data_card_slot = -1;
  result = 0;

  if (event == EVENT_RESOLVE_ACTIVATION && instance->parent_controller != -1 && instance->parent_card != -1)
  {
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
  }

  if (is_in_play(parent == instance ? player : instance->parent_controller,
                 parent == instance ? card : instance->parent_card))
  {
    if (parent->internal_card_id == g_stack_proxy_internal_card_id)
    {
      lookup_player = g_card_on_stack_controller;
      lookup_card = g_card_on_stack;
    }
    else if (parent == instance)
    {
      lookup_player = player;
      lookup_card = card;
    }
    else
    {
      lookup_player = instance->parent_controller;
      lookup_card = instance->parent_card;
    }

    data_card_location = dispatch_function_to_all_cards_in_play(lookup_player, lookup_card, clone_find_data_card_callback, -1);
    if (data_card_location != -1)
    {
      data_card_controller = (data_card_location >> 8) & 0xff;
      data_card_slot = data_card_location & 0xff;
      data_card = &PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot);
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
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
                                 TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("prompts.txt", "VESUVAN_DOPPELGANGER");
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
                              TARGET_SPECIAL_USE_ORIGINAL_TYPE,
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
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
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
                                TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                0,
                                0))
    {
      instance->number_of_targets = 0;
      kill_card(player, card, KILL_DESTROY);
    }
    else
    {
      selected_target = instance->targets[0];
      target_instance = &PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card);
      if (global_cards_data[target_instance->internal_card_id].id == CARD_ID_VESUVAN_DOPPELGANGER)
      {
        target_data_card_location =
            dispatch_function_to_all_cards_in_play(selected_target.player, selected_target.card, clone_find_data_card_callback, -1);
        if (target_data_card_location == -1)
        {
          source_internal_card_id = 0;
        }
        else
        {
          source_internal_card_id =
              PLAYER_CARD_INSTANCE((target_data_card_location >> 8) & 0xff, target_data_card_location & 0xff)
                  .info_slot;
        }
      }
      else
      {
        source_internal_card_id = target_instance->internal_card_id;
      }

      data_card_slot = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_DATA_CARD));
      if (data_card_slot != -1)
      {
        data_card = &PLAYER_CARD_INSTANCE(player, data_card_slot);
        data_card->state = ((((unsigned int)(((player == 0) - 1) >> 8)) & 0x1000) | STATE_IN_PLAY);
        data_card->token_status |= 0x10008;
        data_card->damage_target_player = (char)player;
        data_card->damage_target_card = card;
        copied_internal_id = create_a_card_type(source_internal_card_id);
        if (copied_internal_id != -1)
        {
          instance->internal_card_id = copied_internal_id;
          instance->dummy3 = copied_internal_id;
          instance->regen_status |= 0x1000000;
          instance->state &= 0xffffdfff;
          instance->color = global_cards_data[copied_internal_id].color;
          if ((global_cards_data[copied_internal_id].type & TYPE_ARTIFACT) != 0)
          {
            ++g_duel_summary.artifact_counts[player];
          }
          if ((global_cards_data[copied_internal_id].type & TYPE_ENCHANTMENT) != 0)
          {
            ++g_duel_summary.enchantment_counts[player];
          }
          g_card_types_in_play[player] |= global_cards_data[copied_internal_id].type;
          data_card->info_slot = source_internal_card_id;
          data_card->eot_toughness |= global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
          dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
          global_cards_data[copied_internal_id].code_pointer =
              global_cards_data[target_instance->internal_card_id].code_pointer;
          global_cards_data[copied_internal_id].extra_ability |= 1;
        }
      }
      instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & 0x20000) == 0 && card == g_affected_card && player == g_affected_card_controller && is_in_play(player, card))
  {
    g_event_result = instance->dummy3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE && player == g_current_turn && player == unk_00742f60 && data_card != (card_instance_t *)0 && (data_card->eot_toughness & 2) == 0 && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, TYPE_CREATURE, 0, 0, get_protections_from(player, card), 0, 0, -1, -1, -1, -1, TARGET_SPECIAL_USE_ORIGINAL_TYPE, 0, 0))
    {
      result = 1;
    }
  }

  if (event == EVENT_ACTIVATE && card == g_affected_card && player == g_affected_card_controller && g_current_phase == EVENT_UPKEEP_PHASE && player == g_current_turn && player == unk_00742f60 && data_card != (card_instance_t *)0 && (data_card->eot_toughness & 2) == 0)
  {
    load_text("prompts.txt", "VESUVAN_DOPPELGANGER");
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
                              TARGET_SPECIAL_USE_ORIGINAL_TYPE,
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
      data_card->eot_toughness |= 6;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && parent != instance)
  {
    if (data_card != (card_instance_t *)0 && (data_card->eot_toughness & 4) != 0)
    {
      if (C_real_validate_target(instance->targets[0].player,
                                 instance->targets[0].card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(instance->parent_controller, instance->parent_card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                 0,
                                 0))
      {
        target_instance = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
        if (global_cards_data[target_instance->internal_card_id].id == CARD_ID_VESUVAN_DOPPELGANGER)
        {
          target_data_card_location =
              dispatch_function_to_all_cards_in_play(instance->targets[0].player, instance->targets[0].card, clone_find_data_card_callback, -1);
          if (target_data_card_location == -1)
          {
            source_internal_card_id = 0;
          }
          else
          {
            source_internal_card_id =
                PLAYER_CARD_INSTANCE((target_data_card_location >> 8) & 0xff, target_data_card_location & 0xff)
                    .info_slot;
          }
        }
        else
        {
          source_internal_card_id = target_instance->internal_card_id;
        }

        copied_internal_id = create_a_card_type(source_internal_card_id);
        if (copied_internal_id != -1)
        {
          parent->internal_card_id = copied_internal_id;
          parent->dummy3 = copied_internal_id;
          parent->regen_status |= 0x1000000;
          parent->state &= 0xffffdfff;
          parent->color = global_cards_data[copied_internal_id].color;
          data_card->info_slot = source_internal_card_id;
          data_card->eot_toughness &= ~1;
          data_card->eot_toughness |= global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_function_to_all_cards_in_play(instance->parent_controller,
                                                 instance->parent_card,
                                                 C_vesuvan_doppelganger_helper,
                                                 -1);
          dispatch_event_to_single_card(instance->parent_controller,
                                        instance->parent_card,
                                        EVENT_CAST_SPELL,
                                        1 - instance->parent_controller,
                                        -1);
          dispatch_event_to_single_card(instance->parent_controller,
                                        instance->parent_card,
                                        EVENT_RESOLVE_SPELL,
                                        1 - instance->parent_controller,
                                        -1);
          global_cards_data[copied_internal_id].code_pointer =
              global_cards_data[target_instance->internal_card_id].code_pointer;
          global_cards_data[copied_internal_id].extra_ability |= 1;
        }
      }

      data_card->eot_toughness &= ~4;
    }

    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  if (event == EVENT_CLEANUP && data_card != (card_instance_t *)0)
  {
    data_card->eot_toughness &= ~2;
  }

  if (parent == instance && (instance->state & STATE_INVISIBLE) == 0 && data_card != (card_instance_t *)0 && data_card->info_slot != 0)
  {
    result |= global_cards_data[data_card->info_slot].code_pointer(player, card, event);
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c351
static int clone_find_data_card_callback(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id)
{
  if (global_cards_data[internal_card_id].id == CARD_ID_DATA_CARD && PLAYER_CARD_INSTANCE(player, card).damage_target_player == source_player && PLAYER_CARD_INSTANCE(player, card).damage_target_card == source_card)
  {
    g_event_result = player << 8 | card;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c409
static int C_vesuvan_doppelganger_helper(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id)
{
  if ((global_cards_data[internal_card_id].type & TYPE_EFFECT) && (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_PERMANENT) && PLAYER_CARD_INSTANCE(player, card).damage_source_player == source_player && PLAYER_CARD_INSTANCE(player, card).damage_source_card == source_card)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id);
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id);
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id);
// FUNCTION: MAGIC 0x0047c4e9
// FUNCTION: SHANDALAR 0x0047ffee
int card_veteran_bodyguard(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_or_martyrs_present, player) != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
    }
  }

  if (g_trigger_condition == TRIGGER_END_DAMAGE_PREV && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player != g_current_player && is_in_play(player, card) && (g_current_phase == 0x1a || g_current_phase == 0x19))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = event;
    if (event == EVENT_TRIGGER)
    {
      if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1) != -1)
      {
        g_event_result |= 2;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1);
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_mark_other_protectors, player);
    }
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c736
static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id)
{
  struct
  {
    // int pending_event;
    card_instance_t *preventer;
    int result;
    int source_card_id;
    int protects_damage;
  } s;

  s.protects_damage = 0;
  s.result = 0;
  s.source_card_id = global_cards_data[PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id].id;
  s.preventer = &PLAYER_CARD_INSTANCE(player, card);

  if (g_damage_card_internal_card_id == internal_card_id && s.preventer->info_slot != 0 && s.preventer->damage_target_player == source_player && s.preventer->damage_target_card == -1 && ((s.source_card_id == CARD_ID_VETERAN_BODYGUARD && (s.preventer->token_status & 0x140000) != 0) || (s.source_card_id == CARD_ID_MARTYRS_OF_KORLIS && (s.preventer->eot_toughness & 0x40) != 0)))
  {
    s.protects_damage = 1;
  }

  if (s.protects_damage)
  {
    switch (PLAYER_CARD_INSTANCE(source_player, source_card).info_slot)
    {
    case EVENT_TRIGGER:
      s.result = 1;
      g_event_result = 2;
      break;
    case EVENT_RESOLVE_TRIGGER:
      s.preventer->damage_target_player = (char)source_player;
      s.preventer->damage_target_card = source_card;
      break;
    default:
      break;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0047c8ca
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id)
{
  (void)unused;

  if ((global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS) && card != source_card)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c948
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id)
{
  int result;

  (void)unused1;
  (void)unused2;
  (void)unused3;
  (void)unused4;

  result = 0;
  if (global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS)
  {
    result = 1;
    g_event_result = result;
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c9a2
// FUNCTION: SHANDALAR 0x004804a4
int card_wyluli_wolf(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0,
                             "Select target creature.", 1, &target) == 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target = instance->targets[0];
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      give_pump_until_eot(g_card_on_stack_controller, g_card_on_stack, target.player, target.card, 1, 1);
    }
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047cde7
// FUNCTION: SHANDALAR 0x004808e9
int card_yawgmoth_demon(int player, int card, event_t event)
{
  target_t artifact;

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    if (own_artifact_available(player) != 0 &&
        select_own_artifact_for_cost(player, card, &artifact, "Select an artifact to sacrifice.") != 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      damage_player(player, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047d194
// FUNCTION: SHANDALAR 0x00480c9e
int card_ydwen_efreet(int player, int card, event_t event)
{
  if (g_trigger_condition == TRIGGER_BLOCKER_CHOSEN &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card &&
      g_current_turn != player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER && internal_rand(2) == 0)
    {
      g_combat_assignment_cancelled = 1;
    }
  }

  return 0;
}
