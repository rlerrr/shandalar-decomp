#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

int gain_life_or_prevent_damage(int player, int card, event_t event, int amount);
int FUN_004f6311(int player, int card, int internal_card_id);
int card_guardian_beast(int player, int card, event_t event);

// FUNCTION: MAGIC 0x00431e02
static int helper_destroy_basiclandtype(int source_player,
                                        int source_card,
                                        int test_player,
                                        int test_card,
                                        int internal_card_id)
{
  if (global_cards_data[internal_card_id].type & 1)
  {
    if (FUN_0048463d(test_player, test_card, PLAYER_CARD_INSTANCE(source_player, source_card).info_slot) != 0)
    {
      kill_card(test_player, test_card, KILL_DESTROY);
    }
  }
  return 0;
}

// FUNCTION: MOK 0x004a9310
// FUNCTION: MAGIC 0x005058b1
int helper_dual_land(int player, int card, event_t event, color_test_t available_colors)
{
  int color;
  int first_available_color;
  int num_available_colors;
  color_test_t colors_to_choose_from;

  if (event == EVENT_TAP_CARD && unk_008a9000 != 1)
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

      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        color = first_available_color;
      }
      else if (num_available_colors == 1)
      {
        color = choose_a_color(player, text_lines[0], 1, -1, colors_to_choose_from);
      }
      else
      {
        color = choose_a_color(player, text_lines[0], 1, -1, available_colors);
      }
    }

    if (color == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      produce_mana(player, color, 1);
      undeclare_mana_available_hex(player, color, 1);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      unk_0092664c[6] = color;

      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        load_text("promptsX1.txt", "MULTI_LANDS");
        switch (color)
        {
        case COLOR_BLACK:
          do_dialog(player, player, card, -1, -1, text_lines[0], 0);
          break;

        case COLOR_BLUE:
          do_dialog(player, player, card, -1, -1, text_lines[1], 0);
          break;

        case COLOR_GREEN:
          do_dialog(player, player, card, -1, -1, text_lines[2], 0);
          break;

        case COLOR_RED:
          do_dialog(player, player, card, -1, -1, text_lines[3], 0);
          break;

        case COLOR_WHITE:
          do_dialog(player, player, card, -1, -1, text_lines[4], 0);
          break;
        }
      }
    }
  }

  if (event == EVENT_COUNT_MANA && card == affected_card && player == affected_card_controller && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    declare_mana_available_hex(player, available_colors, 1);
  }

  return 0;
}

// FUNCTION: MOK 0x00427740
// FUNCTION: MAGIC 0x0041f4c0
int helper_mox(int player, int card, event_t event, int color)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((instance->token_status & 3) == 0) || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)) && ((instance->state & 0x10) == 0))
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, color, 1);
    instance->state |= 0x10;
    produced_mana_color = color;
  }

  if ((event == EVENT_COUNT_MANA) && (card == card_on_stack) && (player == card_on_stack_controller) && ((instance->state & 0x10) == 0))
  {
    declare_mana_available(player, color, 1);
  }

  return 0;
}

// FUNCTION: MOK 0x00423dc0
// FUNCTION: MAGIC 0x0041626b
// FUNCTION: SHANDALAR 0x00518a81
int helper_lucky_charm(int player, int card, int event, int color)
{
  int target_color;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += basiclandtypes_controlled[active_player][color] * 0xc;
  }

  if (trigger_condition == 0xd3 && affected_card == card && affected_card_controller == player && player == current_turn && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) && is_in_play(player, card) && has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
  {
    target_color = get_sleighted_color(player, card, color);
    if (((1 << ((unsigned char)target_color & 0x1f)) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).color) != 0 && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1 && global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type != TYPE_LAND)
    {
      if (event == EVENT_TRIGGER)
      {
        if (active_player == player && (unk_00926804 & 2) == 0)
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

// FUNCTION: MOK 0x004bdc80
// FUNCTION: MAGIC 0x00532bea
int helper_circle_of_protection(int player, int card, event_t event, int color)
{
  card_instance_t *instance;
  target_t target;
  unsigned int target_color;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  target_color = 1 << ((unsigned char)color & 0x1f);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player && FUN_00404cff(player, instance->internal_card_id, player) == 0)
    {
      ai_modifier +=
          (basiclandtypes_controlled[unk_008b35ec][color] + unk_008cf1c0[unk_008b35ec][color] / 2) * 0x18;
    }

    if (event == EVENT_CAN_ACTIVATE)
    {
      if ((unk_008b4278 & 4) == 0 || has_mana_w_global_cost_mod(player, card, 7, 1) == 0 || real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0, target_color, 0, unk_009266a4, -1, 0xffffffff, 0xffffffff, TARGET_SPECIAL_DAMAGE_PLAYER, 0, 0) == 0)
      {
        return 0;
      }
      return 99;
    }
    else
    {
      if (event == EVENT_ACTIVATE && (instance->state & STATE_INVISIBLE) == 0)
      {
        charge_mana_w_global_cost_mod(player, card, 0, 1);
        if (spell_fizzled != 1)
        {
          if (unk_008a9000 != 1)
          {
            load_text("prompts.txt", "CIRCLE_OF_PROTECTION");
          }

          if (C_real_select_target(player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
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
                                   &target) == 0)
          {
            spell_fizzled = 1;
          }
          else
          {
            instance->targets[0].player = target.player;
            instance->targets[0].card = target.card;
            instance->number_of_targets = 1;
          }
        }
      }

      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        if (C_real_validate_target(instance->targets[0].player,
                                   instance->targets[0].card,
                                   (char *)0,
                                   player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
                                   COLOR_TEST_0,
                                   unk_009266a4,
                                   ~SUB_WALL,
                                   -1,
                                   -1,
                                   TARGET_SPECIAL_DAMAGE_PLAYER,
                                   0,
                                   0) == 0)
        {
          spell_fizzled = 1;
        }
        else
        {
          target_player = instance->targets[0].player;
          target_card = instance->targets[0].card;
          if (PLAYER_CARD_INSTANCE(target_player, target_card).info_slot != 0)
          {
            PLAYER_CARD_INSTANCE(target_player, target_card).info_slot = 0;
          }
        }

        PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
      }

      return 0;
    }
  }
}

// FUNCTION: MOK 0x004bc490
// FUNCTION: MAGIC 0x0052e400
int helper_ward(int player, int card, event_t event, int color)
{
  card_instance_t *instance;
  card_instance_t *other_instance;
  unsigned int ward_color;
  int aura_count;
  int current_card;
  int test_player;
  int old_event_result;
  int protection_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  ward_color = 1 << ((unsigned char)color & 0x1f);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
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
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "ANY_WARD");
      }

      if (!FUN_00551638(player, player, card))
      {
        spell_fizzled = 1;
      }
      else
      {
        if (player == active_player && (unk_00926804 & 2) == 0 && FUN_0052adf2(player, card) != 0)
        {
          ai_modifier -= 0x60;
        }
        if (instance->targets[0].player == active_player)
        {
          aura_count = unk_008cf1c0[unk_008b35ec][color];
          ai_modifier += (aura_count + 1) * C_get_abilities(instance->targets[0].player, instance->targets[0].card, EVENT_POWER, -1) * 3;
        }
        if (instance->targets[0].player == unk_008b35ec)
        {
          ai_modifier -= 0x60;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
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
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        instance->damage_target_player = instance->targets[0].player;
        instance->damage_target_card = instance->targets[0].card;
      }
      instance->number_of_targets = 0;
    }

    if (instance->damage_target_card != -1)
    {
      for (test_player = 0; test_player < 2; ++test_player)
      {
        for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card)
        {
          other_instance = &PLAYER_CARD_INSTANCE(test_player, current_card);
          old_event_result = event_result;
          if (is_in_play(test_player, current_card) && other_instance->damage_target_card == instance->damage_target_card && other_instance->damage_target_player == instance->damage_target_player && (unsigned char)other_instance->color == ward_color && (player != test_player || card != current_card) && (global_cards_data[other_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0)
          {
            kill_card(test_player, current_card, KILL_BURY);
          }
          event_result = old_event_result;
        }
      }
    }

    if (instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1 && (instance->state & STATE_INVISIBLE) == 0 && event == EVENT_ABILITIES)
    {
      protection_color = get_sleighted_color(player, card, color);
      event_result |= 0x800 << (((unsigned char)protection_color - 1) & 0x1f);
    }

    if (event == EVENT_CAST_SPELL && instance->damage_target_player == PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == instance->damage_target_card && (ward_color & (unsigned char)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color) != 0 && (instance->state & STATE_INVISIBLE) == 0)
    {
      event_result = 1;
    }

    if (event == EVENT_SHOULD_AI_PLAY && current_phase == EVENT_CLEANUP && card == affected_card && player == affected_card_controller)
    {
      test_player = 1 - player;
      aura_count = 0;
      for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card)
      {
        if ((ward_color & (unsigned char)PLAYER_CARD_INSTANCE(test_player, current_card).color) != 0 && is_in_play(test_player, current_card))
        {
          aura_count += 0x18;
        }
      }

      if (aura_count != 0)
      {
        ai_modifier += aura_count + 0x18;
      }
    }

    return 0;
  }
}

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

// FUNCTION: MAGIC 0x00505dd2
// FUNCTION: SHANDALAR 0x00555e68
int card_badlands(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "BADLANDS");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505e42
// FUNCTION: SHANDALAR 0x00555ed8
int card_bayou(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "BAYOU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505eb2
// FUNCTION: SHANDALAR 0x00555f48
int card_plateau(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "PLATEAU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f22
// FUNCTION: SHANDALAR 0x00555fb8
int card_savannah(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "SAVANNAH");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f92
// FUNCTION: SHANDALAR 0x00556028
int card_scrubland(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "SCRUBLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506002
// FUNCTION: SHANDALAR 0x00556098
int card_taiga(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "TAIGA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506072
// FUNCTION: SHANDALAR 0x00556108
int card_tropical_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "TROPICAL_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005060e2
// FUNCTION: SHANDALAR 0x00556178
int card_tundra(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "TUNDRA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506152
// FUNCTION: SHANDALAR 0x005561e8
int card_underground_sea(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "UNDERGROUND_SEA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005061c2
// FUNCTION: SHANDALAR 0x00556258
int card_volcanic_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text((int)"promptsX1.txt", "VOLCANIC_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x0053aa50
// FUNCTION: SHANDALAR 0x00488ca0
int card_dummy(int player, int card, event_t event)
{
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
    ++unk_00939520[player][COLOR_WHITE];
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
    FUN_004e4ff3(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_WHITE, 2);
    if (result != 0 && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      charge_mana(player, COLOR_WHITE, 2);
      if (spell_fizzled != 1)
      {
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "NORTHERN_PALADIN");
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
                                 text_lines[0],
                                 1,
                                 &target) == 0)
        {
          spell_fizzled = 1;
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
      spell_fizzled = 1;
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

// FUNCTION: MAGIC 0x0054920c
// FUNCTION: SHANDALAR 0x0049744a
int card_serra_angel(int player, int card, event_t event)
{
  if (affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
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
    int pending_event;
    card_instance_t *preventer;
    int result;
    int source_card_id;
    int protects_damage;
  } s;

  s.protects_damage = 0;
  s.result = 0;
  s.source_card_id = global_cards_data[PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id].id;
  s.preventer = &PLAYER_CARD_INSTANCE(player, card);

  if (unk_009266a4 == internal_card_id && s.preventer->info_slot != 0 && s.preventer->damage_target_player == source_player && s.preventer->damage_target_card == -1 && ((s.source_card_id == CARD_ID_VETERAN_BODYGUARD && (s.preventer->token_status & 0x140000) != 0) || (s.source_card_id == CARD_ID_MARTYRS_OF_KORLIS && (s.preventer->eot_toughness & 0x40) != 0)))
  {
    s.protects_damage = 1;
  }

  if (s.protects_damage)
  {
    s.pending_event = PLAYER_CARD_INSTANCE(source_player, source_card).info_slot;
    if (s.pending_event == EVENT_TRIGGER)
    {
      s.result = 1;
      event_result = 2;
    }
    else if (s.pending_event == EVENT_RESOLVE_TRIGGER)
    {
      s.preventer->damage_target_player = (char)source_player;
      s.preventer->damage_target_card = source_card;
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
    event_result = result;
  }

  return result;
}

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

  if (trigger_condition == TRIGGER_END_DAMAGE_PREV && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && affected_card == card && affected_card_controller == player && current_turn == player && player != human_player && is_in_play(player, card) && (current_phase == 0x1a || current_phase == 0x19))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = event;
    if (event == EVENT_TRIGGER)
    {
      if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1) != -1)
      {
        event_result |= 2;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1);
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_mark_other_protectors, player);
    }
  }

  if (event == EVENT_CLEANUP && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00548bd9
// FUNCTION: SHANDALAR 0x00496e1d
int card_samite_healer(int player, int card, event_t event)
{
  int result;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if ((unk_008b4278 & 4) == 0)
    {
      result = 0;
    }
    if (PLAYER_CARD_INSTANCE(player, card).state & 0x20010)
    {
      result = 0;
    }
    if (result == 0)
    {
      return 0;
    }
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
                              unk_009266a4,
                              -1,
                              -1,
                              -1,
                              0,
                              0,
                              0))
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "SAMITE_HEALER");
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
                             unk_009266a4,
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             text_lines,
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
      spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
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
                                unk_009266a4,
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
    {
      --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
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

// FUNCTION: MAGIC 0x0054e36e
// FUNCTION: SHANDALAR 0x0049c5b0
int card_white_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && affected_card == card && affected_card_controller == player)
  {
    event_result |= KEYWORD_PROT_BLACK << (((char)get_sleighted_color(player, card, COLOR_BLACK) - 1U) & 0x1f);
    saved_event_result = event_result;
    FUN_0054e470(player, card, COLOR_BLACK);
    event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054baf9
// FUNCTION: SHANDALAR 0x00499d35
int card_orcish_artillery(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (PLAYER_CARD_INSTANCE(player, card).state & 0x20010)
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "ORCISH_ARTILLERY");
    }
    FUN_0054ac4d(player, card, 2);
    if (spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (FUN_0054af10(player, card, event, 2) != 0)
    {
      damage_player(player, 3, card_on_stack_controller, card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054060c
// FUNCTION: SHANDALAR 0x0048e853
int card_dragon_whelp(int player, int card, event_t event)
{
  int *info_slot_ptr;
  int result;
  unsigned int *state_ptr;
  unsigned int saved_max_x_value;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][0];
    result = 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
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
    FUN_004e503e(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
    saved_max_x_value = max_x_value;
    if (result != 0)
    {
      x_value = 0;
      if (player == human_player)
      {
        if ((((player == active_player) && ((unk_00926804 & 2) == 0)) || ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) == 0x30000)) || unk_00715fb0 != 1)
        {
          max_x_value = -1;
        }
        else
        {
          max_x_value = 3 - ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) >> 16);
        }
        charge_mana(player, COLOR_RED, -1);
        max_x_value = saved_max_x_value;
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_RED, 1);
        x_value = 1;
      }

      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      *state_ptr &= 0xff0000;
      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += x_value * 0x10001;
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
      spell_fizzled = 1;
    }
    else
    {
      info_slot_ptr =
          &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot;
      *info_slot_ptr += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
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
        result = create_legacy_effect(card_on_stack_controller,
                                      card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      card_on_stack_controller,
                                      card_on_stack);
        if (result != -1)
        {
          state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, result).info_slot;
          *state_ptr |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    result = has_mana(player, COLOR_RED, 1);
    result = FUN_004c0a36(result, 0, 3);
    result -= PLAYER_CARD_INSTANCE(player, card).info_slot;
  }
  else
  {
    if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card) && (affected_card_controller == player) && ((char)PLAYER_CARD_INSTANCE(player, card).info_slot != 0)) && player == current_turn)
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
          event_result |= 2;
        }
        if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
        {
          kill_card(player, card, KILL_DESTROY);
        }
      }
    }
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * 0xc;
      }
      else
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * -0xc;
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x20;
    return 0;
  }
  else if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][0];
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot == 0) || (has_mana(player, COLOR_RED, 1) == 0))
    {
      return 0;
    }
    return 1;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      charge_mana(player, COLOR_RED, 1);
      if (spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((active_player == player) && ((unk_00926804 & 2) == 0))
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
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
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot = 0x20;
      legacy_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
                                         unk_00896534,
                                         card_on_stack_controller,
                                         card_on_stack);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549451
// FUNCTION: SHANDALAR 0x0049768d
int card_goblin_king(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].subtype == 3)
  {
    if (event == EVENT_ABILITIES)
    {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_RED) - 1);
    }
    if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++event_result;
    }
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
    ++unk_00939520[player][COLOR_RED];
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    result = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      result = has_mana(player, COLOR_RED, 1);
      if (result != 0)
      {
        if (human_player == player)
        {
          charge_mana(player, 4, -1);
          if (x_value < 1)
          {
            spell_fizzled = 1;
          }
          else
          {
            PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
          }
        }
        else
        {
          charge_mana(player, 4, 1);
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }

        if (spell_fizzled == 1)
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
        spell_fizzled = 1;
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
          result = create_legacy_effect(card_on_stack_controller,
                                        card_on_stack,
                                        LEGACY_EFFECT_PUMP,
                                        card_on_stack_controller,
                                        card_on_stack);
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
      if (event == EVENT_CAN_WASTE_MANA && raw_mana_available[player][COLOR_RED] != 0)
      {
        event_result |= 1;
      }

      if (event == EVENT_SHOULD_AI_PLAY)
      {
        if (active_player == player)
        {
          ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
        }
        else
        {
          ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
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

// FUNCTION: MAGIC 0x0047894c
// FUNCTION: SHANDALAR 0x0047c44c
int card_rock_hydra(int player, int card, event_t event)
{
  int *piVar1;
  unsigned int uVar3;
  int iVar4;
  unsigned int local_18;
  target_t local_14;
  int local_c;
  int local_8;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    FUN_00551572(player, card, x_value);
    if (active_player == player)
    {
      ai_modifier += (x_value + x_value * 2 - 9) << 3;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    local_c = create_legacy_effect(player, card, unk_0093d848, player, card);
    if (local_c != -1)
    {
      uVar3 = C_get_special_counters(player, card);
      PLAYER_CARD_INSTANCE(player, local_c).info_slot = (uVar3 << 8) | uVar3;
      PLAYER_CARD_INSTANCE(player, local_c).eot_toughness = 0x40c;
      PLAYER_CARD_INSTANCE(player, local_c).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = local_c;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((unk_008b4278 & 4) == 0) || ((iVar4 = has_mana(player, 4, 1)) == 0)) || (((uVar3 = C_get_special_counters(player, card)) == 0) || ((iVar4 = FUN_00482a97(player, card, 0xffffffff)) == 0)))
    {
      if (((current_phase == 4) && (player == human_player) && (player == unk_00742f60)) && ((iVar4 = has_mana(player, 4, 3)) != 0))
      {
        if (((player == active_player) && ((unk_00926804 & 2) == 0)) && (hand_count[player] - *(int *)((char *)basiclandtypes_controlled + player * 0x20 + 0x10) == 3))
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
    FUN_004e4ff3(0);
    iVar4 = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if ((unk_008b4278 & 4) == 0)
      {
        charge_mana(player, 4, 3);
        if (spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        if (spell_fizzled != 1)
        {
          local_8 = 0;
          do
          {
            load_text((int)"promptsX1.txt", "ROCK_HYDRA");
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
                                         unk_009266a4,
                                         -1,
                                         -1,
                                         -1,
                                         0,
                                         0,
                                         0,
                                         text_lines[0],
                                         1,
                                         &local_14);
            if (iVar4 == 0)
            {
              spell_fizzled = 1;
            }
            else if (PLAYER_CARD_INSTANCE(local_14.player, local_14.card).damage_source_player == player && PLAYER_CARD_INSTANCE(local_14.player, local_14.card).damage_source_card == card)
            {
              PLAYER_CARD_INSTANCE(player, card).targets[0].player = local_14.player;
              PLAYER_CARD_INSTANCE(player, card).targets[0].card = local_14.card;
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              local_8 = 1;
            }
            else if (unk_008a9000 == 1)
            {
              FUN_004e51bb();
            }
            else
            {
              FUN_004a61d6(text_lines[1]);
              Sleep(0x9c4);
              FUN_004a61d6("");
            }
          } while ((spell_fizzled != 1) && (local_8 == 0));
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id != -1)
      {
        if ((unk_008b4278 & 4) == 0)
        {
          add_special_counter(card_on_stack_controller, card_on_stack);
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
              unk_009266a4,
              ~SUB_WALL,
              -1,
              -1,
              0,
              0,
              0);
          if (uVar3 == 0)
          {
            spell_fizzled = 1;
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

    if ((((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4)) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card)) && ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)))
    {
      uVar3 = C_get_special_counters(player, card);
      if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot < (int)uVar3)
      {
        local_18 = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot;
      }
      else
      {
        local_18 = C_get_special_counters(player, card);
      }
      piVar1 = &PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot;
      *piVar1 -= local_18;
      FUN_005514cd(player, card, local_18);
      piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                    .info_slot;
      *piVar1 -= (local_18 << 8) | local_18;
    }
    iVar4 = 0;
  }
  return iVar4;
}

// FUNCTION: MAGIC 0x004796cc
// FUNCTION: SHANDALAR 0x0047d1d2
int card_sedge_troll(int player, int card, event_t event)
{
  int color;

  if (card == affected_card && player == affected_card_controller)
  {
    if (basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLACK)] > 0)
    {
      if (event == EVENT_POWER)
      {
        ++event_result;
      }
      if (event == EVENT_TOUGHNESS)
      {
        ++event_result;
      }
    }
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    color = get_hacked_color(player, card, COLOR_BLACK);
    ((int *)((char *)unk_00939520 + player * 0x20))[color] += 2;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, 1, 1);
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
    ++unk_00939520[player][COLOR_RED];
    return 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
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
    FUN_004e503e(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      if (human_player == player)
      {
        charge_mana(player, 4, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
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
      spell_fizzled = 1;
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
        legacy = create_legacy_effect(card_on_stack_controller,
                                      card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      card_on_stack_controller,
                                      card_on_stack);
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
  else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_RED] != 0)
  {
    event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (active_player == player)
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
      }
      else
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
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
    FUN_004e4ff3(0);
    return 0;
  }
  else if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "STONE_GIANT");
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
                                       text_lines[0],
                                       1,
                                       &selected_target);
    if (legacy_card == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((active_player == player) && ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).token_status & 0x20) != 0))
      {
        ai_modifier += -0x60;
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
      spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
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

// FUNCTION: MAGIC 0x00542d29
// FUNCTION: SHANDALAR 0x00490f6c
int card_uthden_troll(int player, int card, event_t event)
{
  unsigned int color;

  color = single_color_test_bit_to_color_t(
      *(char *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color);
  if (event == EVENT_UNTAP_PHASE)
  {
    ((int *)((char *)unk_00939520 + player * 0x20))[color] += 2;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d355
// FUNCTION: SHANDALAR 0x0048b5a9
int card_ironclaw_orcs(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && affected_card == card && affected_card_controller == player)
  {
    if (C_get_abilities(attacking_card_controller, attacking_card, EVENT_POWER, card) > 1)
    {
      event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b8ad
// FUNCTION: SHANDALAR 0x00499ae9
int card_lord_of_atlantis(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].subtype == 1)
  {
    if (event == EVENT_ABILITIES)
    {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
    }
    else if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++event_result;
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY && (PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
    {
      PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).regen_status |= 0xe000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b360
// FUNCTION: SHANDALAR 0x0049959d
int card_sea_serpent(int player, int card, event_t event)
{
  if (is_in_play(player, card))
  {
    if (basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      event_result = 1;
    }
  }

  return 0;
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
    event_result = player << 8 | card;
    return 1;
  }

  return 0;
}

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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"promptsX1.txt", "CLONE");
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
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
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
      data_card_slot = add_card_to_hand(player, FUN_004b5f03(CARD_ID_DATA_CARD));
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
            ++artifact_cards_in_play[player];
          }
          if ((global_cards_data[cloned_internal_card_id].type & TYPE_ENCHANTMENT) != 0)
          {
            ++enchantments_in_play[player];
          }
          card_types_in_play[player] |= global_cards_data[cloned_internal_card_id].type;
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
      FUN_0051a41c(player, card);
    }
  }

  if (((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x20000) == 0)) && ((card == affected_card) && (player == affected_card_controller)))
  {
    event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
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
    ++unk_00939520[player][COLOR_BLUE];
    return 0;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLUE, 1) != 0)
    {
      if (player == human_player)
      {
        charge_mana(player, COLOR_BLUE, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLUE, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
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
      spell_fizzled = 1;
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
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= 0xfff7ffff;
        legacy_card = create_legacy_effect(card_on_stack_controller,
                                           card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           card_on_stack_controller,
                                           card_on_stack);
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
  else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_BLUE] != 0)
  {
    event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
      else
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_BLUE] * -0xc;
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

// FUNCTION: MAGIC 0x0054cc76
// FUNCTION: SHANDALAR 0x0049aeb3
int card_phantasmal_forces(int player, int card, event_t event)
{
  unsigned int *upkeep_flags_ptr;

  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (affected_card == card)) && (affected_card_controller == player))
  {
    if ((human_player == player) && (player == unk_00742f60))
    {
      upkeep_flags_ptr = &PLAYER_CARD_INSTANCE(player, card).upkeep_flags;
      *upkeep_flags_ptr |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_blue = PLAYER_CARD_INSTANCE(player, card).upkeep_blue + '\x01';
    }
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(card_on_stack_controller, card_on_stack, KILL_BURY);
  }
  else if ((event == EVENT_SHOULD_AI_PLAY) && (basiclandtypes_controlled[player][COLOR_BLUE] < 1))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b1ad
// FUNCTION: SHANDALAR 0x004993ea
int card_pirate_ship(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(1);
    result = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "PIRATE_SHIP");
      }
      FUN_0054ac4d(player, card, 1);
      if (spell_fizzled != 1)
      {
        state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
        *state_ptr |= 0x10;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      FUN_0054af10(player, card, event, 1);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }

    card_sea_serpent(player, card, event);
    result = 0;
  }

  return result;
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
    FUN_004e4ff3(1);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "PRODIGAL_SORCERER");
    }
    FUN_0054ac4d(player, card, 1);
    if (spell_fizzled != 1)
    {
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
      *state_ptr |= 0x10;
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    FUN_0054af10(player, card, event, 1);
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
    if ((event == EVENT_SHOULD_AI_PLAY) && (player == human_player) && (active_player == player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
    {
      ai_modifier += 0x18;
    }

    if (((event == EVENT_ATTACK_RATING) && (affected_card == card)) && (affected_card_controller == player))
    {
      unk_00925d3c += 0x30;
      result = 0;
    }
    else if (((event == EVENT_BLOCK_RATING) && (affected_card == card)) && (affected_card_controller == player))
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
    if (parent->internal_card_id == unk_0091a80c)
    {
      lookup_player = card_on_stack_controller;
      lookup_card = card_on_stack;
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

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    load_text((int)"prompts.txt", "VESUVAN_DOPPELGANGER");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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

      data_card_slot = add_card_to_hand(player, FUN_004b5f03(CARD_ID_DATA_CARD));
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
            ++artifact_cards_in_play[player];
          }
          if ((global_cards_data[copied_internal_id].type & TYPE_ENCHANTMENT) != 0)
          {
            ++enchantments_in_play[player];
          }
          card_types_in_play[player] |= global_cards_data[copied_internal_id].type;
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

  if (event == EVENT_CHANGE_TYPE && (unk_008b4278 & 0x20000) == 0 && card == affected_card && player == affected_card_controller && is_in_play(player, card))
  {
    event_result = instance->dummy3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (current_phase == EVENT_UPKEEP_PHASE && player == current_turn && player == unk_00742f60 && data_card != (card_instance_t *)0 && (data_card->eot_toughness & 2) == 0 && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, TYPE_CREATURE, 0, 0, get_protections_from(player, card), 0, 0, -1, -1, -1, -1, TARGET_SPECIAL_USE_ORIGINAL_TYPE, 0, 0))
    {
      result = 1;
    }
  }

  if (event == EVENT_ACTIVATE && card == affected_card && player == affected_card_controller && current_phase == EVENT_UPKEEP_PHASE && player == current_turn && player == unk_00742f60 && data_card != (card_instance_t *)0 && (data_card->eot_toughness & 2) == 0)
  {
    load_text((int)"prompts.txt", "VESUVAN_DOPPELGANGER");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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

// FUNCTION: MAGIC 0x0053f22d
// FUNCTION: SHANDALAR 0x0048d477
int card_fungusaur(int player, int card, event_t event)
{
  if (event == EVENT_UNKNOWN80)
  {
    if (((((unk_008b4278 & 0x200) != 0) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player)) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == card)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }

    return 0;
  }

  if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (card == affected_card)) && (player == affected_card_controller)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }
  if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && ((card == affected_card) && ((player == affected_card_controller) && (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)))) && (player == current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if ((event == EVENT_RESOLVE_TRIGGER) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).counter_power++;

      PLAYER_CARD_INSTANCE(player, card).counter_toughness++;

      FUN_005513d7(player, card, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054926e
// FUNCTION: SHANDALAR 0x004974ab
int card_giant_spider(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        if (human_player == player)
        {
          event_result &= ~KEYWORD_FLYING;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cdc0
// FUNCTION: SHANDALAR 0x0049affe
int card_force_of_nature(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        if (human_player == player)
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
    load_text((int)"prompts.txt", "FORCE_OF_NATURE");
    do_dialog(player, player, card, -1, -1, text_lines[0], 0);
    damage_player(player, 8, card_on_stack_controller, card_on_stack);
  }
  else if ((event == EVENT_SHOULD_AI_PLAY) && (basiclandtypes_controlled[player][COLOR_GREEN] < 4))
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
  else if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0))
  {
    undeclare_mana_available_and_produce_it(player, COLOR_GREEN, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    produced_mana_color = COLOR_GREEN;
  }
  else if ((event == EVENT_COUNT_MANA) && (affected_card == card) && (affected_card_controller == player))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_GREEN, 1);
    }
  }
  else if ((event == EVENT_ATTACK_RATING) && (affected_card == card) && (affected_card_controller == player))
  {
    unk_00925d3c += 0x18 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
  }
  else if ((event == EVENT_BLOCK_RATING) && (affected_card == card) && (affected_card_controller == player))
  {
    unk_00925d3c -= 0x60 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549014
// FUNCTION: SHANDALAR 0x00497258
int card_verduran_enchantress(int player, int card, event_t event)
{
  int library_position;

  if ((((((trigger_condition == TRIGGER_SPELL_CAST) && (affected_card == card)) && (affected_card_controller == player)) && ((current_turn == player) && (human_player == player))) && (((PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0) && ((affected_card_controller == player) && (PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1)))) && ((global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id]
                                                                                                                                                                                                                                                                                                                                                                                                .type &
                                                                                                                                                                                                                                                                                                                                                                                            TYPE_ENCHANTMENT) != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      if ((unk_008b35ec == player) || ((unk_00926804 & 2) != 0))
      {
        event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
      else
      {
        library_position = 0;
        while ((library_position < 500) && (global_library[player][library_position] != -1))
        {
          ++library_position;
        }

        if ((hand_count[player] < 8) && (5 < library_position))
        {
          event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
      FUN_0043e18b(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549404
// FUNCTION: SHANDALAR 0x00497640
int card_scathe_zombies(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) || (event == EVENT_RESOLVE_ACTIVATION))
  {
    result = FUN_0054276d(player, card, event, 1, 1);
  }
  else
  {
    result = 0;
  }

  return result;
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
      if (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card)
                                .internal_card_id]
              .subtype == SUB_ZOMBIE)
      {
        color = get_hacked_color(player, card, COLOR_BLACK);
        event_result |= (1 << (((char)color - 1U) & 0x1f)) + KEYWORD_REGENERATION;
      }
    }
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card == card)) && (player == affected_card_controller))
  {
    FUN_0055117d(FUN_005493a6, -1);
    TENTATIVE_reassess_all_cards();
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543640
// FUNCTION: SHANDALAR 0x00491883
int card_frozen_shade(int player, int card, event_t event)
{
  int *info_slot_ptr;
  int result;
  unsigned int *state_ptr;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    result = 0;
  }
  else if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_BLACK];
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_BLACK, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e503e(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_BLACK, 1);
    if (result != 0)
    {
      if (player == human_player)
      {
        charge_mana(player, COLOR_BLACK, -1);
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1)
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
          state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot;
          *state_ptr |= 0x80000;
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
      spell_fizzled = 1;
    }
    else
    {
      info_slot_ptr =
          &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot;
      *info_slot_ptr += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      info_slot_ptr =
          &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot;
      *info_slot_ptr += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        state_ptr =
            (unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                                  PLAYER_CARD_INSTANCE(player, card).parent_card)
                .info_slot;
        *state_ptr &= 0xfff7ffff;
        result = create_legacy_effect(card_on_stack_controller,
                                      card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      card_on_stack_controller,
                                      card_on_stack);
        if (result != -1)
        {
          PLAYER_CARD_INSTANCE(player, result).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, result).counter_toughness = 1;
          state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, result).info_slot;
          *state_ptr |= 0x80000;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    result = has_mana(player, COLOR_BLACK, 1);
  }
  else if (event == EVENT_TOU_BOOST)
  {
    result = has_mana(player, COLOR_BLACK, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && (raw_mana_available[player][COLOR_BLACK] != 0))
  {
    event_result |= 1;
    result = 0;
  }
  else
  {
    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
    result = 0;
  }

  return result;
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
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "ROYAL_ASSASSIN");
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
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
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
    FUN_004e4ff3(0);
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
      spell_fizzled = 1;
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
  else if (((event == EVENT_ATTACK_RATING) && (affected_card == card)) && (affected_card_controller == player))
  {
    unk_00925d3c += 0x30;
    return 0;
  }
  else if (((event == EVENT_BLOCK_RATING) && (affected_card == card)) && (affected_card_controller == player))
  {
    unk_00925d3c += -0x30;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d4c9
// FUNCTION: SHANDALAR 0x0048b71c
int card_hypnotic_specter(int player, int card, event_t event)
{
  if ((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4))
  {
    if ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = affected_card;
    }
  }
  else if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (affected_card == card)) && (affected_card_controller == player) && (PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1)) && (player == current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      discard((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, 1, 0);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005237dc
// FUNCTION: SHANDALAR 0x004dafcb
int card_bad_moon(int player, int card, event_t event)
{
  if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && (((1 << (char)get_sleighted_color(player, card, COLOR_BLACK)) & PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color) != 0))
  {
    ++event_result;
  }

  if (event == EVENT_CAN_CAST)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x00520ab6
// FUNCTION: SHANDALAR 0x004d829b
int card_cursed_land(int player, int card, event_t event)
{
  int ai_tweak;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "CURSED_LAND");
    }
    spell_fizzled = (unsigned int)(FUN_00551b60(player, 1 - player, card) == 0);
    if (spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec)
      {
        ai_modifier += 0x30;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }
  else if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  }
  else
  {
    if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
    {
      ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
      if (ai_tweak < 2)
      {
        ai_tweak = 1;
      }
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec)
      {
        ai_modifier += ai_tweak * 0x18;
      }
      else
      {
        ai_modifier += ai_tweak * -0x18;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00521465
// FUNCTION: SHANDALAR 0x004d8c4a
int card_evil_presence(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "EVIL_PRESENCE");
    }
    spell_fizzled = (unsigned int)(FUN_00551b60(player, 1 - player, card) == 0);
    if (spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec)
      {
        ai_modifier += 0x40 / (basiclandtypes_controlled[unk_008b35ec][7] + 1);
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
      {
        ai_modifier += -0x60;
      }
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
              .internal_card_id == 0)
      {
        ai_modifier += -0x90;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                (char *)0,
                                player,
                                ANYBODY,
                                ANYBODY,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;

      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .internal_card_id = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x1000000;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x20000) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && (affected_card != -1)))) && is_in_play(player, card))
  {
    event_result = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052dc98
// FUNCTION: SHANDALAR 0x004e5495
int card_unholy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text((int)"prompts.txt", "UNHOLY_STRENGTH");
  }
  return FUN_0052dd74(player, card, event, 2, 1);
}

// FUNCTION: MAGIC 0x0052dd05
// FUNCTION: SHANDALAR 0x004e5502
int card_weakness(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text((int)"prompts.txt", "WEAKNESS");
  }
  return FUN_0052dd74(player, card, event, -2, -1);
}

// FUNCTION: MAGIC 0x005301b4
// FUNCTION: SHANDALAR 0x004e79b0
int card_warp_artifact(int player, int card, event_t event)
{
  int ai_tweak;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 0x40,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "WARP_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             0,
                             0,
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec)
      {
        ai_modifier +=
            ((char)global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                         .internal_card_id]
                     .cc[1] *
                 3 +
             3) *
            4;
      }
      else
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player) && ((unk_00926804 & 2) == 0))
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state |= 0x40000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }
  else if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  }
  else
  {
    if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
    {
      ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
      if (ai_tweak < 2)
      {
        ai_tweak = 1;
      }
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec)
      {
        ai_modifier += ai_tweak * 0x18;
      }
      else
      {
        ai_modifier += ai_tweak * -0x18;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x005335b5
// FUNCTION: SHANDALAR 0x004eadb0
int card_phantasmal_terrain(int player, int card, event_t event)
{
  int land_type;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "PHANTASMAL_TERRAIN");
    }
    if (FUN_00551b60(player, 1 - player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      if ((player == unk_008b35ec) || ((unk_00926804 & 2) != 0))
      {
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "PHANTASMAL_TERRAIN2");
        }
        land_type = choose_a_color(player,
                                   text_lines[1],
                                   0,
                                   -1,
                                   COLOR_TEST_WHITE | COLOR_TEST_RED | COLOR_TEST_GREEN | COLOR_TEST_BLUE | COLOR_TEST_BLACK);
        if (land_type == -1)
        {
          spell_fizzled = 1;
        }
      }
      else if (unk_008a9000 == 1)
      {
        land_type = internal_rand(5) + 1;
        unk_00939340 = land_type;
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
        land_type = unk_00939340;
      }

      if (spell_fizzled != 1)
      {
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
        {
          ai_modifier += -0x30;
        }
        PLAYER_CARD_INSTANCE(player, card).info_slot = land_type;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      --PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .internal_card_id = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x1000000;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x200) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && (affected_card != -1)))) && is_in_play(player, card))
    {
      event_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0052f80a
// FUNCTION: SHANDALAR 0x004e7006
int card_copy_artifact(int player, int card, event_t event)
{
  target_t selected_target;
  int copied_internal_id;
  int result;
  int special_copy;

  if (event == EVENT_CAN_CAST)
  {
    result = real_target_available((int *)0,
                                   TARGET_SCAN_DIRECT,
                                   player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_ARTIFACT,
                                   TYPE_NONE,
                                   0,
                                   get_protections_from(player, card),
                                   0,
                                   0,
                                   -1,
                                   -1,
                                   0xffffffff,
                                   0xffffffff,
                                   TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                   0,
                                   0);
  }
  else if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "COPY_ARTIFACT");
    }

    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card)
          .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
          .player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card)
          .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
          .card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    result = 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                    PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                    (char *)0,
                                    player,
                                    2,
                                    2,
                                    TARGET_ZONE_IN_PLAY,
                                    TYPE_ARTIFACT,
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
                                    0);
    if (result == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      special_copy =
          global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                .original_internal_card_id]
                  .id ==
              CARD_ID_PRIMAL_CLAY ||
          global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                .original_internal_card_id]
                  .id ==
              CARD_ID_SHAPESHIFTER;
      if (special_copy)
      {
        copied_internal_id =
            create_a_card_type(PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                    PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                   .internal_card_id);
      }
      else
      {
        copied_internal_id =
            create_a_card_type(PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                    PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                   .original_internal_card_id);
      }

      if (copied_internal_id != -1)
      {
        PLAYER_CARD_INSTANCE(player, card).dummy3 = copied_internal_id;
        PLAYER_CARD_INSTANCE(player, card).internal_card_id = copied_internal_id;
        PLAYER_CARD_INSTANCE(player, card).regen_status = KEYWORD_RECALC_CHANGE_TYPE;
        global_cards_data[copied_internal_id].type |= TYPE_ENCHANTMENT;
      }

      PLAYER_CARD_INSTANCE(player, card).color =
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
              .color;
      if (special_copy)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) !=
            0)
        {
          ++*(int *)(gs_cardtitle_damage_008cfd30 + player * 4 + 0x50);
        }
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_ARTIFACT) !=
            0)
        {
          ++artifact_cards_in_play[player];
        }
        if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type &
             TYPE_ENCHANTMENT) != 0)
        {
          ++enchantments_in_play[player];
        }
        card_types_in_play[player] |=
            (unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
        PLAYER_CARD_INSTANCE(player, card).state |=
            (((player == 0) - 1) & STATE_POWER_STRUGGLE) | STATE_ETB_THIS_TURN |
            STATE_SUMMONSICK | STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP |
            STATE_IN_PLAY;
        trigger_cause_controller = player;
        trigger_cause = card;
        dispatch_trigger_twice_once_with_each_player_as_reason(human_player,
                                                               TRIGGER_COMES_INTO_PLAY,
                                                               &gs_card_into_play_0091c840,
                                                               0);
      }
      else
      {
        FUN_0051a41c(player, card);
      }
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    result = 0;
  }
  else
  {
    if (event == EVENT_CHANGE_TYPE && (unk_008b4278 & 0x20000) == 0 &&
        card == affected_card && player == affected_card_controller && is_in_play(player, card))
    {
      event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
    }

    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00534336
// FUNCTION: SHANDALAR 0x004ebb33
int card_flight(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "FLIGHT");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec)
      {
        ai_modifier += -0x18;
      }
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card)
               .regen_status &
           KEYWORD_FLYING) != 0)
      {
        ai_modifier += -99;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((PLAYER_CARD_INSTANCE(player, card).info_slot != 0) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && (event == EVENT_ABILITIES))))
    {
      event_result |= KEYWORD_FLYING;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00522a4f
// FUNCTION: SHANDALAR 0x004da23c
int card_psychic_venom(int player, int card, event_t event)
{
  int land_color;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "PSYCHIC_VENOM");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
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
                             0,
                             0,
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec)
      {
        land_color = single_color_test_bit_to_color_t(
            global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                  .internal_card_id]
                .color);
        ai_modifier += 0x60 / (basiclandtypes_controlled[unk_008b35ec][land_color] + 1);
      }
      if (selected_target.player == active_player)
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player) && ((unk_00926804 & 2) == 0))
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state |= 0x40000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((event == EVENT_TAP_CARD) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && (affected_card != -1)))
    {
      damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 2, player, card);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00524db3
// FUNCTION: SHANDALAR 0x004dc5a1
int card_creature_bond(int player, int card, event_t event)
{
  int effect_card;

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "CREATUREBOND");
    }
    spell_fizzled = (unsigned int)(FUN_00551638(player, 1 - player, card) == 0);
    if (spell_fizzled != 1)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec)
      {
        ai_modifier += 0x18;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && ((effect_card = add_card_to_hand(player, unk_00939348)) != -1))))
    {
      PLAYER_CARD_INSTANCE(player, effect_card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, effect_card).state |= STATE_IN_PLAY;
      *(int *)&PLAYER_CARD_INSTANCE(player, effect_card).display_pic_csv_id = 0x32;
      PLAYER_CARD_INSTANCE(player, effect_card).info_slot =
          C_get_abilities(affected_card_controller, affected_card, EVENT_TOUGHNESS, -1);
      PLAYER_CARD_INSTANCE(player, effect_card).damage_target_player = (char)affected_card_controller;
      TENTATIVE_set_timestamps(player, effect_card);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0051a64c
// FUNCTION: SHANDALAR 0x004d1e33
int card_animate_artifact(int player, int card, event_t event)
{
  int animated_internal_card_id;
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 0x40,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "ANIMATE_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             0,
                             0,
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      ai_modifier += -0x18;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      animated_internal_card_id = create_a_card_type(
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
              .internal_card_id);
      if (animated_internal_card_id != -1)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                    PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                                   .internal_card_id]
                 .type &
             0x42) == 0x40)
        {
          global_cards_data[animated_internal_card_id].toughness =
              (short)(char)global_cards_data
                  [PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                        PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                       .internal_card_id]
                      .cc[1];
          global_cards_data[animated_internal_card_id].power =
              global_cards_data[animated_internal_card_id].toughness;
        }
        global_cards_data[animated_internal_card_id].type = 0x42;
        PLAYER_CARD_INSTANCE(player, card).info_slot = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .regen_status |= 0x1000000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x200) == 0)) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && (affected_card != -1)))) && is_in_play(player, card))
    {
      event_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .token_status |= 0x40;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0051d83c
// FUNCTION: SHANDALAR 0x004d5020
int card_feedback(int player, int card, event_t event)
{
  int ai_tweak;
  target_t selected_target;

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
    if (ai_tweak < 2)
    {
      ai_tweak = 1;
    }
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec)
    {
      ai_modifier += ai_tweak * 0x18;
    }
    else
    {
      ai_modifier += ai_tweak * -0x18;
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
                                 4,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "FEEDBACK");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ENCHANTMENT,
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
                             0,
                             0,
                             text_lines[0],
                             1,
                             &selected_target) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec)
      {
        ai_modifier += 0x30;
      }
      if (selected_target.player == active_player)
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ENCHANTMENT,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }
  else if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00523399
// FUNCTION: SHANDALAR 0x004dab86
int card_lifetap(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if ((event == EVENT_TAP_CARD) && (affected_card_controller != player))
    {
      color = get_hacked_color(player, card, 3);
      if (FUN_0048463d(affected_card_controller, affected_card, color) != 0)
      {
        gain_life(player, 1, player, card);
        return 0;
      }
    }
    if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card)) && (affected_card_controller == player))
    {
      color = get_hacked_color(player, card, 3);
      ai_modifier += (basiclandtypes_controlled[unk_008b35ec][color] * 3 + 3) * 8;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00533e91
// FUNCTION: SHANDALAR 0x004eb68f
int card_wild_growth(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "WILD_GROWTH");
    }
    if (FUN_00551b60(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
    {
      ai_modifier += basiclandtypes_controlled[active_player][COLOR_GREEN] * 0xc;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (((event == EVENT_TAP_CARD) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && (trigger_cause != -1))))
  {
    undeclare_mana_available_and_produce_it((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                            COLOR_GREEN,
                                            1);
    return 0;
  }
  else
  {
    if ((event == EVENT_COUNT_MANA) && (((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)) && ((affected_card != -1) && ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & STATE_TAPPED) == 0))))
    {
      declare_mana_available(affected_card_controller, COLOR_GREEN, 1);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0053152d
// FUNCTION: SHANDALAR 0x004e8d27
int card_regeneration(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  card_instance_t *target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  parent = (card_instance_t *)0;
  target = (card_instance_t *)0;

  if (event == EVENT_UNTAP_PHASE)
  {
    unk_00939520[player][COLOR_GREEN] += 2;
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
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
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }
  else if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "REGENERATION");
    }

    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }

    if (spell_fizzled != 1 && player == active_player && (unk_00926804 & 2) == 0)
    {
      target = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
      if ((target->regen_status & 0x200) != 0 || instance->targets[0].player == unk_008b35ec)
      {
        ai_modifier -= 0x30;
      }
      if (FUN_0052adf2(player, card) != 0)
      {
        ai_modifier -= 0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE && (unk_008b4278 & 0x200) != 0 && instance->info_slot == 0)
  {
    target = &PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card);
    if (has_mana_w_global_cost_mod(player, card, 3, 1) != 0 && target->kill_code == KILL_DESTROY && (target->state & 0x800002) == STATE_IN_PLAY)
    {
      return 99;
    }
    return 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    FUN_004e4ff3(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    charge_mana_w_global_cost_mod(player, card, 3, 1);
    if (spell_fizzled != 1)
    {
      unk_007a7c1c = 1;
      ++instance->info_slot;
    }
    return 0;
  }
  else
  {
    if (event == EVENT_RESOLVE_ACTIVATION && (unk_008b4278 & 0x200) != 0)
    {
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->info_slot = 0;
      FUN_00542a2a(instance->damage_target_player, instance->damage_target_card);
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x005293ba
// FUNCTION: SHANDALAR 0x004e0bab
int card_web(int player, int card, event_t event)
{
  int test_card;
  int greatest_power;

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "WEB");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      if ((spell_fizzled != 1) && (active_player == player) && ((unk_00926804 & 2) == 0))
      {
        greatest_power = -1;
        for (test_card = 0; test_card < active_cards_count[1 - player]; ++test_card)
        {
          if (is_in_play(1 - player, test_card) && (greatest_power < PLAYER_CARD_INSTANCE(1 - player, test_card).power))
          {
            greatest_power = PLAYER_CARD_INSTANCE(1 - player, test_card).power;
          }
        }

        if ((PLAYER_CARD_INSTANCE(player, card).targets[0].card != -1) && (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).power <= greatest_power) && (greatest_power < PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).power + 2))
        {
          ai_modifier += greatest_power * 3 + 0x18;
        }

        if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                  PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                 .regen_status &
             (KEYWORD_FLYING | KEYWORD_TRAMPLE)) == 0)
        {
          ai_modifier += PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                              PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                             .power;
        }
        else
        {
          ai_modifier += -0x18;
        }
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }
  else
  {
    if (((event == EVENT_TOUGHNESS) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
    {
      event_result += 2;
    }
    if (((event == EVENT_POWER) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
    {
      event_result |= KEYWORD_REACH;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052b9f0
// FUNCTION: SHANDALAR 0x004e31e9
int card_wanderlust(int player, int card, event_t event)
{
  int life_delta;

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "WANDERLUST");
    }
    if (FUN_00551638(player, 1 - player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec)
      {
        ai_modifier += 0x30;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
      {
        ai_modifier += -0x60;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player)) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }
  else
  {
    if (event == EVENT_CLEANUP)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    }
    if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
    {
      life_delta = 0x18 - life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player];
      if (life_delta < 2)
      {
        life_delta = 1;
      }
      if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec)
      {
        ai_modifier += life_delta * 0x18;
      }
      else
      {
        ai_modifier += life_delta * -0x18;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0052c052
// FUNCTION: SHANDALAR 0x004e384b
int card_instill_energy(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *target;
  int selected_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
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
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }
  else if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "INSTILL_ENERGY");
    }

    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }

    if (spell_fizzled != 1 && player == active_player)
    {
      target = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
      if (global_cards_data[target->internal_card_id].subtype == 0 && (target->token_status & STATUS_WALL_CAN_ATTACK) == 0)
      {
        ai_modifier -= 0x30;
      }
      if ((global_cards_data[target->internal_card_id].extra_ability & 1) != 0 && instance->targets[0].player == player)
      {
        ai_modifier += 0x30;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      target = &PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card);
      if ((target->state & STATE_IN_PLAY) != 0)
      {
        instance->eot_toughness = 1;
        target->state &= 0xfffcffff;
      }
    }
    instance->number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    selected_color = single_color_test_bit_to_color_t((unsigned char)instance->color);
    if (unk_0072c440[selected_color] == 0 || has_mana_w_global_cost_mod(player, card, 7, 0) != 0)
    {
      if (instance->info_slot == 0 && affected_card_controller == human_player && (instance->state & STATE_INVISIBLE) == 0)
      {
        return 1;
      }
    }
    return 0;
  }
  else if (event == EVENT_ACTIVATE && instance->info_slot == 0)
  {
    selected_color = single_color_test_bit_to_color_t((unsigned char)instance->color);
    if (unk_0072c440[selected_color] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 0, 0);
    }

    if (spell_fizzled != 1)
    {
      ++instance->info_slot;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      target = &PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card);
      target->state &= ~STATE_TAPPED;
    }
    return 0;
  }
  else
  {
    if (trigger_condition == TRIGGER_LEAVE_PLAY && affected_card == card && affected_card_controller == player && instance->eot_toughness != 0 && instance->damage_target_player != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1 && trigger_cause_controller == player && trigger_cause == card && current_turn == player)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).state |= 0x30000;
      }
    }

    if (event == EVENT_CLEANUP && affected_card == card && affected_card_controller == player)
    {
      instance->eot_toughness = 0;
      instance->info_slot = 0;
    }

    if (event == EVENT_CARDCONTROLLED && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1)
    {
      target = &PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card);
      if ((target->state & STATE_IN_PLAY) != 0)
      {
        instance->eot_toughness = 1;
        target->state &= 0xfffcffff;
      }
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x005347a7
// FUNCTION: SHANDALAR 0x004ebfa3
int card_lifeforce(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    ai_modifier += basiclandtypes_controlled[1 - player][COLOR_GREEN] * 5 + 0x18;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (card_on_stack_controller == -1)
    {
      return 0;
    }
    if (((unk_008b4278 & 0x20) != 0) && (has_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2) != 0))
    {
      color = get_sleighted_color(player, card, COLOR_BLACK);
      if (C_real_validate_target(card_on_stack_controller,
                                 card_on_stack,
                                 (char *)0,
                                 player,
                                 ANYBODY,
                                 ANYBODY,
                                 0,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 1 << (((unsigned char)color) & 0x1f),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_SPELL_ON_STACK,
                                 0,
                                 0) != 0)
      {
        return 99;
      }
    }
    return 0;
  }
  else if ((event == EVENT_ACTIVATE) && (card_on_stack_controller != -1))
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2) != 0)
    {
      charge_mana_w_global_cost_mod(player, card, COLOR_GREEN, 2);
      if (spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = card_on_stack_controller;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card_on_stack;
      }
    }
    return 0;
  }
  else
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      color = get_sleighted_color(player, card, COLOR_BLACK);
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                 (char *)0,
                                 player,
                                 ANYBODY,
                                 ANYBODY,
                                 0,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 1 << (((unsigned char)color) & 0x1f),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_SPELL_ON_STACK,
                                 0,
                                 0) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                  KILL_BURY);
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00523a60
// FUNCTION: SHANDALAR 0x004db252
int card_aspect_of_wolf(int player, int card, event_t event)
{
  int color;

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "ASPECTOFWOLF");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else if ((spell_fizzled != 1) && (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec))
    {
      ai_modifier += -0x18;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((event == EVENT_POWER) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
    {
      color = get_hacked_color(player, card, 3);
      event_result += basiclandtypes_controlled[player][color] / 2;
    }
    if (((event == EVENT_TOUGHNESS) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
    {
      color = get_hacked_color(player, card, 3);
      event_result += (basiclandtypes_controlled[player][color] + 1) / 2;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00521abd
// FUNCTION: SHANDALAR 0x004d92a4
int card_living_artifact(int player, int card, event_t event)
{
  card_instance_t *instance;
  int ai_scale;
  int color_index;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  ai_scale = 0;
  color_index = 0;

  if (event == EVENT_DEAL_DAMAGE && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4)
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
    {
      instance->eot_toughness += PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot;
    }
    return 0;
  }
  else
  {
    if (trigger_condition == TRIGGER_DEAL_DAMAGE && affected_card == card && affected_card_controller == player && instance->eot_toughness != 0 && current_turn == player)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        FUN_005513d7(player, card, instance->eot_toughness);
        instance->eot_toughness = 0;
      }
    }

    if (event == EVENT_CAN_CAST)
    {
      return real_target_available((int *)0,
                                   TARGET_SCAN_DIRECT,
                                   player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_ARTIFACT,
                                   TYPE_NONE,
                                   0,
                                   get_protections_from(player, card),
                                   0,
                                   0,
                                   -1,
                                   -1,
                                   0xffffffff,
                                   0xffffffff,
                                   0,
                                   0,
                                   0);
    }
    else if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "LIVING_ARTIFACT");
      }

      if (!FUN_00551ed7(player, 2, card))
      {
        spell_fizzled = 1;
      }

      if (spell_fizzled != 1)
      {
        if (life[player] < life[1 - player])
        {
          ai_scale = 3;
        }
        else
        {
          ai_scale = 1;
        }

        if (instance->targets[0].player == unk_008b35ec)
        {
          ai_modifier += global_cards_data[PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card)
                                               .internal_card_id]
                             .cc[1] *
                         ai_scale * 0x18;
        }
        if (instance->targets[0].player == active_player)
        {
          ai_modifier += (ai_scale * 0x18) / 2;
        }
      }
      return 0;
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(instance->targets[0].player,
                                 instance->targets[0].card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
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
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        instance->damage_target_player = instance->targets[0].player;
        instance->damage_target_card = instance->targets[0].card;
      }
      instance->number_of_targets = 0;
      return 0;
    }
    else if (event == EVENT_CAN_ACTIVATE)
    {
      if (current_phase == 4 && player == human_player && player == unk_00742f60 && instance->info_slot == 0 && C_get_special_counters(player, card) != 0)
      {
        color_index = single_color_test_bit_to_color_t((unsigned char)instance->color);
        if (unk_0072c440[color_index] == 0 || has_mana_w_global_cost_mod(player, card, 7, 0) != 0)
        {
          if (player == active_player && (unk_00926804 & 2) == 0)
          {
            unk_008b3270 |= 3;
          }
          return 1;
        }
      }
      return 0;
    }
    else if (event == EVENT_ACTIVATE && card == affected_card && player == affected_card_controller)
    {
      color_index = single_color_test_bit_to_color_t((unsigned char)instance->color);
      if (unk_0072c440[color_index] != 0)
      {
        charge_mana_w_global_cost_mod(player, card, 0, 0);
      }
      if (spell_fizzled != 1)
      {
        ++instance->info_slot;
        FUN_00551334(player, card);
      }
      return 0;
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      gain_life(player, 1, player, card);
      return 0;
    }
    else
    {
      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
      {
        instance->info_slot = 0;
        instance->eot_toughness = 0;
      }
      return 0;
    }
  }
}

// FUNCTION: MAGIC 0x004385c3
// FUNCTION: SHANDALAR 0x004faf62
int card_earthbind(int player, int card, event_t event)
{
  target_t selected_target;
  unsigned int illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 1 - player,
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
                                 0,
                                 0);
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
    {
      load_text((int)"promptsX1.txt", "EARTHBIND");
      illegal_abilities = get_protections_from(player, card);
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               illegal_abilities,
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
                               &selected_target) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets] =
            selected_target;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
        if ((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).regen_status & KEYWORD_FLYING) == 0)
        {
          ai_modifier += -0x60;
        }
        else
        {
          ai_modifier += 0x18;
        }
      }
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                                 0,
                                 0) == 0)
      {
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).damage_target_player =
            (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_card =
            PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    if ((((event == EVENT_ABILITIES) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)) && (affected_card != -1))
    {
      event_result &= 0xffffffdf;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0052b842
// FUNCTION: SHANDALAR 0x004e303a
int card_burrowing(int player, int card, event_t event)
{
  int result;

  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text((int)"prompts.txt", "BURROWING");
  }

  result = FUN_0052d7a5(player, card, event, 1 << ((char)get_hacked_color(player, card, 4) - 1U));
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (player == active_player))
  {
    if (((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).regen_status & (1 << ((char)get_hacked_color(player, card, 4) - 1U))) != 0) || (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec))
    {
      ai_modifier += -0x60;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00527e71
// FUNCTION: SHANDALAR 0x004df65e
int card_firebreathing(int player, int card, event_t event)
{
  int local;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939520[player][COLOR_RED];
  }

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "FIREBREATHING");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else if ((player == active_player) && ((unk_00926804 & 2) == 0))
    {
      if (FUN_0052adf2(player, card) != 0)
      {
        ai_modifier += -0x60;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        ai_modifier += 0xc;
      }
      else
      {
        ai_modifier += -0xc;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, 4, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == human_player)
    {
      local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
      if (raw_mana_available[player][local] == 0)
      {
        FUN_004e503e(0);
      }
      else
      {
        unk_00715fa8 = 1;
      }
    }
    else
    {
      unk_00715fa8 = 1;
    }

    event_result = ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) | PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, 4, 1) != 0)
    {
      if (player == human_player)
      {
        local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
        if (raw_mana_available[player][local] == 0)
        {
          charge_mana(player, 4, -1);
        }
        else
        {
          x_value = charge_mana_w_global_cost_mod(player, card, 4, 1);
        }
        if (x_value < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      }
      else
      {
        local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
        if (raw_mana_available[player][local] == 0)
        {
          charge_mana(player, 4, 1);
        }
        else
        {
          charge_mana_w_global_cost_mod(player, card, 4, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }
      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player =
            (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card =
            PLAYER_CARD_INSTANCE(player, card).damage_target_card;
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
    if (PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                PLAYER_CARD_INSTANCE(player, card).damage_target_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
             .info_slot &= 0xfff7ffff;
        local = create_legacy_effect(card_on_stack_controller,
                                     card_on_stack,
                                     unk_008a8de8,
                                     (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          PLAYER_CARD_INSTANCE(player, local).counter_power =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += ((basiclandtypes_controlled[player][COLOR_RED] * 3) + 3) << 2;
      }
      else
      {
        ai_modifier += -((((basiclandtypes_controlled[player][COLOR_RED] * 3) + 3) << 2));
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

// FUNCTION: MAGIC 0x00522fdc
// FUNCTION: SHANDALAR 0x004da7c9
int card_manabarbs(int player, int card, event_t event)
{
  if ((event == EVENT_TAP_CARD) && is_in_play(player, card))
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_LAND) && produced_mana_color != -1)
    {
      damage_player(affected_card_controller, 1, player, card);
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card)
  {
    if (affected_card_controller == player)
    {
      ai_modifier += (basiclandtypes_controlled[active_player][7] - basiclandtypes_controlled[unk_008b35ec][7]) * 0x18;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005238a7
// FUNCTION: SHANDALAR 0x004db096
int card_orcish_oriflamme(int player, int card, event_t event)
{
  if (event == EVENT_POWER)
  {
    if (((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & STATE_ATTACKING) != 0) && (player == human_player) && (player == affected_card_controller) && is_in_play(player, card))
    {
      ++event_result;
    }
    return 0;
  }
  else if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
    {
      ai_modifier += hand_count[player] * 0xc;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x005230ea
// FUNCTION: SHANDALAR 0x004da8d7
int card_mana_flare(int player, int card, event_t event)
{
  struct
  {
    int current_internal_id;
    int land_color;
    int color_index;
    int color_count;
  } locals;

  if ((event == EVENT_COUNT_MANA) && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_LAND))
  {
    if ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & STATE_TAPPED) == 0)
    {
      locals.land_color = *(char *)((char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card) + 0x58);
      locals.color_index = 0;
      locals.color_count = 0;
      for (; locals.color_index < 7; ++locals.color_index)
      {
        if ((locals.land_color & (1 << (unsigned char)locals.color_index)) != 0)
        {
          ++locals.color_count;
        }
      }

      if (locals.color_count > 0)
      {
        declare_mana_available_hex(affected_card_controller, locals.land_color, 1);
      }
      else
      {
        declare_mana_available(affected_card_controller, single_color_test_bit_to_color_t(locals.land_color), 1);
      }
    }
    return 0;
  }

  if ((event == EVENT_TAP_CARD) && is_in_play(player, card))
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id != -1)
    {
      locals.current_internal_id = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id;
    }
    else
    {
      locals.current_internal_id = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).original_internal_card_id;
    }

    if ((global_cards_data[locals.current_internal_id].type & TYPE_LAND) != 0)
    {
      if (produced_mana_color != -1)
        produce_mana(affected_card_controller, produced_mana_color, 1);

      return 0;
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (affected_card == card)
    {
      if (affected_card_controller == player)
      {
        ai_modifier += 0x30;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00526202
// FUNCTION: SHANDALAR 0x004dd9ef
int card_holy_armor(int player, int card, event_t event)
{
  int amount;
  int color;
  int effect_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][1];
  }

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "HOLY_ARMOR");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else if ((player == active_player) && ((unk_00926804 & 2) == 0))
    {
      ai_modifier += -0x30;
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        ai_modifier += 0xc;
      }
      else
      {
        ai_modifier += -0xc;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      color = single_color_test_bit_to_color_t((unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
      if (player == human_player)
      {
        if (raw_mana_available[player][color] == 0)
        {
          charge_mana(player, COLOR_WHITE, -1);
        }
        else
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
          amount = 1;
        }
        if (amount < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = amount;
        }
      }
      else
      {
        if (raw_mana_available[player][color] == 0)
        {
          charge_mana(player, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }
      if (spell_fizzled == 1)
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
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .info_slot &= 0xffff00ff;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                PLAYER_CARD_INSTANCE(player, card).damage_target_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .info_slot &= 0xfff7ffff;
        effect_card = create_legacy_effect(card_on_stack_controller,
                                           card_on_stack,
                                           unk_008a8de8,
                                           (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                           PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (effect_card != -1)
        {
          *(short *)&PLAYER_CARD_INSTANCE(player, effect_card).initial_color =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, effect_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_WHITE] * 0xc;
      }
      else
      {
        ai_modifier += basiclandtypes_controlled[player][COLOR_WHITE] * -0xc;
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

// FUNCTION: MAGIC 0x0052e233
// FUNCTION: SHANDALAR 0x004e5a30
int card_castle(int player, int card, event_t event)
{
  if ((event == EVENT_TOUGHNESS) && is_in_play(player, card))
  {
    if ((player == affected_card_controller) && ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & 0x14) == 0) && is_in_play(affected_card_controller, affected_card))
    {
      event_result += 2;
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x0052dae0
// FUNCTION: SHANDALAR 0x004e52db
int card_holy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text((int)"prompts.txt", "HOLY_STRENGTH");
  }

  return FUN_0052dd74(player, card, event, 1, 2);
}

// FUNCTION: MAGIC 0x0052e2ed
// FUNCTION: SHANDALAR 0x004e5aea
int card_black_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_BLACK));
}

// FUNCTION: MAGIC 0x0052e324
// FUNCTION: SHANDALAR 0x004e5b21
int card_green_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_GREEN));
}

// FUNCTION: MAGIC 0x0052e35b
// FUNCTION: SHANDALAR 0x004e5b58
int card_blue_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_BLUE));
}

// FUNCTION: MAGIC 0x0052e392
// FUNCTION: SHANDALAR 0x004e5b8f
int card_red_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_RED));
}

// FUNCTION: MAGIC 0x0052e3c9
// FUNCTION: SHANDALAR 0x004e5bc6
int card_white_ward(int player, int card, event_t event)
{
  return helper_ward(player, card, event, get_sleighted_color(player, card, COLOR_WHITE));
}

// FUNCTION: MAGIC 0x0052112c
// FUNCTION: SHANDALAR 0x004d8911
int card_karma(int player, int card, event_t event)
{
  int color;
  int damage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (((current_phase == EVENT_UPKEEP_PHASE) && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0)) && ((unk_00742f60 == human_player) && ((color = get_hacked_color(player, card, 1),
                                                                                                                                                 basiclandtypes_controlled[unk_00742f60][color] != 0))))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    color = get_hacked_color(player, card, 1);
    damage_player(human_player,
                  basiclandtypes_controlled[human_player][color],
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }
  else
  {
    if (event == EVENT_CLEANUP)
    {
      PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    }
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      damage_player(1 - human_player,
                    basiclandtypes_controlled[1 - human_player][get_hacked_color(player, card, 1)],
                    player,
                    card);
      if (is_in_play(player, card))
      {
        color = get_hacked_color(player, card, 1);
        if (basiclandtypes_controlled[unk_008b35ec][color] != 0)
        {
          damage = 0x18 - life[unk_008b35ec] / basiclandtypes_controlled[unk_008b35ec][color];
          if (damage < 2)
          {
            damage = 1;
          }
          ai_modifier += damage * 0x18;
        }
        color = get_hacked_color(player, card, 1);
        if (basiclandtypes_controlled[active_player][color] != 0)
        {
          damage = 0x18 - life[active_player] / basiclandtypes_controlled[active_player][color];
          if (damage < 2)
          {
            damage = 1;
          }
          ai_modifier += damage * -0x18;
        }
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00438b5c
// FUNCTION: SHANDALAR 0x004fb4fd
int card_farmstead(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  parent = (card_instance_t *)0;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }
  else
  {
    if (event == EVENT_CAST_SPELL && affected_card == card && player == affected_card_controller)
    {
      load_text("promptsX1.txt", "FARMSTEAD");
      if (!FUN_00551b60(player, player, card))
      {
        spell_fizzled = 1;
      }
      if (spell_fizzled != 1)
      {
        if (instance->targets[0].player == unk_008b35ec)
        {
          ai_modifier -= 0x60;
        }
        if (instance->targets[0].player == active_player)
        {
          ai_modifier += 0x30;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if (C_real_validate_target(instance->targets[0].player,
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
                                 0) == 0)
      {
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      }
      else
      {
        instance->damage_target_player = instance->targets[0].player;
        instance->damage_target_card = instance->targets[0].card;
      }
      instance->number_of_targets = 0;
    }

    if (event == EVENT_CAN_ACTIVATE)
    {
      if (current_phase == 4 && instance->damage_target_player == human_player && instance->info_slot == 0 && has_mana_w_global_cost_mod(player, card, 5, 2) != 0)
      {
        if (player == active_player && (unk_00926804 & 2) == 0 && (internal_rand(100) < ((basiclandtypes_controlled[player][COLOR_WHITE] + 1) / 2) * 0x14 || life[active_player] < 5))
        {
          unk_008b3270 |= 3;
        }
        return 1;
      }
      return 0;
    }
    else
    {
      if (event == EVENT_ACTIVATE)
      {
        ++instance->info_slot;
        charge_mana_w_global_cost_mod(player, card, 5, 2);
        if (spell_fizzled == 1)
        {
          instance->info_slot = 0;
        }
        else
        {
          instance->targets[0].player = instance->damage_target_player;
          instance->targets[0].card = -1;
          instance->number_of_targets = 1;
        }
      }

      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        gain_life(instance->targets[0].player, 1, player, card);
        parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
        parent->number_of_targets = 0;
      }

      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
      {
        instance->info_slot = 0;
      }

      return 0;
    }
  }
}

// FUNCTION: MAGIC 0x00532b0e
// FUNCTION: SHANDALAR 0x004ea30b
int card_co_p_white(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_WHITE));
}

// FUNCTION: MAGIC 0x00532ad7
// FUNCTION: SHANDALAR 0x004ea2d4
int card_co_p_black(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLACK));
}

// FUNCTION: MAGIC 0x00532b7c
// FUNCTION: SHANDALAR 0x004ea379
int card_co_p_blue(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLUE));
}

// FUNCTION: MAGIC 0x00532b45
// FUNCTION: SHANDALAR 0x004ea342
int card_co_p_red(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_RED));
}

// FUNCTION: MAGIC 0x00532bb3
// FUNCTION: SHANDALAR 0x004ea3b0
int card_co_p_green(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_GREEN));
}

// FUNCTION: MAGIC 0x0052398e
// FUNCTION: SHANDALAR 0x004db180
int card_crusade(int player, int card, event_t event)
{
  int color;

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card))
  {
    if (is_in_play(affected_card_controller, affected_card))
    {
      color = get_sleighted_color(player, card, 5);
      if ((1 << (((unsigned char)color) & 0x1f) & (int)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color) != 0)
      {
        ++event_result;
      }
    }
    return 0;
  }

  return event == EVENT_CAN_CAST;
}

// FUNCTION: MAGIC 0x004ab9c2
// FUNCTION: SHANDALAR 0x004a3232
int card_blaze_of_glory(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return (current_phase == PHASE_BEFORE_BLOCKING && real_target_available((int *)0,
                                                                            TARGET_SCAN_DIRECT,
                                                                            player,
                                                                            1 - human_player,
                                                                            1 - human_player,
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
                                                                            0,
                                                                            0,
                                                                            0))
               ? 1
               : 0;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text("promptsX1.txt", "BLAZE_OF_GLORY");
    if (C_real_select_target(player,
                             1 - human_player,
                             1 - human_player,
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
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      spell_fizzled = 1;
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
                               1 - human_player,
                               1 - human_player,
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
      create_legacy_effect(player, card, unk_008a8df0, selected_target.player, selected_target.card);
    }
    else
    {
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052700b
// FUNCTION: SHANDALAR 0x004de7f8
int card_blessing(int player, int card, event_t event)
{
  int amount;
  int color;
  int effect_card;

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
                                 0,
                                 0,
                                 0);
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "BLESSING");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else if ((player == active_player) && ((unk_00926804 & 2) == 0))
    {
      ai_modifier += -0x60;
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        ai_modifier += 0xc;
      }
      else
      {
        ai_modifier += -0xc;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      color = single_color_test_bit_to_color_t((unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
      if (player == human_player)
      {
        if (raw_mana_available[player][color] == 0)
        {
          charge_mana(player, COLOR_WHITE, -1);
        }
        else
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
          amount = 1;
        }
        if (amount < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = amount;
        }
      }
      else
      {
        if (raw_mana_available[player][color] == 0)
        {
          charge_mana(player, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }
      if (spell_fizzled == 1)
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
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      *(char *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_target_card)
           .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                PLAYER_CARD_INSTANCE(player, card).damage_target_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .info_slot &= 0xfff7ffff;
        effect_card = create_legacy_effect(card_on_stack_controller,
                                           card_on_stack,
                                           unk_008a8de8,
                                           (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                           PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (effect_card != -1)
        {
          *(short *)&PLAYER_CARD_INSTANCE(player, effect_card).display_pic_csv_id =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          *(short *)&PLAYER_CARD_INSTANCE(player, effect_card).initial_color =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, effect_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == active_player)
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_WHITE] * 3 + 6) * 4;
      }
      else
      {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_WHITE] * 3 + 6) * -4;
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

// FUNCTION: MAGIC 0x00533bc2
// FUNCTION: SHANDALAR 0x004eb3bf
int card_conversion(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card)) && (affected_card_controller == player) && FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, -1) == 0)
  {
    ai_modifier +=
        (landsofcolor_controlled[active_player][COLOR_WHITE] - landsofcolor_controlled[1 - active_player][COLOR_WHITE]) * 0xc;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 5;
    return 0;
  }
  else if (((event == EVENT_DECLARE_BLOCKERS) && (affected_card == card)) && (affected_card_controller == player) && player == human_player && current_turn == human_player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    PLAYER_CARD_INSTANCE(player, card).untap_status += 2;
    return 0;
  }
  else if (event == 0x86)
  {
    kill_card(card_on_stack_controller, card_on_stack, KILL_BURY);
    return 0;
  }

  if ((event == EVENT_SET_COLOR) && ((unk_008b4278 & 0x200) == 0) && is_in_play(player, card) && is_in_play(affected_card_controller, affected_card) && FUN_00484581(affected_card_controller, get_hacked_color(player, card, 4)) != 0)
  {
    event_result = get_hacked_color(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot) - 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00408b2d
// FUNCTION: SHANDALAR 0x0044e84e
int card_wrath_of_god(int player, int card, event_t event)
{
  int current_card;
  int current_player;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          kill_card(current_player, current_card, KILL_BURY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00403a6b
// FUNCTION: SHANDALAR 0x00449791
int card_armageddon(int player, int card, event_t event)
{
  int current_player;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          kill_card(current_player, current_card, KILL_DESTROY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040bdc6
// FUNCTION: SHANDALAR 0x004024fc
int card_resurrection(int player, int card, event_t event)
{
  struct
  {
    int can_select;
    int selectable[500];
    int hand_card;
    int graveyard_index;
    char (*prompt)[300];
  } locals;

  if (event == EVENT_CAN_CAST)
  {
    return FUN_004832f4(player, 2);
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
    {
      if (((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1))
      {
        locals.can_select = 1;
        locals.graveyard_index = FUN_004087cc(player, 2);
      }
      else
      {
        for (locals.hand_card = 0;
             (locals.hand_card < 500 && global_graveyard_slots[player][locals.hand_card] != -1);
             ++locals.hand_card)
        {
          if ((global_cards_data[global_graveyard_slots[player][locals.hand_card]].type & TYPE_CREATURE) == 0)
          {
            locals.selectable[locals.hand_card] = 0;
          }
          else
          {
            locals.selectable[locals.hand_card] = 1;
          }
        }
        load_text((int)"promptsX1.txt", "RESURRECTION");
        locals.prompt = text_lines;
        locals.can_select = FUN_004a62d7(player,
                                         global_graveyard_slots[player],
                                         locals.selectable,
                                         500,
                                         (int)&locals.prompt,
                                         1,
                                         (int)&locals.graveyard_index,
                                         0,
                                         1);
      }

      if (locals.can_select == 0 || global_graveyard_slots[player][locals.graveyard_index] == -1)
      {
        spell_fizzled = 1;
      }
      else
      {
        locals.hand_card = add_card_to_hand(player, global_graveyard_slots[player][locals.graveyard_index]);
        if (locals.hand_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, locals.hand_card).state |= 0x20;
          PLAYER_CARD_INSTANCE(player, card).info_slot = locals.graveyard_index;
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = locals.hand_card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                  .internal_card_id == global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] &&
          (global_cards_data[global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]].type & TYPE_CREATURE) != 0)
      {
        FUN_0051a41c(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card);
        FUN_004b15f7(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).info_slot);
      }
      else
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .token_status |= 4;
        kill_card(player, card, KILL_DESTROY);
        spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_BURY);
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x00406d4f
// FUNCTION: SHANDALAR 0x0044ca6b
int card_raise_dead(int player, int card, event_t event)
{
  struct
  {
    int hand_card;
    int graveyard_index;
    int result;
  } locals;

  if (event == EVENT_CAN_CAST)
  {
    locals.result = 0;
    for (locals.hand_card = 0;
         locals.hand_card < 500 && locals.result == 0 && global_graveyard_slots[player][locals.hand_card] != -1;
         ++locals.hand_card)
    {
      if ((global_cards_data[global_graveyard_slots[player][locals.hand_card]].type & TYPE_CREATURE) != 0)
      {
        locals.result = 1;
      }
    }
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
    {
      if (((player == active_player) && (unk_00926804 & 2) == 0) || unk_008a9000 == 1)
      {
        locals.graveyard_index = FUN_004087cc(player, 2);
      }
      else
      {
        if ((player == unk_008b35ec) && (unk_008a9000 != 1))
        {
          load_text("prompts.txt", "RAISEDEAD");
        }
        do
        {
          locals.graveyard_index =
              show_deck(player, global_graveyard_slots[player], 500, text_lines, 0, gs_cancel_008a8c20);
        } while (locals.graveyard_index != -1 && (global_cards_data[global_graveyard_slots[player][locals.graveyard_index]].type & TYPE_CREATURE) == 0);
      }

      if (locals.graveyard_index == -1 || global_graveyard_slots[player][locals.graveyard_index] == -1 || (global_cards_data[global_graveyard_slots[player][locals.graveyard_index]].type & TYPE_CREATURE) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        locals.hand_card = add_card_to_hand(player, global_graveyard_slots[player][locals.graveyard_index]);
        PLAYER_CARD_INSTANCE(player, locals.hand_card).state |= 0x20;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = locals.hand_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 108)) = locals.graveyard_index;
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      locals.graveyard_index = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 108));
      if (locals.graveyard_index == -1 || global_graveyard_slots[player][locals.graveyard_index] == -1 || (global_cards_data[global_graveyard_slots[player][locals.graveyard_index]].type & TYPE_CREATURE) == 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .targets[6]
            .card = -1;
      }
      else
      {
        FUN_004b15f7(player, locals.graveyard_index);
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state &= 0xffffffdf;
        ++hand_count[player];
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_BURY);
    }

    locals.result = 0;
  }
  return locals.result;
}

// FUNCTION: MAGIC 0x004097ba
// FUNCTION: SHANDALAR 0x0044f4db
int card_drain_life(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *effect;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, COLOR_BLACK, 3) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if ((unk_008b4278 & 0x400) == 0)
    {
      x_value = 0;
      charge_mana(player, COLOR_COLORLESS, -1);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      instance->info_slot = x_value;
    }
    else
    {
      instance->info_slot = PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).info_slot;
    }
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "DRAIN_LIFE");
    }
    FUN_0054ac4d(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    effect_card = FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, instance->info_slot);
    if (effect_card == 0 || instance->info_slot < 1)
    {
      instance->info_slot = 0;
    }
    else
    {
      effect_card = add_card_to_hand(player, 0xD7);
      if (effect_card != -1)
      {
        effect = &PLAYER_CARD_INSTANCE(player, effect_card);
        effect->damage_target_player = instance->internal_card_id;
        effect->state |= 2;
        *(int *)((char *)effect + 0x5c) = 0x44;
        effect->token_status = 0xD7;
        TENTATIVE_set_timestamps(player, effect_card);
        effect->damage_source_player = (char)player;
        effect->damage_source_card = card;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  if (((event == EVENT_DEAL_DAMAGE) && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4) && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
  {
    instance->damage_source_player = (char)affected_card_controller;
    instance->damage_source_card = affected_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004019b4
// FUNCTION: SHANDALAR 0x004476d4
int card_braingeyser(int player, int card, event_t event)
{
  target_t target;
  int cards_to_draw;
  int target_player;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, 7, 3) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "BRAINGEYSER");
    }
    if (C_real_select_target(player,
                             2,
                             player,
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
                             &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    for (cards_to_draw = 0; cards_to_draw < PLAYER_CARD_INSTANCE(player, card).info_slot; ++cards_to_draw)
    {
      FUN_0043e18b(target_player);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040948b
// FUNCTION: SHANDALAR 0x0044f1ac
int card_disintegrate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;
  int damage_dealt;
  int legacy_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    instance->info_slot = x_value;
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "DISINTEGRATE");
    }
    FUN_0054ac4d(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
    ai_modifier -= 0x30 / FUN_00404c4c(player, instance->internal_card_id);
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    damage_dealt = FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, instance->info_slot);
    if (damage_dealt != 0 && target_card != -1)
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3d10, target_player, target_card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x200;
      }
      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status = 0x8000000;
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00409df5
// FUNCTION: SHANDALAR 0x0044fb16
int card_stone_rain(int player, int card, event_t event)
{
  int target_player;
  int target_card;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 1,
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
                                 0);
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (card == affected_card)
    {
      if (player == affected_card_controller)
      {
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "STONE_RAIN");
        }
        if (FUN_00551b60(player, 2, card) != 0)
        {
          ai_modifier +=
              ((-(unsigned int)(PLAYER_CARD_INSTANCE(player, card).targets[0].player == player) & 0xfffffffbU) * 3 + 9) * 4;
        }
        else
        {
          spell_fizzled = 1;
        }
        if (spell_fizzled == 1)
        {
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        }
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(target_player,
                               target_card,
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
      spell_fizzled = 1;
    }
    else
    {
      kill_card(target_player, target_card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00403557
// FUNCTION: SHANDALAR 0x0044927d
int card_earthquake(int player, int card, event_t event)
{
  int current_card;
  int current_player;
  int damage_amount;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x24 / FUN_00404c4c(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_amount = PLAYER_CARD_INSTANCE(player, card).info_slot;
    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      damage_player(current_player, damage_amount, player, card);
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) == 0)
        {
          damage_creature(current_player, current_card, damage_amount, player, card);
        }
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
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00404dce
// FUNCTION: SHANDALAR 0x0044aaee
int card_fireball(int player, int card, event_t event)
{
  struct
  {
    int player_target_selected[2];
    int possible_targets;
    int damage_per_target;
    int target_index;
    int select_ok;
    target_t chosen_target;
    int num_targets;
  } s;
  int result;
  int mana_available_minus_one;
  int mana_to_pay;
  int num_selected_targets;
  int invalid_targets;
  unsigned int illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    if (player == unk_008b35ec || (unk_00926804 & 2) != 0)
    {
      unk_0091bfb4 = 1;
    }
    else if (has_mana(player, 7, 2) == 0)
    {
      return 0;
    }

    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
    {
      result = FUN_00404c4c(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
      ai_modifier -= 0x48 / result;

      if (((player == active_player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;

        if (unk_008a9000 == 1)
        {
          if ((unk_008b4278 & 0x400) == 0)
          {
            illegal_abilities = get_protections_from(player, card);
            real_target_available(&s.possible_targets,
                                  TARGET_SCAN_DIRECT,
                                  player,
                                  2,
                                  1 - player,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE,
                                  TYPE_NONE,
                                  0,
                                  illegal_abilities,
                                  0,
                                  0,
                                  -1,
                                  -1,
                                  0xffffffff,
                                  0xffffffff,
                                  0,
                                  0,
                                  0);
            ++s.possible_targets;
            if (s.possible_targets > 5)
            {
              s.possible_targets = 5;
            }
            unk_00939340 =
                FUN_004c0a36(internal_rand((x_value + 1) / 2) + 1, 1, s.possible_targets);
          }
          else
          {
            unk_00939340 = PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).number_of_targets;
          }

          FUN_004e4f11();
        }
        else
        {
          FUN_004e5089();
        }

        s.num_targets = unk_00939340;
        if (s.num_targets == 99)
        {
          s.num_targets = 1;
        }

        result = x_value - s.num_targets;
        s.player_target_selected[0] = 0;

        for (s.target_index = 0; s.target_index < s.num_targets; ++s.target_index)
        {
          do
          {
            FUN_0054ac4d(player, card, s.damage_per_target);
            if (s.player_target_selected[0] == 0)
            {
              break;
            }
          } while (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1);

          PLAYER_CARD_INSTANCE(player, card).targets[(s.num_targets - 1) - s.target_index].player =
              PLAYER_CARD_INSTANCE(player, card).targets[0].player;
          PLAYER_CARD_INSTANCE(player, card).targets[(s.num_targets - 1) - s.target_index].card =
              PLAYER_CARD_INSTANCE(player, card).targets[0].card;
          if (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
          {
            s.player_target_selected[0] = 1;
          }
          else
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .state |= STATE_CANNOT_TARGET | STATE_TARGETTED;
          }
        }

        PLAYER_CARD_INSTANCE(player, card).number_of_targets = s.num_targets;
        PLAYER_CARD_INSTANCE(player, card).info_slot = (result + 1) / s.num_targets;

        for (s.target_index = 0; s.target_index < s.num_targets; ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                .state &= ~0x300000;
          }
        }
      }
      else if ((unk_008b4278 & 0x400) == 0)
      {
        mana_available_minus_one = has_mana(player, COLOR_ANY, 1) - 1;
        illegal_abilities = get_protections_from(player, card);
        real_target_available(&s.possible_targets,
                              TARGET_SCAN_DIRECT,
                              player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              0,
                              illegal_abilities,
                              0,
                              0,
                              -1,
                              -1,
                              0xffffffff,
                              0xffffffff,
                              0,
                              0,
                              0);
        s.possible_targets += 2;
        mana_to_pay = mana_available_minus_one;
        num_selected_targets = 1;
        if (mana_to_pay < 1)
        {
          spell_fizzled = 1;
        }
        else
        {
          num_selected_targets = 1;
          if (s.possible_targets > 1 && mana_to_pay > 2)
          {
            num_selected_targets = 2;
          }
          if (num_selected_targets > s.possible_targets)
          {
            num_selected_targets = s.possible_targets;
          }
          s.damage_per_target = mana_to_pay - (num_selected_targets - 1);
          if (s.damage_per_target < 1)
          {
            s.damage_per_target = 1;
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.damage_per_target;
          x_value = 0;
          unk_008ce510[4] = 1;
          charge_mana(player, COLOR_COLORLESS, mana_to_pay);
          if (spell_fizzled != 1)
          {
            x_value = mana_to_pay - (num_selected_targets - 1);
            PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
            s.player_target_selected[1] = 0;
            s.player_target_selected[0] = 0;
            s.target_index = 0;
            while (s.target_index < num_selected_targets && spell_fizzled != 1)
            {
              do
              {
                if (unk_008a9000 != 1)
                {
                  load_text((int)"prompts.txt", "FIREBALL");
                  sprintf(text_lines[0], text_lines[0], s.target_index + 1, num_selected_targets);
                }

                s.select_ok = C_real_select_target(player,
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
                                                   text_lines[0],
                                                   1,
                                                   &s.chosen_target);

                if (unk_008a9000 != 1 && s.chosen_target.card == -1 &&
                    s.player_target_selected[s.chosen_target.player] != 0)
                {
                  FUN_004a61d6(text_lines[1]);
                  Sleep(0x9c4);
                  FUN_004a61d6("");
                }
              } while (s.select_ok != 0 && s.chosen_target.card == -1 &&
                       s.player_target_selected[s.chosen_target.player] != 0);

              if (s.select_ok == 0)
              {
                spell_fizzled = 1;
              }
              else
              {
                if (s.chosen_target.card == -1)
                {
                  s.player_target_selected[s.chosen_target.player] = 1;
                }
                else
                {
                  PLAYER_CARD_INSTANCE(s.chosen_target.player, s.chosen_target.card).state |=
                      STATE_CANNOT_TARGET | STATE_TARGETTED;
                  TENTATIVE_reassess_all_cards();
                }

                PLAYER_CARD_INSTANCE(player, card)
                    .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                    .player = s.chosen_target.player;
                PLAYER_CARD_INSTANCE(player, card)
                    .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                    .card = s.chosen_target.card;
                ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
              }

              ++s.target_index;
            }

            for (s.target_index = 0;
                 s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
                 ++s.target_index)
            {
              if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
              {
                PLAYER_CARD_INSTANCE(
                    PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                    PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                    .state &= ~0x300000;
              }
            }

            if (spell_fizzled == 1)
            {
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
            }
          }
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).info_slot;
        num_selected_targets = PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).number_of_targets;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        s.player_target_selected[1] = 0;
        s.player_target_selected[0] = 0;
        s.target_index = 0;
        while (s.target_index < num_selected_targets && spell_fizzled != 1)
        {
          do
          {
            if (unk_008a9000 != 1)
            {
              load_text((int)"prompts.txt", "FIREBALL");
              sprintf(text_lines[0], text_lines[0], s.target_index + 1, num_selected_targets);
            }

            s.select_ok = C_real_select_target(player,
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
                                               text_lines[0],
                                               1,
                                               &s.chosen_target);

            if (unk_008a9000 != 1 && s.chosen_target.card == -1 &&
                s.player_target_selected[s.chosen_target.player] != 0)
            {
              FUN_004a61d6(text_lines[1]);
              Sleep(0x9c4);
              FUN_004a61d6("");
            }
          } while (s.select_ok != 0 && s.chosen_target.card == -1 &&
                   s.player_target_selected[s.chosen_target.player] != 0);

          if (s.select_ok == 0)
          {
            spell_fizzled = 1;
          }
          else
          {
            if (s.chosen_target.card == -1)
            {
              s.player_target_selected[s.chosen_target.player] = 1;
            }
            else
            {
              PLAYER_CARD_INSTANCE(s.chosen_target.player, s.chosen_target.card).state |=
                  STATE_CANNOT_TARGET | STATE_TARGETTED;
              TENTATIVE_reassess_all_cards();
            }

            PLAYER_CARD_INSTANCE(player, card)
                .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .player = s.chosen_target.player;
            PLAYER_CARD_INSTANCE(player, card)
                .targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets]
                .card = s.chosen_target.card;
            ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
          }

          ++s.target_index;
        }

        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          if (PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card != -1)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card)
                .state &= ~0x300000;
          }
        }

        if (spell_fizzled == 1)
        {
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      invalid_targets = 0;
      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        result = PLAYER_CARD_INSTANCE(player, card).info_slot;
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player =
              PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card =
              PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card;
          FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, result);
        }
      }
      else
      {
        illegal_abilities = get_protections_from(player, card);
        for (s.target_index = 0;
             s.target_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets;
             ++s.target_index)
        {
          if (C_real_validate_target(
                  PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card,
                  (char *)0,
                  player,
                  2,
                  2,
                  TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                  TYPE_CREATURE,
                  TYPE_NONE,
                  0,
                  illegal_abilities,
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
            ++invalid_targets;
          }
          else
          {
            damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].player,
                            PLAYER_CARD_INSTANCE(player, card).targets[s.target_index].card,
                            PLAYER_CARD_INSTANCE(player, card).info_slot,
                            player,
                            card);
          }
        }
      }

      if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == invalid_targets)
      {
        spell_fizzled = 1;
      }

      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_BURY);
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x00408c24
// FUNCTION: SHANDALAR 0x0044e945
int card_flashfires(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = get_hacked_color(player, card, 5);
    dispatch_function_to_all_cards_in_play(player, card, helper_destroy_basiclandtype, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004037e1
// FUNCTION: SHANDALAR 0x00449507
int card_hurricane(int player, int card, event_t event)
{
  int current_card;
  int current_player;
  int damage_amount;

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x24 / FUN_00404c4c(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    damage_amount = PLAYER_CARD_INSTANCE(player, card).info_slot;
    current_player = player;
    while (current_player >= 0 && current_player < 2)
    {
      damage_player(current_player, damage_amount, player, card);
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (C_get_abilities(current_player, current_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) != 0)
        {
          damage_creature(current_player, current_card, damage_amount, player, card);
        }
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
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402c15
// FUNCTION: SHANDALAR 0x00448939
int card_tranquility(int player, int card, event_t event)
{
  int current_card;
  int current_player;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0)
        {
          kill_card(current_player, current_card, KILL_DESTROY);
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040293f
// FUNCTION: SHANDALAR 0x00448662
int card_stream_of_life(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_CAST)
  {
    if (((active_player == player) && ((unk_00926804 & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= (life[player] * 0x18) / FUN_00404c4c(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "STREAMOFLIFE");
    }
    if (C_real_select_target(player,
                             2,
                             player,
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
                             &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = x_value;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).info_slot, player, card);
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040727c
// FUNCTION: SHANDALAR 0x0044cf98
int card_regrowth(int player, int card, event_t event)
{
  struct
  {
    int hand_card;
    int graveyard_index;
    int result;
  } locals;

  if (event == EVENT_CAN_CAST)
  {
    locals.result = 0;
    for (locals.hand_card = 0;
         locals.hand_card < 500 && locals.result == 0 && global_graveyard_slots[player][locals.hand_card] != -1;
         ++locals.hand_card)
    {
      locals.result = 1;
    }
  }
  else
  {
    if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
    {
      if (((player == active_player) && (unk_00926804 & 2) == 0) || unk_008a9000 == 1)
      {
        locals.graveyard_index = FUN_004087cc(player, -1);
      }
      else
      {
        if (player == unk_008b35ec)
        {
          load_text("prompts.txt", "REGROWTH");
        }
        locals.graveyard_index =
            show_deck(player, global_graveyard_slots[player], 500, text_lines, 0, gs_cancel_008a8c20);
      }

      if (locals.graveyard_index == -1 || global_graveyard_slots[player][locals.graveyard_index] == -1)
      {
        spell_fizzled = 1;
      }
      else
      {
        locals.hand_card = add_card_to_hand(player, global_graveyard_slots[player][locals.graveyard_index]);
        PLAYER_CARD_INSTANCE(player, locals.hand_card).state |= 0x20;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = locals.hand_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 108)) = locals.graveyard_index;
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      locals.graveyard_index = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 108));
      if (locals.graveyard_index == -1 || global_graveyard_slots[player][locals.graveyard_index] == -1)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .targets[6]
            .card = -1;
      }
      else
      {
        FUN_004b15f7(player, locals.graveyard_index);
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state &= 0xffffffdf;
        ++hand_count[player];
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_BURY);
    }

    locals.result = 0;
  }
  return locals.result;
}

// FUNCTION: MAGIC 0x00403b62
// FUNCTION: SHANDALAR 0x00449888
int card_tsunami(int player, int card, event_t event)
{
  int current_player;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        if (is_in_play(current_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
        {
          if (FUN_0048463d(current_player, current_card, get_hacked_color(player, card, 2)) != 0)
          {
            kill_card(current_player, current_card, KILL_DESTROY);
          }
        }
      }
    }
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00465b27
// FUNCTION: SHANDALAR 0x00455247
int card_basalt_monolith(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_UNTAP) && (card == affected_card)) && (player == affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) != 0) && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      result = 0;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) && has_mana(player, 7, 3) == 0)
    {
      result = 0;
    }
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      result = 0;
    }
    return result;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      if (has_mana(player, 7, 3))
      {
        charge_mana(player, 0, 3);
        if (spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      ai_modifier -= 0x24;
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      produced_mana_color = 0;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if ((event == EVENT_RESOLVE_ACTIVATION) && (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).state & STATE_TAPPED) && PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
  {
    unk_007abc78 |= 2;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 1;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .eot_toughness = 0;
  }

  if ((((((trigger_condition == 0xcb) || (trigger_condition == 0xe2)) || (trigger_condition == 0xe3)) || (trigger_condition == 0xe4)) && ((card == affected_card) && (player == affected_card_controller))) && ((current_turn == player) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) && PLAYER_CARD_INSTANCE(player, card).info_slot != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
    }
  }

  if (((((event == EVENT_COUNT_MANA) && (card == affected_card)) && (player == affected_card_controller)) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
  {
    declare_mana_available(player, COLOR_COLORLESS, 3);
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
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "CONSERVATOR");
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

// FUNCTION: MAGIC 0x00467167
// FUNCTION: SHANDALAR 0x00456897
int card_gauntlet_of_might(int player, int card, event_t event)
{
  int chosen_color;
  unsigned int affected_color;

  if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) && is_in_play(player, card) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    chosen_color = get_sleighted_color(player, card, 4);
    affected_color = (unsigned char)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color;
    if (((1 << ((unsigned char)chosen_color & 0x1f)) & affected_color) != 0)
    {
      ++event_result;
    }
  }

  if (event == EVENT_TAP_CARD)
  {
    chosen_color = get_hacked_color(player, card, 4);
    if (FUN_0048463d(affected_card_controller, affected_card, chosen_color) != 0 && is_in_play(player, card) && ((((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)) && produced_mana_color != -1))
    {
      undeclare_mana_available_and_produce_it(affected_card_controller, COLOR_RED, 1);
    }
  }

  if (event == EVENT_COUNT_MANA)
  {
    chosen_color = get_hacked_color(player, card, 4);
    if (FUN_0048463d(affected_card_controller, affected_card, chosen_color) != 0 && is_in_play(player, card) && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
    {
      declare_mana_available(affected_card_controller, COLOR_RED, 1);
    }
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
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "JADE_MONOLITH");
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

// FUNCTION: MAGIC 0x00417d1f
// FUNCTION: SHANDALAR 0x0051a531
int card_jayemdae_tome(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player && active_player == player && (unk_00926804 & 2) == 0)
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

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0xc;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 1;
    if (((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 || ((has_mana(player, COLOR_ANY, 4) != 0 && current_phase == 4) && player == human_player))
      {
        if (player == active_player && (unk_00926804 & 2) == 0 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
        {
          can_activate = 0;
        }
      }
      else
      {
        can_activate = 0;
      }
    }
    else
    {
      can_activate = 0;
    }

    return can_activate;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      can_activate = has_mana(player, COLOR_ANY, 4);
      if (can_activate != 0)
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
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "ROD_OF_RUIN");
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

// FUNCTION: MAGIC 0x004186d8
// FUNCTION: SHANDALAR 0x0051aee6
int card_sol_ring(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0xc0 / (basiclandtypes_controlled[active_player][7] + 1);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (!is_animated_and_sick(player, card) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      return 1;
    }
    return 0;
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

// FUNCTION: MAGIC 0x00417228
// FUNCTION: SHANDALAR 0x00519a3d
int card_soul_net(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0x90;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_CREATURE) && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).token_status & 0x10) == 0 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 4 && (instance->state & 0x20) == 0 && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & 0x20) == 0 && (((instance->state & STATE_TAPPED) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if ((instance->targets[0].player & 1) == 0)
    {
      ++instance->info_slot;
    }
    else
    {
      instance->info_slot = 1;
    }
  }

  if (trigger_condition == 0xd5 && affected_card == card && affected_card_controller == player && instance->info_slot != 0 && player == current_turn && ((((unsigned int)instance->info_slot |= 0x100), ((instance->state & STATE_TAPPED) == 0)) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)))
  {
    if (event == EVENT_TRIGGER)
    {
      if (!has_mana_w_global_cost_mod(player, card, COLOR_COLORLESS, 1))
      {
        instance->info_slot = 0;
      }
      else if (active_player == player && (unk_00926804 & 2) == 0)
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
        --instance->info_slot;
      }
    }
    if (instance->info_slot != 0)
    {
      instance->state &= ~0x100;
    }
  }

  return 0;
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
      if (active_player == human_player && (unk_00926804 & 2) == 0)
      {
        selected_target.player = human_player;
        selected_target.card = FUN_00534ddb(human_player, 1);
        load_text((int)"prompts.txt", "WINTERORB");
        do_dialog(player, player, card, selected_target.player, selected_target.card, text_lines[1], 0);
      }
      else
      {
        load_text((int)"prompts.txt", "WINTERORB");
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
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "HELM_OF_CHATZUK");
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

// FUNCTION: MAGIC 0x00415072
// FUNCTION: SHANDALAR 0x00517888
int card_howling_mine(int player, int card, event_t event)
{
  if (event == EVENT_DRAW_PHASE && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE)))
  {
    ++event_result;
  }

  return 0;
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

// FUNCTION: MAGIC 0x0041c706
// FUNCTION: SHANDALAR 0x0051ef24
int card_clockwork_beast(int player, int card, event_t event)
{
  return FUN_0041c752(player, card, event, 7);
}

// FUNCTION: MAGIC 0x00468878
// FUNCTION: SHANDALAR 0x00457f9e
int card_living_wall(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return FUN_0054276d(player, card, event, 0, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6f2a
// FUNCTION: SHANDALAR 0x004b0fe3
int card_disenchant(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x10;
    load_text((int)"prompts.txt", "DISENCHANT");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT | TYPE_ENCHANTMENT,
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
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004abcd8
// FUNCTION: SHANDALAR 0x004a354a
int card_guardian_angel(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if ((unk_008b4278 & 4) == 0 || !real_target_available((int *)0,
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
                                                          unk_009266a4,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"promptsX1.txt", "GUARDIAN_ANGLE");
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
                              unk_009266a4,
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
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
      instance->info_slot = x_value;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
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
                                unk_009266a4,
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
      damage = &PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card);
      if (instance->info_slot < damage->info_slot)
      {
        damage->info_slot -= instance->info_slot;
      }
      else
      {
        damage->info_slot = 0;
      }
      create_legacy_effect(player,
                           card,
                           unk_008cf1bc,
                           (int)damage->damage_source_player,
                           damage->damage_source_card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8b77
// FUNCTION: SHANDALAR 0x004b2c3e
int card_death_ward(int player, int card, event_t event)
{
  card_instance_t *instance;
  int current_player;
  int current_card;
  int internal_card_id;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if ((event == EVENT_CAN_CAST) && (unk_008b4278 & 2) != 0)
  {
    FUN_004e4ff3(0);
    for (current_player = 0; current_player < 2; ++current_player)
    {
      for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
      {
        internal_card_id = PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id;
        if (is_in_play(current_player, current_card) && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(current_player, current_card).mana_color == 2)
        {
          return 99;
        }
      }
    }
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && ((player == affected_card_controller) && (unk_008b4278 & 2) != 0))
  {
    int found_dead_creature;

    found_dead_creature = 0;
    do
    {
      instance->number_of_targets = 0;
      load_text((int)"prompts.txt", "DEATH_WARD");
      if (!C_real_select_target(player,
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
                                0,
                                0,
                                text_lines[0],
                                1,
                                &instance->targets[instance->number_of_targets]))
      {
        spell_fizzled = 1;
      }
      else
      {
        ++instance->number_of_targets;
      }
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if (PLAYER_CARD_INSTANCE(target_player, target_card).mana_color == 2)
      {
        if (((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & 2) != 0) || target_player == unk_008b35ec)
        {
          ai_modifier -= 0x30;
        }
        found_dead_creature = 1;
      }
      else if (unk_008a9000 == 1)
      {
        FUN_004e51bb();
      }
      else
      {
        load_text((int)"prompts.txt", "DEATH_WARD2");
        FUN_004a61d6(text_lines[1]);
        Sleep(0x9c4);
        FUN_004a61d6("");
      }
    } while ((spell_fizzled != 1) && !found_dead_creature);
  }

  if ((event == EVENT_RESOLVE_SPELL) && (unk_008b4278 & 2) != 0)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
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
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      FUN_00542a2a(target_player, target_card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8305
// FUNCTION: SHANDALAR 0x004b23c6
int card_righteousness(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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
                                 TARGET_STATE_BLOCKING,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "RIGHTEOUSNESS");
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
                              TARGET_STATE_BLOCKING,
                              0,
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
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
                                TARGET_STATE_BLOCKING,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      effect_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, selected_target.player, selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = 7;
        PLAYER_CARD_INSTANCE(player, effect_card).power = 7;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f895d
// FUNCTION: SHANDALAR 0x004b2a22
int card_swords_to_plowshares(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "SWORD_TO_PLOWSHARES");
    if (!FUN_00551638(player, 1 - player, card))
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
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
      gain_life(target_player, C_get_abilities(target_player, target_card, EVENT_POWER, -1), player, card);
      kill_card(target_player, target_card, KILL_REMOVE);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fdfb2
// FUNCTION: SHANDALAR 0x004b80dc
int card_healing_salve(int player, int card, event_t event)
{
  return gain_life_or_prevent_damage(player, card, event, 3);
}

// FUNCTION: MAGIC 0x004fdfd8
int gain_life_or_prevent_damage(int player, int card, event_t event, int amount)
{
  card_instance_t *instance;
  target_t selected_target;
  int done;
  int cancelled;
  int target_index;
  int source_player;
  int source_card;
  char prompt[300];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && (amount == 0))
    {
      return 0;
    }
    if ((unk_008b4278 & 4) == 0)
    {
      return 1;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x60;
    if ((unk_008b4278 & 4) == 0)
    {
      load_text((int)"prompts.txt", "HEALING_SALVE");
      if (!C_real_select_target(player,
                                2,
                                player,
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
                                &selected_target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = selected_target;
        instance->number_of_targets = 1;
        instance->info_slot = amount;
      }
    }
    else
    {
      instance->number_of_targets = 0;
      source_player = -1;
      source_card = 0;
      cancelled = 0;
      done = 0;
      spell_fizzled = -1;
      while ((instance->number_of_targets < amount) && !cancelled && (spell_fizzled != 1) && !done)
      {
        load_text((int)"prompts.txt", "HEALING_SALVE2");
        strcpy(prompt, text_lines[1]);
        if (!C_real_select_target(player,
                                  2,
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
                                  0,
                                  prompt,
                                  3,
                                  &selected_target))
        {
          if (selected_target.card == -1)
          {
            spell_fizzled = 1;
          }
          else
          {
            cancelled = 1;
          }
        }
        else if (((PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player == source_player) && (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card == source_card)) || source_player == -1)
        {
          source_player = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_player;
          source_card = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).damage_source_card;
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state |= 0x200000;
          TENTATIVE_reassess_all_cards();
          instance->targets[instance->number_of_targets] = selected_target;
          ++instance->number_of_targets;
          if (instance->number_of_targets == 19)
          {
            done = 1;
          }
          if (unk_00715fb0 == 1)
          {
            while ((instance->number_of_targets < amount) && !done)
            {
              instance->targets[instance->number_of_targets] = selected_target;
              ++instance->number_of_targets;
              if (instance->number_of_targets == 19)
              {
                done = 1;
              }
            }
          }
        }
        else if (unk_008a9000 == 1)
        {
          FUN_004e51bb();
        }
        else
        {
          load_text((int)"prompts.txt", "HEALING_SALVE3");
          FUN_004a61d6(text_lines[2]);
          Sleep(0x9c4);
          FUN_004a61d6("");
        }
      }
      for (target_index = 0; target_index < instance->number_of_targets; ++target_index)
      {
        PLAYER_CARD_INSTANCE(instance->targets[target_index].player, instance->targets[target_index].card).state &= ~0x300000;
      }
      if (spell_fizzled == 1)
      {
        instance->number_of_targets = 0;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    while (instance->number_of_targets > 0)
    {
      --instance->number_of_targets;
      selected_target = instance->targets[instance->number_of_targets];
      if ((unk_008b4278 & 4) == 0)
      {
        gain_life(selected_target.player, instance->info_slot, player, card);
      }
      else if (!C_real_validate_target(selected_target.player,
                                       selected_target.card,
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
                                       unk_009266a4,
                                       ~SUB_WALL,
                                       -1,
                                       -1,
                                       0,
                                       0,
                                       0))
      {
        spell_fizzled = 1;
      }
      else if (PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).info_slot;
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac52b
// FUNCTION: SHANDALAR 0x004a3da3
int card_psionic_blast(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"promptsX1.txt", "PSIONIC_BLAST");
    FUN_0054ac4d(player, card, 4);
    if (player == active_player)
    {
      ai_modifier += (3 - PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).toughness) * 0xc;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, 4) != 0)
    {
      damage_player(player, 2, player, card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f94f3
// FUNCTION: SHANDALAR 0x004b35bf
int card_jump(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "JUMP");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
    else
    {
      if (instance->targets[0].player == unk_008b35ec)
      {
        ai_modifier -= 0x18;
      }
      if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).regen_status & 0x20) != 0)
      {
        ai_modifier += -99;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
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
      effect_card = create_legacy_effect(player, card, unk_00896534, target_player, target_card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).token_status = 0;
        PLAYER_CARD_INSTANCE(player, effect_card).regen_status = 0x20;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa4eb
// FUNCTION: SHANDALAR 0x004b45ba
int card_unsummon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier -= 0x30;
    load_text((int)"prompts.txt", "UNSUMMON");
    if (!FUN_00551638(player, 1 - player, card))
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    if (!C_real_validate_target(target_player,
                                target_card,
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
      kill_card(target_player, target_card, 5);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f64d5
// FUNCTION: SHANDALAR 0x004b0590
int card_ancestral_recall(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text((int)"prompts.txt", "ANCESTRAL_RECALL");
    if (!C_real_select_target(player,
                              2,
                              player,
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
                              &selected_target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    FUN_0043e18b(target_player);
    FUN_0043e18b(target_player);
    FUN_0043e18b(target_player);
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6108
// FUNCTION: SHANDALAR 0x004b01c2
int card_mana_short(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int color;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    target_t selected_target;

    load_text((int)"prompts.txt", "MANASHORT");
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
                              &selected_target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    FUN_0055117d(FUN_004f6311, target_player);
    for (color = 0; color < 8; ++color)
    {
      raw_mana_available[target_player][color] = 0;
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6311
int FUN_004f6311(int player, int card, int internal_card_id)
{
  unk_00938e2c = 1;
  produced_mana_color = -1;
  if ((PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x10) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }
  unk_00938e2c = 0;
  return 0;
}

// FUNCTION: MAGIC 0x004f7c84
// FUNCTION: SHANDALAR 0x004b1d3f
int card_howl_from_beyond(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (((player == active_player) && ((unk_00926804 & 2) == 0)) && has_mana(player, 7, 2) == 0)
    {
      return 0;
    }
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 (unsigned char)player,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "HOWL_FROM_BEYOND");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier += ((((current_phase < 0x15) - 1) & -2) * 3 + 9) * -4;
      instance->info_slot = x_value;
      if ((player == active_player) && (PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x30000) != 0)
      {
        ai_modifier += -99;
      }
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
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = (short)instance->info_slot;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9b91
// FUNCTION: SHANDALAR 0x004b3c5f
int card_terror(int player, int card, event_t event)
{
  card_instance_t *instance;
  int illegal_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_ARTIFACT,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 illegal_color,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "TERROR");
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_ARTIFACT,
                              0,
                              get_protections_from(player, card),
                              COLOR_TEST_0,
                              illegal_color,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    illegal_color = 1 << ((unsigned char)get_sleighted_color(player, card, 1) & 0x1f);
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_ARTIFACT,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                illegal_color,
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
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f6d49
// FUNCTION: SHANDALAR 0x004b0e02
int card_shatter(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text((int)"prompts.txt", "SHATTER");
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT,
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
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier -= 0x10;
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
                                TYPE_ARTIFACT,
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
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f7857
// FUNCTION: SHANDALAR 0x004b1910
int card_tunnel(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "TUNNEL");
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
                              TARGET_SPECIAL_WALL,
                              0,
                              0,
                              text_lines[0],
                              1,
                              &instance->targets[0]))
    {
      spell_fizzled = 1;
    }
    else
    {
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
                                TARGET_SPECIAL_WALL,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f9e9e
// FUNCTION: SHANDALAR 0x004b3f6b
int card_lightning_bolt(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(1);
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "LIGHTNING_BOLT");
    FUN_0054ac4d(player, card, 3);
    if (spell_fizzled != 1)
    {
      ai_modifier -= 0x24;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f801d
// FUNCTION: SHANDALAR 0x004b20dc
int card_berserk(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (current_phase < 0x1e && real_target_available((int *)0,
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
                                                      0) != 0)
    {
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"prompts.txt", "BERSERK");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
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
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, unk_008b49c4, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 0x80;
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power =
            PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).power;
        PLAYER_CARD_INSTANCE(player, effect_card).state |= 0x4000;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa213
// FUNCTION: SHANDALAR 0x004b42e2
int card_giant_growth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int effect_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier += ((((current_phase < 0x15) - 1) & -2) * 3 + 0xc) * -4;
    if (current_phase < 0x15)
    {
      ai_modifier -= 0xc;
    }
    load_text((int)"prompts.txt", "GIANT_GROWTH");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
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
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      effect_card =
          create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, instance->targets[0].player, instance->targets[0].card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).counter_power = 3;
        PLAYER_CARD_INSTANCE(player, effect_card).power = 3;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  if ((event == 0x3b) && has_mana(player, 3, 1) != 0)
  {
    unk_007a7d80[player] += 3;
    unk_007a7d88[player] += 3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f66bc
// FUNCTION: SHANDALAR 0x004b0777
int card_simulacrum(int player, int card, event_t event)
{
  card_instance_t *damage;
  card_instance_t *instance;
  int current_card;
  int current_player;
  int target_card;
  int target_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (!real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                               get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                               0xffffffff, 0, 0, 0))
    {
      return 0;
    }
    if ((unk_008b4278 & 4) == 0)
    {
      return 1;
    }
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                 unk_009266a4, -1, 0xffffffff, 0xffffffff, 0x20, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"prompts.txt", "SIMULACRUM");
    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;

    if (!C_real_validate_target(target_player, target_card, (char *)0, player, player, player,
                                TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                                ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      spell_fizzled = 1;
    }
    else
    {
      if ((unk_008b4278 & 4) != 0)
      {
        instance->eot_toughness = 0;
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
          {
            damage = &PLAYER_CARD_INSTANCE(current_player, current_card);
            if (damage->internal_card_id == unk_009266a4 && damage->damage_target_player == player && damage->damage_target_card == -1)
            {
              damage_creature(target_player, target_card, damage->info_slot,
                              (int)damage->damage_source_player, damage->damage_source_card);
              instance->eot_toughness += damage->info_slot;
              damage->info_slot = 0;
            }
          }
        }
      }

      instance->info_slot = 0;
      for (current_player = 0; current_player < 2; ++current_player)
      {
        for (current_card = 0; current_card < 151; ++current_card)
        {
          if (unk_0093b280[current_player][current_card][player][0] > 0)
          {
            damage_creature(target_player, target_card,
                            unk_0093b280[current_player][current_card][player][0],
                            current_player, current_card);
            instance->info_slot += unk_0093b280[current_player][current_card][player][0];
            unk_0093b280[current_player][current_card][player][0] = 0;
          }
        }
      }

      life[player] += instance->info_slot;
      instance->info_slot = 0;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fa7dc
// FUNCTION: SHANDALAR 0x004b48ac
int card_purelace(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *target_instance;
  target_t target;
  int color;
  int valid;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0xff, 0, 0,
                                   get_protections_from(player, card), 0, 0, -1, -1, 0xffffffff,
                                   0xffffffff, 0, 0, 0);
    }

    if ((((player == unk_008b35ec) || ((unk_00926804 & 2) != 0)) || ((active_player == player) && (unk_008b35ec == human_player))) && C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                                                                                                                                             TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                                                                                                                                                 TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                                                                                                                                             TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                                                                                                                                             ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 99;
    }
    return 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    ai_modifier -= 0x18;
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "ANY_LACE");
      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY,
                                TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY |
                                    TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                                text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
        if ((active_player == player) && (((global_cards_data[instance->internal_card_id].color & PLAYER_CARD_INSTANCE(target.player, target.card).color) != 0) || (target.player == unk_008b35ec)))
        {
          ai_modifier -= 0x30;
        }
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (unk_008ce508 == -1)
    {
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0,
                                     COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0);
    }
    else
    {
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0,
                                     TYPE_EFFECT | TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT |
                                         TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1,
                                     ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (!valid)
    {
      spell_fizzled = 1;
    }
    else
    {
      color = single_color_test_bit_to_color_t(global_cards_data[instance->internal_card_id].color);
      color = get_sleighted_color(player, card, color);
      target_instance = &PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card);
      target_instance->color = (char)(1 << ((unsigned char)color & 0x1f));
      target_instance->state |= 0x2000;
      if (unk_008a9000 != 1)
      {
        play_sound_effect(WAV_CHANGEC);
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043c1a5
// FUNCTION: SHANDALAR 0x004feb50
int card_raging_river(int player, int card, event_t event)
{
  int current_card;
  int defender;
  int done;
  int legacy_card;
  int selected_cards[151];
  card_instance_t *instance;
  char message[600];
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    return player == unk_008b35ec || (unk_00926804 & 2) != 0;
  }

  if ((event == EVENT_RESOLVE_SPELL) && dispatch_function_to_all_cards_in_play(player, card, FUN_00483190, player) == -1)
  {
    *(int *)((char *)instance + 0x14) |= 0x1000000;
  }
  if ((event == 0x92) && (player == human_player) && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    defender = 1 - human_player;
    if ((unk_008b35ec == defender) || ((unk_00926804 & 2) != 0))
    {
      for (current_card = 0; current_card < active_cards_count[defender]; ++current_card)
      {
        selected_cards[current_card] = 0;
      }

      done = 0;
      while (done == 0)
      {
        load_text((int)"promptsX1.txt", "RAGING_RIVER");
        if (!C_real_select_target(defender,
                                  defender,
                                  defender,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_CREATURE,
                                  TYPE_NONE,
                                  0,
                                  KEYWORD_FLYING,
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
                                  2,
                                  &target))
        {
          done = 1;
        }
        else
        {
          legacy_card = create_legacy_effect(player, card, unk_008b3bd4, target.player, target.card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 2;
          }
          selected_cards[target.card] = 1;
        }
      }

      for (current_card = 0; current_card < active_cards_count[defender]; ++current_card)
      {
        if (is_in_play(defender, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(defender, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && selected_cards[current_card] == 0 && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(defender, current_card) + 0x24) & 0x20) == 0))
        {
          legacy_card = create_legacy_effect(player, card, unk_008b3bd4, defender, current_card);
          if (legacy_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 1;
          }
        }
      }
    }
    else
    {
      FUN_0043c7ab(defender, player, card);
    }
  }

  if (trigger_condition == 0xde && card == card_on_stack && player == card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0) && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause) + 0x24) & 0x20) == 0) && current_turn == human_player && player == human_player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, trigger_cause_controller, trigger_cause);
      load_text((int)"promptsX1.txt", "RAGING_RIVER2");
      sprintf(message, " %s\n %s", "Attack from left bank", "Attack from right bank");
      if (do_dialog(player, player, card, -1, -1, message, 0) == 0)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 2;
      }
    }
  }

  if (event == 0x77 && card == card_on_stack && player == card_on_stack_controller && ((*(unsigned char *)((char *)instance + 0x17) & 1) != 0))
  {
    legacy_card = dispatch_function_to_all_cards_in_play(player, card, FUN_00483242, player);
    if (legacy_card != -1)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, legacy_card) + 0x14) |= 0x1000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fc353
// FUNCTION: SHANDALAR 0x004b647a
int card_blue_elemental_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int color;
  int valid;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      color = get_sleighted_color(player, card, 4);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 4);
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "BLUE_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 4);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (unk_008ce508 == -1)
    {
      color = get_sleighted_color(player, card, 4);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      color = get_sleighted_color(player, card, 4);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).mana_color & (1 << ((unsigned char)color & 0x1f))) != 0)
      {
        kill_card(target_player, target_card, KILL_DESTROY);
      }
    }
    else
    {
      spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fc90c
// FUNCTION: SHANDALAR 0x004b6a32
int card_counterspell(int player, int card, event_t event)
{
  card_instance_t *instance;
  int valid;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                  -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
    ai_modifier -= 0x24;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                   (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                   COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                   TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    if (valid)
    {
      if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x20) != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
      }
    }
    else
    {
      spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fd465
// FUNCTION: SHANDALAR 0x004b7590
int card_spell_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  int cost;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }

    if ((active_player == player) && ((unk_00926804 & 2) == 0) && !has_mana_w_global_cost_mod(player, card, 7, 2))
    {
      return 0;
    }

    cost = (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].cc[1];
    if (cost == -1)
    {
      cost = x_value;
    }
    cost += (int)(char)global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].cc[0];
    instance->info_slot = cost;

    if (!has_mana(player, 2, 1) || !has_mana_w_global_cost_mod(player, card, 7, cost + 1) || !C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      return 0;
    }

    unk_0091bfb4 = 1;
    return 99;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller) && (unk_008ce508 != -1))
  {
    unk_008ce518 = 1;
    charge_mana_w_global_cost_mod(player, card, 0, instance->info_slot);
    if (spell_fizzled != 1)
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
      x_value = instance->info_slot;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0,
                                COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
    {
      spell_fizzled = 1;
    }
    else if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state & 0x20) != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ad5b4
// FUNCTION: SHANDALAR 0x004a4e37
int card_sacrifice(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int iid;
  int amount;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                                 0, 0, 0, -1, -1, 0xffffffff, 0xffffffff, 0, 0, 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"promptsX1.txt", "SACRIFICE");
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE,
                              TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL,
                              -1, -1, 0, 0, 0, text_lines[0], 0, &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
      kill_card(player, target.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    iid = PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id;
    amount = (int)(char)global_cards_data[iid].cc[0] + (int)(char)global_cards_data[iid].cc[1];
    if (global_cards_data[iid].cc[1] == 0xff)
    {
      ++amount;
    }
    produce_mana(player, COLOR_BLACK, amount);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fdeec
// FUNCTION: SHANDALAR 0x004b8014
int card_dark_ritual(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (hand_count[player] < 8)
    {
      ai_modifier -= 0x3c;
    }
    else
    {
      ai_modifier -= 0x18;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    produce_mana(player, COLOR_BLACK, 3);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fd933
// FUNCTION: SHANDALAR 0x004b7a5b
int card_red_elemental_blast(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int color;
  int valid;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (unk_008ce508 == -1)
    {
      FUN_004e4ff3(0);
      color = get_sleighted_color(player, card, 2);
      return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, 0x1047, 0, 0,
                                   get_protections_from(player, card), 1 << ((unsigned char)color & 0x1f), 0, -1, -1,
                                   0xffffffff, 0xffffffff, 0, 0, 0);
    }
    color = get_sleighted_color(player, card, 2);
    return C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                  TYPE_NONE, TYPE_NONE, 0, 0, 1 << ((unsigned char)color & 0x1f),
                                  COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                  TARGET_SPECIAL_SPELL_ON_STACK, 0, 0)
               ? 99
               : 0;
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "RED_ELEMENTAL_BLAST");
      color = get_sleighted_color(player, card, 2);
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY,
                                TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE, 0, get_protections_from(player, card),
                                1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    valid = 0;
    if (unk_008ce508 == -1)
    {
      color = get_sleighted_color(player, card, 2);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE,
                                     TYPE_NONE, 0, get_protections_from(player, card),
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, 0, 0, 0);
    }
    else
    {
      color = get_sleighted_color(player, card, 2);
      valid = C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                                     (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0,
                                     1 << ((unsigned char)color & 0x1f), COLOR_TEST_0, -1, ~SUB_WALL,
                                     -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0);
    }

    if (valid)
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;
      if ((PLAYER_CARD_INSTANCE(target_player, target_card).mana_color & (1 << ((unsigned char)color & 0x1f))) != 0)
      {
        kill_card(target_player, target_card, KILL_DESTROY);
      }
    }
    else
    {
      spell_fizzled = 1;
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
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
  card_instance_t *instance;
  char message[300];
  color_t color;
  color_t choice_hint;
  unsigned int available_colors;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((instance->token_status & 3) == 0) || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)) && ((instance->state & 0x10) == 0))
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    ai_modifier -= 0x24;
    available_colors = (unsigned int)(unsigned char)instance->damage_source_card;
    if ((((active_player == player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1)) || (unk_009252e0 != 0))
    {
      choice_hint = ~COLOR_COLORLESS;
      for (color = COLOR_BLACK; color < 6 && choice_hint == ~COLOR_COLORLESS; color += COLOR_BLACK)
      {
        if (((unk_00938e2c & (1 << ((unsigned char)color & 0x1f))) != 0) && ((available_colors & (1 << ((unsigned char)color & 0x1f))) != 0))
        {
          choice_hint = color;
        }
      }
      if (choice_hint == ~COLOR_COLORLESS && (unk_00938e2c & 1) != 0)
      {
        choice_hint = COLOR_BLACK;
      }
      if (choice_hint == ~COLOR_COLORLESS && (unk_00938e2c & 0x40) != 0)
      {
        choice_hint = COLOR_BLACK;
      }
      if (choice_hint == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
    }
    else
    {
      choice_hint = ~COLOR_COLORLESS;
    }

    if (spell_fizzled != 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "BLACK_LOTUS");
      }
      color = choose_a_color(player, text_lines[0], 1, choice_hint,
                             (int)(unsigned char)instance->damage_source_card);
      if (color == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, color, 3);
        produced_mana_color = color;
        instance->state |= 0x10;
        if (((active_player == player) && ((unk_00926804 & 2) == 0)) && (unk_008a9000 != 1))
        {
          load_text((int)"prompts.txt", "BLACK_LOTUS2");
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
        if (unk_008a9000 != 1)
        {
          play_sound_effect(WAV_SACRFICE);
        }
        kill_card(player, card, KILL_SACRIFICE);
        unk_007a7c1c = 1;
      }
    }
  }

  if ((event == EVENT_COUNT_MANA) && (card == affected_card) && (player == affected_card_controller) && ((instance->state & 0x10) == 0))
  {
    declare_mana_available_hex(player, (int)(unsigned char)instance->damage_source_card, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00411981
// FUNCTION: SHANDALAR 0x0051419a
int card_celestial_prism(int player, int card, event_t event)
{
  card_instance_t *instance;
  char message[300];
  color_t color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 2) && ((((instance->token_status & 3) == 0) || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0))) && ((instance->state & 0x10) == 0))
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
      if ((active_player == player) && ((unk_00926804 & 2) == 0))
      {
        if (unk_008a9000 == 1)
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
        if (unk_008a9000 != 1)
        {
          load_text((int)"prompts.txt", "CELESTIAL_PRISM");
        }
        color = choose_a_color(player, text_lines[0], 1, color,
                               (int)(unsigned char)instance->damage_source_card);
        if (color == ~COLOR_COLORLESS)
        {
          spell_fizzled = 1;
        }
        else if (spell_fizzled != 1)
        {
          produce_mana(player, color, 1);
          undeclare_mana_available_hex(player, (int)(unsigned char)instance->damage_source_card, 1);
          instance->state |= 0x10;
          produced_mana_color = color;
          if (((active_player == player) && ((unk_00926804 & 2) == 0)) && (unk_008a9000 != 1))
          {
            load_text((int)"prompts.txt", "CELESTIAL_PRISM2");
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

// FUNCTION: MAGIC 0x00466435
// FUNCTION: SHANDALAR 0x00455b59
int card_copper_tablet(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == affected_card) && (player == affected_card_controller))
  {
    ai_modifier += (life[player] - life[1 - player]) * 0x18;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((current_phase == 4) && ((((char)instance->regen_status & 1) == 0)) && (unk_00742f60 == human_player) && (((instance->state & 0x10) == 0) || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0)))
    {
      instance->eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if ((event == 4) && (card == affected_card) && (player == affected_card_controller))
  {
    instance->regen_status |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
  }
  if (event == 0x86)
  {
    damage_player(human_player, 1, card_on_stack_controller, card_on_stack);
  }
  if (event == 0x22)
  {
    instance->regen_status &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00414cec
// FUNCTION: SHANDALAR 0x00517500
int card_disrupting_scepter(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 3) && player == human_player && ((((instance->token_status & 3) == 0) || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0))) && ((instance->state & 0x10) == 0))
    {
      return 1;
    }
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && ((instance->state & 0x10) == 0) && has_mana(player, 7, 3) && player == human_player)
  {
    charge_mana(player, 0, 3);
    if (spell_fizzled != 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "DISRUPTING_SCEPTER");
      }
      if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE,
                                0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1,
                                0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0].player = target.player;
        instance->targets[0].card = target.card;
        instance->number_of_targets = 1;
        instance->state |= 0x10;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    discard(instance->targets[0].player, 0, 0);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004675a1
// FUNCTION: SHANDALAR 0x00456cd1
int card_icy_manipulator(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *parent;
  card_instance_t *target_instance;
  target_t target;
  keyword_t illegal_abilities;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    ai_modifier += 0xc;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, 7, 1) && (((instance->token_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && (instance->state & STATE_TAPPED) == 0)
    {
      illegal_abilities = get_protections_from(player, card);
      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                TYPE_NONE,
                                0,
                                illegal_abilities,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                -1,
                                0xffffffff,
                                0xffffffff,
                                0,
                                0,
                                0))
      {
        return 1;
      }
    }
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
  }
  else
  {
    if ((event == EVENT_ACTIVATE) && has_mana(player, 7, 1))
    {
      charge_mana(player, 0, 1);
      if (spell_fizzled != 1)
      {
        load_text((int)"promptsX1.txt", "ICY_MANIPULATOR");
        illegal_abilities = get_protections_from(player, card);
        if (!C_real_select_target(player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                  TYPE_NONE,
                                  0,
                                  illegal_abilities,
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
          instance->targets[0] = target;
          instance->number_of_targets = 1;
          if (active_player == player)
          {
            target_instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
            if ((global_cards_data[target_instance->internal_card_id].type & TYPE_LAND) != 0)
            {
              ai_modifier -= 0x18;
            }
            if (target.player == active_player)
            {
              ai_modifier -= 0x60;
            }
          }
          instance->state |= STATE_TAPPED;
        }
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      target = instance->targets[0];
      illegal_abilities = get_protections_from(player, card);
      if (!C_real_validate_target(target.player,
                                  target.card,
                                  (char *)0,
                                  player,
                                  2,
                                  2,
                                  TARGET_ZONE_IN_PLAY,
                                  TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
                                  TYPE_NONE,
                                  0,
                                  illegal_abilities,
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
        FUN_004f7783(target.player, target.card);
      }
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00436955
// FUNCTION: SHANDALAR 0x004f92f9
int card_consecrate_land(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
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
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"promptsX1.txt", "CONSECRATE_LAND");
    if (!C_real_select_target(player,
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
                              0,
                              text_lines[0],
                              1,
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      instance->number_of_targets = 1;
      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        if (target.player == unk_008b35ec)
        {
          ai_modifier -= 0x30;
        }
        else
        {
          if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_CREATURE) != 0)
          {
            ai_modifier += 0x60;
          }
        }
      }
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) |= 0x4000000;
      FUN_0055117d(FUN_00437375, -1);
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x34 && instance->damage_target_card == card_on_stack && instance->damage_target_player == card_on_stack_controller && card_on_stack != -1)
  {
    event_result |= 0x20000;
  }
  if (event == 0x77 && card_on_stack != -1 && *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x64) == 2 && instance->damage_target_card == card_on_stack && instance->damage_target_player == card_on_stack_controller)
  {
    *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x64) = 0;
    *(int *)((char *)&PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack) + 0x10) = 0;
    ++event_result;
  }
  if (event == 0x77 && card == card_on_stack && player == card_on_stack_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1)
  {
    *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) &= ~0x4000000;
  }
  if (trigger_condition == 0xd4 && card == card_on_stack && player == card_on_stack_controller && instance->damage_target_player != -1 && instance->damage_target_card != -1 && PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id != -1 && player == trigger_cause_controller && trigger_cause == card && player == current_turn)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      *(int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x14) &= ~0x4000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043aee2
// FUNCTION: SHANDALAR 0x004fd888
int card_lance(int player, int card, event_t event)
{
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"promptsX1.txt", "LANCE");
  }

  return FUN_0052d7a5(player, card, event, 0x100);
}

// FUNCTION: MAGIC 0x004feb03
// FUNCTION: SHANDALAR 0x004b8c28
int card_reverse_damage(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int damage_sources[300];
    int damage_amounts[150];
    int internal_card_ids[150];
    int selected;
    int current_card;
    int controller;
    int count;
    int best_damage;
  } s;
  int result;

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if ((unk_008b4278 & 4) == 0)
    {
      result = 1;
    }
    else
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TYPE_NONE,
                                     TYPE_NONE,
                                     0,
                                     0,
                                     0,
                                     0,
                                     unk_009266a4,
                                     -1,
                                     0xffffffff,
                                     0xffffffff,
                                     0x20,
                                     0,
                                     0);
      if (result != 0)
      {
        result = 99;
      }
    }
  }
  else
  {
    if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) && ((unk_008b4278 & 4) != 0))
    {
      load_text((int)"prompts.txt", "REVERSE_DAMAGE");
      result = C_real_select_target(player,
                                    2,
                                    2,
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
                                    &s.target);
      if (result == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      if ((unk_008b4278 & 4) == 0)
      {
        s.count = 0;
        s.best_damage = 0;
        for (s.controller = 0; s.controller < 2; s.controller = s.controller + 1)
        {
          for (s.current_card = 0; s.current_card < 150; s.current_card = s.current_card + 1)
          {
            if (0 < unk_0093b280[s.controller][s.current_card][player][0])
            {
              s.damage_sources[s.count * 2] = s.controller;
              s.damage_sources[s.count * 2 + 1] = s.current_card;
              s.damage_amounts[s.count] = unk_0093b280[s.controller][s.current_card][player][0];
              if (PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id == -1)
              {
                s.internal_card_ids[s.count] = *(int *)((char *)&PLAYER_CARD_INSTANCE(s.controller, s.current_card) + 0x38);
                s.count = s.count + 1;
              }
              else
              {
                s.internal_card_ids[s.count] = PLAYER_CARD_INSTANCE(s.controller, s.current_card).internal_card_id;
                s.count = s.count + 1;
              }
            }
          }
        }
        if (0 < s.count)
        {
          if (((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1))
          {
            s.best_damage = 0;
            for (s.current_card = 0; s.current_card < s.count; s.current_card = s.current_card + 1)
            {
              if (s.best_damage < s.damage_amounts[s.current_card])
              {
                s.best_damage = s.current_card;
              }
            }
            s.selected = s.best_damage;
          }
          else
          {
            load_text((int)"prompts.txt", "REVERSE_DAMAGE");
            s.selected = FUN_004b413c(player, s.internal_card_ids, s.damage_amounts, s.count, 0x89684c, 1, text_lines[1]);
          }
          gain_life(player,
                    unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] *
                        2,
                    player, card);
          unk_0093b280[s.damage_sources[s.selected * 2]][s.damage_sources[s.selected * 2 + 1]][player][0] = 0;
        }
        if (life[1 - player] < 1)
        {
          ai_modifier += 1000;
        }
        else
        {
          ai_modifier += (s.best_damage * 100) / life[1 - player] - 100;
        }
      }
      else
      {
        s.target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
        s.target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
        result = C_real_validate_target(s.target.player,
                                        s.target.card,
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
                                        unk_009266a4,
                                        ~SUB_WALL,
                                        -1,
                                        -1,
                                        TARGET_SPECIAL_DAMAGE_PLAYER,
                                        0,
                                        0);
        if (result == 0)
        {
          spell_fizzled = 1;
        }
        else if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
        {
          gain_life(player,
                    unk_0093b280[(char)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_player]
                                [PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_source_card][player][0] *
                            2 +
                        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot,
                    player, card);
          PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot = 0;
        }
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .number_of_targets = 0;
      }
      kill_card(player, card, KILL_BURY);
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00467a6c
// FUNCTION: SHANDALAR 0x00457199
int card_jade_statue(int player, int card, event_t event)
{
  int animated_internal_card_id;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    *(int *)((char *)instance + 0x1c) = instance->internal_card_id;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    instance->state |= 0x3000000;
    *(int *)((char *)instance + 0x34) = -1;
  }
  if (((event == 0x79) || ((event == 0x78) && (card == card_on_stack) && (player == card_on_stack_controller))) && (instance->state & 4) == 0 && !has_mana(player, 7, 2))
  {
    event_result = 1;
  }
  if ((((trigger_condition == 0xdc) && (current_phase == 0x15) && (current_turn == human_player)) || ((trigger_condition == 0xdd) && (current_phase == 0x17) && (current_turn != human_player))) && card == card_on_stack && player == card_on_stack_controller && instance->info_slot == 0 && (instance->state & STATE_TAPPED) == 0 && trigger_cause_controller == player && trigger_cause == card)
  {
    if (!has_mana(player, 7, 2))
    {
      unk_0078944c = 1;
    }
    else
    {
      if (event == 0x7d)
      {
        event_result |= 2;
      }
      if (event == 0x7e)
      {
        FUN_00443ee2(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, 0, 2);
        obliterate_top_card_of_stack();
        if (spell_fizzled == 1)
        {
          unk_0078944c = 1;
          spell_fizzled = 0;
        }
        else
        {
          animated_internal_card_id = create_a_card_type(instance->internal_card_id);
          if (animated_internal_card_id != -1)
          {
            global_cards_data[animated_internal_card_id].type |= TYPE_CREATURE;
            global_cards_data[animated_internal_card_id].power = 3;
            global_cards_data[animated_internal_card_id].toughness = 6;
            *(int *)((char *)instance + 0x1c) = animated_internal_card_id;
            *(int *)((char *)instance + 0x24) |= 0x1000000;
            *(int *)((char *)instance + 0x34) = instance->internal_card_id;
          }
          instance->info_slot = 1;
        }
      }
    }
  }
  if ((event == 0x3c) && (unk_008b4278 & 0x20000) == 0 && card == card_on_stack && player == card_on_stack_controller && is_in_play(player, card))
  {
    event_result = *(int *)((char *)instance + 0x1c);
  }
  if (trigger_condition == 0xcc && card == card_on_stack && player == card_on_stack_controller && current_turn == player && *(int *)((char *)instance + 0x34) != -1)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      FUN_005001c4(*(int *)((char *)instance + 0x1c));
      *(int *)((char *)instance + 0x1c) = *(int *)((char *)instance + 0x34);
      *(int *)((char *)instance + 0x34) = -1;
      instance->info_slot = 0;
    }
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
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, legacy_card) + 0x34) = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20f;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, legacy_card) + 0x14) = 0x10020;
      *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 8) = (unsigned char)player;
      *(int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x44) = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e3ef
// FUNCTION: SHANDALAR 0x0049c630
int card_black_knight(int player, int card, event_t event)
{
  int color;
  unsigned int saved_event_result;

  if ((event == 0x34) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    color = get_sleighted_color(player, card, 5);
    event_result |= 0x800 << ((((unsigned char)color) - 1U) & 0x1f);
    saved_event_result = event_result;
    FUN_0054e470(player, card, 5);
    event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00474f4d
// FUNCTION: SHANDALAR 0x00478a33
int card_juggernaut(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == 0x78) && attacking_card == card && player == attacking_card_controller && global_cards_data[PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack).internal_card_id].subtype == 0)
  {
    event_result = 1;
  }
  if ((event == 0x89) && ((*(unsigned char *)((char *)instance + 9) & 0x80) == 0))
  {
    instance->state |= 0x8000;
    if (FUN_0044125c(player, card))
    {
      unk_008b60e0 = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00439f56
// FUNCTION: SHANDALAR 0x004fc8fa
int card_invisibility(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0,
                                 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"promptsX1.txt", "INVISIBILITY");
    spell_fizzled = !FUN_00551638(player, player, card);
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
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x78 && instance->damage_target_card == attacking_card && instance->damage_target_player == attacking_card_controller && (instance->state & 0x20) == 0 && global_cards_data[PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack).internal_card_id].subtype != 0)
  {
    ++event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00528bc0
// FUNCTION: SHANDALAR 0x004e03af
int card_fear(int player, int card, event_t event)
{
  card_instance_t *instance;
  int color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == 0x78 && instance->damage_target_card == attacking_card && instance->damage_target_player == attacking_card_controller && (instance->state & 0x20) == 0)
  {
    color = get_sleighted_color(player, card, 1);
    if ((global_cards_data[PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack).internal_card_id].type & TYPE_ARTIFACT) == 0 && (((1 << ((unsigned char)color & 0x1f)) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack).mana_color) == 0))
    {
      ++event_result;
    }
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0,
                                 0,
                                 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "FEAR");
    }
    spell_fizzled = !FUN_00551638(player, player, card);
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
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
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
    FUN_004e4ff3(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text((int)"promptsX1.txt", "DWARVEN_DTEAM");
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
                                text_lines[0],
                                1,
                                &target))
      {
        spell_fizzled = 1;
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
        spell_fizzled = 1;
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
    FUN_004e4ff3(0);
  }
  else if ((event == EVENT_ACTIVATE) && ((instance->state & 0x20010) == 0))
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "DWARVEN_WARRIORS");
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
                              text_lines[0],
                              1,
                              &target))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
    }
    else
    {
      create_legacy_effect(card_on_stack_controller, card_on_stack, unk_007abc7c, target.player, target.card);
    }
    parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
    parent->number_of_targets = 0;
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
    if ((((player == active_player) && ((unk_00926804 & 2) == 0)) || (unk_008a9000 == 1)) || (unk_009252e0 != 0))
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
        spell_fizzled = 1;
      }
    }
    else
    {
      choice_hint = -1;
    }
    if (spell_fizzled != 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "BIRDS_OF_PARADISE");
      }
      color = choose_a_color(player, text_lines[0], 1, choice_hint, (unsigned char)instance->damage_source_card);
      if (color == ~COLOR_COLORLESS)
      {
        spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, color, 1);
        undeclare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
        instance->state |= STATE_TAPPED;
        produced_mana_color = color;
        if (((player == active_player) && ((unk_00926804 & 2) == 0)) && (unk_008a9000 != 1))
        {
          load_text((int)"prompts.txt", "BIRDS_OF_PARADISE2");
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
  else if ((event == EVENT_COUNT_MANA) && (card == affected_card) && (player == affected_card_controller))
  {
    if ((instance->state & 0x20010) == 0)
    {
      declare_mana_available_hex(player, (unsigned char)instance->damage_source_card, 1);
    }
  }
  else if ((event == 0x8a) && (card == affected_card) && (player == affected_card_controller))
  {
    unk_00925d3c += 0x60 / (basiclandtypes_controlled[player][7] + 2);
  }
  else if ((event == 0x8b) && (card == affected_card) && (player == affected_card_controller))
  {
    unk_00925d3c -= 0x60 / (basiclandtypes_controlled[player][7] + 2);
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
    if ((instance->state & 0x20010) == 0 && player != human_player && current_phase < 0x1a && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, 0xffffffff, 0xffffffff, 0x80, 0, 0x1000))
    {
      return 1;
    }
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text((int)"promptsX1.txt", "NETTLING_IMP");
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
                                text_lines[0],
                                1,
                                &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = target;
        instance->number_of_targets = 1;
        instance->state |= STATE_TAPPED;
        if (player == active_player)
        {
          ai_modifier += 0x30;
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
        spell_fizzled = 1;
      }
      else
      {
        create_legacy_effect(card_on_stack_controller, card_on_stack, unk_0079140c, target.player, target.card);
      }
      parent = &PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card);
      parent->number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054c131
// FUNCTION: SHANDALAR 0x0049a36e
int card_sengir_vampire(int player, int card, event_t event)
{
  int dead_internal_id;
  int shift_index;
  int count_triggered;
  int tracked;

  if (event == 0x6e && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card != -1)
    {
      shift_index = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player;
      if (PLAYER_CARD_INSTANCE(
              shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
              .internal_card_id == -1)
      {
        dead_internal_id =
            PLAYER_CARD_INSTANCE(
                shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
                .original_internal_card_id;
      }
      else
      {
        dead_internal_id =
            PLAYER_CARD_INSTANCE(
                shift_index, PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card)
                .internal_card_id;
      }

      if ((global_cards_data[dead_internal_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(player, card).number_of_targets < 0x13)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].card =
            PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets].player =
            shift_index;
        ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (event == 0x77)
  {
    count_triggered = 0;
    for (tracked = 0; tracked < PLAYER_CARD_INSTANCE(player, card).number_of_targets; ++tracked)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[tracked].card == affected_card && PLAYER_CARD_INSTANCE(player, card).targets[tracked].player == affected_card_controller && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).token_status & 0x4000000) == 0)
      {
        shift_index = tracked;
        if (!count_triggered)
        {
          ++PLAYER_CARD_INSTANCE(player, card).info_slot;
          count_triggered = 1;
        }

        while (++shift_index < PLAYER_CARD_INSTANCE(player, card).number_of_targets)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[shift_index - 1].player =
              PLAYER_CARD_INSTANCE(player, card).targets[shift_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[shift_index - 1].card =
              PLAYER_CARD_INSTANCE(player, card).targets[shift_index].card;
        }
        --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      }
    }
  }
  else if (trigger_condition == 0xd5 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && current_turn == player && affected_card == card && affected_card_controller == player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      FUN_005513d7(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
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

// FUNCTION: MAGIC 0x0054d8ca
// FUNCTION: SHANDALAR 0x0049bb0a
int card_lord_of_the_pit(int player, int card, event_t event)
{
  card_instance_t *instance;
  int creature_to_sacrifice;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == 0x87)
  {
    if (FUN_0054dccd(player, card) == 0)
    {
      event_result |= 1;
    }
  }
  else if (event == 4 && affected_card == card && affected_card_controller == player)
  {
    ++instance->info_slot;
    if (FUN_0054dccd(player, card) == 0)
    {
      event_result |= 1;
    }
    else
    {
      creature_to_sacrifice = -1;
      for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
      {
        if (current_card != card && is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          creature_to_sacrifice = current_card;
          break;
        }
      }
      if (creature_to_sacrifice != -1)
      {
        kill_card(player, creature_to_sacrifice, KILL_SACRIFICE);
      }
    }
  }
  else if (event == 0x86)
  {
    damage_player(player, 7, player, card);
  }
  else
  {
    if (event == 199 && FUN_0054dccd(player, card) == 0)
    {
      damage_player(player, 7, player, card);
    }
    if ((event == 0x22 || event == 199) && affected_card == card && affected_card_controller == player)
    {
      instance->info_slot = 0;
    }
    if (event == 0x8a && affected_card == card && affected_card_controller == player)
    {
      unk_00925d3c -= 0x30;
    }
    else if (event == 0x8b && affected_card == card && affected_card_controller == player)
    {
      unk_00925d3c += 0x30;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054df54
// FUNCTION: SHANDALAR 0x0049c195
int card_nether_shadow(int player, int card, event_t event)
{
  int hand_card;
  int hand_player;

  if (card == affected_card)
  {
    if (player == affected_card_controller)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;
    }
  }

  if (event == 0x8d ||
      (event == 0x77 &&
       card == affected_card &&
       player == affected_card_controller &&
       (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
       PLAYER_CARD_INSTANCE(player, card).kill_code != 4))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0)
    {
      hand_player = active_player;
    }
    else
    {
      hand_player = unk_008b35ec;
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
         (PLAYER_CARD_INSTANCE(player, card).state & 0x10) != 0))
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

// FUNCTION: MAGIC 0x0052a3fd
// FUNCTION: SHANDALAR 0x004e1bf2
int card_paralyze(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_ok;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    spell_fizzled = !FUN_00551638(player, 2, card);
    if (!spell_fizzled)
    {
      *((char *)&PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card) + 0x28) += 4;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_ok = C_real_validate_target(instance->targets[0].player,
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
                                       0,
                                       0,
                                       0);
    if (!target_ok)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      FUN_004f7783(instance->damage_target_player, instance->damage_target_card);
    }
    *((char *)instance + 0x32) = 0;
    return 0;
  }

  if (event == 0x82 && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1)
  {
    *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card) + 0x118)) &= 0xfffffffc;
    return 0;
  }

  if (event == 0x84 && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1 && (PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).state & 0x10) != 0 && instance->damage_target_player == human_player && unk_00742f60 == human_player)
  {
    *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card) + 0x58)) |= 0x10;
    *((char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card) + 0x28) += 4;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005195e5
// FUNCTION: SHANDALAR 0x004d0dc7
int card_animate_dead(int player, int card, event_t event)
{
  int allowed[500];
  char prompt[900];
  char graveyard_name[100];
  int chosen_graveyard;
  int graveyard_data[4];
  int current_player;
  int current_slot;

  if (event == EVENT_CAN_CAST)
  {
    if (player == unk_008b35ec || (unk_00926804 & 2) != 0)
    {
      return (unk_007a7c58[0] | unk_007a7c58[1]) & TYPE_CREATURE;
    }

    if (unk_008a9000 == 1)
    {
      unk_00939340 = internal_rand(2);
      FUN_004e4f11();
    }
    else
    {
      FUN_004e5089();
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot = unk_00939340;
    return unk_007a7c58[unk_00939340] & TYPE_CREATURE;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (((player == active_player) && (unk_00926804 & 2) == 0) || unk_008a9000 == 1)
    {
      chosen_graveyard = PLAYER_CARD_INSTANCE(player, card).info_slot;
      graveyard_data[3] = FUN_004087cc(chosen_graveyard, TYPE_CREATURE);
    }
    else
    {
      graveyard_data[1] = 0;
      graveyard_data[0] = 0;
      for (current_player = 0; current_player < 2; current_player = current_player + 1)
      {
        current_slot = 0;
        while (1)
        {
          if (499 < current_slot || global_graveyard_slots[current_player][current_slot] == -1)
          {
            break;
          }
          if ((global_cards_data[global_graveyard_slots[current_player][current_slot]].type & TYPE_CREATURE) != 0)
          {
            graveyard_data[current_player] = graveyard_data[current_player] + 1;
            break;
          }
          current_slot = current_slot + 1;
        }
      }

      if (graveyard_data[0] == 0 || graveyard_data[1] == 0)
      {
        if (graveyard_data[0] == 0)
        {
          chosen_graveyard = 1;
        }
        else
        {
          chosen_graveyard = 0;
        }
      }
      else
      {
        load_text("prompts.txt", "ANIMATE_DEAD");
        if ((unk_00926804 & 2) == 0 || player != active_player)
        {
          sprintf(prompt, "%s\n%s\n%s", text_lines[0], text_lines[1], text_lines[2]);
        }
        else
        {
          sprintf(prompt, "%s\n%s\n%s", text_lines[1], text_lines[0], text_lines[2]);
        }
        chosen_graveyard = do_dialog(player, player, card, -1, -1, prompt, 0);
        if (chosen_graveyard == 2)
        {
          spell_fizzled = 1;
        }
        else if ((unk_00926804 & 2) != 0 && player == active_player)
        {
          chosen_graveyard = 1 - chosen_graveyard;
        }
      }

      if (spell_fizzled != 1)
      {
        if (chosen_graveyard == 0)
        {
          load_text("prompts.txt", "ANIMATE_DEAD");
          strcpy(prompt, text_lines[4]);
        }
        else
        {
          load_text("prompts.txt", "ANIMATE_DEAD");
          FUN_00449bef(graveyard_name);
          sprintf(prompt, text_lines[3], graveyard_name);
        }

        memset(allowed, 0, 2000);
        for (current_slot = 0; current_slot < 500 && global_graveyard_slots[chosen_graveyard][current_slot] != -1;
             ++current_slot)
        {
          if ((global_cards_data[global_graveyard_slots[chosen_graveyard][current_slot]].type & TYPE_CREATURE) != 0)
          {
            allowed[current_slot] = 1;
          }
        }

        graveyard_data[3] =
            FUN_004b41f2(player, global_graveyard_slots[chosen_graveyard], allowed, 500, prompt, 0, &gs_cancel_008a8c20);
        if (graveyard_data[3] == -1)
        {
          spell_fizzled = 1;
        }
      }
    }

    if ((((spell_fizzled == 1) || (graveyard_data[3] == -1)) || (global_graveyard_slots[chosen_graveyard][graveyard_data[3]] == -1)) || ((global_cards_data[global_graveyard_slots[chosen_graveyard][graveyard_data[3]]].type & TYPE_CREATURE) == 0))
    {
      spell_fizzled = 1;
    }
    else
    {
      current_player = add_card_to_hand(player, global_graveyard_slots[chosen_graveyard][graveyard_data[3]]);
      if (current_player != -1)
      {
        PLAYER_CARD_INSTANCE(player, card).damage_target_card = current_player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_player = player;
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
             .state &= 0xffffefff;
        if (chosen_graveyard != 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                 PLAYER_CARD_INSTANCE(player, card).damage_target_card)
               .state |= 0x1000;
        }
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
             .state |= 0x20;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x78)) = chosen_graveyard;
        *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x7c)) = graveyard_data[3];
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    graveyard_data[3] = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x7c));
    chosen_graveyard = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x78));
    if (global_graveyard_slots[chosen_graveyard][graveyard_data[3]] == -1)
    {
      kill_card(player, card, KILL_DESTROY);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
          .internal_card_id = -1;
      spell_fizzled = 1;
    }
    else
    {
      FUN_004b15f7(chosen_graveyard, graveyard_data[3]);
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      FUN_0051a41c((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card);
      *((short *)((char *)&PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                PLAYER_CARD_INSTANCE(player, card).damage_target_card) +
                  0x0e)) = -1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else
  {
    if (((((trigger_condition == 0xd4) && (card == affected_card)) && ((player == affected_card_controller) && ((PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1) && (PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                                                                                                                                                                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                                                                                                                                                                                        .internal_card_id != -1)))) &&
         (player == trigger_cause_controller)) &&
        ((card == trigger_cause) && (current_turn == player)))
    {
      if (event == 0x7d)
      {
        event_result |= 2;
      }
      if (event == 0x7e)
      {
        if (PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0)
        {
          *(unsigned int *)((char *)&PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                          PLAYER_CARD_INSTANCE(player, card).damage_target_card) +
                            0x18) |= 8;
        }
        current_player = (char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
        PLAYER_CARD_INSTANCE(player, card).damage_target_player = -1;
        kill_card(current_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card, KILL_BURY);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004076dc
// FUNCTION: SHANDALAR 0x0044d3fa
int card_demonic_tutor(int player, int card, event_t event)
{
  int found_card;
  unsigned int type_mask;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6c && affected_card == card && affected_card_controller == player && player == active_player && (unk_00926804 & 2) == 0 && global_library[player][0] == -1)
  {
    spell_fizzled = 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    found_card = -1;

    if (((player == active_player) && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
    {
      if (player == active_player)
      {
        if (unk_008a9000 == 1)
        {
          unk_00939340 = internal_rand(4);
          FUN_004e4f11();
        }
        else
        {
          FUN_004e5089();
        }

        switch (unk_00939340)
        {
        default:
        case 0:
          type_mask = TYPE_CREATURE;
          break;
        case 1:
          type_mask = TYPE_ENCHANTMENT;
          break;
        case 2:
          type_mask = TYPE_ARTIFACT;
          break;
        case 3:
          type_mask = TYPE_LAND;
          break;
        }
      }
      else
      {
        type_mask = TYPE_CREATURE | TYPE_ENCHANTMENT;
      }

      found_card = FUN_00483e3e(player, type_mask);
      if (found_card == -1)
      {
        found_card = FUN_00483e3e(player, 0xffffffff);
      }
    }
    else
    {
      load_text((int)"prompts.txt", "DEMONIC_TUTOR");
      found_card = show_deck(player, global_library[player], 500, text_lines, 1, &gs_cancel_008a8c20);
    }

    if (found_card != -1 && global_library[player][found_card] != -1)
    {
      add_card_to_hand(player, global_library[player][found_card]);
      remove_card_from_deck(player, found_card);
      ++hand_count[player];
      TENTATIVE_reassess_all_cards();
      FUN_004b59b2(player, player);
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040808e
// FUNCTION: SHANDALAR 0x0044ddad
int card_mind_twist(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int i;
  int amount;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (player == active_player && (unk_00926804 & 2) == 0 && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    load_text((int)"prompts.txt", "MINDTWIST");
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
      instance->info_slot = x_value;
      instance->targets[0] = target;
      *((char *)instance + 0x32) = 1;
      if (player == active_player)
      {
        amount = (hand_count[1 - player] - x_value) * 0xc;
        if (amount < 1)
        {
          amount = 0;
        }
        ai_modifier -= amount;
        if (x_value <= hand_count[1 - player])
        {
          amount = x_value;
        }
        else
        {
          amount = hand_count[1 - player];
        }
        ai_modifier += amount * 0x18;
      }
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (i = 0; i < instance->info_slot; ++i)
    {
      discard(instance->targets[0].player, 1, 0);
    }
    *((char *)instance + 0x32) = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d2d4
// FUNCTION: SHANDALAR 0x004e4ace
int card_pestilence(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    if (FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) != 0)
    {
      ai_modifier -= 0x60;
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana_w_global_cost_mod(player, card, 1, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, 1, 1) != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 1, 1);
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    damage_player(1 - player, 1, card_on_stack_controller, card_on_stack);
    damage_player(player, 1, card_on_stack_controller, card_on_stack);
    dispatch_function_to_all_cards_in_play(card_on_stack_controller, card_on_stack, FUN_0052d761, -1);
    return 0;
  }

  if (trigger_condition == 0xcd && affected_card == card && affected_card_controller == player && current_turn == player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e && FUN_005510dc(player, TYPE_CREATURE) == 0 && FUN_005510dc(1 - player, TYPE_CREATURE) == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
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
    if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && has_mana(player, 7, 5) != 0 && (((PLAYER_CARD_INSTANCE(player, card).token_status & 3) == 0) || ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) && (PLAYER_CARD_INSTANCE(player, card).state & 0x10) == 0)
    {
      if (active_player == player && (unk_00926804 & 2) == 0 && unk_00939330 > 0)
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
    return 0;
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
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    bee_card = FUN_004b5f03(0x375);
    bee_card = add_card_to_hand(player, bee_card);
    if (bee_card != -1)
    {
      FUN_0051a41c(player, bee_card);
      *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(player, bee_card) + 0x14)) |= 0x10;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .info_slot = 0;
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
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "FORCEFIELD");
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

// FUNCTION: MAGIC 0x0051f540
// FUNCTION: SHANDALAR 0x004d6d23
int card_power_leak(int player, int card, event_t event)
{
  target_t target;
  char message[44];
  int damage;
  int dialog_result;
  int source_player;
  int source_card;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ENCHANTMENT,
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

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "POWERLEAK");
    }
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ENCHANTMENT,
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
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((player == active_player) && (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player))
      {
        ai_modifier -= 0x60;
      }
      ai_modifier += 0x30;
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_ENCHANTMENT,
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }
  else if (event == 0x73)
  {
    if (((current_phase == 4) && (unk_00742f60 == human_player)) && (((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)))
    {
      *(unsigned int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x58) |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }
  else if (((event == 4) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }
  else if (event == 0x86)
  {
    damage = has_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, COLOR_ANY, 1);
    if ((damage == 1) && (10 < life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player]))
    {
      damage = 0;
    }
    if (damage > 1)
    {
      if (((damage < 8) && (3 < *(int *)((char *)gs_cardtitle_damage_008cfd30 + (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player * 4 + 0x48))) && (7 < life[(int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player]))
      {
        damage = 0;
      }
      else
      {
        damage = 2;
      }
    }

    load_text((int)"prompts.txt", "POWERLEAK");
    sprintf(message, " %s\n %s\n %s", text_lines[1], text_lines[2], text_lines[3]);
    dialog_result = do_dialog((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                              player,
                              card,
                              (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                              PLAYER_CARD_INSTANCE(player, card).damage_target_card,
                              message,
                              damage);
    source_player = card_on_stack_controller;
    source_card = card_on_stack;
    if (dialog_result == 0)
    {
      damage = 2;
    }
    else if (dialog_result == 1)
    {
      FUN_00443ee2(player, card, 0x7e, 0, 0);
      charge_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 0, 1);
      obliterate_top_card_of_stack();
      if (spell_fizzled == 1)
      {
        damage = 2;
      }
      else
      {
        damage = 1;
      }
    }
    else
    {
      FUN_00443ee2(player, card, 0x7e, 0, 0);
      charge_mana((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 0, 2);
      obliterate_top_card_of_stack();
      if (spell_fizzled == 1)
      {
        damage = 2;
      }
      else
      {
        damage = 0;
      }
    }
    damage_player((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player, damage, source_player, source_card);
    spell_fizzled = -1;
    return 0;
  }
  else
  {
    if (event == 0x22)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040a06a
// FUNCTION: SHANDALAR 0x0044fd8c
int card_drain_power(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int target_player;
  int color_index;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "DRAIN_POWER");
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
      instance->targets[0] = target;
      *((char *)instance + 0x32) = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    FUN_0055117d(drain_power_draw_mana_from_land, target_player);
    if (target_player != player)
    {
      for (color_index = 0; color_index < 8; ++color_index)
      {
        raw_mana_available[player][color_index] += raw_mana_available[target_player][color_index];
        raw_mana_available[target_player][color_index] = 0;
      }
    }
    *((char *)instance + 0x32) = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f71db
// FUNCTION: SHANDALAR 0x004b1292
int card_twiddle(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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

  if (event == 0x6c && affected_card == card && affected_card_controller == player)
  {
    ai_modifier -= 0x30;
    load_text((int)"prompts.txt", "TWIDDLE");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = target;
      *((char *)instance + 0x32) = 1;
      if ((unk_008b4278 & 0x400) == 0)
      {
        instance->info_slot = (PLAYER_CARD_INSTANCE(target.player, target.card).state & STATE_TAPPED) != 0;
      }
      else
      {
        instance->info_slot = PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).info_slot;
      }
      if (player == active_player)
      {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_LAND) != 0)
        {
          ai_modifier -= 0x18;
        }
        if ((instance->info_slot == 0 && target.player == active_player) || (instance->info_slot == 1 && target.player == unk_008b35ec))
        {
          ai_modifier -= 0x60;
        }
      }
    }
    return 0;
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
                                TYPE_ARTIFACT | TYPE_CREATURE | TYPE_LAND,
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
    else if (instance->info_slot == 0)
    {
      FUN_004f7783(instance->targets[0].player, instance->targets[0].card);
    }
    else
    {
      PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).state &= ~STATE_TAPPED;
    }
    *((char *)instance + 0x32) = 0;
    kill_card(player, card, KILL_BURY);
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

  if (trigger_condition == 0xdf && affected_card == card && affected_card_controller == player && current_turn == player && player != human_player && player == trigger_cause_controller && card == trigger_cause)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      giant_copy_internal_id = create_a_card_type(instance->internal_card_id);
      if (giant_copy_internal_id != -1)
      {
        global_cards_data[giant_copy_internal_id].code_pointer = FUN_00481e25;
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

  if (trigger_condition == 0xd4 && affected_card == card && affected_card_controller == player && current_turn == player && player == trigger_cause_controller && card == trigger_cause && *((char *)instance + 0x20) != -1)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
      {
        if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == FUN_00481e25 && *((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 8) == player && *((int *)((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 0x44)) == card)
        {
          kill_card(player, current_card, KILL_DESTROY);
        }
      }
    }
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player && *((char *)instance + 0x20) != -1)
  {
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == FUN_00481e25 && *((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 8) == player && *((int *)((char *)&PLAYER_CARD_INSTANCE(player, current_card) + 0x44)) == card)
      {
        kill_card(player, current_card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00529afb
// FUNCTION: SHANDALAR 0x004e12ec
int card_stasis(int player, int card, event_t event)
{
  card_instance_t *instance;
  int current_card;
  int max_cards;
  int ai_tweak;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (FUN_00404cff(player, instance->internal_card_id, -1) == 0)
    {
      ai_modifier += landsofcolor_controlled[1 - player][2] - landsofcolor_controlled[player][2];
    }
    instance->damage_target_player = (char)player;
    return 0;
  }

  if (event == 0x85 && card == affected_card && player == affected_card_controller && player == human_player && unk_00742f60 == human_player)
  {
    *(unsigned int *)((char *)instance + 0x5c) |= 1;
    ++instance->upkeep_blue;
    return 0;
  }

  if (event == 0x86)
  {
    kill_card(card_on_stack_controller, card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == 199)
  {
    if (basiclandtypes_controlled[player][2] < 1)
    {
      kill_card(player, card, KILL_BURY);
    }
    else
    {
      max_cards = active_cards_count[active_player];
      if (max_cards <= active_cards_count[unk_008b35ec])
      {
        max_cards = active_cards_count[unk_008b35ec];
      }

      ai_tweak = 0;
      for (current_card = 0; current_card < max_cards; ++current_card)
      {
        if (is_in_play(unk_008b35ec, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).state & STATE_TAPPED) == 0)
          {
            if (has_vigilance(unk_008b35ec, current_card) == 0)
            {
              ai_tweak += PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).power;
            }
          }
          else
          {
            ai_tweak += PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).power * 2;
          }
        }

        if (is_in_play(active_player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(active_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(active_player, current_card).state & STATE_TAPPED) == 0)
          {
            if (has_vigilance(active_player, current_card) == 0)
            {
              ai_tweak -= PLAYER_CARD_INSTANCE(active_player, current_card).power;
            }
          }
          else
          {
            ai_tweak -= PLAYER_CARD_INSTANCE(active_player, current_card).power * 2;
          }
        }
      }

      ai_modifier += ai_tweak * 0xc;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bfaf
// FUNCTION: SHANDALAR 0x0049a1eb
int card_scavenging_ghoul(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    result = FUN_0054276d(player, card, EVENT_CAN_ACTIVATE, 0, 0);
    if (C_get_special_counters(player, card) == 0)
    {
      result = 0;
    }
    return result;
  }

  if (event == EVENT_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    result = FUN_0054276d(player, card, EVENT_ACTIVATE, 0, 0);
    FUN_005514cd(player, card, 1);
    return result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (unk_008b4278 & 0x200) != 0)
  {
    return FUN_0054276d(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
  }

  if ((trigger_condition == 0xcd || event == 199) && affected_card == card && affected_card_controller == player && unk_008cfdac != 0 && player == current_turn)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      FUN_005513d7(player, card, unk_008cfdac);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052b3d4
// FUNCTION: SHANDALAR 0x004e2bc8
int card_power_surge(int player, int card, event_t event)
{
  card_instance_t *instance;
  int current_card;
  int damage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == 0x6a)
  {
    instance->info_slot = 0;
    for (current_card = 0; current_card < active_cards_count[human_player]; ++current_card)
    {
      if (is_in_play(human_player, current_card) && (PLAYER_CARD_INSTANCE(human_player, current_card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(human_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        ++instance->info_slot;
      }
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (current_phase == 4 && (instance->info_slot & 1) == 0 && unk_00742f60 == human_player)
    {
      *(unsigned int *)((char *)instance + 0x5c) |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (event == 4 && card == affected_card && player == affected_card_controller)
  {
    instance->info_slot |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }

  if (event == 0x86)
  {
    damage_player(human_player, instance->info_slot, card_on_stack_controller, card_on_stack);
    instance->info_slot = 0;
    return 0;
  }

  if (event == 0x22)
  {
    instance->info_slot &= ~1;
  }

  if (event == 199)
  {
    damage = 0;
    for (current_card = 0; current_card < active_cards_count[1 - human_player]; ++current_card)
    {
      if (is_in_play(1 - human_player, current_card) && (PLAYER_CARD_INSTANCE(1 - human_player, current_card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(1 - human_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        ++damage;
      }
    }
    damage_player(1 - human_player, damage, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052af2f
// FUNCTION: SHANDALAR 0x004e2723
int card_smoke(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t selected_target;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (FUN_00404cff(player, instance->internal_card_id, -1) == 0)
    {
      ai_modifier += (landsofcolor_controlled[unk_008b35ec][2] - landsofcolor_controlled[active_player][2]) * 0xc;
    }
    return 0;
  }

  if (event == 0x82 && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_CREATURE) != 0)
  {
    PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).untap_status &= ~2;
    unk_00925d38 |= 2;
    return 0;
  }

  if (current_phase == 1 && affected_card == card && affected_card_controller == player)
  {
    if (event == 0x7d && real_target_available((int *)0, TARGET_SCAN_DIRECT, human_player, human_player, human_player, 0x200, TYPE_CREATURE, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x800, 0) == 0 && real_target_available((int *)0, TARGET_SCAN_DIRECT, human_player, human_player, human_player, 0x200, TYPE_CREATURE, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x400, 0) != 0)
    {
      event_result |= 2;
    }

    if (event == 0x7e)
    {
      if (active_player == human_player && (unk_00926804 & 2) == 0)
      {
        selected_target.player = human_player;
        selected_target.card = FUN_00534ddb(human_player, 2);
        load_text(0, "SMOKE");
        do_dialog(player,
                  player,
                  card,
                  selected_target.player,
                  selected_target.card,
                  text_lines[1],
                  0);
      }
      else
      {
        load_text(0, "SMOKE");
        C_real_select_target(human_player,
                             human_player,
                             human_player,
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
                             TARGET_STATE_COULD_UNTAP | TARGET_STATE_TAPPED,
                             0,
                             text_lines[0],
                             0,
                             &selected_target);
      }

      PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).untap_status |= 2;
      for (current_card = 0; current_card < active_cards_count[human_player]; ++current_card)
      {
        if (is_in_play(human_player, current_card) && (PLAYER_CARD_INSTANCE(human_player, current_card).state & STATE_TAPPED) != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(human_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (PLAYER_CARD_INSTANCE(human_player, current_card).untap_status & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(human_player, current_card).untap_status &= ~1;
        }
      }
    }
  }

  if (event == 0x22)
  {
    instance->info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00401c46
// FUNCTION: SHANDALAR 0x00447966
int card_wheel_of_fortune(int player, int card, event_t event)
{
  int current_player;
  int player_index;
  int cards_in_hand;
  int current_card;
  int player_zero_library;
  int player_one_library;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    ai_modifier += hand_count[player] * -0x18 + 0x30;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = human_player;
    for (player_index = 0; player_index < 2; ++player_index)
    {
      cards_in_hand = hand_count[current_player];
      for (current_card = 0; current_card < cards_in_hand; ++current_card)
      {
        discard(current_player, 1, 0);
      }
      current_player = current_player == 0 ? 1 : 0;
    }

    TENTATIVE_reassess_all_cards();
    player_zero_library = 0;
    player_one_library = 0;
    for (current_card = 0; current_card < 500; ++current_card)
    {
      if (global_library[0][current_card] != -1)
      {
        ++player_zero_library;
      }
      if (global_library[1][current_card] != -1)
      {
        ++player_one_library;
      }
    }

    if (player_zero_library < 7 && player_one_library < 7)
    {
      life[0] = -99;
      life[1] = -99;
    }
    else
    {
      FUN_0040246a(0, 7);
      FUN_0040246a(1, 7);
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004024c5
// FUNCTION: SHANDALAR 0x004481e7
int card_channel(int player, int card, event_t event)
{
  PLAYER_CARD_INSTANCE(player, card).mana_color = 1;
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, unk_008a8c34, -1, -1);
    declare_mana_available(player, COLOR_COLORLESS, 1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004391fe
// FUNCTION: SHANDALAR 0x004fbb9f
int card_fastbond(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (player == human_player && (unk_008b4278 & 1) != 0 && current_phase > 0x13 && current_phase < 0x1f)
  {
    unk_008b4278 &= ~1;
  }

  if ((trigger_condition == 0xdb || trigger_condition == 0xd3) && affected_card == card && affected_card_controller == player && unk_008cfdb0 > 0 && current_turn == player && trigger_cause_controller == player && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type & TYPE_LAND) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      damage_player(player, 1, player, card);
    }
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
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
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
                             text_lines[0],
                             1,
                             &target) == 0)
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
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

// FUNCTION: MAGIC 0x00523ef2
// FUNCTION: SHANDALAR 0x004db6e2
int card_lure(int player, int card, event_t event)
{
  card_instance_t *instance;
  int blocking_card;

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
                                 0,
                                 0,
                                 0);
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text(0, "LURE");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    instance->info_slot = 0;
    return 0;
  }

  if (trigger_condition == 0xda && instance->info_slot == 0)
  {
    instance->info_slot = 1;
    trigger_condition = -1;
    if ((int)instance->damage_target_player == human_player && (PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).state & 4) != 0 && is_in_play(affected_card_controller, affected_card) && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).blocking == -1 && affected_card_controller != human_player && FUN_0052460c(affected_card_controller, affected_card, instance->damage_target_player, instance->damage_target_card) != 0)
    {
      if (PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).blocking == -1)
      {
        blocking_card = instance->damage_target_card;
      }
      else
      {
        blocking_card =
            PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).blocking;
      }

      if (event == 0x7d)
      {
        event_result |= 2;
      }
      if (event == 0x7e)
      {
        PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).blocking = (unsigned char)blocking_card;
        PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state |= 0x8008;
      }
    }
    trigger_condition = 0xda;
    instance->info_slot = 0;
  }

  return 0;
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
    if ((player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x44) != 0))
    {
      if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = card;
      }
      else
      {
        blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }

      for (current_card = 0; current_card < active_cards_count[opponent]; current_card = current_card + 1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (((PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card) && (blocking_iid != -1)) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, unk_008b3104, opponent, current_card);
        }
      }
    }

    if ((player != human_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
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
        for (current_card = 0; current_card < active_cards_count[opponent]; current_card = current_card + 1)
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

// FUNCTION: MAGIC 0x004fafeb
// FUNCTION: SHANDALAR 0x004b50c0
int card_magical_hack(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  struct
  {
    unsigned int available_colors;
    int old_color;
    int new_color;
    target_t target;
    char prompt[200];
  } s;
  card_ptr_t *raw_card;
  int target_iid;

  if (event == EVENT_CAN_CAST)
  {
    if ((unk_008b35ec == player) || (((unsigned char)unk_00926804 & 2) != 0))
    {
      if (unk_008ce508 == -1)
      {
        FUN_004e4ff3(0);
        return 1;
      }
      return 99;
    }
    if ((unk_008ce508 == -1) || (unk_008b35ec != human_player))
    {
      FUN_004e4ff3(0);
      return 1;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
    {
      load_text((int)"prompts.txt", "MAGICAL_HACK");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
                               &s.target) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_008ce508;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = unk_008ce4f4;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }

    if (spell_fizzled != 1)
    {
      if ((player == active_player) && (((unsigned char)unk_00926804 & 2) == 0))
      {
        target_iid =
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .internal_card_id;
        raw_card = &global_raw_cards_storage[global_cards_data[target_iid].id];
        s.available_colors = *(unsigned int *)&raw_card->hack_colors;
        if (s.available_colors == 0)
        {
          spell_fizzled = 1;
        }
        else
        {
          if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).token_status & STATUS_HACKED) != 0)
          {
            s.old_color = single_color_test_bit_to_color_t((unsigned char)s.available_colors);
            s.old_color = get_hacked_color(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, s.old_color);
            s.available_colors = 1 << ((unsigned char)s.old_color & 0x1f);
          }

          do
          {
            s.old_color = internal_rand(5) + 1;
          } while ((s.available_colors & (1 << ((unsigned char)s.old_color & 0x1f))) == 0);

          do
          {
            s.new_color = internal_rand(5) + 1;
          } while (s.new_color == s.old_color);

          if (unk_008a9000 == 1)
          {
            unk_00939340 = s.old_color;
            FUN_004e4f11();
            unk_00939340 = s.new_color;
            FUN_004e4f11();
          }
          else
          {
            FUN_004e5089();
            s.old_color = unk_00939340;
            FUN_004e5089();
            s.new_color = unk_00939340;
          }

          PLAYER_CARD_INSTANCE(player, card).info_slot = s.new_color * 0x100 + s.old_color;
          if (unk_008a9000 != 1)
          {
            load_text((int)"prompts.txt", "MAGICAL_HACK");
            strcpy(s.prompt, "\n");
            sprintf(s.prompt + strlen(s.prompt), text_lines[1], "", "");
            do_dialog(player,
                      player,
                      card,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                      s.prompt,
                      0);
          }
        }
      }
    }

    if (spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    s.target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(s.target.player, s.target.card).token_status |= STATUS_HACKED;
    FUN_004faee2(s.target.player,
                 s.target.card,
                 (unsigned char)PLAYER_CARD_INSTANCE(player, card).info_slot,
                 *((unsigned char *)&PLAYER_CARD_INSTANCE(player, card).info_slot + 1));
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fba06
// FUNCTION: SHANDALAR 0x004b5b0c
int card_sleight_of_mind(int player, int card, event_t event)
{
  extern card_ptr_t global_raw_cards_storage[2000];
  card_instance_t *instance;
  target_t target;
  unsigned int available_colors;
  int target_iid;
  int old_color;
  int new_color;

  if (event == EVENT_CAN_CAST)
  {
    if ((unk_008b35ec == player) || ((unk_00926804 & 2) != 0))
    {
      if (unk_008ce508 == -1)
      {
        FUN_004e4ff3(0);
        return 1;
      }
      return 99;
    }
    if (unk_008ce508 == -1 || unk_008b35ec != human_player)
    {
      FUN_004e4ff3(0);
      return 1;
    }
    return 99;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
    {
      load_text(0, "SLEIGHT OF MIND");
      if (C_real_select_target(player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
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
                               &target) == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[0] = target;
        instance->number_of_targets = 1;
      }
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }

    if (spell_fizzled != 1)
    {
      target_iid =
          PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id;
      available_colors = global_raw_cards_storage[global_cards_data[target_iid].id].sleight_color;
      if (available_colors == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        if ((PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).token_status & STATUS_SLEIGHTED) != 0)
        {
          old_color = single_color_test_bit_to_color_t((unsigned char)available_colors);
          old_color = get_sleighted_color(instance->targets[0].player, instance->targets[0].card, old_color);
          available_colors = 1 << ((unsigned char)old_color & 0x1f);
        }

        do
        {
          old_color = internal_rand(5) + 1;
        } while ((available_colors & (1 << ((unsigned char)old_color & 0x1f))) == 0);

        do
        {
          new_color = internal_rand(5) + 1;
        } while (new_color == old_color);

        if (unk_008a9000 == 1)
        {
          unk_00939340 = old_color;
          FUN_004e4f11();
          unk_00939340 = new_color;
          FUN_004e4f11();
        }
        else
        {
          FUN_004e5089();
          old_color = unk_00939340;
          FUN_004e5089();
          new_color = unk_00939340;
        }

        instance->info_slot = new_color * 0x100 + old_color;
      }
    }

    if (spell_fizzled == 1)
    {
      instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target = instance->targets[0];
    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_CHANGET);
    }
    PLAYER_CARD_INSTANCE(target.player, target.card).token_status |= STATUS_SLEIGHTED;
    FUN_004fb8fd(target.player,
                 target.card,
                 (unsigned char)instance->info_slot,
                 (unsigned char)((unsigned int)instance->info_slot >> 8));
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040db0e
// FUNCTION: SHANDALAR 0x0051032b
int card_black_vise(int player, int card, event_t event)
{
  card_instance_t *instance;
  int damaged_player;
  int damage;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == 199 && is_in_play(player, card))
  {
    if ((((unsigned char *)&instance->state)[1] & 0x10) == 0)
    {
      damaged_player = active_player;
    }
    else
    {
      damaged_player = unk_008b35ec;
    }

    damage = hand_count[damaged_player] - 4;
    if (damage < 1)
    {
      damage = 0;
    }
    if (damage != 0)
    {
      damage = 0x18 - life[damaged_player] / damage;
      if (damage < 2)
      {
        damage = 1;
      }
      if (damaged_player == 0)
      {
        ai_modifier += damage * 0x18;
      }
      else
      {
        ai_modifier -= damage * 0x18;
      }
    }
  }

  if (trigger_condition == 0xcb && card == affected_card && player == affected_card_controller && player == current_turn && ((((unsigned char *)&instance->state)[1] & 0x10) == 0 || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0))
  {
    if ((((unsigned char *)&instance->state)[1] & 0x10) == 0)
    {
      damaged_player = active_player;
    }
    else
    {
      damaged_player = unk_008b35ec;
    }

    if (human_player == damaged_player && hand_count[damaged_player] > 4)
    {
      if (event == 0x7d)
      {
        event_result |= 2;
      }
      if (event == 0x7e)
      {
        damage_player(damaged_player, hand_count[damaged_player] - 4, player, card);
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040a8d0
// FUNCTION: SHANDALAR 0x00401000
int card_contract_from_below(int player, int card, event_t event)
{
  int cards_in_hand;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += hand_count[player] * -0x18 + 0x48;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    cards_in_hand = hand_count[player];
    for (current_card = 0; current_card < cards_in_hand; ++current_card)
    {
      discard(player, 0, 0);
    }
    FUN_004817fd(player);
    FUN_0040246a(player, 7);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402229
// FUNCTION: SHANDALAR 0x00447f4b
int card_timetwister(int player, int card, event_t event)
{
  int current_player;
  int player_index;
  int graveyard_index;
  int cards_in_hand;
  int current_card;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    ai_modifier += hand_count[player] * -0x18 + 0x30;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    current_player = human_player;
    for (player_index = 0; player_index < 2; ++player_index)
    {
      cards_in_hand = hand_count[current_player];
      for (current_card = 0; current_card < cards_in_hand; ++current_card)
      {
        discard(current_player, 1, 0);
      }

      for (graveyard_index = 0;
           graveyard_index < 500 && global_graveyard_slots[current_player][graveyard_index] != -1;
           ++graveyard_index)
      {
        FUN_004b5cf5(current_player, global_graveyard_slots[current_player][graveyard_index]);
        global_graveyard_slots[current_player][graveyard_index] = -1;
      }

      TENTATIVE_reassess_all_cards();
      FUN_004b59b2(player, current_player);
      FUN_0040246a(current_player, 7);
      current_player = current_player == 0 ? 1 : 0;
    }

    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004013ed
// FUNCTION: SHANDALAR 0x0044710d
int card_time_walk(int player, int card, event_t event)
{
  int current_player;
  int current_card;
  int effect_card;
  int effect_found;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0x60;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (unk_0092664c[7] == -1)
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
        unk_0092664c[7] = player;
      }
    }

    effect_card = create_legacy_effect(player, card, unk_008b49c4, -1, -1);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x120;
    }
    kill_card(player, card, KILL_BURY);
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

    if (unk_0092664c[7] == -1)
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
        unk_0092664c[7] = player;
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

// FUNCTION: MAGIC 0x0040159b
// FUNCTION: SHANDALAR 0x004472bb
int card_balance(int player, int card, event_t event)
{
  struct
  {
    int player_1_count;
    int pad;
    int player_0_count;
    int current_card;
    char land_prompt[300];
    char creature_prompt[300];
  } locals;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "BALANCE");
        strcpy(locals.land_prompt, text_lines[0]);
        strcpy(locals.creature_prompt, text_lines[1]);
      }

      do
      {
        locals.player_1_count = 0;
        locals.player_0_count = locals.player_1_count;
        locals.current_card = 0;
        while (1)
        {
          if ((active_cards_count[1] <= active_cards_count[0] ? active_cards_count[0] : active_cards_count[1]) <= locals.current_card)
          {
            break;
          }
          if (is_in_play(0, locals.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(0, locals.current_card).internal_card_id].type & TYPE_LAND) != 0)
          {
            ++locals.player_0_count;
          }
          if (is_in_play(1, locals.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(1, locals.current_card).internal_card_id].type & TYPE_LAND) != 0)
          {
            ++locals.player_1_count;
          }
          ++locals.current_card;
        }

        if (unk_008a9000 != 1)
        {
          strcpy(text_lines[0], locals.land_prompt);
        }

        if (locals.player_1_count < locals.player_0_count)
        {
          sacrifice_a_land(0);
        }
        else if (locals.player_0_count < locals.player_1_count)
        {
          sacrifice_a_land(1);
        }

        TENTATIVE_reassess_all_cards();
      } while (locals.player_0_count != locals.player_1_count);

      do
      {
        if (hand_count[1] < hand_count[0])
        {
          discard(0, 0, 0);
        }
        if (hand_count[0] < hand_count[1])
        {
          discard(1, 0, 0);
        }
      } while (hand_count[0] != hand_count[1]);

      do
      {
        locals.player_1_count = 0;
        locals.player_0_count = locals.player_1_count;
        locals.current_card = 0;
        while (1)
        {
          if ((active_cards_count[1] <= active_cards_count[0] ? active_cards_count[0] : active_cards_count[1]) <= locals.current_card)
          {
            break;
          }
          if (is_in_play(0, locals.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(0, locals.current_card).internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(0, locals.current_card).kill_code != 3)
          {
            ++locals.player_0_count;
          }
          if (is_in_play(1, locals.current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(1, locals.current_card).internal_card_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(0, locals.current_card).kill_code != 3)
          {
            ++locals.player_1_count;
          }
          ++locals.current_card;
        }

        if (unk_008a9000 != 1)
        {
          strcpy(text_lines[0], locals.creature_prompt);
        }

        if (locals.player_1_count < locals.player_0_count)
        {
          locals.current_card = FUN_00551921(0);
          kill_card(0, locals.current_card, KILL_SACRIFICE);
        }
        if (locals.player_0_count < locals.player_1_count)
        {
          locals.current_card = FUN_00551921(1);
          kill_card(1, locals.current_card, KILL_SACRIFICE);
        }

        TENTATIVE_reassess_all_cards();
      } while (locals.player_0_count != locals.player_1_count);

      kill_card(player, card, KILL_BURY);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0046667e
// FUNCTION: SHANDALAR 0x00455da6
int card_cyclopean_tomb(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *legacy;
  target_t target;
  int chosen_land_type;
  int legacy_card;
  int can_activate;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  chosen_land_type = get_hacked_color(player, card, 1) - 1;

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    TENTATIVE_set_timestamps(player, card);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (current_phase == 4 && player == current_turn && player == human_player && (instance->state & STATE_TAPPED) == 0 && (((instance->untap_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && has_mana(player, 7, 2) != 0)
    {
      can_activate = real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player, 0x200, TYPE_LAND, 0, 0,
                                           get_protections_from(player, card), 0, 0, chosen_land_type,
                                           -1, -1, -1, 0x100, 0, 0);
      if (can_activate != 0)
      {
        if (player == unk_008b35ec || (unk_00926804 & 2) != 0)
        {
          return 1;
        }
        if (internal_rand(100) < ((has_mana(player, 7, 1) + 1) / 2) * 0x14)
        {
          unk_008b3270 |= 3;
          return 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && affected_card == card && affected_card_controller == player && !spell_fizzled)
  {
    if (player == 1 - human_player && (unk_00926804 & 2) == 0)
    {
      if (FUN_00466e6d(player, card, 1 - player))
      {
        instance->state |= STATE_TAPPED;
      }
    }
    else
    {
      load_text("promptsX1.txt", "CYCLOPEAN_TOMB");
      if (!C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                chosen_land_type, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0, text_lines[0], 1,
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
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                chosen_land_type, ~SUB_WALL, -1, -1,
                                TARGET_SPECIAL_NOT_LAND_SUBTYPE, 0, 0))
    {
      spell_fizzled = 1;
    }
    else
    {
      legacy_card = create_legacy_effect(affected_card_controller, affected_card, unk_00789108,
                                         instance->targets[0].player, instance->targets[0].card);
      if (legacy_card != -1)
      {
        legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
        legacy->info_slot = chosen_land_type;
        legacy->token_status = 0x10000;
        legacy->timestamp = instance->timestamp;
        *(int *)((char *)legacy + 0x48) = 1;
      }
    }
    instance->number_of_targets = 0;
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player)
  {
    legacy_card = create_legacy_effect(player, card, unk_00789108, -1, -1);
    if (legacy_card != -1)
    {
      legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
      legacy->timestamp = instance->timestamp;
      *(int *)((char *)legacy + 0x48) = 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00517382
// FUNCTION: SHANDALAR 0x004ceb62
int card_island_sanctuary(int player, int card, event_t event)
{
  int current_card;
  int legacy_card;
  int found_attacker;
  int index;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (FUN_00404cff(player, PLAYER_CARD_INSTANCE(player, card).internal_card_id, player) == 0)
    {
      ai_modifier += 0x30;
    }
    return 0;
  }

  if (trigger_condition == 0xcf && current_phase == 10 && current_turn == human_player && affected_card == card && affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).damage_source_card == 0)
  {
    if (event == 0x7d)
    {
      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 2) == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 2;
          found_attacker = 0;
          current_card = 0;
          while (current_card < active_cards_count[unk_008b35ec] && !found_attacker)
          {
            if (is_in_play(unk_008b35ec, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && (PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).regen_status & 0x20) == 0 && (PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).regen_status & (1U << (((unsigned char)get_hacked_color(player, card, 2) - 1) & 0x1f))) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).internal_card_id].subtype != 0 || (PLAYER_CARD_INSTANCE(unk_008b35ec, current_card).token_status & 0x800) != 0))
            {
              found_attacker = 1;
            }
            ++current_card;
          }
          if (found_attacker && internal_rand(8 - hand_count[player]) == 0)
          {
            PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
          }
        }
        if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) != 0)
        {
          event_result |= 2;
        }
      }
      else
      {
        event_result |= 1;
      }
    }

    if (event == 0x7e)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
      unk_007a79b0[player] |= 1;
      legacy_card = create_legacy_effect(player, card, unk_008b49c4, -1, -1);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x400020;
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = (1 << (((unsigned char)get_hacked_color(player, card, 2) - 1) & 0x1f)) | 0x20;
        if ((PLAYER_CARD_INSTANCE(player, card).token_status & 2) != 0)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 2;
          for (index = 0; index < 6; ++index)
          {
            PLAYER_CARD_INSTANCE(player, legacy_card).hack_mode[index] = PLAYER_CARD_INSTANCE(player, card).hack_mode[index];
          }
        }
      }
      unk_0091c4fc = 1;
      ++PLAYER_CARD_INSTANCE(player, card).damage_source_card;
    }
  }

  if ((event == EVENT_CLEANUP || event == 199) && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).damage_source_card;
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
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == card)
    {
      instance->info_slot |= 1;
    }
    return 0;
  }

  if (event == 0x73)
  {
    if ((instance->info_slot & 1) != 0 && (unk_008b4278 & 4) != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (instance->info_slot & 1) != 0 && (unk_008b4278 & 4) != 0)
  {
    damage_source_player = (instance->state & 0x1000) != 0;
    do
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "PERSONAL_INCARNATION");
      }

      if (!C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                                COLOR_TEST_0, COLOR_TEST_0, unk_009266a4, ~SUB_WALL,
                                -1, -1, 0, 0, 0, text_lines[0], 1, &target))
      {
        spell_fizzled = 1;
      }
      else
      {
        selected = &PLAYER_CARD_INSTANCE(target.player, target.card);
        if (selected->damage_target_player == player && selected->damage_target_card == card)
        {
          if (unk_008a9000 != 1)
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
          damage_to_deal = choose_a_number(player, text_lines[1], max_damage);
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
    } while (!spell_fizzled && (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player != player || PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card != card));
  }

  if (event == EVENT_CLEANUP)
  {
    instance->info_slot &= ~1;
    return 0;
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player && instance->kill_code != 4 && is_in_play(player, card))
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

// FUNCTION: MAGIC 0x0051baf5
// FUNCTION: SHANDALAR 0x004d32db
int card_control_magic(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player && unk_008a9000 != 1)
  {
    load_text("prompts.txt", "CONTROL_MAGIC");
  }

  return FUN_0051bcf0(player, card, event, TYPE_CREATURE);
}

// FUNCTION: MAGIC 0x0051bb62
// FUNCTION: SHANDALAR 0x004d3348
int card_steal_artifact(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event != 0x3c && (instance->token_status & 0x80) == 0 && instance->damage_target_player != -1 && (global_cards_data[PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).internal_card_id].type & TYPE_ARTIFACT) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player && unk_008a9000 != 1)
  {
    load_text("prompts.txt", "STEAL_ARTIFACT");
  }

  return FUN_0051bcf0(player, card, event, TYPE_ARTIFACT);
}

// FUNCTION: MAGIC 0x004f6456
// FUNCTION: SHANDALAR 0x004b0512
int card_siren_s_call(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    if (human_player == player || current_phase > 0x14)
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, unk_00789314, -1, -1);
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00402d0c
// FUNCTION: SHANDALAR 0x00448a30
int card_volcanic_eruption(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int *state_ptr;
  target_t selected_target;
  int required_subtype;
  int current_target;
  int selecting_done;
  int invalid_targets;
  int damage_effect_card;
  char prompt[52];

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    real_target_available(unk_00743038 == 0 ? &max_x_value : (int *)0,
                          TARGET_SCAN_DIRECT,
                          player,
                          2,
                          2,
                          TARGET_ZONE_IN_PLAY,
                          TYPE_NONE,
                          TYPE_NONE,
                          0,
                          get_protections_from(player, card),
                          0,
                          0,
                          get_hacked_color(player, card, 4) - 1,
                          -1,
                          -1,
                          -1,
                          0,
                          0,
                          0);
    if (player == human_player && (unk_00926804 & 2) == 0 && (max_x_value == 0 || has_mana(player, 7, 4) == 0))
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    ai_modifier -= 0x24 / FUN_00404c4c(player, instance->internal_card_id);
    instance->number_of_targets = 0;
    required_subtype = get_hacked_color(player, card, 4) - 1;
    current_target = 0;
    selecting_done = 0;

    while (current_target < x_value && selecting_done == 0 && spell_fizzled != 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text((int)"prompts.txt", "VOLCANIC_ERUPTION");
        sprintf(prompt, text_lines[0], current_target + 1, x_value);
      }

      if (!C_real_select_target(player,
                                2,
                                1 - player,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                required_subtype,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                prompt,
                                1,
                                &selected_target))
      {
        if (selected_target.card == -1)
        {
          spell_fizzled = 1;
        }
        else
        {
          selecting_done = 1;
        }
      }
      else
      {
        state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).state;
        *state_ptr |= 0x300000;
        TENTATIVE_reassess_all_cards();
        instance->targets[instance->number_of_targets].player = selected_target.player;
        instance->targets[instance->number_of_targets].card = selected_target.card;
        ++instance->number_of_targets;
      }

      ++current_target;
    }

    for (current_target = 0; current_target < instance->number_of_targets; ++current_target)
    {
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(instance->targets[current_target].player,
                                                        instance->targets[current_target].card)
                      .state;
      *state_ptr &= 0xffcfffff;
    }

    if (spell_fizzled == 1)
    {
      instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    required_subtype = get_hacked_color(player, card, 4) - 1;
    invalid_targets = 0;

    for (current_target = 0; current_target < instance->number_of_targets; ++current_target)
    {
      if (!C_real_validate_target(instance->targets[current_target].player,
                                  instance->targets[current_target].card,
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
                                  required_subtype,
                                  ~SUB_WALL,
                                  -1,
                                  -1,
                                  0,
                                  0,
                                  0))
      {
        ++invalid_targets;
      }
      else
      {
        kill_card(instance->targets[current_target].player,
                  instance->targets[current_target].card,
                  KILL_DESTROY);
      }
    }

    if (instance->number_of_targets == invalid_targets)
    {
      spell_fizzled = 1;
    }

    if (spell_fizzled != 1 && (damage_effect_card = add_card_to_hand(player, unk_00939348)) != -1)
    {
      PLAYER_CARD_INSTANCE(player, damage_effect_card).original_internal_card_id = instance->internal_card_id;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).state |= 2;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).eot_toughness = 0x109;
      PLAYER_CARD_INSTANCE(player, damage_effect_card).info_slot = instance->number_of_targets - invalid_targets;
      TENTATIVE_set_timestamps(player, damage_effect_card);
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0040a9ed
// FUNCTION: SHANDALAR 0x0040111d
int card_darkpact(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int target_player;
  int index;
  int new_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    load_text("promptsX1.txt", "DARKPACT");
    if (!C_real_select_target(player, 2, 2, TARGET_ZONE_PLAYERS, TYPE_NONE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              text_lines[0], 1, &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    target_player = instance->targets[0].player;
    if (global_library[player][0] != -1)
    {
      instance->info_slot = global_library[player][0];
      remove_card_from_deck(player, 0);
      for (index = 0; index < 16 && global_ante_cards[target_player][index] != -1; ++index)
      {
        real_put_on_top_of_deck(player, global_ante_cards[target_player][index]);
        global_ante_cards[target_player][index] = -1;
      }
      global_ante_cards[target_player][0] = (char)instance->info_slot;
      new_card = add_card_to_hand(player, instance->info_slot);
      instance->info_slot = new_card;
      if (instance->info_slot != -1)
      {
        PLAYER_CARD_INSTANCE(player, instance->info_slot).token_status |= 0x10000;
      }
      load_text("promptsX1.txt", "DARKPACT");
      if (player == human_player)
      {
        do_dialog(player, player, card, player, instance->info_slot, text_lines[1], 0);
      }
      else
      {
        do_dialog(player, player, card, player, instance->info_slot, text_lines[2], 0);
      }
      if (instance->info_slot != -1)
      {
        PLAYER_CARD_INSTANCE(player, instance->info_slot).internal_card_id = -1;
      }
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
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
    FUN_004e4ff3(0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text((int)"promptsX1.txt", "DEMONIC_HORDES");
      if (!FUN_00551b60(player, 2, card))
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->state |= 0x10;
        ai_modifier += instance->targets[0].player == player ? -24 : 36;
      }

      if (spell_fizzled == 1)
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
        spell_fizzled = 1;
      }
      else
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }

      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
    }

    if (event == EVENT_SETUP_UPKEEP_COSTS && card == affected_card && player == affected_card_controller && player == human_player && unk_00742f60 == player)
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
        load_text((int)"promptsX1.txt", "DEMONIC_HORDES_2");
        if (human_player == player || (unk_00926804 & 2) != 0)
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
                               text_lines[0],
                               0,
                               &selected_target);
        }
        else if (FUN_0048194e(1 - player, player, (int *)&selected_target))
        {
          do_dialog(player,
                    player,
                    0,
                    selected_target.player,
                    selected_target.card,
                    text_lines[1],
                    0);
        }

        if (selected_target.player != -1)
        {
          if (unk_008a9000 != 1)
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

// FUNCTION: MAGIC 0x0040aee0
// FUNCTION: SHANDALAR 0x00401611
int card_demonic_attorney(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    FUN_004817fd(player);
    FUN_004817fd(1 - player);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f5d83
// FUNCTION: SHANDALAR 0x004afe3b
int card_fork(int player, int card, event_t event)
{
  card_instance_t *instance;
  int new_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    instance->info_slot = x_value;
    if ((global_cards_data[PLAYER_CARD_INSTANCE(unk_008ce508, unk_008ce4f4).internal_card_id].type & (TYPE_INSTANT | TYPE_INTERRUPT)) == 0 || !C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0,
                                                                                                                                                                      TYPE_NONE, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                                                                                                                                                                      -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0,
                                                                                                                                                                      0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    new_card = add_card_to_hand(player,
                                PLAYER_CARD_INSTANCE(instance->targets[0].player,
                                                     instance->targets[0].card)
                                    .internal_card_id);
    if (new_card != -1)
    {
      *(char *)((char *)&PLAYER_CARD_INSTANCE(player, new_card) + 0x1a) = 0x10;
      PLAYER_CARD_INSTANCE(player, new_card).token_status |= 8;
      x_value = instance->info_slot;
      unk_008b4278 |= 0x400;
      FUN_0051a41c(player, new_card);
      unk_008b4278 &= ~0x400;
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
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
  else if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && card == affected_card && player == affected_card_controller)
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
    FUN_004e4ff3(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (unk_008a9000 != 1)
    {
      load_text((int)"prompts.txt", "GAEAS_LIEGE");
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
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
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
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 3;
      effect_card = create_legacy_effect(card_on_stack_controller,
                                         card_on_stack,
                                         unk_008b49c4,
                                         selected_target.player,
                                         selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(card_on_stack_controller, effect_card).token_status |= 0x11020;
      }
    }

    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
  }
  else if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) && card == affected_card && player == affected_card_controller && (instance->info_slot & 0x4000) != 0)
  {
    instance->info_slot &= 0xffffbfff;
    C_get_abilities(player, card, EVENT_POWER, -1);
    C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043a7ae
// FUNCTION: SHANDALAR 0x004fd152
int card_kudzu(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t new_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 1 - player,
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
                                 ~SUB_WALL,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    load_text("promptsX1.txt", "KUDZU");
    if (!FUN_00551b60(player, 1 - player, card))
    {
      spell_fizzled = 1;
    }
    else if (instance->targets[0].player == player)
    {
      ai_modifier -= 0x18;
    }
    else
    {
      ai_modifier += (basiclandtypes_controlled[1 - player][7] - basiclandtypes_controlled[player][7]) * 0xc;
    }
    return 0;
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
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    return 0;
  }

  if (event == 0x81 && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1 && (instance->state & 0x20) == 0)
  {
    load_text("promptsX1.txt", "KUDZU_2");

    new_target.player = -1;
    new_target.card = -1;

    if (instance->damage_target_player == human_player || (unk_00926804 & 2) != 0)
    {
      if (!C_real_select_target(instance->damage_target_player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
                                TYPE_NONE,
                                0,
                                get_protections_from(0, 0),
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                text_lines[1],
                                0,
                                &new_target))
      {
        new_target.player = -1;
        new_target.card = -1;
      }
    }
    else if (FUN_0048194e(instance->damage_target_player, 1 - instance->damage_target_player, (int *)&new_target))
    {
      do_dialog(0, 0, 0, new_target.player, new_target.card, text_lines[2], 0);
    }

    kill_card(affected_card_controller, affected_card, KILL_DESTROY);

    if (new_target.player != -1)
    {
      instance->damage_target_player = (char)new_target.player;
      instance->damage_target_card = new_target.card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00517dc9
// FUNCTION: SHANDALAR 0x004cf5a7
int card_living_lands(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *effect_instance;
  int effect_card;
  int animated_internal_card_id;
  int current_internal_id;
  int found;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  instance->info_slot = get_hacked_color(player, card, 3);

  if (event == 0x3c && is_in_play(player, card))
  {
    if ((unk_008b4278 & 0x20000) == 0)
    {
      unk_008b4278 |= 0x10000;
    }
    else if (FUN_00484581(event_result, instance->info_slot) != 0 && dispatch_function_to_all_cards_in_play(player, card, FUN_005180ed, player) == -1)
    {
      effect_card = create_legacy_effect(player, card, unk_0093933c, affected_card_controller, affected_card);
      if (effect_card != -1)
      {
        effect_instance = &PLAYER_CARD_INSTANCE(player, effect_card);
        effect_instance->info_slot = instance->info_slot;
        effect_instance->token_status |= 0x10000;

        animated_internal_card_id = unk_0093f4b8;
        found = 0;
        while (animated_internal_card_id < unk_0093f4b8 + 16 && !found)
        {
          if (*(int *)&global_cards_data[animated_internal_card_id].id == *(int *)&global_cards_data[event_result].id)
          {
            found = 1;
          }
          else
          {
            ++animated_internal_card_id;
          }
        }
        if (!found)
        {
          animated_internal_card_id = create_a_card_type(event_result);
          if (animated_internal_card_id != -1)
          {
            current_internal_id = animated_internal_card_id;
            global_cards_data[current_internal_id].type |= TYPE_CREATURE;
            global_cards_data[current_internal_id].extra_ability |= 0x8000;
            global_cards_data[current_internal_id].power = 1;
            global_cards_data[current_internal_id].toughness = 1;
            global_cards_data[current_internal_id].cc[0] = 1;
          }
        }
        if (animated_internal_card_id != -1)
        {
          effect_instance->dummy3 = animated_internal_card_id;
        }
      }
    }
  }

  if (event == 0x77 && card == affected_card && player == affected_card_controller)
  {
    dispatch_function_to_all_cards_in_play(player, card, FUN_0051819c, player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00517a2c
// FUNCTION: SHANDALAR 0x004cf20b
int card_kormus_bell(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *effect_instance;
  int effect_card;
  int animated_internal_card_id;
  int current_internal_id;
  int found;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  instance->info_slot = get_hacked_color(player, card, 1);

  if (event == 0x3c && is_in_play(player, card) && ((instance->state & STATE_TAPPED) == 0 || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0))
  {
    if ((unk_008b4278 & 0x20000) == 0)
    {
      unk_008b4278 |= 0x10000;
    }
    else if (FUN_00484581(event_result, instance->info_slot) != 0 && dispatch_function_to_all_cards_in_play(player, card, FUN_005180ed, player) == -1)
    {
      effect_card = create_legacy_effect(player, card, unk_0093933c, affected_card_controller, affected_card);
      if (effect_card != -1)
      {
        effect_instance = &PLAYER_CARD_INSTANCE(player, effect_card);
        effect_instance->info_slot = instance->info_slot;
        effect_instance->token_status |= 0x10000;

        animated_internal_card_id = unk_0093f4b8;
        found = 0;
        while (animated_internal_card_id < unk_0093f4b8 + 16 && !found)
        {
          if (*(int *)&global_cards_data[animated_internal_card_id].id == *(int *)&global_cards_data[event_result].id)
          {
            found = 1;
          }
          else
          {
            ++animated_internal_card_id;
          }
        }
        if (!found)
        {
          animated_internal_card_id = create_a_card_type(event_result);
          if (animated_internal_card_id != -1)
          {
            current_internal_id = animated_internal_card_id;
            global_cards_data[current_internal_id].type |= TYPE_CREATURE;
            global_cards_data[current_internal_id].extra_ability |= 0x8000;
            global_cards_data[current_internal_id].power = 1;
            global_cards_data[current_internal_id].toughness = 1;
            global_cards_data[current_internal_id].cc[0] = 1;
          }
        }
        if (animated_internal_card_id != -1)
        {
          effect_instance->dummy3 = animated_internal_card_id;
        }
      }
    }
  }

  if (event == 0x77 && card == affected_card && player == affected_card_controller)
  {
    dispatch_function_to_all_cards_in_play(player, card, FUN_0051819c, player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac114
// FUNCTION: SHANDALAR 0x004a3988
int card_natural_selection(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int available[10];
  int selected[3];
  int top_three[3];
  int count;
  int choice_count;
  int index;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION");
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
      if (player == active_player)
      {
        ai_modifier += 0x18;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION_2");
    count = 0;
    for (index = 0; index < 3; ++index)
    {
      top_three[index] = -1;
      available[index] = 0;
      if (global_library[instance->targets[0].player][index] != -1)
      {
        top_three[index] = global_library[instance->targets[0].player][index];
        available[index] = 1;
        ++count;
      }
    }

    if ((player == human_player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1)
    {
      if (count > 0)
      {
        choice_count =
            FUN_004a62d7(player, top_three, available, count, 0x89684c, 1, selected, 0, count);
      }
      else
      {
        choice_count = 0;
      }
    }
    else if (internal_rand(2) == 0)
    {
      choice_count = 0;
    }
    else
    {
      choice_count = 0;
      for (index = 0; index < count; ++index)
      {
        selected[index] = index;
        ++choice_count;
      }
    }

    if (choice_count < count)
    {
      FUN_004b59b2(player, instance->targets[0].player);
    }
    else
    {
      for (index = 0; index < choice_count; ++index)
      {
        global_library[instance->targets[0].player][index] = top_three[selected[index]];
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
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
    if (unk_008a9000 != 1)
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

  if (event == EVENT_RESOLVE_ACTIVATION && (player == human_player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1 && unk_009252e0 == 0)
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
      if (unk_008a9000 != 1)
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

// FUNCTION: MAGIC 0x0043af43
// FUNCTION: SHANDALAR 0x004fd8eb
int card_lich(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage_card;
  int current_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (is_in_play(player, card) && life[player] != -99)
  {
    life[player] = 0;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & (TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND)) != 0)
      {
        ++life[player];
      }
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  else
  {
    if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller && active_player == player)
    {
      ai_modifier -= life[player] - 5;
    }

    if (event == EVENT_RESOLVE_SPELL)
    {
      unk_008b44d0[player] = 1;
    }

    if (event == EVENT_DEAL_DAMAGE && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id == unk_009266a4 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == -1 && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).info_slot != 0)
    {
      instance->targets[instance->info_slot].player = affected_card_controller;
      instance->targets[instance->info_slot].card = affected_card;
      ++instance->info_slot;
    }

    if (trigger_condition == TRIGGER_DEAL_DAMAGE && card == affected_card && player == affected_card_controller && instance->info_slot != 0 && player == current_turn)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= RESOLVE_TRIGGER_MANDATORY;
      }

      if (event == EVENT_RESOLVE_TRIGGER)
      {
        do
        {
          damage_card = &PLAYER_CARD_INSTANCE(instance->targets[instance->info_slot - 1].player,
                                              instance->targets[instance->info_slot - 1].card);
          FUN_0043b4f3(player, damage_card->info_slot);
          --instance->info_slot;
          if (instance->info_slot == 0)
          {
            break;
          }
        } while (life[player] != -99);
      }
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY && card == affected_card && player == affected_card_controller)
    {
      if (is_in_play(player, card))
      {
        if (unk_008a9000 == 1)
        {
          life[player] = -99;
        }
        else
        {
          FUN_004b4110(player);
        }
      }
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0051b55f
// FUNCTION: SHANDALAR 0x004d2d45
int card_animate_wall(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  unsigned int illegal_abilities;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    illegal_abilities = get_protections_from(player, card);
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 illegal_abilities,
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 ~SUB_WALL,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "ANIMATE_WALL");
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
                              TARGET_SPECIAL_WALL,
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
    }
    return 0;
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
                                TARGET_SPECIAL_WALL,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = (char)instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
    return 0;
  }

  if (event == 0x34 && instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller && affected_card != -1 && (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).token_status & 0x80) == 0)
  {
    PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).token_status |= 0x800;
  }

  if (event == 0x77 && affected_card == card && affected_card_controller == player && instance->damage_target_card != -1)
  {
    PLAYER_CARD_INSTANCE(instance->damage_target_player, instance->damage_target_card).token_status &= ~0x800;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004f8901
// FUNCTION: SHANDALAR 0x004b29c6
int card_fog(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, unk_007894a0, -1, -1);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00534ac2
// FUNCTION: SHANDALAR 0x004ec2be
int card_deathgrip(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    ai_modifier += basiclandtypes_controlled[1 - player][COLOR_GREEN] * 5 + 0x18;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    if ((unk_008b4278 & 0x20) != 0 && has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2) != 0)
    {
      if (C_real_validate_target(unk_008ce508,
                                 unk_008ce4f4,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 0,
                                 TYPE_NONE,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 1 << get_sleighted_color(player, card, COLOR_GREEN),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_SPELL_ON_STACK,
                                 0,
                                 0))
      {
        return 99;
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2) != 0 && unk_008ce508 != -1)
  {
    charge_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2);
    if (spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = unk_008ce508;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = unk_008ce4f4;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                0,
                                TYPE_NONE,
                                TYPE_NONE,
                                0,
                                0,
                                1 << get_sleighted_color(player, card, COLOR_GREEN),
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                TARGET_SPECIAL_SPELL_ON_STACK,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                KILL_BURY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004fcc3f
// FUNCTION: SHANDALAR 0x004b6d67
int card_power_sink(int player, int card, event_t event)
{
  card_instance_t *instance;
  int target_player;
  int target_card;
  int mana_paid;
  int color;
  int current_card;
  card_instance_t *target_instance;
  int internal_card_id;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    FUN_004e4ff3(0);
    if (unk_008ce508 == -1)
    {
      return 0;
    }
    if (player == active_player && (unk_00926804 & 2) == 0 && has_mana(player, COLOR_COLORLESS, 2) == 0)
    {
      return 0;
    }
    if (!C_real_validate_target(unk_008ce508,
                                unk_008ce4f4,
                                (char *)0,
                                player,
                                2,
                                2,
                                0,
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
                                TARGET_SPECIAL_SPELL_ON_STACK,
                                0,
                                0))
    {
      return 0;
    }
    return 99;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (unk_008ce508 == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = unk_008ce508;
      instance->targets[0].card = unk_008ce4f4;
      instance->number_of_targets = 1;
      instance->info_slot = x_value;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                0,
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
                                TARGET_SPECIAL_SPELL_ON_STACK,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      target_player = instance->targets[0].player;
      target_card = instance->targets[0].card;

      if (target_player == human_player || (unk_00926804 & 2) != 0)
      {
        FUN_00443ee2(player, card, 0x7e, 0, 0);
        mana_paid = charge_mana(target_player, 0, instance->info_slot);
        obliterate_top_card_of_stack();
        spell_fizzled = 0;
      }
      else if (has_mana(target_player, COLOR_COLORLESS, 1) == 0)
      {
        mana_paid = 0;
      }
      else
      {
        mana_paid = charge_mana(target_player, 0, instance->info_slot);
      }

      if (mana_paid < instance->info_slot)
      {
        for (color = 0; color < 7 && mana_paid < instance->info_slot; ++color)
        {
          while (raw_mana_available[target_player][color] > 0 && mana_paid < instance->info_slot)
          {
            --raw_mana_available[target_player][color];
            --total_raw_mana_available[target_player];
            if (color != 6)
            {
              ++mana_paid;
            }
          }
        }

        unk_008ce510[6] = instance->info_slot;
        for (current_card = 0; current_card < active_cards_count[target_player] && mana_paid < instance->info_slot;
             ++current_card)
        {
          if (is_in_play(target_player, current_card))
          {
            target_instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
            internal_card_id = target_instance->internal_card_id;
            if ((global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0 && (target_instance->state & STATE_TAPPED) == 0 && (((target_instance->state & 3) == 0) || (global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0))
            {
              drain_power_draw_mana_from_land(target_player, current_card, internal_card_id);
              for (color = 0; color < 7 && mana_paid < instance->info_slot; ++color)
              {
                while (raw_mana_available[target_player][color] > 0 && mana_paid < instance->info_slot)
                {
                  --raw_mana_available[target_player][color];
                  --total_raw_mana_available[target_player];
                  if (color != 6)
                  {
                    ++mana_paid;
                  }
                }
              }
            }
          }
        }
        unk_008ce510[6] = 0;
      }

      if (mana_paid < instance->info_slot)
      {
        kill_card(target_player, target_card, KILL_BURY);
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005194e4
// FUNCTION: SHANDALAR 0x004d0cc6
int card_gloom(int player, int card, event_t event)
{
  int chosen_color;

  if (event == 0x7f && affected_card == card && affected_card_controller == player)
  {
    chosen_color = get_sleighted_color(player, card, 5);
    unk_0072c440[chosen_color] += 3;
    return 0;
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_CAST_SPELL || event == 199) && affected_card == card && affected_card_controller == player)
  {
    chosen_color = get_sleighted_color(player, card, 5);
    ai_modifier += ((basiclandtypes_controlled[1 - player][chosen_color] - basiclandtypes_controlled[player][get_sleighted_color(player, card, 5)] * 2) * 3 + 3) * 4;
  }

  return 0;
}
