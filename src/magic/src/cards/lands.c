#include <stdio.h>
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
      ((event == EVENT_TAP_CARD || event == EVENT_PLAY_ABILITY) && g_affected_card == card &&
       g_affected_card_controller == player))
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x31);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      (global_card_instances[player])[card].mana_color = (char)(1 << (unsigned char)(network_random(player, 5) + 1));
    }
    else if (player == g_active_player)
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
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;
  int can_activate;

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
    can_activate = (g_land_can_be_played & 4) != 0;
    if ((instance->state & STATE_TAPPED) != 0 || is_animated_and_sick(player, card))
    {
      can_activate = 0;
    }
    if (can_activate)
    {
      can_activate = real_target_available((int *)0,
                                           TARGET_SCAN_DAMAGE_TARGET,
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
                                           0);
    }
    return can_activate ? 99 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & 4) != 0)
  {
    if (unk_00938e2c != 0)
    {
      instance->number_of_targets = 0;
      g_spell_fizzled = 1;
      return 0;
    }

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
                              -1,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              "Select damage to a creature.",
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
      g_duel_special_land_card_ids[6] = -1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && instance->number_of_targets != 0)
  {
    target = instance->targets[0];
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
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot < 1)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CHECK_PUMP && (instance->state & STATE_TAPPED) == 0)
  {
    ++unk_007a7d88[player];
  }

  return 0;
}
// FUNCTION: MAGIC 0x004b6bd6
// FUNCTION: SHANDALAR 0x0042a995
int card_strip_mine(int player, int card, event_t event)
{
  struct
  {
    char dialog[600];
    int choice;
    int result;
  } s;
  card_instance_t *instance;
  card_instance_t *parent;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_COUNT_MANA || event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    s.choice = 0;
    if (DAT_007aadf0 != 0 && unk_00938e2c == 0)
    {
      if (player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        sprintf(s.dialog, " Add colorless mana\n Destroy target land\n Cancel");
        s.choice = do_dialog(player, player, card, -1, -1, s.dialog, 1);
      }
      else
      {
        s.choice = 1;
      }
    }

    if (s.choice == 0)
    {
      s.result = mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
      instance->number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      g_duel_special_land_card_ids[6] = -1;
      if (!select_target_land_and_store(player, 1 - player, card))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->state |= STATE_TAPPED;
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0xf);
        }
        kill_card(player, card, KILL_SACRIFICE);
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
      }
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
                                TYPE_LAND,
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
      kill_card(target.player, target.card, KILL_DESTROY);
    }

    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE || event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

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

    if (g_duel_summary.hand_counts[player] == 7)
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      }
      s.default_action = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      }
      s.default_action = 0;
    }

    if (((g_other_player == player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
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
      g_produced_mana_color = -1;
    }
    else
    {
      g_spell_fizzled = 1;
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

static __inline int mishras_factory_common(int player, int card, event_t event, int is_assembly_worker)
{
  struct
  {
    int available[4];
    char dialog[600];
    target_t target;
    int choice;
    int default_choice;
    int can_tap;
    int assembly_worker_iid;
    int legacy_card;
    unsigned int saved_no_auto_tapping;
  } s;
  card_instance_t *instance;
  card_instance_t *parent;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  s.assembly_worker_iid = find_internal_card_id_by_csv_id(0x38e);

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  s.can_tap = (instance->state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (s.can_tap || has_mana(player, COLOR_COLORLESS, 1))
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && unk_00939330 > 0)
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    s.available[0] = s.can_tap;
    s.available[1] = has_mana(player, COLOR_COLORLESS, 1);
    s.available[2] = s.can_tap &&
                     real_target_available((int *)0,
                                           TARGET_SCAN_DIRECT,
                                           player,
                                           2,
                                           2,
                                           TARGET_ZONE_IN_PLAY,
                                           TYPE_NONE,
                                           TYPE_NONE,
                                           0,
                                           get_protections_from(player, card),
                                           COLOR_TEST_0,
                                           COLOR_TEST_0,
                                           s.assembly_worker_iid,
                                           -1,
                                           -1,
                                           -1,
                                           0,
                                           0,
                                           0);
    s.available[3] = 1;

    s.default_choice = 0;
    if (s.available[1] && g_current_phase < PHASE_DECLARE_BLOCKERS)
    {
      s.default_choice = 1;
    }
    if (s.available[2] && g_current_phase > PHASE_BEFORE_BLOCKING && g_current_phase < PHASE_MAIN2)
    {
      s.default_choice = 2;
    }
    if (!s.available[s.default_choice])
    {
      for (s.default_choice = 0; s.default_choice < 4 && !s.available[s.default_choice]; ++s.default_choice)
      {
      }
    }

    sprintf(s.dialog,
            " Add colorless mana\n Animate %s\n Give target Assembly Worker +1/+1\n Cancel",
            is_assembly_worker ? "Assembly Worker" : "Mishra's Factory");

    if (unk_00938e2c != 0)
    {
      s.choice = 0;
    }
    else
    {
      do
      {
        s.choice = do_dialog(player, player, card, -1, -1, s.dialog, s.default_choice);
      } while (s.choice < 0 || s.choice > 3 || !s.available[s.choice]);
    }

    instance->info_slot = s.choice;
    if (s.choice == 0)
    {
      instance->number_of_targets = 0;
      return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
    }
    else if (s.choice == 1)
    {
      s.saved_no_auto_tapping = instance->state & STATE_NO_AUTO_TAPPING;
      instance->state |= STATE_NO_AUTO_TAPPING;
      charge_mana(player, COLOR_COLORLESS, 1);
      if (s.saved_no_auto_tapping == 0)
      {
        instance->state &= ~STATE_NO_AUTO_TAPPING;
      }
      instance->number_of_targets = 0;
      g_duel_special_land_card_ids[6] = -1;
    }
    else if (s.choice == 2)
    {
      if (!C_real_select_target(player,
                                2,
                                player,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                s.assembly_worker_iid,
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                "Select target Assembly Worker.",
                                1,
                                &s.target))
      {
        g_spell_fizzled = 1;
      }
      else
      {
        instance->state |= STATE_TAPPED;
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
        instance->targets[0] = s.target;
        instance->number_of_targets = 1;
      }
      g_duel_special_land_card_ids[6] = -1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (unk_00939330 > 0)
    {
      --unk_00939330;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    if (parent->internal_card_id == -1 || instance->info_slot == 0)
    {
      return 0;
    }

    if (instance->info_slot == 1)
    {
      parent->internal_card_id = s.assembly_worker_iid;
      parent->dummy3 = s.assembly_worker_iid;
      ++g_duel_summary.creature_counts[instance->parent_controller];
      ++g_creature_count_summary[instance->parent_controller];
      parent->state |= STATE_IN_PLAY;
      dispatch_event_to_single_card(instance->parent_controller,
                                    instance->parent_card,
                                    EVENT_CAST_SPELL,
                                    1 - instance->parent_controller,
                                    -1);
      parent->state |= STATE_SUMMONSICK;
      dispatch_event_to_single_card(instance->parent_controller,
                                    instance->parent_card,
                                    EVENT_RESOLVE_SPELL,
                                    1 - instance->parent_controller,
                                    -1);
    }
    else if (instance->info_slot == 2 && instance->number_of_targets != 0)
    {
      s.target = instance->targets[0];
      if (!C_real_validate_target(s.target.player,
                                  s.target.card,
                                  (char *)0,
                                  player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_NONE,
                                  TYPE_NONE,
                                  0,
                                  get_protections_from(player, card),
                                  COLOR_TEST_0,
                                  COLOR_TEST_0,
                                  s.assembly_worker_iid,
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
        s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                             g_card_on_stack,
                                             LEGACY_EFFECT_PUMP,
                                             s.target.player,
                                             s.target.card);
        if (s.legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).counter_toughness = 1;
        }
      }
    }
    parent->number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL || event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b74b2
// FUNCTION: SHANDALAR 0x0042b270
int card_mishra_s_factory(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_CHANGE_TYPE &&
      (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) == 0 &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      is_in_play(player, card) &&
      PLAYER_CARD_INSTANCE(player, card).dummy3 != 0)
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    g_ai_modifier += player == g_other_player ? 0x30 : -0x30;
  }

  return mishras_factory_common(player, card, event, 0);
}

// FUNCTION: MAGIC 0x004b85f5
// FUNCTION: SHANDALAR 0x0042c3ae
int card_assembly_worker(int player, int card, event_t event)
{
  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(0x1fc);
    PLAYER_CARD_INSTANCE(player, card).dummy3 = 0;
    --g_duel_summary.creature_counts[player];
    --g_creature_count_summary[player];
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(0x1fc);
    return 0;
  }

  return mishras_factory_common(player, card, event, 1);
}

// FUNCTION: MAGIC 0x004b978b
// FUNCTION: SHANDALAR 0x0042d548
int card_mishra_s_workshop(int player, int card, event_t event)
{
  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
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
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_ARTIFACT, 3);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_ARTIFACT;
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
  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
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
    g_produced_mana_color = color;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL && g_duel_ai_mode_state != 1)
  {
    play_sound_effect(color + WAV_GREY);
  }

  return 0;
}
