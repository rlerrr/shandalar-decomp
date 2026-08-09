#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int create_damage_effect_copy_for_target_player(int player, int card);

// FUNCTION: MAGIC 0x004efd03
// FUNCTION: SHANDALAR 0x004a9dc3
int card_power_up(int player, int card, event_t event)
{
  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
  {
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
    {
      if (affected_card != -1)
      {
        if (event == EVENT_POWER)
        {
          event_result += PLAYER_CARD_INSTANCE(player, card).counter_power;
        }

        if (event == EVENT_TOUGHNESS)
        {
          event_result += PLAYER_CARD_INSTANCE(player, card).power;
        }
      }
    }
  }

  if (event == EVENT_CLEANUP)
  {
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
        PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x0f000000;
    }
    kill_card(player, card, KILL_REMOVE);
  }
}

// FUNCTION: MAGIC 0x004f0186
// FUNCTION: SHANDALAR 0x004aa246
int card_unblockable(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY)
  {
    if (attacking_card == PLAYER_CARD_INSTANCE(player, card).damage_target_card &&
        attacking_card_controller == PLAYER_CARD_INSTANCE(player, card).damage_target_player)
    {
      ++event_result;
    }
  }

  if (event == EVENT_CLEANUP)
  {
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f0246
// FUNCTION: SHANDALAR 0x004aa306
int card_no_attack(int player, int card, event_t event)
{
  if (event != EVENT_CLEANUP)
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
    }
  }
}

// FUNCTION: MAGIC 0x004f03ad
// FUNCTION: SHANDALAR 0x004aa46d
int card_add_ability(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f0a6b
// FUNCTION: SHANDALAR 0x004aab29
int card_take_ability(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f0c29
// FUNCTION: SHANDALAR 0x004aace7
int card_damage(int player, int card, event_t event)
{
  struct
  {
    int source_internal_card_id;
    int redirected_source_internal_card_id;
    int protection_source_internal_card_id;
    unsigned int target_abilities;
    int source_color;
    int redirected_source_card;
    int redirected_source_player;
  } s;

  if (card == affected_card &&
      player == affected_card_controller &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1 &&
      (land_can_be_played & 4) != 0)
  {
    s.target_abilities =
        C_get_abilities((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                        EVENT_ABILITIES,
                        -1);
    if ((s.target_abilities & 0x1ff800) != 0)
    {
      s.source_color =
          single_color_test_bit_to_color_t(
              (color_test_t)PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .color);
      if ((s.target_abilities & (0x800 << (s.source_color - 1))) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        kill_card(player, card, KILL_BURY);
      }
      else if ((s.target_abilities & 0x10000) != 0)
      {
        if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .internal_card_id == -1)
        {
          s.protection_source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .original_internal_card_id;
        }
        else
        {
          s.protection_source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id;
        }
        if ((global_cards_data[s.protection_source_internal_card_id].type & TYPE_ARTIFACT) != 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
          kill_card(player, card, KILL_BURY);
        }
      }
    }
  }

  if (event == EVENT_DEAL_DAMAGE &&
      card == affected_card &&
      player == affected_card_controller &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;

    if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .original_internal_card_id == damage_effect_internal_card_id &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness = (unsigned int)(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
      create_damage_effect_copy_for_target_player((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                            PLAYER_CARD_INSTANCE(player, card).damage_source_card);
    }

    if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == -1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0xe);
      }

      if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id == -1 ||
          global_cards_data[PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                                .internal_card_id]
                  .id != unk_008a8de8)
      {
        unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                    [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] +=
            PLAYER_CARD_INSTANCE(player, card).info_slot;

        if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .internal_card_id == -1)
        {
          s.source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .original_internal_card_id;
        }
        else
        {
          s.source_internal_card_id =
              PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                  .internal_card_id;
        }

        unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                    [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] =
            (unsigned int)global_cards_data[s.source_internal_card_id].type;
        if ((unk_0093b280[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player]
                           [PLAYER_CARD_INSTANCE(player, card).damage_source_card]
                           [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] &
             TYPE_ARTIFACT) == 0)
        {
          unk_0093b280[0][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
        else
        {
          unk_0093b280[1][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
      }
      else
      {
        s.redirected_source_player =
            (int)PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                      PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_source_player;
        s.redirected_source_card =
            PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_source_card;

        unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] +=
            PLAYER_CARD_INSTANCE(player, card).info_slot;

        if (PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).internal_card_id == -1)
        {
          s.redirected_source_internal_card_id =
              PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).original_internal_card_id;
        }
        else
        {
          s.redirected_source_internal_card_id =
              PLAYER_CARD_INSTANCE(s.redirected_source_player, s.redirected_source_card).internal_card_id;
        }

        unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                    [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] =
            (unsigned int)global_cards_data[s.redirected_source_internal_card_id].type;
        if ((unk_0093b280[s.redirected_source_player][s.redirected_source_card]
                           [(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][1] &
             TYPE_ARTIFACT) == 0)
        {
          unk_0093b280[0][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
        else
        {
          unk_0093b280[1][150][(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player][0] = 1;
        }
      }

      life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player] -=
          PLAYER_CARD_INSTANCE(player, card).info_slot;
      duel_summary.player_damage_totals[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player] +=
          PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    else if (is_in_play((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card))
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .damage_on_card += PLAYER_CARD_INSTANCE(player, card).info_slot;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x16);
        request_duel_display_refresh_if_human((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                      PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                      6,
                      2);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f1e72
// FUNCTION: SHANDALAR 0x004abf34
int card_asp_sting(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f208d
// FUNCTION: SHANDALAR 0x004ac14c
int card_stoning(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f21b5
// FUNCTION: SHANDALAR 0x004ac273
int card_t_elemental_FX(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f2360
// FUNCTION: SHANDALAR 0x004ac420
int card_marsh_gas_1(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f272d
// FUNCTION: SHANDALAR 0x004ac7ed
int card_fog_effect(int player, int card, event_t event)
{
  int dummy;
}

// FUNCTION: MAGIC 0x004f29a0
// FUNCTION: SHANDALAR 0x004aca60
int card_channel_1(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
  } s;
}

// FUNCTION: MAGIC 0x004f2b93
// FUNCTION: SHANDALAR 0x004acc56
int card_generic(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f3587
// FUNCTION: SHANDALAR 0x004ad642
int card_asterisk_FX(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
    int dummy4;
  } s;
}

// FUNCTION: MAGIC 0x004f3e18
// FUNCTION: SHANDALAR 0x004aded3
int card_piggy_FX(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
    int dummy4;
  } s;
}

// FUNCTION: MAGIC 0x004f41d5
// FUNCTION: SHANDALAR 0x004ae290
int card_titanias_leg(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x004f45e9
// FUNCTION: SHANDALAR 0x004ae6a4
int card_disintegrt_FX(int player, int card, event_t event)
{
  return ((card << 2) * 5) * 3;
}

// FUNCTION: MAGIC 0x004f47f7
// FUNCTION: SHANDALAR 0x004ae8b2
int card_sirens_call_FX(int player, int card, event_t event)
{
  int dummy1;
  int dummy2;
  int dummy3;
}

// FUNCTION: MAGIC 0x004f4a09
// FUNCTION: SHANDALAR 0x004aeac7
int card_ebony_horse_FX(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
  }
}

// FUNCTION: MAGIC 0x004f4cc8
// FUNCTION: SHANDALAR 0x004aed84
int card_poltergeist_FX(int player, int card, event_t event)
{
  if (event == EVENT_CHANGE_TYPE)
  {
  }
}

// FUNCTION: MAGIC 0x004f5053
// FUNCTION: SHANDALAR 0x004af111
int card_graveyard(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
    int dummy4;
    int dummy5;
  } s;
}

// FUNCTION: MAGIC 0x004f56f2
// FUNCTION: SHANDALAR 0x004af7ac
int card_damage_legacy(int player, int card, event_t event)
{
  struct
  {
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
  } s;
}

// FUNCTION: MAGIC 0x004f5b91
// FUNCTION: SHANDALAR 0x004afc4a
int card_activation(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_RESOLVE_ACTIVATION && event != EVENT_UPKEEP_COSTS_UNPAID &&
      event != EVENT_RESOLVE_TRIGGER)
  {
    return 0;
  }

  card_on_stack_controller = PLAYER_CARD_INSTANCE(player, card).parent_controller;
  card_on_stack = PLAYER_CARD_INSTANCE(player, card).parent_card;
  result = (*global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].code_pointer)(
      player, card, event);
  card_on_stack_controller = -1;
  card_on_stack = -1;

  return result;
}

// FUNCTION: MAGIC 0x004f5c93
// FUNCTION: SHANDALAR 0x004afd4c
int card_draw_card(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE && current_phase == PHASE_DRAW && unk_00742f60 == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && trigger_condition == -1)
  {
    unk_008b3270 |= 3;
    return 1;
  }

  if (event == EVENT_ACTIVATE)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    kill_card(card_on_stack_controller, card_on_stack, KILL_REMOVE);
    draw_card_for_player(player);
  }

  return 0;
}
