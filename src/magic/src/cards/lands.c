#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

extern card_ptr_t global_raw_cards_storage[2000];

// GLOBAL: SHANDALAR 0x0097c858
static int assembly_worker_cleanup_player;

// GLOBAL: SHANDALAR 0x0097c854
static int assembly_worker_cleanup_card;

static int __cdecl assembly_worker_cleanup_attached_aura(int test_player,
                                                         int test_card,
                                                         int test_internal_card_id);

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

  if (event == EVENT_UNTAP_PHASE)
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
      (global_card_instances[player])[card].mana_color = (char)g_ai_land_score_modifier;
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
  target_t target;
  int can_activate;
  int done;

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_other_player)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = (g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 ||
        is_animated_and_sick(player, card))
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

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0)
  {
    if (g_required_mana_color_mask != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      g_spell_fizzled = 1;
      return 0;
    }

    done = 0;
    while (!done)
    {
      load_text("prompts.txt", "OASIS");
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
        g_spell_fizzled = 1;
        done = 1;
      }
      else if (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card == -1)
      {
        if (g_duel_ai_mode_state != 1 && player != g_other_player)
        {
          set_duel_prompt_text(g_text_lines[1]);
          Sleep(2000);
          set_duel_prompt_text("");
        }
      }
      else
      {
        done = 1;
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        g_produced_mana_color = -1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION &&
      PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
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
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot > 0)
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CHECK_PUMP &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    ++g_global_toughness_bonus[player];
  }

  return 0;
}
// FUNCTION: MAGIC 0x004b6bd6
// FUNCTION: SHANDALAR 0x0042a995
int card_strip_mine(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    char dialog[0x384];
    int choice;
    int result;
  } s;

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "STRIPMINE");
    }
    if (g_produced_mana_color_valid == 0)
    {
      s.choice = 0;
    }
    else if (g_required_mana_color_mask != 0)
    {
      s.choice = 0;
    }
    else
    {
      if (g_active_player == player || (g_duel_network_flags & 2) != 0)
      {
        if (g_duel_ai_mode_state != 1)
        {
          sprintf(s.dialog, " %s\n %s\n %s", g_text_lines[1], g_text_lines[2], g_text_lines[3]);
        }
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
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      g_produced_mana_color = -1;
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "STRIPMINE");
      }
      if (select_target_land_and_store(player, 1 - player, card) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0xf);
        }
        kill_card(player, card, KILL_SACRIFICE);
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    return s.result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.target.player,
                               s.target.card,
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
                               0) != 0)
    {
      kill_card(s.target.player, s.target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }

    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_RESOLVE_SPELL)
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
    int default_action;
    int action;
  } s;

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
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
        sprintf(s.dialog, " %s\n _%s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      }
      s.default_action = 0;
    }

    if (g_produced_mana_color_valid == 0)
    {
      s.action = 0;
    }
    else if (g_required_mana_color_mask != 0)
    {
      s.action = 0;
    }
    else if (player == g_active_player || (g_duel_network_flags & 2) != 0)
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
      return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
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
    if (PARENT_CARD_INSTANCE(player, card).info_slot == 1)
    {
      PARENT_CARD_INSTANCE(player, card).info_slot = 0;
      draw_card_for_player(player);
    }
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

#pragma inline_depth(255)
#pragma inline_recursion(on)
static __inline int mishras_factory_common(int player, int card, event_t event, int is_assembly_worker)
{
  struct
  {
    int available[4];
    char dialog[0x4ac];
    target_t target;
    int choice;
    int default_choice;
    int can_tap;
    int legacy_card;
    unsigned int saved_no_auto_tapping;
  } s;

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  s.can_tap = (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (s.can_tap ||
        (!is_assembly_worker &&
         has_mana(player, COLOR_COLORLESS, 1) &&
         ((player != g_other_player || (g_duel_network_flags & 2) != 0) ||
          g_current_phase < PHASE_DECLARE_BLOCKERS)) ||
        (is_assembly_worker &&
         (g_active_player == player || (g_duel_network_flags & 2) != 0) &&
         g_duel_ai_mode_state != 1 &&
         g_duel_network_state == 0 &&
         has_mana(player, COLOR_COLORLESS, 1)))
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_ai_speculation_depth > 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", is_assembly_worker ? "ASSEMBLY_WORKER" : "MISHRAS_FACTORY");
    }

    s.default_choice = 3;
    if (s.can_tap)
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n", g_text_lines[0]);
      }
      s.available[0] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " _%s\n", g_text_lines[0]);
      }
      s.available[0] = 0;
    }

    s.available[1] = has_mana(player, COLOR_COLORLESS, 1);
    if (s.available[1])
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[1]);
      }
      if (!is_assembly_worker && g_current_phase < PHASE_DECLARE_BLOCKERS)
      {
        s.default_choice = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[1]);
      }
    }

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
                                           find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                                           -1,
                                           -1,
                                           -1,
                                           0,
                                           0,
                                           0);
    if (s.available[2])
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[2]);
      }
      if (g_current_phase > PHASE_BEFORE_BLOCKING && g_current_phase < PHASE_MAIN2)
      {
        s.default_choice = 2;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[2]);
      }
    }

    s.available[3] = 1;
    if (g_duel_ai_mode_state != 1)
    {
      sprintf(s.dialog + strlen(s.dialog), " %s", g_text_lines[3]);
    }

    if (g_required_mana_color_mask != 0)
    {
      s.choice = 0;
    }
    else
    {
      do
      {
        s.choice = do_dialog(player, player, card, -1, -1, s.dialog, s.default_choice);
      } while (!s.available[s.choice]);
    }

    PLAYER_CARD_INSTANCE(player, card).info_slot = s.choice;
    if (s.choice == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
    }
    else if (s.choice == 1)
    {
      s.saved_no_auto_tapping = PLAYER_CARD_INSTANCE(player, card).state & STATE_NO_AUTO_TAPPING;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_NO_AUTO_TAPPING;
      charge_mana(player, COLOR_COLORLESS, 1);
      if (s.saved_no_auto_tapping == 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_NO_AUTO_TAPPING;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      g_produced_mana_color = -1;
    }
    else if (s.choice == 2)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", is_assembly_worker ? "ASSEMBLY_WORKER" : "MISHRAS_FACTORY");
      }
      if (C_real_select_target(player,
                               2,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
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
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
      g_produced_mana_color = -1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_ai_speculation_depth > 0)
    {
      --g_ai_speculation_depth;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PARENT_CARD_INSTANCE(player, card).internal_card_id == -1 ||
        PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
    {
      return 0;
    }

    if (PLAYER_CARD_INSTANCE(player, card).info_slot == 1)
    {
      PARENT_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER);
      PARENT_CARD_INSTANCE(player, card).dummy3 = PARENT_CARD_INSTANCE(player, card).internal_card_id;
      ++g_duel_summary.creature_counts[PLAYER_CARD_INSTANCE(player, card).parent_controller];
      ++g_duel_summary.artifact_counts[PLAYER_CARD_INSTANCE(player, card).parent_controller];
      PARENT_CARD_INSTANCE(player, card).state |= STATE_IN_PLAY;
      dispatch_event_to_single_card(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card,
                                    EVENT_CAST_SPELL,
                                    1 - PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    -1);
      PARENT_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK;
      dispatch_event_to_single_card(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card,
                                    EVENT_RESOLVE_SPELL,
                                    1 - PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    -1);
    }
    else if (PLAYER_CARD_INSTANCE(player, card).info_slot == 2 && PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
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
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0))
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
      else
      {
        g_spell_fizzled = 1;
      }
    }
    PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL || event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}
#pragma inline_depth()
#pragma inline_recursion(off)

// FUNCTION: MAGIC 0x004b74b2
// FUNCTION: SHANDALAR 0x0042b270
int card_mishra_s_factory(int player, int card, event_t event)
{
  struct
  {
    int default_choice;
    int available[4];
    int result;
    unsigned int saved_no_auto_tapping;
    int can_tap;
    target_t target;
    char dialog[0x4ac];
    int unused;
    int legacy_card;
    int choice;
  } s;

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    s.can_tap = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card))
    {
      s.can_tap = 1;
    }
    if (s.can_tap ||
        (has_mana(player, COLOR_ANY, 1) &&
         ((player != g_other_player || (g_duel_network_flags & 2) != 0) ||
          g_current_phase < PHASE_DECLARE_BLOCKERS)))
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_ai_speculation_depth > 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "MISHRAS_FACTORY");
    }

    s.default_choice = 3;
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n", g_text_lines[1]);
      }
      s.available[0] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " _%s\n", g_text_lines[1]);
      }
      s.available[0] = 0;
    }

    if (has_mana(player, COLOR_ANY, 1))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[2]);
      }
      if (g_current_phase < PHASE_DECLARE_BLOCKERS)
      {
        s.default_choice = 1;
      }
      s.available[1] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[2]);
      }
      s.available[1] = 0;
    }

    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        !is_animated_and_sick(player, card) &&
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
                              find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                              -1,
                              -1,
                              -1,
                              0,
                              0,
                              0))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[3]);
      }
      if (g_current_phase < PHASE_MAIN2 && g_current_phase >= PHASE_DECLARE_BLOCKERS)
      {
        s.default_choice = 2;
      }
      s.available[2] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[3]);
      }
      s.available[2] = 0;
    }

    s.available[3] = 1;
    if (g_duel_ai_mode_state != 1)
    {
      sprintf(s.dialog + strlen(s.dialog), " %s", g_text_lines[4]);
    }

    if (g_required_mana_color_mask != 0)
    {
      s.choice = 0;
    }
    else
    {
      do
      {
        s.choice = do_dialog(player, player, card, -1, -1, s.dialog, s.default_choice);
      } while (!s.available[s.choice]);
    }

    PLAYER_CARD_INSTANCE(player, card).info_slot = s.choice;
    if (s.choice == 0)
    {
      s.result = mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      s.saved_no_auto_tapping = PLAYER_CARD_INSTANCE(player, card).state & STATE_NO_AUTO_TAPPING;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_NO_AUTO_TAPPING;
      charge_mana(player, COLOR_COLORLESS, 1);
      if (s.saved_no_auto_tapping == 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_NO_AUTO_TAPPING;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      g_produced_mana_color = -1;
    }
    else if (s.choice == 2)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "MISHRAS_FACTORY");
      }
      if (C_real_select_target(player,
                               2,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
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
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
      g_produced_mana_color = -1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_ai_speculation_depth > 0)
    {
      --g_ai_speculation_depth;
    }
    return s.result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PARENT_CARD_INSTANCE(player, card).internal_card_id != -1 &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      s.choice = PLAYER_CARD_INSTANCE(player, card).info_slot;
      if (s.choice == 1)
      {
        PARENT_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER);
        PARENT_CARD_INSTANCE(player, card).dummy3 = PARENT_CARD_INSTANCE(player, card).internal_card_id;
        ++g_duel_summary.creature_counts[g_card_on_stack_controller];
        ++g_duel_summary.artifact_counts[g_card_on_stack_controller];
        PARENT_CARD_INSTANCE(player, card).state |= STATE_IN_PLAY;
        dispatch_event_to_single_card(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      EVENT_CAST_SPELL,
                                      1 - g_card_on_stack_controller,
                                      -1);
        PARENT_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK;
        dispatch_event_to_single_card(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      EVENT_RESOLVE_SPELL,
                                      1 - g_card_on_stack_controller,
                                      -1);
      }
      else if (s.choice == 2 && PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
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
                                   get_protections_from(player, card),
                                   COLOR_TEST_0,
                                   COLOR_TEST_0,
                                   find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                                   -1,
                                   -1,
                                   -1,
                                   0,
                                   0,
                                   0))
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
        else
        {
          g_spell_fizzled = 1;
        }
      }
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    return 0;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x18;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
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

  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b85f5
// FUNCTION: SHANDALAR 0x0042c3ae
int card_assembly_worker(int player, int card, event_t event)
{
  struct
  {
    int default_choice;
    int available[4];
    int result;
    unsigned int saved_no_auto_tapping;
    int can_tap;
    target_t target;
    char dialog[0x4ac];
    int unused;
    int legacy_card;
    int choice;
  } s;

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(CARD_ID_MISHRAS_FACTORY);
    PLAYER_CARD_INSTANCE(player, card).dummy3 = 0;
    --g_duel_summary.artifact_counts[player];
    --g_duel_summary.creature_counts[player];
    assembly_worker_cleanup_player = player;
    assembly_worker_cleanup_card = card;
    dispatch_three_arg_callback_to_cards_in_play(assembly_worker_cleanup_attached_aura, -1);
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(CARD_ID_MISHRAS_FACTORY);
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    s.can_tap = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card))
    {
      s.can_tap = 1;
    }
    if (s.can_tap ||
        ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
         g_duel_ai_mode_state != 1 &&
         g_duel_network_state == 0 &&
         has_mana(player, COLOR_ANY, 1)))
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_ai_speculation_depth > 0)
      {
        g_activation_event_flags |= 3;
      }
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    s.result = 0;
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ASSEMBLY_WORKER");
    }

    s.default_choice = 3;
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && !is_animated_and_sick(player, card))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " %s\n", g_text_lines[1]);
      }
      s.available[0] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog, " _%s\n", g_text_lines[1]);
      }
      s.available[0] = 0;
    }

    if (has_mana(player, COLOR_ANY, 1))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[2]);
      }
      s.available[1] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[2]);
      }
      s.available[1] = 0;
    }

    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
        !is_animated_and_sick(player, card) &&
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
                              find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                              -1,
                              -1,
                              -1,
                              0,
                              0,
                              0))
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " %s\n", g_text_lines[3]);
      }
      if (g_current_phase < PHASE_MAIN2 && g_current_phase >= PHASE_DECLARE_BLOCKERS)
      {
        s.default_choice = 2;
      }
      s.available[2] = 1;
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        sprintf(s.dialog + strlen(s.dialog), " _%s\n", g_text_lines[3]);
      }
      s.available[2] = 0;
    }

    s.available[3] = 1;
    if (g_duel_ai_mode_state != 1)
    {
      sprintf(s.dialog + strlen(s.dialog), " %s", g_text_lines[4]);
    }

    if (g_required_mana_color_mask != 0)
    {
      s.choice = 0;
    }
    else
    {
      do
      {
        s.choice = do_dialog(player, player, card, -1, -1, s.dialog, s.default_choice);
      } while (!s.available[s.choice]);
    }

    PLAYER_CARD_INSTANCE(player, card).info_slot = s.choice;
    if (s.choice == 0)
    {
      s.result = mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else if (s.choice == 1)
    {
      s.saved_no_auto_tapping = PLAYER_CARD_INSTANCE(player, card).state & STATE_NO_AUTO_TAPPING;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_NO_AUTO_TAPPING;
      charge_mana(player, COLOR_COLORLESS, 1);
      if (s.saved_no_auto_tapping == 0)
      {
        PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_NO_AUTO_TAPPING;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      g_produced_mana_color = -1;
    }
    else if (s.choice == 2)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ASSEMBLY_WORKER");
      }
      if (C_real_select_target(player,
                               2,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
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
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        undeclare_mana_available(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        g_spell_fizzled = 1;
      }
      g_produced_mana_color = -1;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_ai_speculation_depth > 0)
    {
      --g_ai_speculation_depth;
    }
    return s.result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PARENT_CARD_INSTANCE(player, card).internal_card_id != -1 &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      s.choice = PLAYER_CARD_INSTANCE(player, card).info_slot;
      if (s.choice == 1)
      {
        PARENT_CARD_INSTANCE(player, card).internal_card_id = find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER);
        PARENT_CARD_INSTANCE(player, card).dummy3 = PARENT_CARD_INSTANCE(player, card).internal_card_id;
        ++g_duel_summary.creature_counts[g_card_on_stack_controller];
        ++g_duel_summary.artifact_counts[g_card_on_stack_controller];
        PARENT_CARD_INSTANCE(player, card).state |= STATE_IN_PLAY;
        dispatch_event_to_single_card(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      EVENT_CAST_SPELL,
                                      1 - g_card_on_stack_controller,
                                      -1);
        PARENT_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK;
        dispatch_event_to_single_card(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      EVENT_RESOLVE_SPELL,
                                      1 - g_card_on_stack_controller,
                                      -1);
      }
      else if (s.choice == 2 && PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
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
                                   get_protections_from(player, card),
                                   COLOR_TEST_0,
                                   COLOR_TEST_0,
                                   find_internal_card_id_by_csv_id(CARD_ID_ASSEMBLY_WORKER),
                                   -1,
                                   -1,
                                   -1,
                                   0,
                                   0,
                                   0))
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
          else
          {
            g_spell_fizzled = 1;
          }
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      PARENT_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    return 0;
  }

  if (event == EVENT_COUNT_MANA)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    return mana_producer_sound_on_resolve(player, card, event, COLOR_COLORLESS);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0042d474
static int __cdecl assembly_worker_cleanup_attached_aura(int test_player,
                                                         int test_card,
                                                         int test_internal_card_id)
{
  if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player ==
          assembly_worker_cleanup_player &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == assembly_worker_cleanup_card &&
      (global_cards_data[test_internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
      global_raw_cards_storage[global_cards_data[test_internal_card_id].id].subtype != 0x6c)
  {
    kill_card(test_player, test_card, KILL_DESTROY);
  }

  return 0;
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
