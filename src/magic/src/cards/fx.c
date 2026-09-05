#include "defs.h"
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int gain_control(int player, int card);
int rewire_control_aura_source(int player, int card, int internal_card_id);
int assign_blocker_to_attacker(int blocker_player, int blocker_card, int attacker_player, int attacker_card);

// FUNCTION: MAGIC 0x0047d4df
// FUNCTION: SHANDALAR 0x00480feb
int card_beast_FX(int player, int card, event_t event)
{
  int found_other;
  int current_card;

  if ((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player != player &&
      (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_DYING) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  if ((event == EVENT_GRAVEYARD_FROM_PLAY &&
       g_affected_card == card &&
       g_affected_card_controller == player) ||
      (event == EVENT_TAP_CARD &&
       (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_affected_card_controller &&
       PLAYER_CARD_INSTANCE(player, card).damage_source_card == g_affected_card))
  {
    current_card = 0;
    found_other = 0;
    for (;current_card < g_active_cards_count[player] && found_other == 0; current_card++)
    {
      if (PLAYER_CARD_INSTANCE(player, current_card).internal_card_id == g_control_aura_legacy_internal_card_id &&
          current_card != card &&
          (int)PLAYER_CARD_INSTANCE(player, current_card).damage_target_player ==
              (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
          PLAYER_CARD_INSTANCE(player, current_card).damage_target_card ==
              PLAYER_CARD_INSTANCE(player, card).damage_target_card)
      {
        found_other = 1;
      }
    }

    if (found_other == 0)
    {
      if (event == EVENT_GRAVEYARD_FROM_PLAY)
      {
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .token_status &= ~STATUS_CANNOT_BE_DESTROYED;
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .regen_status &= ~0x20000;
      }

      if (PLAYER_CARD_INSTANCE(player, card).targets[0].card != -1)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .token_status |= STATUS_CONTROLLED;
        if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player !=
            PLAYER_CARD_INSTANCE(player, card).targets[0].player)
        {
          current_card = gain_control((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                      PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
              .damage_target_player = PLAYER_CARD_INSTANCE(player, card).damage_target_player;
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
              .damage_target_card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047db7e
// FUNCTION: SHANDALAR 0x00481688
int card_blaze_FX(int player, int card, event_t event)
{
  int blocker_player;
  int copy_card;
  int copy_internal_card_id;
  int copied_count;
  int current_card;
  int done;
  int original_blocking;
  int target_card;
  int target_player;
  int test_blocking;

  copied_count = 0;
  done = 0;
  copy_internal_card_id = -1;
  target_player = (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
  target_card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;

  if (g_trigger_condition == TRIGGER_MUST_BLOCK &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_BLOCKING) == 0)
      {
        blocker_player = 1 - target_player;
        current_card = 0;
        while (current_card < g_active_cards_count[blocker_player] && done == 0)
        {
          if (is_in_play(blocker_player, current_card) &&
              (PLAYER_CARD_INSTANCE(blocker_player, current_card).state & STATE_ATTACKING) != 0 &&
              assign_blocker_to_attacker(target_player, target_card, blocker_player, current_card) != 0)
          {
            original_blocking = (int)(char)PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking;
            if (original_blocking == -1)
            {
              PLAYER_CARD_INSTANCE(target_player, target_card).blocking = (char)current_card;
            }
            else
            {
              PLAYER_CARD_INSTANCE(target_player, target_card).blocking =
                  PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking;
            }
            PLAYER_CARD_INSTANCE(target_player, target_card).state |= STATE_BLOCKING;
            if (g_duel_ai_mode_state != 1 && original_blocking == -1)
            {
              play_sound_effect(WAV_BLOCK2);
            }
            ++copied_count;
            done = 1;
          }
          ++current_card;
        }
      }

      if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_BLOCKING) != 0)
      {
        original_blocking = (int)(char)PLAYER_CARD_INSTANCE(target_player, target_card).blocking;
        blocker_player = 1 - target_player;
        for (current_card = 0; current_card < g_active_cards_count[blocker_player]; ++current_card)
        {
          if (is_in_play(blocker_player, current_card) &&
              (PLAYER_CARD_INSTANCE(blocker_player, current_card).state & STATE_ATTACKING) != 0 &&
              assign_blocker_to_attacker(target_player, target_card, blocker_player, current_card) != 0)
          {
            test_blocking = (int)(char)PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking;
            if (test_blocking == -1)
            {
              test_blocking = current_card;
            }

            if (original_blocking != test_blocking &&
                ((int)(char)PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking == -1 ||
                 (int)(char)PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking == current_card))
            {
              if (copy_internal_card_id == -1)
              {
                copy_internal_card_id =
                    create_a_card_type(PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id);
                if (copy_internal_card_id != -1)
                {
                  global_cards_data[copy_internal_card_id].code_pointer = card_two_headed_giant_of_foriys_legacy;
                  global_cards_data[copy_internal_card_id].extra_ability = 0;
                  global_cards_data[copy_internal_card_id].id = g_multiblock_display_internal_card_id;
                }
              }

              if (copy_internal_card_id != -1)
              {
                copy_card = add_card_to_hand(target_player, copy_internal_card_id);
                if (copy_card != -1)
                {
                  PLAYER_CARD_INSTANCE(target_player, copy_card).state =
                      PLAYER_CARD_INSTANCE(target_player, target_card).state & ~STATE_BLOCKING;
                  PLAYER_CARD_INSTANCE(target_player, copy_card).regen_status =
                      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status;
                  PLAYER_CARD_INSTANCE(target_player, copy_card).token_status =
                      STATUS_SPECIAL_BLOCKER | STATUS_OBLITERATED;
                  PLAYER_CARD_INSTANCE(target_player, copy_card).damage_source_player = (char)target_player;
                  PLAYER_CARD_INSTANCE(target_player, copy_card).damage_source_card = target_card;
                  if ((int)(char)PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking == -1)
                  {
                    PLAYER_CARD_INSTANCE(target_player, copy_card).blocking = (char)current_card;
                  }
                  else
                  {
                    PLAYER_CARD_INSTANCE(target_player, copy_card).blocking =
                        PLAYER_CARD_INSTANCE(blocker_player, current_card).blocking;
                  }
                  PLAYER_CARD_INSTANCE(target_player, copy_card).state |= STATE_BLOCKING;
                  PLAYER_CARD_INSTANCE(target_player, copy_card).display_pic_info =
                      global_cards_data[PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id].id;
                  ++copied_count;
                  g_battlefield_extra_ability_flags |= 4;
                }
              }
            }
          }
        }

        PLAYER_CARD_INSTANCE(target_player, target_card).blocking = (char)original_blocking;
        PLAYER_CARD_INSTANCE(target_player, target_card).state |= STATE_BLOCKING;
      }

      if (copied_count > 1)
      {
        for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
        {
          if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id == copy_internal_card_id)
          {
            PLAYER_CARD_INSTANCE(target_player, current_card).timestamp = copied_count;
          }
        }
        PLAYER_CARD_INSTANCE(target_player, target_card).token_status |= STATUS_SPECIAL_BLOCKER;
      }

      TENTATIVE_reassess_all_cards(0, 0xff);
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_BURY);
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_turn == player &&
      target_player == g_trigger_cause_controller &&
      target_card == g_trigger_cause)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }

    if (event == EVENT_RESOLVE_TRIGGER &&
        (int)(char)PLAYER_CARD_INSTANCE(target_player, target_card).blocking != -1)
    {
      for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
      {
        if (is_in_play(target_player, current_card) &&
            global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy &&
            (int)PLAYER_CARD_INSTANCE(target_player, current_card).damage_source_player == target_player &&
            PLAYER_CARD_INSTANCE(target_player, current_card).damage_source_card == target_card)
        {
          kill_card(target_player, current_card, KILL_DESTROY);
        }
      }
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      target_card == g_affected_card &&
      target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (int)(char)PLAYER_CARD_INSTANCE(target_player, target_card).blocking != -1)
  {
    for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
    {
      if (is_in_play(target_player, current_card) &&
          global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy &&
          (int)PLAYER_CARD_INSTANCE(target_player, current_card).damage_source_player == target_player &&
          PLAYER_CARD_INSTANCE(target_player, current_card).damage_source_card == target_card)
      {
        kill_card(target_player, current_card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047eece
// FUNCTION: SHANDALAR 0x004829d8
int card_control_FX(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_CONTROLLED) != 0)
    {
      if ((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player == -1)
      {
        if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                    .internal_card_id != -1 &&
            (((PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                    PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                   .state &
               STATE_POWER_STRUGGLE) != 0 &&
              (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_active_player) ||
             ((PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                    PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                   .state &
               STATE_POWER_STRUGGLE) == 0 &&
              (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_other_player)))
        {
          gain_control((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                       PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
            .token_status |= STATUS_CONTROLLED;
        if (PLAYER_CARD_INSTANCE(player, card).damage_source_player !=
            PLAYER_CARD_INSTANCE(player, card).damage_target_player)
        {
          gain_control((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                       PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        }
      }
    }
    else
    {
      dispatch_three_arg_callback_to_cards_in_play(rewire_control_aura_source, -1);
    }
  }

  if (event == EVENT_ATTACK_RATING &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_ai_score -= 0x18;
  }

  if (event == EVENT_BLOCK_RATING &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_ai_score += 0x18;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047f4c4
// FUNCTION: SHANDALAR 0x00482fd0
int card_c_tomb_FX(int player, int card, event_t event)
{
  int current_card;
  int current_player;
  int found;
  int total;
  target_t selected;

  if (g_current_phase == PHASE_UPKEEP &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == -1 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == -1)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
    selected.player = -1;
    selected.card = -1;
    total = 0;

    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) &&
            PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == g_duel_generated_internal_card_id_23 &&
            PLAYER_CARD_INSTANCE(current_player, current_card).info_slot == PLAYER_CARD_INSTANCE(player, card).info_slot)
        {
          ++total;
          if ((int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player != -1)
          {
            selected.player = current_player;
            selected.card = current_card;
          }
        }
      }
    }

    if ((g_active_player == player || (g_duel_network_flags & 2) != 0) && total > 2)
    {
      do
      {
        load_text("promptsX1.txt", "CYCLOPEAN_TOMB_EFFECT");
        C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             0, 0, 0, g_text_lines[0], 0, &selected);
        current_card = 0;
        found = 0;
        while (current_card < g_active_cards_count[player] && found == 0)
        {
          if (is_in_play(player, current_card) &&
              PLAYER_CARD_INSTANCE(player, current_card).internal_card_id == g_duel_generated_internal_card_id_23 &&
              PLAYER_CARD_INSTANCE(player, current_card).info_slot == PLAYER_CARD_INSTANCE(player, card).info_slot &&
              (int)PLAYER_CARD_INSTANCE(player, current_card).damage_target_player == selected.player &&
              PLAYER_CARD_INSTANCE(player, current_card).damage_target_card == selected.card)
          {
            found = 1;
            selected.player = player;
            selected.card = current_card;
          }
          ++current_card;
        }

        if (found == 0 && g_duel_ai_mode_state != 1)
        {
          set_duel_prompt_text(g_text_lines[1]);
          Sleep(0x9c4);
          set_duel_prompt_text("");
        }
      } while (found == 0);
    }

    if (selected.player != -1)
    {
      kill_card(selected.player, selected.card, KILL_REMOVE);
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(selected.player, selected.card).damage_target_player,
                           PLAYER_CARD_INSTANCE(selected.player, selected.card).damage_target_card)
          .regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
      C_get_abilities((int)PLAYER_CARD_INSTANCE(selected.player, selected.card).damage_target_player,
                      PLAYER_CARD_INSTANCE(selected.player, selected.card).damage_target_card,
                      EVENT_CHANGE_TYPE,
                      -1);
    }

    if (total < 3)
    {
      kill_card(player, card, KILL_REMOVE);
    }
  }

  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      event == EVENT_CHANGE_TYPE)
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047fb81
// FUNCTION: SHANDALAR 0x00483690
int card_desert_FX(int player, int card, event_t event)
{
  if ((g_trigger_condition == TRIGGER_END_COMBAT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }

    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1)
      {
        damage_creature((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                        1,
                        (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_source_card);
      }
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047fd07
// FUNCTION: SHANDALAR 0x00483817
int card_erhnam_djinn_FX(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    g_event_result |= PLAYER_CARD_INSTANCE(player, card).info_slot;
  }

  if (g_current_phase == PHASE_UPKEEP &&
      g_current_player == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0)
  {
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .regen_status |= 0x8000000;
    kill_card(player, card, KILL_BURY);
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047ff00
// FUNCTION: SHANDALAR 0x00483a11
int card_guardian_FX(int player, int card, event_t event)
{
  int done;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
        has_mana(player, COLOR_ANY, 1) != 0 &&
        has_effect_source_type((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                               0xffffffff) != 0)
    {
      return 99;
    }
    return 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      charge_mana(player, COLOR_COLORLESS, 1);
      if (g_spell_fizzled != 1)
      {
        done = 0;
        do
        {
          load_text("promptsX1.txt", "GUARDIAN_EFFECT");
          if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                                   0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                                   -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target) == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            if ((int)PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player ==
                    (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
                PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card ==
                    PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            {
              SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              done = 1;
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
          }
        } while (g_spell_fizzled != 1 && done == 0);
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card).internal_card_id != -1)
      {
        SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
        if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                   g_damage_card_internal_card_id, -1, -1, -1, 0, 0, 0) == 0)
        {
          g_spell_fizzled = 1;
        }
        else if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot > 0)
        {
          --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
        }
      }
#ifdef MODERN_FIXES
      /* The original clears the selected damage card's target count here. */
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
#else
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card).number_of_targets = 0;
#endif
    }

    if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00480533
// FUNCTION: SHANDALAR 0x00484044
int card_p_gremlin_FX(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card).untap_status &= ~3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0048065a
// FUNCTION: SHANDALAR 0x0048416b
int card_nettling_imp_FX(int player, int card, event_t event)
{
  int target_internal_card_id;

  /* Original uses event 0x89 for declare-attackers legality. */
  if (event == 0x89 &&
      (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                            PLAYER_CARD_INSTANCE(player, card).damage_target_card)
           .state &
       STATE_UNKNOWN8000) == 0 &&
      (global_cards_data[PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                              PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                             .internal_card_id]
           .type &
       TYPE_CREATURE) != 0)
  {
    PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .state |= STATE_UNKNOWN8000;
    if (can_attack((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card) != 0)
    {
      g_attacking_creature_count = 1;
    }
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    target_internal_card_id = PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                   PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                                  .internal_card_id;
    if (is_in_play((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card) != 0 &&
        (global_cards_data[target_internal_card_id].type & TYPE_CREATURE) != 0 &&
        global_cards_data[target_internal_card_id].subtype != 0 &&
        (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                              PLAYER_CARD_INSTANCE(player, card).damage_target_card)
             .state &
         (STATE_ATTACKED | STATE_SUMMONSICK_BOTH)) == 0)
    {
      kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                KILL_DESTROY);
    }
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00480b07
// FUNCTION: SHANDALAR 0x00484619
int card_living_land_FX(int player, int card, event_t event)
{
  if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card).info_slot !=
      PLAYER_CARD_INSTANCE(player, card).info_slot)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  if (event == EVENT_CHANGE_TYPE &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) != 0)
  {
    if (is_basic_land_internal_card_id_of_color(g_event_result, PLAYER_CARD_INSTANCE(player, card).info_slot) != 0 &&
        is_in_play(g_affected_card_controller, g_affected_card) != 0)
    {
      g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status |= STATUS_ANIMATED;
    }
    else
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00480d5c
// FUNCTION: SHANDALAR 0x0048486e
int card_river_FX(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *river_effect;
  card_instance_t *target;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  target = &PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card);

  if (event == EVENT_BLOCK_LEGALITY &&
      instance->damage_target_card == g_affected_card &&
      (int)instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1 &&
      (target->regen_status & 0x20) == 0)
  {
    current_card = 0;
    while (current_card < g_active_cards_count[g_attacking_card_controller] && g_event_result == 0)
    {
      river_effect = &PLAYER_CARD_INSTANCE(g_attacking_card_controller, current_card);
      if (river_effect->internal_card_id == g_duel_generated_internal_card_id_2a &&
          (int)river_effect->damage_target_player == g_attacking_card_controller &&
          river_effect->damage_target_card == g_attacking_card &&
          instance->eot_toughness != river_effect->eot_toughness)
      {
        g_event_result = 1;
      }
      ++current_card;
    }
  }

  if (g_trigger_condition == TRIGGER_END_COMBAT &&
      g_affected_card == card &&
      player == g_affected_card_controller &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00481050
// FUNCTION: SHANDALAR 0x00484b5d
int card_rukh_egg_FX(int player, int card, event_t event)
{
  int rukh_card;

  if (event == EVENT_CLEANUP)
  {
    rukh_card = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_RUKH));
    if (rukh_card != -1)
    {
      process_card_enters_play(player, rukh_card);
      PLAYER_CARD_INSTANCE(player, rukh_card).color =
          (char)(2 << ((unsigned char)get_sleighted_color(
                            (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                            PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                            COLOR_RED) -
                        1));
      PLAYER_CARD_INSTANCE(player, rukh_card).token_status |= STATUS_TOKEN;
      if ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_SLEIGHTED) != 0)
      {
        PLAYER_CARD_INSTANCE(player, rukh_card).token_status |= STATUS_SLEIGHTED;
        PLAYER_CARD_INSTANCE(player, rukh_card).color_id[COLOR_RED] =
            PLAYER_CARD_INSTANCE(player, card).color_id[COLOR_RED];
      }
    }
    kill_card(player, card, KILL_REMOVE);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00481248
// FUNCTION: SHANDALAR 0x00484d55
int card_sewer_FX(int player, int card, event_t event)
{
  int target_player;
  int target_card;

  if (event == EVENT_PREVENT_DAMAGE &&
      (g_current_phase == PHASE_NORMAL_COMBAT_DAMAGE || g_current_phase == PHASE_FIRST_STRIKE_DAMAGE) &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    target_player = (int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player;
    target_card = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card;
    if (target_player == g_current_player)
    {
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_ATTACKING) != 0 &&
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player ==
              PLAYER_CARD_INSTANCE(player, card).damage_target_player &&
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card ==
              PLAYER_CARD_INSTANCE(player, card).damage_target_card)
      {
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
      }
    }
    else if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == target_player &&
             PLAYER_CARD_INSTANCE(player, card).damage_target_card == target_card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00481589
// FUNCTION: SHANDALAR 0x00485094
int card_xmorgrant_FX(int player, int card, event_t event)
{
  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == g_affected_card &&
      (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    if (event == EVENT_CHANGE_TYPE)
    {
      g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
    }
    if (event == EVENT_POWER)
    {
      ++g_event_result;
    }
    if (event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
  }

  return 0;
}
