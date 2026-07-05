#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x00517370
// FUNCTION: SHANDALAR 0x004ceb50
int card_x_cyclopean(int player, int card, event_t event)
{
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

        animated_internal_card_id = g_card_count;
        found = 0;
        while (animated_internal_card_id < g_card_count + 16 && !found)
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

        animated_internal_card_id = g_card_count;
        found = 0;
        while (animated_internal_card_id < g_card_count + 16 && !found)
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

// FUNCTION: MAGIC 0x005180ed
int FUN_005180ed(int a1, int a2, int player, int card, int internal_card_id)
{
  int result; // ebp-0x4

  result = 0;
  if (internal_card_id == unk_0093933c)
  {
    if ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
    {
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
      {
        result = 1;
        event_result = result;
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0051819c
int FUN_0051819c(int parent_player, int parent_card, int player, int card, int internal_card_id)
{
  if (internal_card_id == unk_0093933c && PLAYER_CARD_INSTANCE(player, card).damage_source_player == parent_player && PLAYER_CARD_INSTANCE(player, card).damage_source_card == parent_card)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051823d
// FUNCTION: SHANDALAR 0x004cfa1d
int card_sylvan_library(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00518959
// FUNCTION: SHANDALAR 0x004d0138
int card_land_tax(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00519218
// FUNCTION: SHANDALAR 0x004d09fa
int card_kismet(int player, int card, event_t event)
{
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

// FUNCTION: MAGIC 0x0051a41c
int FUN_0051a41c(int player, int card)
{
  int internal_card_id;
  int card_type;

  if (player == -1 || card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  card_type = (unsigned char)global_cards_data[internal_card_id].type;

  /* Special-case a couple of card ids: if they can't be cast, immediately bury them. */
  if ((global_cards_data[internal_card_id].id == 0x107 || global_cards_data[internal_card_id].id == 0x27) &&
      dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) == 0)
  {
    kill_card(player, card, 2);
    return 0;
  }

  if ((card_type & 2) != 0)
  {
    creature_cards_in_play[player] += 1;
  }
  if ((card_type & 0x40) != 0)
  {
    artifact_cards_in_play[player] += 1;
  }
  if ((card_type & 4) != 0)
  {
    enchantments_in_play[player] += 1;
  }

  card_types_in_play[player] |= (unsigned int)card_type;

  PLAYER_CARD_INSTANCE(player, card).state |= 0x30022;
  dispatch_event(player, card, 0x6c);

  PLAYER_CARD_INSTANCE(player, card).state |= ((unsigned int)player >= 1u ? 0x400000 : 0) | 0x80;
  dispatch_event_to_single_card(player, card, 0x71, 1 - player, -1);

  PLAYER_CARD_INSTANCE(player, card).state &= ~0x20;

  trigger_cause_controller = player;
  trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_COMES_INTO_PLAY, gs_card_into_play_0091c840, 0);

  if ((card_type & 1) != 0)
  {
    unk_008cfdb0 += 1;
  }

  return 0;
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "ANIMATE_ARTIFACT");
    }
    if (!C_real_select_target(player,
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
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      ai_modifier += -0x18;
    }
    else
    {
      spell_fizzled = 1;
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
        PLAYER_CARD_INSTANCE(player, card).dummy3 = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .regen_status |= 0x1000000;
      }
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
    event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .token_status |= 0x40;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051ae64
// FUNCTION: SHANDALAR 0x004d264b
int card_titania_s_song(int player, int card, event_t event)
{
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

// FUNCTION: MAGIC 0x0051bcf0
int FUN_0051bcf0(int player, int card, event_t event, unsigned int required_type)
{
  struct
  {
    target_t target;
    int new_card;
  } s;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, required_type, 0, 0,
                                 get_protections_from(player, card), 0, 0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                             ~SUB_WALL, -1, -1, 0, 0, 0, text_lines[0], 1, &s.target))
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = s.target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player, 2, 2, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                               ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if (FUN_004823a5(player, card))
      {
        PLAYER_CARD_INSTANCE(player, card).token_status |= 0x1000000;
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player != player)
        {
          if (unk_008a9000 != 1)
          {
            play_sound_effect(WAV_CONTROL);
          }
          s.new_card = gain_control(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                    PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          PLAYER_CARD_INSTANCE(player, card).damage_target_player = player;
          PLAYER_CARD_INSTANCE(player, card).damage_target_card = s.new_card;
        }
      }
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (trigger_condition == (trigger_t)0xd4 && affected_card == card && affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card).internal_card_id != -1 &&
      trigger_cause_controller == player && trigger_cause == card && current_turn == player)
  {
    if (event == (event_t)0x7d)
    {
      event_result |= 2;
    }

    if (event == (event_t)0x7e)
    {
      if ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x1000000) != 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).damage_source_player == -1)
        {
          if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                   PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                      .internal_card_id != -1 &&
              ((((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                       PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                      .state &
                  0x400000) != 0) &&
                PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec) ||
               (((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                       PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                      .state &
                  0x400000) == 0) &&
                PLAYER_CARD_INSTANCE(player, card).damage_target_player == active_player)))
          {
            gain_control((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          }
        }
        else
        {
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card)
              .token_status |= 0x1000000;
          if (PLAYER_CARD_INSTANCE(player, card).damage_source_player != PLAYER_CARD_INSTANCE(player, card).damage_target_player)
          {
            gain_control((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
          }
        }
      }
      else
      {
        FUN_0055117d(FUN_0051c73d, -1);
      }
    }
  }

  return 0;
}
// FUNCTION: MAGIC 0x0051c73d
int FUN_0051c73d(int player, int card, int internal_card_id)
{
  if (((global_cards_data[internal_card_id].id == CARD_ID_CONTROL_MAGIC) ||
       (global_cards_data[internal_card_id].id == CARD_ID_STEAL_ARTIFACT) ||
       (internal_card_id == unk_008d0340)) &&
      ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player == affected_card_controller) &&
      (PLAYER_CARD_INSTANCE(player, card).damage_source_card == affected_card))
  {
    PLAYER_CARD_INSTANCE(player, card).damage_source_player =
        PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_player;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card =
        PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_source_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051c8c0
int gain_control(int player, int card)
{
  struct
  {
    int target_index;   /* [ebp-0x18] */
    int test_player;    /* [ebp-0x14] */
    int test_card;      /* [ebp-0x10] */
    int timestamp_slot; /* [ebp-0x0c] */
    int new_card;       /* [ebp-0x08] */
    int opponent;       /* [ebp-0x04] */
  } s;

  s.opponent = 1 - player;
  s.timestamp_slot = PLAYER_CARD_INSTANCE(player, card).timestamp;
  s.new_card = add_card_to_hand(s.opponent, PLAYER_CARD_INSTANCE(player, card).internal_card_id);
  if (s.new_card != -1)
  {
    memcpy(&PLAYER_CARD_INSTANCE(s.opponent, s.new_card), &PLAYER_CARD_INSTANCE(player, card), 0x12c);
    PLAYER_CARD_INSTANCE(s.opponent, s.new_card).state |= 0x30000;
    PLAYER_CARD_INSTANCE(s.opponent, s.new_card).state &= 0xfffffff3;
    TENTATIVE_timestamp_player[s.timestamp_slot] = s.opponent;
    TENTATIVE_timestamp_card[s.timestamp_slot] = s.new_card;

    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card)
        {
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player = (char)s.opponent;
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card = s.new_card;
        }
        if ((int)(char)PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player == player &&
            PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card == card)
        {
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_player = (char)s.opponent;
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_source_card = s.new_card;
        }
        if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets != 0)
        {
          for (s.target_index = 0; s.target_index < PLAYER_CARD_INSTANCE(s.test_player, s.test_card).number_of_targets; ++s.target_index)
          {
            if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player == player &&
                PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card == card)
            {
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].player = s.opponent;
              PLAYER_CARD_INSTANCE(s.test_player, s.test_card).targets[s.target_index].card = s.new_card;
            }
          }
        }
      }
    }
  }

  PLAYER_CARD_INSTANCE(player, card).token_status |= 8;
  kill_card(player, card, KILL_REMOVE);
  if ((unk_007abc78 & 0x10000000) != 0)
  {
    dispatch_event(s.opponent, s.new_card, EVENT_CARDCONTROLLED);
  }
  return s.new_card;
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
      load_text("prompts.txt", "FEEDBACK");
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

// FUNCTION: MAGIC 0x0051df5d
// FUNCTION: SHANDALAR 0x004d5741
int card_brainwash(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0051e631
int FUN_0051e631(int player, int card, int internal_card_id)
{
  if ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) &&
      (PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && (affected_card != -1))
  {
    global_cards_data[internal_card_id].code_pointer(player, card, 0x79);
    if (spell_fizzled == 1)
    {
      event_result += 1;
      spell_fizzled = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051e6fc
// FUNCTION: SHANDALAR 0x004d5ee2
int card_spirit_shackle(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0051ee47
// FUNCTION: SHANDALAR 0x004d662b
int card_relic_bind(int player, int card, event_t event)
{
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
      load_text("prompts.txt", "POWERLEAK");
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

    load_text("prompts.txt", "POWERLEAK");
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

// FUNCTION: MAGIC 0x0051fe40
// FUNCTION: SHANDALAR 0x004d7623
int card_energy_flux(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00520013
// FUNCTION: SHANDALAR 0x004d77f8
int card_erosion(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00520ab6
// FUNCTION: SHANDALAR 0x004d829b
int card_cursed_land(int player, int card, event_t event)
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
      load_text("prompts.txt", "CURSED_LAND");
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
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec)
    {
      ai_modifier += MAX(0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * 0x18;
    }
    else
    {
      ai_modifier += MAX(0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player], 1) * -0x18;
    }
  }
  return 0;
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
      load_text("prompts.txt", "EVIL_PRESENCE");
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

// FUNCTION: MAGIC 0x005223bf
// FUNCTION: SHANDALAR 0x004d9bac
int card_blight(int player, int card, event_t event)
{
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
      load_text("prompts.txt", "PSYCHIC_VENOM");
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
  } s;

  if ((event == EVENT_COUNT_MANA) && (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id].type & TYPE_LAND))
  {
    if ((PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).state & STATE_TAPPED) == 0)
    {
      s.land_color = *(char *)((char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card) + 0x58);
      s.color_index = 0;
      s.color_count = 0;
      for (; s.color_index < 7; ++s.color_index)
      {
        if ((s.land_color & (1 << (unsigned char)s.color_index)) != 0)
        {
          ++s.color_count;
        }
      }

      if (s.color_count > 0)
      {
        declare_mana_available_hex(affected_card_controller, s.land_color, 1);
      }
      else
      {
        declare_mana_available(affected_card_controller, single_color_test_bit_to_color_t(s.land_color), 1);
      }
    }
    return 0;
  }

  if ((event == EVENT_TAP_CARD) && is_in_play(player, card))
  {
    if (PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id != -1)
    {
      s.current_internal_id = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).internal_card_id;
    }
    else
    {
      s.current_internal_id = PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).original_internal_card_id;
    }

    if ((global_cards_data[s.current_internal_id].type & TYPE_LAND) != 0)
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

// FUNCTION: MAGIC 0x00523399
// FUNCTION: SHANDALAR 0x004dab86
int card_lifetap(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if ((event == EVENT_TAP_CARD) && (affected_card_controller != player))
  {
    if (FUN_0048463d(affected_card_controller, affected_card, get_hacked_color(player, card, COLOR_GREEN)) != 0)
    {
      gain_life(player, 1, player, card);
      return 0;
    }
  }
  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card)) && (affected_card_controller == player))
  {
    ai_modifier += ((basiclandtypes_controlled[unk_008b35ec][get_hacked_color(player, card, COLOR_GREEN)] + 1) * 3) * 8;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0052348b
// FUNCTION: SHANDALAR 0x004dac78
int card_fortified_area(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005235d1
// FUNCTION: SHANDALAR 0x004dadbe
int card_sunken_city(int player, int card, event_t event)
{
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

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    ai_modifier += creature_cards_in_play[player] * 0xc;
  }
  return 0;
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

// FUNCTION: MAGIC 0x00523a60
// FUNCTION: SHANDALAR 0x004db252
int card_aspect_of_wolf(int player, int card, event_t event)
{
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "ASPECTOFWOLF");
    }
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      spell_fizzled = 0;
    }

    if ((spell_fizzled != 1) && (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec))
    {
      ai_modifier += -0x18;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
  {
    event_result += basiclandtypes_controlled[player][get_hacked_color(player, card, 3)] / 2;
  }
  if (((event == EVENT_TOUGHNESS) && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller) && ((affected_card != -1) && is_in_play(player, card))))
  {
    event_result += (basiclandtypes_controlled[player][get_hacked_color(player, card, 3)] + 1) / 2;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00523ef2
// FUNCTION: SHANDALAR 0x004db6e2
int card_lure(int player, int card, event_t event)
{
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
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (trigger_condition == 0xda && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    trigger_condition = -1;
    if ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player
        && (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                .state &
            4) != 0
        && is_in_play(affected_card_controller, affected_card)
        && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).blocking == -1
        && affected_card_controller != human_player
        && FUN_0052460c(affected_card_controller,
                        affected_card,
                        (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                        PLAYER_CARD_INSTANCE(player, card).damage_target_card) != 0)
    {
      if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
              .blocking == -1)
      {
        blocking_card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
      }
      else
      {
        blocking_card = PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                            .blocking;
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
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052460c
int FUN_0052460c(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  struct
  {
    int result;             /* [ebp-0x14] */
    int attacker_abilities; /* [ebp-0x10] */
    unsigned int selected;  /* [ebp-0x0c] */
    unsigned int tmp2;      /* [ebp-0x08] */
    unsigned int tmp1;      /* [ebp-0x04] */
  } s;

  s.attacker_abilities = C_get_abilities(attacker_player, attacker_card, 0x34, -1);
  FUN_004e698f(&s.tmp1, &s.tmp2);

  if (blocker_player == 1)
  {
    s.selected = s.tmp1;
  }
  else
  {
    s.selected = s.tmp2;
  }

  s.result = FUN_004418c6(blocker_player,
                          blocker_card,
                          attacker_player,
                          attacker_card,
                          (unsigned int)s.attacker_abilities,
                          s.selected);
  return s.result;
}

// FUNCTION: MAGIC 0x00524687
// FUNCTION: SHANDALAR 0x004dbe77
int card_spirit_link(int player, int card, event_t event)
{
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "CREATUREBOND");
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

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

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

// FUNCTION: MAGIC 0x005252e0
// FUNCTION: SHANDALAR 0x004dcace
int card_gaseous_form(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005258c6
// FUNCTION: SHANDALAR 0x004dd0b4
int card_backfire(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00526202
// FUNCTION: SHANDALAR 0x004dd9ef
int card_holy_armor(int player, int card, event_t event)
{
  int local;

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
      load_text("prompts.txt", "HOLY_ARMOR");
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
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == human_player)
    {
      local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
      if (unk_0072c440[local] == 0)
      {
        FUN_004e503e(0);
      }
    }

    unk_00715fa8 = 1;
    unk_00925bb8 = ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) |
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      if (player == human_player)
      {
        local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
        if (unk_0072c440[local] != 0)
        {
          x_value = charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, -1);
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
        if (unk_0072c440[local] != 0)
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, 1);
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
        local = create_legacy_effect(card_on_stack_controller,
                                     card_on_stack,
                                     unk_008a8de8,
                                     (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          *(short *)&PLAYER_CARD_INSTANCE(player, local).initial_color =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
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

// FUNCTION: MAGIC 0x0052700b
// FUNCTION: SHANDALAR 0x004de7f8
int card_blessing(int player, int card, event_t event)
{
  int local;

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
  else if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    ai_modifier += 0x18;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "BLESSING");
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
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    if (player == human_player)
    {
      local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
      if (unk_0072c440[local] == 0)
      {
        FUN_004e503e(0);
      }
    }

    unk_00715fa8 = 1;
    unk_00925bb8 = ((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player << 8) |
                   PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1) != 0)
    {
      if (player == human_player)
      {
        local = single_color_test_bit_to_color_t((int)(unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
        if (unk_0072c440[local] != 0)
        {
          x_value = charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, -1);
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
        if (unk_0072c440[local] != 0)
        {
          charge_mana_w_global_cost_mod(player, card, COLOR_WHITE, 1);
        }
        else
        {
          charge_mana(player, COLOR_WHITE, 1);
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
        local = create_legacy_effect(card_on_stack_controller,
                                     card_on_stack,
                                     unk_008a8de8,
                                     (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_target_card);
        if (local != -1)
        {
          *(short *)&PLAYER_CARD_INSTANCE(player, local).display_pic_csv_id =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          *(short *)&PLAYER_CARD_INSTANCE(player, local).initial_color =
              (short)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
          PLAYER_CARD_INSTANCE(player, local).info_slot |= 0x80000;
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

// FUNCTION: MAGIC 0x00527e71
// FUNCTION: SHANDALAR 0x004df65e
int card_firebreathing(int player, int card, event_t event)
{
  int local;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++unk_00939530[player][0];
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
  else if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "FIREBREATHING");
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
      load_text("prompts.txt", "FEAR");
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

// FUNCTION: MAGIC 0x00528fbc
// FUNCTION: SHANDALAR 0x004e07ad
int card_seeker(int player, int card, event_t event)
{
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
      load_text("prompts.txt", "WEB");
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

// FUNCTION: MAGIC 0x00529fd5
// FUNCTION: SHANDALAR 0x004e17ca
int card_magnetic_mountain(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052a3fd
// FUNCTION: SHANDALAR 0x004e1bf2
int card_paralyze(int player, int card, event_t event)
{
  struct
  {
    target_t tgt;                  /* ebp-0x94 */
    int unused_08;                 /* ebp-0x8c */
    target_t *tgt_ptr;             /* ebp-0x88 */
    target_definition_t td;        /* ebp-0x84 */
    int target_player;             /* ebp-0x30 */
    int target_card;               /* ebp-0x2c */
    int internal_card_id;          /* ebp-0x28 */
    char unused_70[0x24];          /* ebp-0x24 */
    target_definition_t *td_ptr;   /* ebp-0x04 */
  } s;

  s.td_ptr = &s.td;
  s.tgt_ptr = &s.tgt;

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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "PARALYZE");
    }

    if (FUN_00551638(player, 2, card) != 0)
    {
      s.target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      s.target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(s.target_player, s.target_card).mana_to_untap[0] += 4;

      if (active_player == player && (unk_00926804 & 2) == 0)
      {
        if (s.target_player == player)
        {
          ai_modifier += -0x18;
        }
        else
        {
          s.internal_card_id = PLAYER_CARD_INSTANCE(s.target_player, s.target_card).internal_card_id;

          if ((char)global_cards_data[s.internal_card_id].subtype == 0)
          {
            if ((PLAYER_CARD_INSTANCE(s.target_player, s.target_card).initial_color & 8) == 0)
            {
              ai_modifier += -0x18;
            }

            if (((int(__cdecl *)(int, int, int))FUN_0052adf2)(player, card, s.internal_card_id) != 0)
            {
              ai_modifier += -0x60;
            }
          }
        }
      }
    }
    else
    {
      spell_fizzled = 1;
    }

    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
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
                               0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      FUN_004f7783(PLAYER_CARD_INSTANCE(player, card).damage_target_player, PLAYER_CARD_INSTANCE(player, card).damage_target_card);
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_UNTAP && PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller && affected_card != -1)
  {
    *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                          PLAYER_CARD_INSTANCE(player, card).damage_target_card).dummy3 &= 0xfffffffc;
    return 0;
  }

  if (event == EVENT_SET_UNTAP_COST && PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller && affected_card != -1 &&
      (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                            PLAYER_CARD_INSTANCE(player, card).damage_target_card).state &
       STATE_TAPPED) != 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player && unk_00742f60 == human_player)
  {
    PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).upkeep_flags |= UPKEEP_UNTAP_TRIGGER;
    PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).mana_to_untap[0] += 4;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052adf2
int FUN_0052adf2(int player, int card)
{
  struct
  {
    card_instance_t *test_instance; /* ebp-0x14 */
    int test_player;                /* ebp-0x10 */
    int current_card;               /* ebp-0x0c */
    int max_cards;                  /* ebp-0x08 */
    int found;                      /* ebp-0x04 */
  } s;

  if (active_cards_count[1] <= active_cards_count[0])
  {
    s.max_cards = active_cards_count[0];
  }
  else
  {
    s.max_cards = active_cards_count[1];
  }

  s.found = 0;
  for (s.current_card = 0; s.current_card < s.max_cards; ++s.current_card)
  {
    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      s.test_instance = &global_card_instances[s.test_player][s.current_card];
      if (is_in_play(s.test_player, s.current_card) != 0 &&
          (global_card_instances[player])[card].targets[0].player == (int)(char)s.test_instance->damage_target_player &&
          (global_card_instances[player])[card].targets[0].card == s.test_instance->damage_target_card)
      {
        s.found = 1;
        break;
      }
    }
  }

  return s.found;
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

// FUNCTION: MAGIC 0x0052b3d4
// FUNCTION: SHANDALAR 0x004e2bc8
int card_power_surge(int player, int card, event_t event)
{
  int current_card;
  int damage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == 0x6a)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    for (current_card = 0; current_card < active_cards_count[human_player]; ++current_card)
    {
      if (is_in_play(human_player, current_card) && (PLAYER_CARD_INSTANCE(human_player, current_card).state & STATE_TAPPED) == 0 && (global_cards_data[PLAYER_CARD_INSTANCE(human_player, current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        ++PLAYER_CARD_INSTANCE(player, card).info_slot;
      }
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (current_phase == 4 && (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 && unk_00742f60 == human_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (event == 4 && card == affected_card && player == affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }

  if (event == 0x86)
  {
    damage_player(human_player, PLAYER_CARD_INSTANCE(player, card).info_slot, card_on_stack_controller, card_on_stack);
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    return 0;
  }

  if (event == 0x22)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
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

// FUNCTION: MAGIC 0x0052b842
// FUNCTION: SHANDALAR 0x004e303a
int card_burrowing(int player, int card, event_t event)
{
  int result;

  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text("prompts.txt", "BURROWING");
  }

  result = FUN_0052d7a5(player, card, event, 1 << (get_hacked_color(player, card, 4) - 1U));
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (player == active_player))
  {
    if (((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).regen_status & (1 << (get_hacked_color(player, card, 4) - 1U))) != 0) || (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec))
    {
      ai_modifier += -0x60;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0052b9f0
// FUNCTION: SHANDALAR 0x004e31e9
int card_wanderlust(int player, int card, event_t event)
{
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

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "WANDERLUST");
    }
    if (FUN_00551638(player, 1 - player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      spell_fizzled = 0;
    }
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
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player)) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player)) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  }

  if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card)) && (affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
  }
  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card) && ((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1))
  {
    int life_delta = 0x18 - life[(int)PLAYER_CARD_INSTANCE(player, card).damage_target_player];
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

// FUNCTION: MAGIC 0x0052c052
// FUNCTION: SHANDALAR 0x004e384b
int card_instill_energy(int player, int card, event_t event)
{
  int selected_color;

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

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "INSTILL_ENERGY");
    }

    if (!FUN_00551638(player, player, card))
    {
      spell_fizzled = 1;
    }
    else
    {
      spell_fizzled = 0;
    }

    if (spell_fizzled != 1 && player == active_player)
    {
      if (global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                .internal_card_id]
              .subtype == 0 &&
          (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card)
               .token_status &
           STATUS_WALL_CAN_ATTACK) == 0)
      {
        ai_modifier -= 0x30;
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                  PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                 .internal_card_id]
               .extra_ability &
           1) != 0 &&
          PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        ai_modifier += 0x30;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                PLAYER_CARD_INSTANCE(player, card).damage_target_card)
               .state &
           STATE_IN_PLAY) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .state &= 0xfffcffff;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    selected_color = single_color_test_bit_to_color_t((unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
    if (unk_0072c440[selected_color] == 0 || has_mana_w_global_cost_mod(player, card, 7, 0) != 0)
    {
      if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0 && affected_card_controller == human_player &&
          (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
      {
        return 1;
      }
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    selected_color = single_color_test_bit_to_color_t((unsigned char)PLAYER_CARD_INSTANCE(player, card).color);
    if (unk_0072c440[selected_color] != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 0, 0);
    }

    if (spell_fizzled != 1)
    {
      ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
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
          .state &= ~STATE_TAPPED;
    }
    return 0;
  }

  if (trigger_condition == TRIGGER_LEAVE_PLAY && affected_card == card && affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1 &&
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
              .internal_card_id != -1 &&
      trigger_cause_controller == player && trigger_cause == card && current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .state |= 0x30000;
    }
  }

  if (event == EVENT_CLEANUP && affected_card == card && affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_CARDCONTROLLED && PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller && affected_card != -1)
  {
    if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                              PLAYER_CARD_INSTANCE(player, card).damage_target_card)
             .state &
         STATE_IN_PLAY) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .state &= 0xfffcffff;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052cde5
// FUNCTION: SHANDALAR 0x004e45de
int card_flood(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052d1c1
// FUNCTION: SHANDALAR 0x004e49ba
int card_greed(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052d2d4
// FUNCTION: SHANDALAR 0x004e4ace
int card_pestilence(int player, int card, event_t event)
{
  struct
  {
    int min_toughness;
    int creature_toughness_counts[16];
    int opponent_life_div_swamps;
    int toughness_index;
    int best_score;
    int loop_player;
    int loop_card;
    int max_active;
    int running_score;
    card_instance_t *instance;
    int toughness;
    int swamps;
    int adjust;
    int opponent;
  } s;

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (is_in_play(player, card) != 0)
    {
      s.swamps = basiclandtypes_controlled[player][COLOR_BLACK];
      if (life[player] < s.swamps)
      {
        s.min_toughness = life[player];
      }
      else
      {
        s.min_toughness = s.swamps;
      }

      for (s.toughness_index = 0; s.toughness_index < 16; ++s.toughness_index)
      {
        s.creature_toughness_counts[s.toughness_index] = 0;
      }

      if (active_cards_count[1] < active_cards_count[0])
      {
        s.max_active = active_cards_count[0];
      }
      else
      {
        s.max_active = active_cards_count[1];
      }

      for (s.loop_card = 0; s.loop_card < s.max_active; ++s.loop_card)
      {
        for (s.loop_player = 0; s.loop_player < 2; ++s.loop_player)
        {
          s.instance = &global_card_instances[s.loop_player][s.loop_card];
          if (is_in_play(s.loop_player, s.loop_card) != 0)
          {
            s.toughness = s.instance->toughness;
            if (s.toughness <= s.min_toughness && s.toughness < 16)
            {
              if (s.loop_player == player)
              {
                --s.creature_toughness_counts[s.toughness];
              }
              else
              {
                ++s.creature_toughness_counts[s.toughness];
              }
            }
          }
        }
      }

      s.running_score = 0;
      s.best_score = 0;
      for (s.toughness_index = 0; s.toughness_index < 16; ++s.toughness_index)
      {
        s.adjust = s.creature_toughness_counts[s.toughness_index] * (s.toughness_index + 1);
        s.adjust = (s.adjust + s.adjust * 2) << 3;
        s.running_score += s.adjust;
        if (s.best_score < s.running_score)
        {
          s.best_score = s.running_score;
        }
      }

      if (player == unk_008b35ec)
      {
        ai_modifier -= s.best_score;
      }
      else
      {
        ai_modifier += s.best_score;
      }

      s.opponent = 1 - player;
      if (life[s.opponent] < life[player] && s.swamps != 0)
      {
        s.opponent_life_div_swamps = life[s.opponent] / s.swamps;
        s.adjust = 0x18 - s.opponent_life_div_swamps;
        if (s.adjust <= 1)
        {
          s.adjust = 1;
        }
        s.adjust = (s.adjust + s.adjust * 2) << 3;
        if (player == unk_008b35ec)
        {
          ai_modifier -= s.adjust;
        }
        else
        {
          ai_modifier += s.adjust;
        }
      }
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
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
      return 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    damage_player(1 - player, 1, card_on_stack_controller, card_on_stack);
    damage_player(player, 1, card_on_stack_controller, card_on_stack);
    dispatch_function_to_all_cards_in_play(card_on_stack_controller, card_on_stack, FUN_0052d761, -1);
    return 0;
  }

  if (trigger_condition == TRIGGER_EOT && affected_card == card && affected_card_controller == player && current_turn == player)
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER && FUN_005510dc(player, TYPE_CREATURE) == 0 && FUN_005510dc(1 - player, TYPE_CREATURE) == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d761
int FUN_0052d761(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    damage_creature(test_player, test_card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052d7a5
int FUN_0052d7a5(int player, int card, int event, unsigned int trigger_flag)
{
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

  if ((event == EVENT_CAST_SPELL) && (card == affected_card) && (player == affected_card_controller))
  {
    if (FUN_00551638(player, player, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      spell_fizzled = 0;
    }
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
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
                                0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).damage_target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  }
  if (PLAYER_CARD_INSTANCE(player, card).damage_target_card == card_on_stack &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player == card_on_stack_controller &&
      card_on_stack != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
      event == 0x34)
  {
    event_result |= trigger_flag;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052dae0
// FUNCTION: SHANDALAR 0x004e52db
int card_holy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text("prompts.txt", "HOLY_STRENGTH");
  }

  return FUN_0052dd74(player, card, event, 1, 2);
}

// FUNCTION: MAGIC 0x0052db4f
// FUNCTION: SHANDALAR 0x004e534a
int card_giant_strength(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052dbbc
// FUNCTION: SHANDALAR 0x004e53b9
int card_immolation(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052dc2b
// FUNCTION: SHANDALAR 0x004e5426
int card_divine_transformation(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0052dc98
// FUNCTION: SHANDALAR 0x004e5495
int card_unholy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text("prompts.txt", "UNHOLY_STRENGTH");
  }
  return FUN_0052dd74(player, card, event, 2, 1);
}

// FUNCTION: MAGIC 0x0052dd05
// FUNCTION: SHANDALAR 0x004e5502
int card_weakness(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) && (unk_008a9000 != 1))
  {
    load_text("prompts.txt", "WEAKNESS");
  }
  return FUN_0052dd74(player, card, event, -2, -1);
}

// FUNCTION: MAGIC 0x0052dd74
int FUN_0052dd74(int player, int card, event_t event, int power_modifier, int toughness_modifier)
{
  int preferred_controller;

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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (power_modifier + toughness_modifier < 0)
    {
      preferred_controller = 1 - player;
    }
    else
    {
      preferred_controller = player;
    }

    if (FUN_00551638(player, preferred_controller, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier += -0x18;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(global_card_instances[player][card].targets[0].player,
                               global_card_instances[player][card].targets[0].card,
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
      global_card_instances[player][card].damage_target_player =
          (char)global_card_instances[player][card].targets[0].player;
      global_card_instances[player][card].damage_target_card =
          global_card_instances[player][card].targets[0].card;
    }
    global_card_instances[player][card].number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER) && (global_card_instances[player][card].damage_target_card == affected_card)) && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller) && (affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      event_result += power_modifier;
    }
  }
  if (((event == EVENT_TOUGHNESS) && (global_card_instances[player][card].damage_target_card == affected_card)) && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller) && (affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      event_result += toughness_modifier;
    }
  }

  return 0;
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
        load_text("prompts.txt", "ANY_WARD");
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

// FUNCTION: MAGIC 0x0052ed45
// FUNCTION: SHANDALAR 0x004e6541
int card_unstable_mutation(int player, int card, event_t event)
{
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
      load_text("prompts.txt", "WARP_ARTIFACT");
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

// FUNCTION: MAGIC 0x005308b5
// FUNCTION: SHANDALAR 0x004e80b1
int card_power_struggle(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00530f1a
// FUNCTION: SHANDALAR 0x004e8714
int card_necropolis_of_azar(int player, int card, event_t event)
{
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

// FUNCTION: MAGIC 0x00531d34
// FUNCTION: SHANDALAR 0x004e9531
int card_eternal_warrior(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x005322d7
// FUNCTION: SHANDALAR 0x004e9ad4
int card_the_brute(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00532ad7
// FUNCTION: SHANDALAR 0x004ea2d4
int card_co_p_black(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLACK));
}

// FUNCTION: MAGIC 0x00532b0e
// FUNCTION: SHANDALAR 0x004ea30b
int card_co_p_white(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_WHITE));
}

// FUNCTION: MAGIC 0x00532b45
// FUNCTION: SHANDALAR 0x004ea342
int card_co_p_red(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_RED));
}

// FUNCTION: MAGIC 0x00532b7c
// FUNCTION: SHANDALAR 0x004ea379
int card_co_p_blue(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_BLUE));
}

// FUNCTION: MAGIC 0x00532bb3
// FUNCTION: SHANDALAR 0x004ea3b0
int card_co_p_green(int player, int card, event_t event)
{
  return helper_circle_of_protection(player, card, event, get_sleighted_color(player, card, COLOR_GREEN));
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

// FUNCTION: MAGIC 0x005330c6
// FUNCTION: SHANDALAR 0x004ea8c1
int card_co_p_artifacts(int player, int card, event_t event)
{
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "PHANTASMAL_TERRAIN");
    }
    if (!FUN_00551b60(player, 1 - player, card) == 0)
    {
      if ((player == unk_008b35ec) || ((unk_00926804 & 2) != 0))
      {
        if (unk_008a9000 != 1)
        {
          load_text("prompts.txt", "PHANTASMAL_TERRAIN");
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
        unk_00939340 = internal_rand(5) + 1;
        land_type = unk_00939340;
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
        land_type = unk_00939340;
      }

      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player)
      {
        ai_modifier += -0x30;
      }
      PLAYER_CARD_INSTANCE(player, card).dummy3 = land_type;
    }
    else
    {
      spell_fizzled = 1;
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
      --PLAYER_CARD_INSTANCE(player, card).dummy3;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3;
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
    event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }
  return 0;
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
      load_text("prompts.txt", "WILD_GROWTH");
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (unk_008a9000 != 1)
    {
      load_text("prompts.txt", "FLIGHT");
    }
    if (FUN_00551638(player, player, card))
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
    else
    {
      spell_fizzled = 1;
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    else
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  }

  if (((PLAYER_CARD_INSTANCE(player, card).info_slot != 0) && (PLAYER_CARD_INSTANCE(player, card).targets[0].card == affected_card)) && (((int)PLAYER_CARD_INSTANCE(player, card).targets[0].player == affected_card_controller) && ((affected_card != -1) && (event == EVENT_ABILITIES))))
  {
    event_result |= KEYWORD_FLYING;
  }
  return 0;
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
    if (unk_008ce508 != -1)
    {
      return ((unk_008b4278 & 0x20) != 0 &&
              has_mana_w_global_cost_mod(player, card, COLOR_BLACK, 2) != 0 &&
              C_real_validate_target(unk_008ce508, unk_008ce4f4, (char *)0, player, 2, 2, 0, TYPE_NONE, TYPE_NONE, 0, 0, 1 << get_sleighted_color(player, card, COLOR_GREEN), COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0))
                 ? 99
                 : 0;
    }
    else
    {
      return 0;
    }
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
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
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
      kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                KILL_BURY);
    }
    else
    {
      spell_fizzled = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00534ddb
int FUN_00534ddb(int player, int mode)
{
  struct
  {
    int card_0x40;                  /* ebp-0x5a4 */
    int score_0x40;                 /* ebp-0x5a0 */
    int abilities_0x40;             /* ebp-0x59c */
    int internal_id_0x40;           /* ebp-0x598 */
    int candidate_toughnesses2[50]; /* ebp-0x594 */
    int count2;                     /* ebp-0x4cc */
    int candidate_powers2[50];      /* ebp-0x4c8 */
    int candidate_scores2[50];      /* ebp-0x400 */
    int candidate_abilities2[50];   /* ebp-0x338 */
    int best_toughness2;            /* ebp-0x270 */
    int best_power2;                /* ebp-0x26c */
    int candidate_cards2[50];       /* ebp-0x268 */
    int count1;                     /* ebp-0x1a0 */
    int candidate_scores1[50];      /* ebp-0x19c */
    int candidate_cards1[50];       /* ebp-0xd4 */
    int current_card;               /* ebp-0x0c */
    int best_score;                 /* ebp-0x08 */
    int best_card;                  /* ebp-0x04 */
  } s;

  if (player == -1)
  {
    return -1;
  }

  if (mode == 1)
  {
    s.best_card = -1;
    s.best_score = -10;
    s.count1 = 0;
    for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
    {
      if (is_in_play(player, s.current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id].type & TYPE_LAND) &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_TAPPED))
      {
        s.candidate_cards1[s.count1] = s.current_card;
        s.candidate_scores1[s.count1] = 0;
        ++s.count1;
      }
    }

    for (s.current_card = 0; s.current_card < s.count1; ++s.current_card)
    {
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards1[s.current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++s.candidate_scores1[s.current_card];
      }
      if ((PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_NO_AUTO_TAPPING) != 0)
      {
        s.candidate_scores1[s.current_card] = -1;
      }
    }

    for (s.current_card = 0; s.current_card < s.count1; ++s.current_card)
    {
      if (s.best_score < s.candidate_scores1[s.current_card])
      {
        s.best_score = s.candidate_scores1[s.current_card];
        s.best_card = s.candidate_cards1[s.current_card];
      }
    }
  }

  else if (mode == 2)
  {
    s.best_card = -1;
    s.best_score = -1;
    s.best_power2 = 0;
    s.best_toughness2 = 0;
    s.count2 = 0;

    for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
    {
      if (is_in_play(player, s.current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id].type & TYPE_CREATURE) &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_TAPPED))
      {
        s.candidate_cards2[s.count2] = s.current_card;
        s.candidate_powers2[s.count2] = C_get_abilities(player, s.current_card, EVENT_POWER, -1);
        if (s.best_power2 < s.candidate_powers2[s.count2])
        {
          s.best_power2 = s.candidate_powers2[s.count2];
        }
        s.candidate_toughnesses2[s.count2] = C_get_abilities(player, s.current_card, EVENT_TOUGHNESS, -1);
        if (s.best_toughness2 < s.candidate_toughnesses2[s.count2])
        {
          s.best_toughness2 = s.candidate_toughnesses2[s.count2];
        }
        s.candidate_abilities2[s.count2] = C_get_abilities(player, s.current_card, EVENT_ABILITIES, -1);
        s.candidate_scores2[s.count2] = 0;
        ++s.count2;
      }
    }

    for (s.current_card = 0; s.current_card < s.count2; ++s.current_card)
    {
      if (s.candidate_powers2[s.current_card] == s.best_power2)
      {
        s.candidate_scores2[s.current_card] += 3;
      }
      if (s.candidate_toughnesses2[s.current_card] == s.best_toughness2)
      {
        s.candidate_scores2[s.current_card] += 2;
      }
      if ((s.candidate_abilities2[s.current_card] & 0x20) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      if ((s.candidate_abilities2[s.current_card] & 0x100) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      while (s.candidate_abilities2[s.current_card] != 0)
      {
        if ((s.candidate_abilities2[s.current_card] & 1) != 0)
        {
          ++s.candidate_scores2[s.current_card];
        }
        s.candidate_abilities2[s.current_card] >>= 1;
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards2[s.current_card]).internal_card_id].extra_ability & 0x1000) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, s.candidate_cards2[s.current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++s.candidate_scores2[s.current_card];
      }
    }

    for (s.current_card = 0; s.current_card < s.count2; ++s.current_card)
    {
      if (s.best_score < s.candidate_scores2[s.current_card])
      {
        s.best_score = s.candidate_scores2[s.current_card];
        s.best_card = s.candidate_cards2[s.current_card];
      }
    }
  }

  else if (mode == 0x40)
  {
    s.best_score = -1;
    s.best_card = s.best_score;
    for (s.card_0x40 = 0; s.card_0x40 < active_cards_count[player]; ++s.card_0x40)
    {
      if (is_in_play(player, s.card_0x40) &&
          (PLAYER_CARD_INSTANCE(player, s.card_0x40).state & STATE_TAPPED) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, s.card_0x40).internal_card_id].type & TYPE_ENCHANTMENT) &&
          (PLAYER_CARD_INSTANCE(player, s.card_0x40).dummy3 & 1) != 0)
      {
        s.internal_id_0x40 = PLAYER_CARD_INSTANCE(player, s.card_0x40).internal_card_id;
        s.score_0x40 = 0;

        s.score_0x40 += C_get_abilities(player, s.card_0x40, EVENT_POWER, -1) +
                        C_get_abilities(player, s.card_0x40, EVENT_TOUGHNESS, -1);

        s.abilities_0x40 = C_get_abilities(player, s.card_0x40, EVENT_ABILITIES, -1);
        if ((s.abilities_0x40 & 0x20) != 0)
        {
          ++s.score_0x40;
        }
        if ((s.abilities_0x40 & 0x100) != 0)
        {
          ++s.score_0x40;
        }
        while (s.abilities_0x40 != 0)
        {
          if ((s.abilities_0x40 & 1) != 0)
          {
            ++s.score_0x40;
          }
          s.abilities_0x40 >>= 1;
        }
        if ((global_cards_data[s.internal_id_0x40].extra_ability & 0x1000) != 0)
        {
          ++s.score_0x40;
        }
        if ((global_cards_data[s.internal_id_0x40].extra_ability & 1) != 0)
        {
          ++s.score_0x40;
        }
        if ((char)global_cards_data[s.internal_id_0x40].cc[1] != -1)
        {
          s.score_0x40 += (char)global_cards_data[s.internal_id_0x40].cc[1];
        }
        if (s.best_score < s.score_0x40)
        {
          s.best_score = s.score_0x40;
          s.best_card = s.card_0x40;
        }
      }
    }

    /* fallthrough to return */
  }
  else
  {
    s.best_card = -1;
  }

  return s.best_card;
}
