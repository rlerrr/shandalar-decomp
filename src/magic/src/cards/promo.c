#include <stdio.h>
#include <string.h>
#include "../global_duel_ui_ids.h"
#include "../game_support.h"
#include "../global_strings.h"

extern card_ptr_t global_raw_cards_storage[2000];

int aswan_jaguar_paired_monster(int raw_code);
int aswan_jaguar_find_library_monster(int player, int start_slot);
int polka_apply_effect(int player, int card, int amount);
int polka_has_target_available(int player, int card);
int faerie_dragon_apply_effect(int player, int card, int effect_index);
int get_card_display_pic_num(card_id_t card_id, int player, int card);
int whimsy_build_candidates(int player, int card, target_t *candidates, unsigned int type_mask);
int whimsy_apply_effect(int player, int card, int effect_index);
int choose_orcish_catapult_targets(int player, int card, target_t *targets);
int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off);
int resolve_pandoras_box(int player, int card);
int whimsy_destroy_creatures_then_artifacts_and_enchantments(int player, int card, int internal_card_id);

static int whimsy_target_type_masks[16] =
{
  0x1047, 0x43, 0x102, 0x100, 0, 0x40, 0, 0x44,
  0x100, 0x3, 0x100, 0x100, 0, 0, 0, 0x100
};

static int faerie_dragon_effect_pics[20] =
{
  0x0e, 0x214, 0x247, 0x8f, 0xc4, 0x20, 0x91, 0x85, 0x61, 0x6d,
  0x3b, 0xf5, 0x1a9, 0x100, 0x2e9, 0x104, 0xd0, 0x1cc, 0xf0, 0x365
};

// FUNCTION: MAGIC 0x004a7d10
// FUNCTION: SHANDALAR 0x00419310
int aswan_jaguar_target_available(int player, int card, int selected_monster)
{
  int current_player;
  int current_card;
  int raw_code;
  int result;

  result = 0;
  current_player = 0;
  do
  {
    if (current_player > 1 || result != 0)
    {
      return result;
    }

    for (current_card = 0; current_card < g_active_cards_count[current_player] && result == 0; ++current_card)
    {
      if (is_in_play(current_player, current_card) != 0)
      {
        raw_code = global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].id].subtype;
        if (selected_monster == raw_code || aswan_jaguar_paired_monster(raw_code) == selected_monster)
        {
          if ((global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
              (PLAYER_CARD_INSTANCE(current_player, current_card).regen_status & get_protections_from(player, card)) == 0)
          {
            result = 1;
          }
        }
      }
    }

    ++current_player;
  } while (1);
}

// FUNCTION: MAGIC 0x004a7e97
// FUNCTION: SHANDALAR 0x00419497
int aswan_jaguar_paired_monster(int raw_code)
{
  int result;

  result = -1;
  switch (raw_code)
  {
    case 7:
    case 16:
    case 73:
    case 86:
    case 89:
    case 105:
      result = raw_code + 1;
      break;

    case 8:
    case 17:
    case 74:
    case 87:
    case 90:
    case 106:
      result = raw_code - 1;
      break;
  }

  return result;
}

// FUNCTION: MAGIC 0x004a8700
// FUNCTION: SHANDALAR 0x00419d01
int aswan_jaguar_random_library_monster(int player)
{
  int library_count;
  int random_slot;
  int result;

  for (library_count = 0; library_count < 500 && global_library[player][library_count] != -1; ++library_count)
  {
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    random_slot = internal_rand(library_count);
  }
  else
  {
    random_slot = network_random(1 - player, library_count);
  }

  result = aswan_jaguar_find_library_monster(player, random_slot);
  if (result == 0)
  {
    result = aswan_jaguar_find_library_monster(player, 0);
  }

  return result;
}

// FUNCTION: MAGIC 0x004a87c3
// FUNCTION: SHANDALAR 0x00419dc4
int aswan_jaguar_find_library_monster(int player, int start_slot)
{
  int current_slot;
  int internal_card_id;
  int result;

  current_slot = start_slot;
  result = 0;
  g_event_result = -1;

  while (current_slot < 500 && result == 0)
  {
    internal_card_id = global_library[player][current_slot];
    if (internal_card_id != -1 &&
        global_raw_cards_storage[global_cards_data[internal_card_id].id].card_type == CP_TYPE_CREATURE)
    {
      result = global_raw_cards_storage[global_cards_data[internal_card_id].id].subtype;
      g_event_result = internal_card_id;
    }

    ++current_slot;
  }

  return result;
}

// FUNCTION: MAGIC 0x004a8884
// FUNCTION: SHANDALAR 0x00419e85
int create_aswan_jaguar_legacy(int player, int card, int selected_monster)
{
  int legacy_card;

  legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0a, player, card);
  if (legacy_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= STATUS_PERMANENT;
    PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = selected_monster;
    if (player != 0)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).state |= STATE_OWNED_BY_OPPONENT;
    }
    PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
  }

  return legacy_card;
}

// FUNCTION: MAGIC 0x004a7f99
// FUNCTION: SHANDALAR 0x00419599
int card_aswan_jaguar(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int selected_monster;
    int saved_event_result;
    card_instance_t *instance;
  } s;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_RESOLVE_SPELL)
  {
    s.saved_event_result = g_event_result;
    s.selected_monster = aswan_jaguar_random_library_monster(1 - player);
    create_aswan_jaguar_legacy(player, card, s.selected_monster);
    g_event_result = s.saved_event_result;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (!is_animated_and_sick(player, card) &&
        (s.instance->state & STATE_TAPPED) == 0 &&
        has_mana(player, COLOR_GREEN, 2) != 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                              COLOR_TEST_0, COLOR_TEST_0, -1,
                              PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player,
                                                   s.instance->damage_source_card).info_slot,
                              -1, -1, TARGET_SPECIAL_0x10, 0, 0) != 0)
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
        (s.instance->state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
        aswan_jaguar_target_available(player, card,
                                      PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player,
                                                           s.instance->damage_source_card).info_slot) != 0)
    {
      charge_mana(player, COLOR_GREEN, 2);
      if (g_spell_fizzled != 1)
      {
        load_text("prompts.txt", "ASWANJAGUAR");
        if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE,
                                 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1,
                                 PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player,
                                                      s.instance->damage_source_card).info_slot,
                                 -1, -1, TARGET_SPECIAL_0x10, 0, 0, g_text_lines[0], 1, &s.target))
        {
          SET_TARGET(s.instance->targets[0], s.target);
          s.instance->number_of_targets = 1;
          s.selected_monster = *(int *)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id].id].subtype;
          s.instance->state |= STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (C_real_validate_target(s.instance->targets[0].player, s.instance->targets[0].card,
                                 (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                                 COLOR_TEST_0, COLOR_TEST_0, -1,
                                 PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player,
                                                      s.instance->damage_source_card).info_slot,
                                 -1, -1, TARGET_SPECIAL_0x10, 0, 0))
      {
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x2e);
        }
        kill_card(s.instance->targets[0].player, s.instance->targets[0].card, KILL_BURY);
      }
      else
      {
        g_spell_fizzled = 1;
      }
      PLAYER_CARD_INSTANCE(s.instance->parent_controller, s.instance->parent_card).number_of_targets = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004a89c9
// FUNCTION: SHANDALAR 0x00419fc9
int card_goblin_polka_band(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (has_mana(player, COLOR_ANY, 2) != 0 && (instance->state & 0x20014) == 0) ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && polka_has_target_available(player, card) != 0)
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    if (g_spell_fizzled != 1)
    {
      charge_mana(player, COLOR_RED, -1);
    }
    if (g_spell_fizzled != 1)
    {
      instance->info_slot = g_x_value;
      instance->state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && instance->info_slot > 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x33);
    }
    polka_apply_effect(g_card_on_stack_controller, g_card_on_stack, instance->info_slot);
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004a916a
// FUNCTION: SHANDALAR 0x0041a76b
int card_polka(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_UNTAP &&
      instance->damage_target_card == g_affected_card &&
      instance->damage_target_player == g_affected_card_controller &&
      g_affected_card != -1)
  {
    PLAYER_CARD_INSTANCE((int)instance->damage_target_player, instance->damage_target_card).untap_status &= ~3;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004a8c52
// FUNCTION: SHANDALAR 0x0041a252
int polka_apply_effect(int player, int card, int amount)
{
  struct
  {
    int candidate_count;
    int raw_code;
    int tapped_count;
    int random_index;
    target_t candidates[300];
    int shift_index;
  } s;

  s.candidate_count = choose_orcish_catapult_targets(player, card, s.candidates);
  s.tapped_count = 0;
  while (s.tapped_count < amount && s.candidate_count != 0)
  {
    if ((g_duel_network_flags & 2) == 0)
    {
      s.random_index = internal_rand(s.candidate_count);
    }
    else
    {
      s.random_index = network_random(player, s.candidate_count);
    }

    tap_card_and_dispatch_event(s.candidates[s.random_index].player, s.candidates[s.random_index].card);

    s.raw_code = *(int *)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(s.candidates[s.random_index].player,
                                                                                        s.candidates[s.random_index].card).internal_card_id].id].subtype;
    if (s.raw_code == 0x57 || s.raw_code == 0x56)
    {
      create_legacy_effect(player, card, g_duel_generated_internal_card_id_0b,
                           s.candidates[s.random_index].player,
                           s.candidates[s.random_index].card);
    }

    for (s.shift_index = s.random_index; s.shift_index < s.candidate_count - 1; ++s.shift_index)
    {
      s.candidates[s.shift_index] = s.candidates[s.shift_index + 1];
    }

    --s.candidate_count;
    ++s.tapped_count;
  }

  return s.tapped_count;
}

// FUNCTION: MAGIC 0x004a904f
// FUNCTION: SHANDALAR 0x0041a64f
int polka_has_target_available(int player, int card)
{
  int result;
  int current_player;
  int current_card;

  result = 0;
  current_player = 0;
  while (current_player < 2 && result == 0)
  {
    current_card = 0;
    while (current_card < g_active_cards_count[current_player] && result == 0)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0 &&
          (PLAYER_CARD_INSTANCE(current_player, current_card).regen_status & get_protections_from(player, card)) == 0)
      {
        result = 1;
      }
      ++current_card;
    }
    ++current_player;
  }

  return result;
}

// FUNCTION: MAGIC 0x004a92a3
// FUNCTION: SHANDALAR 0x0041a8a4
int card_faerie_dragon(int player, int card, event_t event)
{
  struct
  {
    int candidate_count;
    int random_index;
    target_t candidates[300];
    card_instance_t *instance;
  } s;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (has_mana(player, COLOR_GREEN, 2) != 0 &&
            has_mana(player, COLOR_ANY, 3) != 0) ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    g_mana_charge[COLOR_COLORLESS] = 1;
    charge_mana(player, COLOR_GREEN, 2);
    if (g_spell_fizzled != 1)
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        s.instance->info_slot = internal_rand(0x14);
      }
      else
      {
        s.instance->info_slot = network_random(player, 0x14);
      }

      s.candidate_count = choose_orcish_catapult_targets(player, card, s.candidates);
      if (s.candidate_count == 0)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          s.random_index = internal_rand(s.candidate_count);
        }
        else
        {
          s.random_index = network_random(player, s.candidate_count);
        }
        s.instance->targets[0] = s.candidates[s.random_index];
        s.instance->number_of_targets = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && s.instance->number_of_targets != 0)
  {
    faerie_dragon_apply_effect(player, card, s.instance->info_slot);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004a955d
// FUNCTION: SHANDALAR 0x0041ab5e
int faerie_dragon_apply_effect(int player, int card, int effect_index)
{
  char dialog[300];
  int target_player;
  int target_card;
  int legacy_card;
  int current_player;
  int current_card;
  int amount;
  card_instance_t *instance;
  card_instance_t *target;
  card_instance_t *legacy;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (C_real_validate_target(instance->targets[0].player, instance->targets[0].card,
                             (char *)0, player, 2, 2, TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0))
  {
    target_player = instance->targets[0].player;
    target_card = instance->targets[0].card;
    target = &PLAYER_CARD_INSTANCE(target_player, target_card);
    legacy_card = -1;

    if (effect_index >= 0 && effect_index < 0x14 && effect_index != 0xd && effect_index != 1)
    {
      load_text("prompts.txt", "FAERIEDRAGON_MESSAGES");
      strcpy(dialog, "\n");
      strcat(dialog, g_text_lines[effect_index]);
      do_dialog(player, player, card, target_player, target_card, dialog, 0);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x30);
      }
    }

    switch (effect_index)
    {
      case 0:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_0f, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->info_slot = 0x80;
          legacy->counter_power = target->power;
          legacy->token_status |= 0x4000;
        }
        break;

      case 1:
        if (target->power < 3)
        {
          load_text("prompts.txt", "FAERIEDRAGON_TAWNOSWAND");
          sprintf(dialog, "\n%s", g_text_lines[0]);
          do_dialog(player, player, card, target_player, target_card, dialog, 0);
          legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_02, target_player, target_card);
          if (g_duel_ai_mode_state != 1)
          {
            play_sound_effect(0x30);
          }
        }
        else
        {
          g_spell_fizzled = 1;
          load_text("prompts.txt", "FAERIEDRAGON_TAWNOSWAND");
          sprintf(dialog, "\n%s", g_text_lines[1]);
          do_dialog(player, player, card, target_player, target_card, dialog, 0);
        }
        break;

      case 2:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->counter_power = 4;
          amount = ClampIntToRange(4, 0, target->toughness - 1);
          legacy->counter_toughness = -amount;
        }
        break;

      case 3:
      case 4:
      case 5:
      case 10:
      case 11:
        if (effect_index == 3)
        {
          amount = get_sleighted_color(player, card, COLOR_GREEN);
        }
        else if (effect_index == 4)
        {
          amount = get_sleighted_color(player, card, COLOR_WHITE);
        }
        else if (effect_index == 5)
        {
          amount = get_sleighted_color(player, card, COLOR_RED);
        }
        else if (effect_index == 10)
        {
          amount = get_sleighted_color(player, card, COLOR_BLACK);
        }
        else
        {
          amount = get_sleighted_color(player, card, COLOR_BLUE);
        }
        target->color = (char)(1 << ((unsigned char)amount));
        break;

      case 6:
        damage_creature(target_player, target_card, 3, g_card_on_stack_controller, g_card_on_stack);
        break;

      case 7:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->regen_status = 0;
          legacy->info_slot = 0x20;
        }
        break;

      case 8:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->counter_power = 3;
          legacy->counter_toughness = 3;
        }
        break;

      case 9:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_03, target_player, target_card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x40;
        }
        target->regen_status = 0x8000000;
        break;

      case 12:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_0f, target_player, target_card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x800000;
        }
        target->regen_status = 0x8000000;
        break;

      case 13:
        load_text("prompts.txt", "FAERIEDRAGON_TWIDDLE");
        sprintf(dialog, "\n%s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
        amount = do_dialog(player,
                           player,
                           card,
                           target_player,
                           target_card,
                           dialog,
                           (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) >> 4);
        if (amount == 0)
        {
          tap_card_and_dispatch_event(target_player, target_card);
        }
        else
        {
          target->state &= ~STATE_TAPPED;
        }
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x30);
        }
        break;

      case 14:
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, LEGACY_EFFECT_PUMP, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->counter_power = -2;
          legacy->counter_toughness = 0;
        }
        break;

      case 15:
        hurkyls_recall_bounce_artifact(target_player, target_card);
        break;

      case 16:
        damage_creature(target_player, target_card, 1, g_card_on_stack_controller, g_card_on_stack);
        break;

      case 17:
        for (current_player = 0; current_player < 2; ++current_player)
        {
          for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
          {
            if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == g_duel_generated_iid_1b &&
                is_in_play(current_player, current_card) != 0 &&
                PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player &&
                PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
            {
              PLAYER_CARD_INSTANCE(current_player, current_card).token_status &= ~0x1000000;
            }
          }
        }
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_iid_1b, target_player, target_card);
        if (legacy_card != -1)
        {
          legacy = &PLAYER_CARD_INSTANCE(player, legacy_card);
          legacy->token_status |= 0x1000000;
          legacy->counter_power = -(global_cards_data[target->internal_card_id].power & 0xbfff);
          legacy->counter_toughness = 2 - (global_cards_data[target->internal_card_id].toughness & 0xbfff);
        }
        break;

      case 18:
        amount = C_get_abilities(target_player, target_card, EVENT_POWER, -1);
        gain_life(target_player, amount);
        kill_card(target_player, target_card, KILL_REMOVE);
        break;

      case 19:
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x38);
          Sleep(0xdac);
        }
        --target->counter_toughness;
        target->special_counters += 0x1000000;
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x37);
        }
        break;

      default:
        load_text("prompts.txt", "FAERIEDRAGON_ERROR");
        sprintf(dialog, "\n%s", g_text_lines[0]);
        do_dialog(player, player, card, target_player, target_card, dialog, 0);
        break;
    }

    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).display_pic_info =
          (get_card_display_pic_num(faerie_dragon_effect_pics[effect_index], player, card) << 16) |
          faerie_dragon_effect_pics[effect_index];
    }
  }
  else
  {
    g_spell_fizzled = 1;
  }

  PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;

  return 0;
}

// FUNCTION: MAGIC 0x004aa60c
// FUNCTION: SHANDALAR 0x0041bc0d
int card_whimsy(int player, int card, event_t event)
{
  struct
  {
    int effect_index;
    int loop_index;
    int candidate_count;
    int random_index;
    target_t candidates[300];
    card_instance_t *instance;
  } s;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && has_mana(player, COLOR_ANY, 3) == 0)
    {
      return 0;
    }
    return 1;
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && player == g_affected_card_controller)
  {
    s.instance->info_slot = g_x_value;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    for (s.loop_index = 0; s.loop_index < s.instance->info_slot; ++s.loop_index)
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        s.effect_index = internal_rand(0x10);
      }
      else
      {
        s.effect_index = network_random(player, 0x10);
      }

      if (whimsy_target_type_masks[s.effect_index] != 0)
      {
        if (player == g_other_player && (g_duel_network_flags & 2) != 0)
        {
          TENTATIVE_wait_for_network_result(player, 0x1b);
          if (g_target_pair_network_packet.target.player == -1)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            s.instance->targets[0] = g_target_pair_network_packet.target;
            s.instance->number_of_targets = 1;
          }
        }
        else
        {
          s.candidate_count = whimsy_build_candidates(player, card, s.candidates, whimsy_target_type_masks[s.effect_index]);
          if (s.candidate_count == 0)
          {
            g_spell_fizzled = 1;
          }
          else
          {
            s.random_index = internal_rand(s.candidate_count);
            s.instance->targets[0] = s.candidates[s.random_index];
            s.instance->number_of_targets = 1;
          }

          if ((g_duel_network_flags & 2) != 0)
          {
            g_target_pair_network_packet.packet_type = 0x1b;
            if (s.candidate_count == 0)
            {
              g_target_pair_network_packet.target.player = -1;
            }
            else
            {
              g_target_pair_network_packet.target.player = 1 - s.instance->targets[0].player;
              g_target_pair_network_packet.target.card = s.instance->targets[0].card;
            }
            TENTATIVE_send_network_result(player, 0x1b);
          }
        }
      }

      if (g_spell_fizzled == 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "PROMPT_FIZZLE");
          set_duel_prompt_text(g_text_lines[0]);
          Sleep(2000);
          set_duel_prompt_text("");
        }
        g_spell_fizzled = 0;
      }
      else
      {
        whimsy_apply_effect(player, card, s.effect_index);
      }
    }

    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(0x39);
    }
    s.instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004aaac3
// FUNCTION: SHANDALAR 0x0041c0c4
int whimsy_build_candidates(int player, int card, target_t *candidates, unsigned int type_mask)
{
  int candidate_count;
  int current_player;
  int current_card;

  candidate_count = 0;
  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (type_mask & global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type) != 0 &&
          (PLAYER_CARD_INSTANCE(current_player, current_card).regen_status & get_protections_from(player, card)) == 0)
      {
        candidates[candidate_count].player = current_player;
        candidates[candidate_count].card = current_card;
        ++candidate_count;
      }
    }

    if ((type_mask & 0x100) != 0)
    {
      candidates[candidate_count].player = current_player;
      candidates[candidate_count].card = -1;
      ++candidate_count;
    }
  }

  return candidate_count;
}

// FUNCTION: MAGIC 0x004aac07
// FUNCTION: SHANDALAR 0x0041c208
int whimsy_apply_effect(int player, int card, int effect_index)
{
  struct
  {
    char dialog[300];
    int top_card;
    int hand_card;
    int random_player;
    int random_choice;
    int loop_index;
  } s;
  int target_player;
  int target_card;

  target_player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
  target_card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
  load_text("prompts.txt", "WHIMSY_MESSAGES");

  switch (effect_index)
  {
    case 0:
      sprintf(s.dialog, "\n%s", g_text_lines[0]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      hurkyls_recall_bounce_artifact(target_player, target_card);
      break;

    case 1:
      if ((g_duel_network_flags & 2) == 0)
      {
        s.random_choice = internal_rand(2);
      }
      else
      {
        s.random_choice = network_random_boolean(player);
      }
      if (s.random_choice == 0)
      {
        sprintf(s.dialog, "\n%s", g_text_lines[2]);
      }
      else
      {
        sprintf(s.dialog, "\n%s", g_text_lines[1]);
      }
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      if (s.random_choice == 0)
      {
        if ((PLAYER_CARD_INSTANCE(target_player, target_card).state & STATE_TAPPED) == 0)
        {
          PLAYER_CARD_INSTANCE(target_player, target_card).state |= STATE_TAPPED;
          if ((global_cards_data[PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id].type & TYPE_LAND) != 0)
          {
            g_produced_mana_color = -1;
          }
          dispatch_event(target_player, target_card, EVENT_TAP_CARD);
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(target_player, target_card).state &= ~STATE_TAPPED;
      }
      break;

    case 2:
      sprintf(s.dialog, "\n%s", g_text_lines[3]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      deal_damage_to_selected_target(player, card, EVENT_RESOLVE_SPELL, 4);
      break;

    case 3:
      sprintf(s.dialog, "\n%s", g_text_lines[4]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      draw_card_for_player(target_player);
      draw_card_for_player(target_player);
      draw_card_for_player(target_player);
      break;

    case 4:
      sprintf(s.dialog, "\n%s", g_text_lines[13]);
      do_dialog(player, player, card, -1, -1, s.dialog, 0);
      g_card_on_stack_controller = player;
      resolve_pandoras_box(player, card);
      g_card_on_stack_controller = -1;
      break;

    case 5:
      sprintf(s.dialog, "\n%s", g_text_lines[5]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      s.top_card = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
      gain_life(target_player,
                (int)(char)global_cards_data[s.top_card].cc[0] +
                    ClampIntToRange((int)(char)global_cards_data[s.top_card].cc[1], 0, 99));
      kill_card(target_player, target_card, KILL_DESTROY);
      break;

    case 6:
      sprintf(s.dialog, "\n%s", g_text_lines[12]);
      do_dialog(player, player, card, -1, -1, s.dialog, 0);
      load_text("prompts.txt", "WHIMSY_BOTTLESULEIMAN");
      sprintf(s.dialog, "%s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      s.random_choice = do_dialog(player, player, card, -1, -1, s.dialog, 1);
      if (coin_flip(player, get_displayed_card_name(player, card), 1) == s.random_choice)
      {
        s.hand_card = add_card_to_hand(player, find_internal_card_id_by_csv_id((card_id_t)0x37a));
        if (s.hand_card != -1)
        {
          process_card_enters_play(player, s.hand_card);
          PLAYER_CARD_INSTANCE(player, s.hand_card).token_status |= 0x10;
        }
      }
      else
      {
        damage_player(player, 5, player, card);
      }
      break;

    case 7:
      sprintf(s.dialog, "\n%s", g_text_lines[6]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      kill_card(target_player, target_card, KILL_BURY);
      break;

    case 8:
      sprintf(s.dialog, "\n%s", g_text_lines[7]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      gain_life(target_player, 3);
      break;

    case 9:
      sprintf(s.dialog, "\n%s", g_text_lines[8]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      kill_card(target_player, target_card, KILL_BURY);
      break;

    case 10:
      sprintf(s.dialog, "\n%s", g_text_lines[14]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      discard(target_player, 0, 0);
      break;

    case 11:
      sprintf(s.dialog, "\n%s", g_text_lines[9]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      for (s.loop_index = 0; s.loop_index < 2; ++s.loop_index)
      {
        s.top_card = global_library[target_player][0];
        if (s.top_card != -1)
        {
          remove_card_from_deck(target_player, 0);
          s.hand_card = add_card_to_hand(target_player, s.top_card);
          if (s.hand_card != -1)
          {
            move_card_to_graveyard(target_player, s.hand_card);
            PLAYER_CARD_INSTANCE(target_player, s.hand_card).internal_card_id = -1;
          }
        }
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x18);
        }
      }
      break;

    case 12:
      sprintf(s.dialog, "\n%s", g_text_lines[10]);
      do_dialog(player, player, card, -1, -1, s.dialog, 0);
      if ((g_duel_network_flags & 2) == 0)
      {
        s.random_player = internal_rand(2);
      }
      else
      {
        s.random_player = network_random_boolean(player);
      }
      s.hand_card = add_card_to_hand(s.random_player,
                                     find_internal_card_id_by_csv_id((card_id_t)0x375));
      if (s.hand_card != -1)
      {
        process_card_enters_play(s.random_player, s.hand_card);
        PLAYER_CARD_INSTANCE(s.random_player, s.hand_card).token_status |= 0x10;
      }
      break;

    case 13:
      sprintf(s.dialog, "\n%s", g_text_lines[11]);
      do_dialog(player, player, card, -1, -1, s.dialog, 0);
      dispatch_three_arg_callback_to_cards_in_play(
          whimsy_destroy_creatures_then_artifacts_and_enchantments,
          -1);
      break;

    case 14:
      sprintf(s.dialog, "\n%s", g_text_lines[15]);
      do_dialog(player, player, card, -1, -1, s.dialog, 0);
      s.hand_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_0d, -1, -1);
      if (s.hand_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.hand_card).display_pic_info = 0x58;
      }
      break;

    case 15:
      sprintf(s.dialog, "\n%s", g_text_lines[16]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      s.hand_card = draw_card_for_player(target_player);
      load_text("prompts.txt", "WHIMSY_SINDBAD");
      do_dialog(player, player, card, target_player, s.hand_card, g_text_lines[0], 0);
      if ((global_cards_data[PLAYER_CARD_INSTANCE(target_player, s.hand_card).internal_card_id].type & TYPE_LAND) == 0)
      {
        move_card_to_graveyard(target_player, s.hand_card);
        PLAYER_CARD_INSTANCE(target_player, s.hand_card).internal_card_id = -1;
        --g_duel_summary.hand_counts[target_player];
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(0x18);
        }
      }
      break;

    default:
      sprintf(s.dialog, "\n%s", g_text_lines[17]);
      do_dialog(player, player, card, target_player, target_card, s.dialog, 0);
      break;
  }

  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: SHANDALAR 0x0051dcf2
int whimsy_destroy_creatures_then_artifacts_and_enchantments(int player,
                                                              int card,
                                                              int internal_card_id)
{
  if (is_in_play(player, card) != 0 &&
      (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }
  regenerate_or_graveyard_triggers();
  if (is_in_play(player, card) != 0 &&
      (global_cards_data[internal_card_id].type & (TYPE_ARTIFACT | TYPE_ENCHANTMENT)) != 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}
