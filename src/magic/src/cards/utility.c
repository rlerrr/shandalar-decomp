#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

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
    int dummy0;
    int dummy1;
    int dummy2;
    int dummy3;
    int dummy4;
    int dummy5;
    int dummy6;
  } s;
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
  int dummy;
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
