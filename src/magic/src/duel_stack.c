#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "duel_engine.h"
#include "game_support.h"
#include "global_strings.h"
#include "shared_startup.h"

// FUNCTION: MAGIC 0x0043e18b
// FUNCTION: SHANDALAR 0x00409a0b
int draw_card_for_player(int player)
{
  struct
  {
    int candidate_ok;      // ebp-0xc
    int drawn_card;        // ebp-0x8
    int desired_type_mask; // ebp-0x4
  } s;

  g_duel_state_0091c4fc = 0;
  dispatch_trigger(player, TRIGGER_REPLACE_CARD_DRAW, gs_draw_a_card_008961c0, 1);
  if (g_duel_state_0091c4fc != 0)
  {
    return 0;
  }

  if (player == g_active_player)
  {
    if (g_selected_wizard_color == -1)
    {
      s.drawn_card = global_library[player][0];
      if (s.drawn_card != -1)
      {
        remove_card_from_deck(player, 0);
        s.drawn_card = add_card_to_hand(player, s.drawn_card);
      }
    }
    else
    {
      s.drawn_card = add_card_to_hand(player, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
    }

    if (s.drawn_card != -1 && PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id != -1)
    {
      TENTATIVE_reassess_all_cards(0, 0x30);
      if (g_duel_ai_mode_state != 1)
      {
        load_text(global_ui_strings_filename, "PROMPT_DRAWACARD");
        if (g_duel_active != 0)
        {
          show_duel_card_preview(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                                 player,
                                 s.drawn_card,
                                 0xf6,
                                 g_text_lines[0],
                                 1);
        }
        else
        {
          show_card_preview_if_human(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                                     0xf6,
                                     g_text_lines[0],
                                     1);
        }
      }
    }
    else if (g_duel_ai_mode_state != 1)
    {
      load_text(global_ui_strings_filename, "PROMPT_DRAWACARD");
      set_duel_prompt_text(g_text_lines[1]);
      Sleep(0x9c4);
      set_duel_prompt_text("");
      exit_duel_thread(0);
    }
    else
    {
      g_life[g_active_player] = -99;
      g_life[1 - g_active_player] = 0x14;
    }
  }
  else
  {
    if (g_opponent_initial_library_index == -1)
    {
      switch (internal_rand(3))
      {
      case 0:
        s.desired_type_mask = 1;
        break;
      case 1:
        s.desired_type_mask = 2;
        break;
      case 2:
        s.desired_type_mask = 0x3c;
        break;
      }

      do
      {
        s.drawn_card = internal_rand(g_card_count);
        s.candidate_ok = is_card_color_compatible_with_mask((int)(char)global_cards_data[s.drawn_card].color,
                                                            g_current_encounter_color,
                                                            g_encounter_color_match_mode);
        if (s.candidate_ok != 0 && GetCardRarity(s.drawn_card) > g_current_encounter_strength)
        {
          s.candidate_ok = 0;
        }
      } while (s.candidate_ok == 0 ||
               (s.desired_type_mask & (unsigned char)global_cards_data[s.drawn_card].type) == 0 ||
               (global_cards_data[s.drawn_card].extra_ability & 0x40) != 0);

      s.drawn_card = add_card_to_hand(player, s.drawn_card);
    }
    else if (global_library[player][0] == -1)
    {
      s.drawn_card = -1;
    }
    else
    {
      s.drawn_card = add_card_to_hand(player, global_library[player][0]);
      if (s.drawn_card != -1)
      {
        remove_card_from_deck(player, 0);
      }
    }

    if (s.drawn_card == -1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text(global_ui_strings_filename, "PROMPT_DRAWACARD");
        set_duel_prompt_text(g_text_lines[2]);
        Sleep(0x9c4);
        set_duel_prompt_text("");
        exit_duel_thread(1);
      }
      else
      {
        g_life[1 - g_active_player] = -99;
        g_life[g_active_player] = 0x14;
      }
    }

    TENTATIVE_reassess_all_cards(0, 0x30);
  }

  ++g_duel_summary.hand_counts[player];
  ++g_duel_summary.cards_drawn;
  if (g_duel_ai_mode_state != 1 && s.drawn_card != -1)
  {
    play_sound_effect(2);
  }
  if (s.drawn_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, s.drawn_card).state |= 1;
  }

  return s.drawn_card;
}

// FUNCTION: MAGIC 0x0043ec38
// FUNCTION: SHANDALAR 0x0040a4b8
int discard_card_from_hand(int player, int card)
{
  dispatch_event_to_single_card(player, card, EVENT_DISCARD, 1 - player, -1);
  move_card_to_graveyard(player, card);
  PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  request_duel_display_refresh_if_human(player, card, 11, 1);
  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(WAV_DISCARD);
  }
  --g_duel_summary.hand_counts[player];
  return 0;
}

// FUNCTION: MAGIC 0x0043ece1
// FUNCTION: SHANDALAR 0x0040a561
int resolve_cast_card(int player, int card)
{
  int force_special_mode;

  force_special_mode = 0;
  if (g_current_action_event_code == 0xd3)
  {
    g_duel_state_00791418 = 1;
    force_special_mode = 1;
  }

  if (!put_card_on_stack(player, card, 0))
  {
  }
  else
  {
    if (!put_card_on_stack(player, card, 1))
    {
    }
    else
    {
      if (!resolve_card_on_stack(player, card))
      {
      }
      else
      {
        if (force_special_mode != 0)
        {
          g_duel_state_00791418 = 0;
        }
        return 1;
      }
    }
  }

  if (force_special_mode != 0)
  {
    g_duel_state_00791418 = 0;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0043edab
// FUNCTION: SHANDALAR 0x0040a62b
int put_card_on_stack(int player, int card, int mode)
{
  struct
  {
    int trace_counter;                          // ebp-0x348
    char prompt_text[300];                      // ebp-0x344
    int tmp_mana;                               // ebp-0x218
    char trace_text[500];                       // ebp-0x214
    int saved_unk_008ce508;                     // ebp-0x20
    int saved_max_x_value;                      // ebp-0x1c
    int chosen_x_value;                         // ebp-0x18
    int color_index;                            // ebp-0x14
    int saved_current_casting_internal_card_id; // ebp-0x10
    int internal_card_id;                       // ebp-0xc
    card_data_t *card_data;                     // ebp-0x8
    int saved_unk_008ce4f4;                     // ebp-0x4
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.color_index = (int)single_color_test_bit_to_color_t((int)(char)global_cards_data[s.internal_card_id].color);
  s.card_data = &global_cards_data[s.internal_card_id];

  if (mode != 0)
  {
    goto respond_to_card_on_stack;
  }

  if (TRACE_ENABLED)
  {
    s.trace_counter = g_duel_trace_counter;
    ++g_duel_trace_counter;
    sprintf(s.trace_text, "%d: Player #%d is casting %s(%d).\n", s.trace_counter, player, global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name, card);
    append_to_trace_txt(s.trace_text);
  }

  if (!can_pay_card_mana_cost(player, player, card))
  {
    return 0;
  }

  g_ai_recorded_action = -1;
  g_adventure_world_state = 0;
  g_max_x_value = -1;

  if ((((unsigned char)s.card_data->type & 0x3c) != 0 ||
       (int)s.card_data->id == 0x107 ||
       (int)s.card_data->id == 0x27) &&
      dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) == 0)
  {
    return 0;
  }

  g_spell_fizzled = -1;

  s.saved_current_casting_internal_card_id = g_current_casting_internal_card_id;
  s.saved_unk_008ce508 = g_current_spell_player;
  s.saved_unk_008ce4f4 = g_current_spell_card;

  g_current_casting_internal_card_id = s.internal_card_id;
  g_current_spell_player = player;
  g_current_spell_card = card;

  push_card_onto_stack(player, card, 0x71, player, 0);
  begin_mana_payment_record();

  if (g_adventure_world_state != 0)
  {
  }
  else
  {
    if ((player == g_active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
    {
      if ((s.card_data->type & 0x40) != 0)
      {
        g_mana_charge[6] = (int)(char)s.card_data->cc[0];
        g_mana_charge[6] += (int)(char)s.card_data->cc[1];
      }
      else
      {
        g_mana_charge[s.color_index] = (int)(char)s.card_data->cc[0];
        g_mana_charge[0] += (int)(char)s.card_data->cc[1];
      }

      charge_mana_w_global_cost_mod(player, card, 0, 0);
    }
    else if ((s.card_data->type & 0x40) != 0)
    {
      charge_mana_w_global_cost_mod(player, card, 6, (int)(char)s.card_data->cc[1] + (int)(char)s.card_data->cc[0]);
    }
    else
    {
      if ((char)s.card_data->cc[0] != 0)
      {
        g_mana_charge[s.color_index] = (int)(char)s.card_data->cc[0];
      }
      if (0 < (char)s.card_data->cc[1])
      {
        g_mana_charge[0] = (int)(char)s.card_data->cc[1];
      }

      charge_mana_w_global_cost_mod(player, card, 0, 0);

      if ((char)s.card_data->cc[1] == -1)
      {
        if (player != g_active_player)
        {
          if (g_duel_ai_mode_state == 1)
          {
            switch (internal_rand(2))
            {
            case 0:
              s.tmp_mana = has_mana(player, COLOR_ANY, 1);
              if (s.tmp_mana != 0)
              {
                g_ai_recorded_choice = s.chosen_x_value = internal_rand(s.tmp_mana) + 1;
              }
              else
              {
                g_ai_recorded_choice = s.chosen_x_value = internal_rand(has_mana(player, COLOR_ANY, 1) + 1);
              }
              break;

            case 1:
              g_ai_recorded_choice = s.chosen_x_value = has_mana(player, COLOR_ANY, 1);
              if (g_life[0] < g_ai_recorded_choice && internal_rand(3) == 0)
              {
                g_ai_recorded_choice = s.chosen_x_value = g_life[0];
              }
              if (g_max_x_value != -1 && g_max_x_value < g_ai_recorded_choice)
              {
                g_ai_recorded_choice = s.chosen_x_value = g_max_x_value;
              }
              break;
            }

            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            s.chosen_x_value = g_ai_recorded_choice;
          }

          s.saved_max_x_value = g_max_x_value;
          g_max_x_value = s.chosen_x_value;
          charge_mana(player, 0, -1);
          g_max_x_value = s.saved_max_x_value;
        }
        else
        {
          s.saved_max_x_value = g_max_x_value;
          g_max_x_value = has_mana(player, COLOR_ANY, 1);
          charge_mana(player, 0, -1);
          g_max_x_value = s.saved_max_x_value;
        }

        if (g_x_value == 0)
        {
          g_ai_modifier -= 100;
        }
      }
    }
  }

  g_max_x_value = -1;

  if (s.internal_card_id != -1)
  {
    g_current_casting_player = player;
    g_duel_state_008b2880 = 1;
    --g_duel_summary.hand_counts[player];

    if ((s.card_data->type & 2) != 0)
    {
      ++g_duel_summary.creature_counts[player];
    }
    if ((s.card_data->type & 0x40) != 0)
    {
      ++g_duel_summary.artifact_counts[player];
    }
    if ((s.card_data->type & 4) != 0)
    {
      ++g_duel_summary.enchantment_counts[player];
    }

    g_card_types_in_play[player] |= s.card_data->type;
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_INVISIBLE;
    g_land_can_be_played |= LCBP_SPELL_BEING_PLAYED;

    if (player == g_active_player || (g_duel_network_flags & 2) != 0 || g_duel_ai_mode_state != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_BOTH;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_NOATTACK;
    }

    g_current_casting_internal_card_id = s.saved_current_casting_internal_card_id;
    g_current_spell_player = s.saved_unk_008ce508;
    g_current_spell_card = s.saved_unk_008ce4f4;

    if (g_spell_fizzled == 1)
    {
      end_mana_payment_record();
      goto finish_put_card_on_stack;
    }

    if (dispatch_event(player, card, 0x6c) != 0)
    {
      g_spell_fizzled = 1;
    }
    if (g_spell_fizzled == 1)
    {
      restore_recorded_mana_to_pool(player);
    }
    end_mana_payment_record();

    if (g_spell_fizzled == 1)
    {
      goto finish_put_card_on_stack;
    }

    if (player == g_other_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state == 1 && s.card_data->cc[1] == -1)
    {
      g_ai_modifier -= count_colored_cards_in_hand(player) * 12;
    }
    return 1;

  respond_to_card_on_stack:
    s.saved_current_casting_internal_card_id = g_current_casting_internal_card_id;
    s.saved_unk_008ce508 = g_current_spell_player;
    s.saved_unk_008ce4f4 = g_current_spell_card;

    g_current_casting_internal_card_id = s.internal_card_id;
    g_current_spell_player = player;
    g_current_spell_card = card;

    if ((s.card_data->type & 1) == 0)
    {
      refresh_stack_proxy_card(1);
    }

    if ((s.card_data->type != 1) &&
        ((s.card_data->type != 0x20) || ((s.card_data->extra_ability & EA_MANA_SOURCE) == 0)))
    {
      if (g_duel_state_007a7d78 != 0)
      {
        g_duel_state_007a7d78 |= 2;
      }

      sprintf(g_ui_message_buffer, (const char *)gs_trying_to_cast_007a79c0, get_displayed_card_name(g_current_spell_player, g_current_spell_card));
      allow_response(-2, g_current_phase, g_ui_message_buffer, 0xd3);
      if ((g_duel_state_007a7d78 & 2) != 0)
      {
        request_duel_display_refresh_if_human(player, card, 2, 1);
        Sleep(3000);
      }
    }

    g_current_casting_internal_card_id = s.saved_current_casting_internal_card_id;
    g_current_spell_player = s.saved_unk_008ce508;
    g_current_spell_card = s.saved_unk_008ce4f4;

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }

    g_land_can_be_played &= ~LCBP_SPELL_BEING_PLAYED;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state |= (((unsigned int)player < 1 ? 0 : 0x400000) | 0x80);

    if (g_duel_ai_mode_state != 1)
    {
      set_stack_damage_targets();
    }

    if (g_spell_fizzled == 1)
    {
      goto finish_put_card_on_stack;
    }

    if (player == g_other_player && g_duel_ai_mode_state != 1 && g_battlefield_ui_flags == 0 && (((unsigned char)s.card_data->type & 0x7e) != 0))
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_CAST1");
      sprintf(s.prompt_text, g_text_lines[0], g_duel_message_text);
      if ((char)s.card_data->cc[1] == -1)
      {
        sprintf(s.prompt_text, g_text_lines[1], g_duel_message_text, g_x_value);
      }

      if (PLAYER_CARD_INSTANCE(player, card).number_of_targets != 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 1)
        {
          raw_do_dialog(player,
                        card,
                        PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                        PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                        s.prompt_text,
                        0);
        }
        else
        {
          raw_do_dialog(player, card, -1, -1, s.prompt_text, 0);
        }
      }
      else if (0)
      {
        // Unreachable in practice, but present in the original binary.
        raw_do_dialog(player, card, (g_ai_recorded_action >> 8), (g_ai_recorded_action & 0xff), s.prompt_text, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt_text, 0);
      }
    }

    if (g_duel_ai_mode_state != 1)
    {
      request_duel_display_refresh_if_human(player, card, 2, 1);
    }
  }
finish_put_card_on_stack:
  if (g_spell_fizzled == 1)
  {
    ++g_duel_summary.hand_counts[player];

    if ((s.card_data->type & 2) != 0)
    {
      --g_duel_summary.creature_counts[player];
    }
    if ((s.card_data->type & 0x40) != 0)
    {
      --g_duel_summary.artifact_counts[player];
    }
    if ((s.card_data->type & 4) != 0)
    {
      --g_duel_summary.enchantment_counts[player];
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xffffff5d;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;

    if (player == g_other_player && (g_duel_network_flags & 2) == 0)
    {
      g_recorded_action_cursor = 1;
    }
    g_spell_fizzled = 0;
    obliterate_top_card_of_stack();
    g_land_can_be_played &= ~LCBP_SPELL_BEING_PLAYED;
    return 0;
  }

  TENTATIVE_set_timestamps(player, card);
  return 1;
}

// FUNCTION: MAGIC 0x0043f9ed
// FUNCTION: SHANDALAR 0x0040b26d
int resolve_card_on_stack(int player, int card)
{
  int internal_card_id;
  char *card_name;
  int saved_trigger_cause;
  int saved_trigger_cause_controller;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  single_color_test_bit_to_color_t((int)(char)global_cards_data[internal_card_id].color);

  if (internal_card_id == -1)
  {
    obliterate_top_card_of_stack();
    return 0;
  }

  saved_trigger_cause_controller = g_trigger_cause_controller;
  saved_trigger_cause = g_trigger_cause;

  g_trigger_cause_controller = player;
  g_trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_SPELL_CAST, gs_casting_00896400, 0);
  g_trigger_cause_controller = saved_trigger_cause_controller;
  g_trigger_cause = saved_trigger_cause;

  if (global_cards_data[internal_card_id].type != 0x01 &&
      (global_cards_data[internal_card_id].type != 0x20 || (global_cards_data[internal_card_id].extra_ability & 0x1000) == 0))
  {
    sprintf(g_ui_message_buffer, gs_cast_008b4720, get_displayed_card_name(player, card));
    allow_response(-2, g_current_phase, g_ui_message_buffer, 0x6c);
  }

  PLAYER_CARD_INSTANCE(player, card).state &= ~0x20;
  PLAYER_CARD_INSTANCE(player, card).state |= 2;
  g_land_can_be_played &= ~LCBP_SPELL_BEING_PLAYED;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != internal_card_id)
  {
    obliterate_top_card_of_stack();
    return 0;
  }

  g_card_types_in_play[player] |= (unsigned char)global_cards_data[internal_card_id].type;

  if (g_duel_ai_mode_state != 1)
  {
    if ((global_cards_data[internal_card_id].type & 2) != 0)
    {
      play_sound_effect(0x11);
    }
    if ((global_cards_data[internal_card_id].type & 0x40) != 0)
    {
      play_sound_effect(0);
    }
    if ((global_cards_data[internal_card_id].type & 4) != 0)
    {
      play_sound_effect(3);
    }
    if ((global_cards_data[internal_card_id].type & 0x10) != 0)
    {
      play_sound_effect(6);
    }
    if ((global_cards_data[internal_card_id].type & 0x20) != 0)
    {
      play_sound_effect(7);
    }
    if ((global_cards_data[internal_card_id].type & 8) != 0)
    {
      play_sound_effect(0x10);
    }
  }

  resolve_top_card_on_stack();
  process_damage_prevention(player);
  g_battlefield_extra_ability_flags &= ~1;
  pending_killed_card_handler = process_killed_card;
  regenerate_or_graveyard_triggers();
  process_damage_prevention(player);

  g_current_casting_player = -1;
  g_current_casting_internal_card_id = g_current_casting_player;

  if ((global_cards_data[internal_card_id].type & 1) != 0)
  {
    ++g_duel_summary.land_entries;
  }

  if (g_spell_fizzled == 1)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PROMPT_FIZZLE");
      set_duel_prompt_text(g_ui_message_buffer);
      Sleep(2000);
      set_duel_prompt_text("");
    }
    g_recorded_action_cursor = 1;
    g_spell_fizzled = 0;
    return 0;
  }

  g_spell_fizzled = 0;
  TENTATIVE_reassess_all_cards(0, 0xff);
  return 1;
}

// FUNCTION: MAGIC 0x0043ff32
// FUNCTION: SHANDALAR 0x0040b7b2
int activate(int who_activates, int player, int card)
{
  struct
  {
    int upkeep_uses_mana_source;
    int untap_color;
    int upkeep_total;
    char upkeep_prompt[600];
    int has_upkeep_costs;
    int i;
    int upkeep_dialog_ai_choice;
    char upkeep_opt0[300];
    char upkeep_opt1[300];
    card_instance_t *instance;
    char prompt[300];
    int result;
    int who_pays; /* also who chooses in do_dialog */
    int was_tapped;
    char trace[500];
  } s;

  s.result = 1;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);
  s.who_pays = player;

  if ((g_duel_network_flags & 2) != 0)
  {
    int trace_counter;

    trace_counter = g_duel_trace_counter;
    ++g_duel_trace_counter;
    sprintf(s.trace,
            "%d: Player #%d is tapping %s(%d).\n",
            trace_counter,
            player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(s.trace);
  }
  if (g_current_phase == EVENT_UPKEEP_PHASE)
  {
    s.who_pays = g_event_player;
  }
  if (s.who_pays == g_other_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
  {
    g_ai_recorded_action = -1;
    dispatch_event_to_single_card(player, card, EVENT_GET_SELECTED_CARD, 1 - player, -1);

    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
    sprintf(s.prompt, g_text_lines[0], g_duel_message_text);

    if (card_activation_uses_x(player, card) != 0)
    {
      sprintf(s.prompt, g_text_lines[1], g_duel_message_text, g_loaded_recorded_action_code);
    }
    else if (((global_cards_data[s.instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
             g_current_player == player && g_loaded_recorded_action_code != 0)
    {
      sprintf(s.prompt, g_text_lines[2], g_duel_message_text, g_loaded_recorded_action_code);
    }

    if (s.instance->number_of_targets != 0)
    {
      if (s.instance->number_of_targets == 1)
      {
        raw_do_dialog(player, card, s.instance->targets[0].player, s.instance->targets[0].card, s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }
    else
    {
      if (g_ai_recorded_action != -1)
      {
        /* Unreachable in practice, but present in the original binary. */
        raw_do_dialog(player, card, (g_ai_recorded_action >> 8), (g_ai_recorded_action & 0xff), s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }
  }

  push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, player, 0);
  g_upkeep_payment_completed = 0;

  if ((s.instance->upkeep_flags & UPKEEP_UPKEEP_TRIGGER) != 0)
  {
    s.has_upkeep_costs = 1;
    s.upkeep_total = 0;

    for (s.i = 0; s.i < 7; s.i++)
    {
      g_mana_charge[s.i] = ((char *)&s.instance->upkeep_colorless)[s.i];

      if (s.i > 0)
      {
        if (has_mana(player, (color_t)s.i, g_mana_charge[s.i]) == 0)
        {
          s.has_upkeep_costs = 0;
        }
      }

      s.upkeep_total += g_mana_charge[s.i];
    }

    if (has_mana(s.who_pays, COLOR_ANY, s.upkeep_total) == 0)
    {
      s.has_upkeep_costs = 0;
    }

    if (s.has_upkeep_costs != 0)
      s.upkeep_dialog_ai_choice = 0;
    else
      s.upkeep_dialog_ai_choice = 1;

    load_text(global_ui_strings_filename, "PROMPT_PAYUPKEEP");
    strcpy(s.upkeep_opt0, g_text_lines[0]);
    strcpy(s.upkeep_opt1, g_text_lines[1]);
    sprintf(s.upkeep_prompt, " %s\n %s", s.upkeep_opt0, s.upkeep_opt1);

    if ((s.instance->upkeep_flags & UPKEEP_UPKEEP_NODIALOG) != 0 ||
        do_dialog(s.who_pays, player, card, -1, -1, s.upkeep_prompt, s.upkeep_dialog_ai_choice) == 0)
    {
      if (s.has_upkeep_costs == 0)
      {
        goto upkeep_cleanup;
      }

      charge_mana(s.who_pays, 0, 0);
      if (g_spell_fizzled == 0)
      {
        dispatch_event_to_single_card(player, card, EVENT_UPKEEP_PHASE, 1 - player, -1);
        if (g_last_single_card_event_result == 0)
        {
          s.instance->upkeep_flags |= UPKEEP_UPKEEP_UNPAID;
        }
      }
    }

    if (g_spell_fizzled != 0)
    {
      g_spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

  upkeep_cleanup:
    for (s.i = 0; s.i < 7; s.i++)
    {
      g_mana_charge[s.i] = 0;
    }

    s.instance->upkeep_flags &= ~UPKEEP_UPKEEP_TRIGGER;
    s.instance->upkeep_flags |= UPKEEP_UPKEEP_CANTPAY;

    refresh_stack_proxy_card(1);
    return 1;
  }

  if ((s.instance->upkeep_flags & UPKEEP_UNTAP_TRIGGER) != 0 && can_pay_untap_cost(player, card) != 0)
  {
    for (s.untap_color = 0; s.untap_color < 7; ++s.untap_color)
    {
      g_mana_charge[s.untap_color] = (int)(char)s.instance->mana_to_untap[s.untap_color];
    }

    charge_mana(player, 0, 0);
    if (g_spell_fizzled == 0)
    {
      dispatch_event_to_single_card(player, card, EVENT_UNTAP_PHASE, 1 - player, -1);
      if (g_last_single_card_event_result == 0)
      {
        s.instance->upkeep_flags |= UPKEEP_UNTAP_PAID;
      }
    }

    if (g_spell_fizzled != 0)
    {
      g_spell_fizzled = 0;
      obliterate_top_card_of_stack();
      return 0;
    }

    s.instance->upkeep_flags &= ~UPKEEP_UNTAP_TRIGGER;
    s.instance->upkeep_flags |= UPKEEP_UNTAP_DONE;
    refresh_stack_proxy_card(1);
    return 1;
  }

  if (dispatch_event(player, card, EVENT_UNKNOWN80) != 0)
  {
    obliterate_top_card_of_stack();
    s.result = 0;
  }
  else
  {
    if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
    {
      g_produced_mana_color = -1;
    }

    begin_mana_payment_record();

    s.was_tapped = s.instance->state & 0x10;
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);

    if ((g_duel_network_flags & 2) != 0 && g_other_player == who_activates && g_battlefield_ui_flags == 0 &&
        s.instance->internal_card_id != g_draw_card_placeholder_internal_card_id)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_TAP1");
      sprintf(s.prompt, g_text_lines[0], g_duel_message_text);

      if (card_activation_uses_x(player, card) != 0)
      {
        sprintf(s.prompt, g_text_lines[1], g_duel_message_text, g_x_value);
      }
      else if (((global_cards_data[s.instance->internal_card_id].extra_ability & (EA_INF_POWER | EA_INF_TOUGHNESS)) != 0) &&
               g_current_player == player && g_x_value != 0)
      {
        sprintf(s.prompt, g_text_lines[2], g_duel_message_text, g_x_value);
      }

      if (s.instance->number_of_targets == 1)
      {
        raw_do_dialog(player, card, s.instance->targets[0].player, s.instance->targets[0].card, s.prompt, 0);
      }
      else
      {
        raw_do_dialog(player, card, -1, -1, s.prompt, 0);
      }
    }

    if (g_spell_fizzled == 1)
    {
      restore_recorded_mana_to_pool(player);
      obliterate_top_card_of_stack();
      s.result = 0;
    }

    end_mana_payment_record();

    if (s.result != 0)
    {
      if (s.was_tapped == 0 && (s.instance->state & 0x10) != 0)
      {
        dispatch_event(player, card, EVENT_TAP_CARD);
      }
      else
      {
        dispatch_event(player, card, EVENT_PLAY_ABILITY);
      }

      if (s.instance->internal_card_id != -1)
      {
        s.upkeep_uses_mana_source = global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE;
      }
      else
      {
        s.upkeep_uses_mana_source = global_cards_data[s.instance->original_internal_card_id].extra_ability & EA_MANA_SOURCE;
      }

      if (s.upkeep_uses_mana_source != 0 && g_produced_mana_color != -1)
      {
        refresh_stack_proxy_card(0);
      }
      else
      {
        refresh_stack_proxy_card(1);
      }

      if (g_duel_ai_mode_state != 1)
      {
        if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
            g_produced_mana_color != -1)
        {
          play_sound_effect(0x12);
        }
        else
        {
          play_sound_effect(0x26);
        }
      }

      if (g_duel_ai_mode_state != 1)
      {
        set_stack_damage_targets();
      }
    }
  }

  if (s.result == 0)
  {
    g_upkeep_payment_completed = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0044096f
// FUNCTION: SHANDALAR 0x0040c1ef
int resolve_activated_ability(int player, int card)
{
  int internal_card_id;
  int has_mana_source_and_color;
  int response_player;
  int saved_trigger_cause_controller;
  int saved_trigger_cause;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  has_mana_source_and_color =
      ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 && g_produced_mana_color != -1);

  if (g_spell_fizzled == 1)
  {
    return 1;
  }

  if (!has_mana_source_and_color && g_upkeep_payment_completed == 0)
  {
    sprintf(g_ui_message_buffer, gs_activate_0091b2d0, get_displayed_card_name(player, card));

    if (g_phase_stop_suppressed != 0 && (g_active_player == player || (g_duel_network_flags & 2) != 0))
    {
      response_player = -1;
    }
    else
    {
      response_player = -2;
    }

    allow_response(response_player, g_current_phase, g_ui_message_buffer, 0x6d);
  }

  resolve_top_card_on_stack();

  saved_trigger_cause_controller = g_trigger_cause_controller;
  saved_trigger_cause = g_trigger_cause;

  g_trigger_cause_controller = player;
  g_trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, 0xd2, gs_tapping_00925d40, 0);

  g_trigger_cause_controller = saved_trigger_cause_controller;
  g_trigger_cause = saved_trigger_cause;

  return 1;
}

// FUNCTION: MAGIC 0x00440ad2
// FUNCTION: SHANDALAR 0x0040c352
int resolve_card_immediately(int player, int card, int arg_3)
{
  struct
  {
    int trace_counter;       /* iVar2 / local_324 */
    char dialog_prompt[300]; /* local_320 */
    char trace_text[500];    /* local_1f4 */
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = g_duel_trace_counter;
    ++g_duel_trace_counter;
    sprintf(s.trace_text, "%d: Player #%d is processing %s(%d).\n",
            s.trace_counter, player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(s.trace_text);
  }

  g_ai_search_result = 1;
  push_card_onto_stack(player, card, 0x7e, arg_3, 0);

  if (g_spell_fizzled == 1)
  {
    obliterate_top_card_of_stack();
    g_ai_search_result = 0;
    return 0;
  }

  if (g_duel_ai_mode_state != 1)
  {
    set_stack_damage_targets();
    if (g_current_turn != g_active_player)
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_PROC1");
      sprintf(s.dialog_prompt, g_text_lines[0], g_duel_message_text);
      raw_do_dialog(player, card, -1, -1, s.dialog_prompt, 0);
    }
  }

  PLAYER_CARD_INSTANCE(player, card).state |= STATE_PROCESSING;
  resolve_top_card_on_stack();

  g_ai_search_result = 0;
}

// FUNCTION: MAGIC 0x00440c61
// FUNCTION: SHANDALAR 0x0040c4e1
int is_card_pending_resolution(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  return ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_IN_PLAY | STATE_INVISIBLE)) != 0) ? 0 : 1;
}

// FUNCTION: MAGIC 0x0044125c
// FUNCTION: SHANDALAR 0x0040cadc
int can_attack(int player, int card)
{
  int saved_affected_card;
  int saved_affected_card_controller;
  int saved_event_result;
  int saved_spell_fizzled;
  int internal_card_id;
  int result;

  saved_affected_card = g_affected_card;
  saved_affected_card_controller = g_affected_card_controller;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (((global_cards_data[internal_card_id].subtype == 0) && ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_WALL_CAN_ATTACK) == 0)) ||
      (((global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_NONCREATURE_CAN_ATTACK) == 0)) ||
      ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_OUBLIETTED | STATE_SUMMONSICK_NOATTACK | STATE_TAPPED)) != 0) || ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_CANT_ATTACK) != 0))
  {
    result = 0;
  }
  else
  {
    saved_spell_fizzled = g_spell_fizzled;
    saved_event_result = g_event_result;
    g_event_result = 0;
    g_affected_card_controller = player;
    g_affected_card = card;
    global_cards_data[internal_card_id].code_pointer(player, card, 0x79);
    result = g_event_result;
    g_event_result = saved_event_result;
    g_spell_fizzled = saved_spell_fizzled;
    g_affected_card_controller = saved_affected_card_controller;
    g_affected_card = saved_affected_card;

    if (result != 0)
    {
      return 0;
    }

    if ((player == g_other_player) && ((g_duel_network_flags & 2) == 0))
    {
      push_affected_card_stack();
      g_event_result = 0;
      g_affected_card_controller = player;
      g_affected_card = card;
      dispatch_three_arg_callback_to_cards_in_play(check_attached_aura_can_pay_cost, -1);
      result = g_event_result;
      pop_affected_card_stack();
      if (result != 0)
      {
        return 0;
      }
    }

    if ((((g_player_special_effect_flags[1 - player] & 1) != 0) || ((g_battlefield_extra_ability_flags & 0x04000000) != 0)) &&
        ((result = dispatch_event(player, card, 0x79)) != 0))
    {
      return 0;
    }

    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x00441514
// FUNCTION: SHANDALAR 0x0040cd94
int has_vigilance(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).state & STATE_VIGILANCE)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x004418c6
// FUNCTION: SHANDALAR 0x0040d146
int can_block_attacker_with_abilities(int blocker_player,
                                      int blocker_card,
                                      int attacker_player,
                                      int attacker_card,
                                      unsigned int attacker_abilities,
                                      unsigned int land_bits)
{
#define BLOCKER_INSTANCE global_card_instances[blocker_player][blocker_card]
  struct
  {
    int result;
    int internal_card_id;
  } s;

  s.internal_card_id = BLOCKER_INSTANCE.internal_card_id;

  if ((((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0) &&
      ((BLOCKER_INSTANCE.state & STATE_NONCREATURE_CAN_BLOCK) == 0))
  {
    return 0;
  }

  if (s.internal_card_id == -1 || (BLOCKER_INSTANCE.state & (STATE_OUBLIETTED | STATE_TAPPED | STATE_BLOCKING)) != 0)
  {
    return 0;
  }

  if (((BLOCKER_INSTANCE.state & STATE_ATTACKING) != 0) && has_vigilance(blocker_player, blocker_card) == 0)
  {
    return 0;
  }

  if ((BLOCKER_INSTANCE.state == 0) & 0x402)
  {
    return 0;
  }

  if ((attacker_abilities & 0x20) != 0)
  {
    if (((unsigned int)C_get_abilities(blocker_player, blocker_card, 0x34, -1) & 0x420) == 0)
    {
      return 0;
    }
  }

  if ((attacker_abilities & 0x1ff800) != 0)
  {
    if ((attacker_abilities &
         (0x800 << (unsigned char)(single_color_test_bit_to_color_t((int)(char)BLOCKER_INSTANCE.color) - 1))) != 0)
    {
      return 0;
    }
  }

  if ((((unsigned char)global_cards_data[BLOCKER_INSTANCE.internal_card_id].type & TYPE_ARTIFACT) != 0) && (attacker_abilities & 0x10000) != 0)
  {
    return 0;
  }

  if ((land_bits & attacker_abilities & 0x1f) != 0)
  {
    return 0;
  }

  push_affected_card_stack();
  s.result = 1;
  g_affected_card_controller = blocker_player;
  g_affected_card = blocker_card;
  g_attacking_card_controller = attacker_player;
  g_attacking_card = attacker_card;
  g_event_result = 0;
  C_dispatch_event_raw(EVENT_BLOCK_LEGALITY);
  if (g_event_result > 0)
  {
    s.result = 0;
  }
  pop_affected_card_stack();

  return s.result;
#undef BLOCKER_INSTANCE
}

// FUNCTION: MAGIC 0x00441d78
// FUNCTION: SHANDALAR 0x0040d614
void reassess_all_cards_and_mana(void)
{
  int player;
  int card;
  card_instance_t *instance;

  for (player = 0; player < 2; ++player)
  {
    instance = global_card_instances[player];
    for (card = 0; card < g_active_cards_count[player]; ++card)
    {
      if ((instance->state & 0x800002) == 2 && instance->internal_card_id != -1)
      {
        instance->regen_status |= 0x0f000000;
        C_get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
      }
      ++instance;
    }
  }

  C_count_colors_of_lands_in_play();
  count_mana();

  for (player = 0; player < 2; ++player)
  {
    instance = global_card_instances[player];
    for (card = 0; card < g_active_cards_count[player]; ++card)
    {
      if (is_in_play(player, card) && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) && (short)instance->damage_on_card < C_get_abilities(player, card, EVENT_TOUGHNESS, -1))
      {
        C_get_abilities(player, card, EVENT_POWER, -1);
      }
      ++instance;
    }
  }
}

// FUNCTION: MAGIC 0x00441f16
// FUNCTION: SHANDALAR 0x0040d7b2
int C_get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  struct
  {
    card_instance_t *test_instance;
    unsigned int current_color;
    int test_card;
    card_instance_t *instance;
    unsigned int current_abilities;
    int cleanup_card;
    int test_player;
    unsigned int result;
  } s;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);
  ++g_affected_card_context_index;
  s.current_abilities = g_adventure_duel_internal_card_id;
  if (g_duel_active != 0)
  {
    push_affected_card_stack();
  }
  g_affected_card_controller = player;
  g_affected_card = card;
  g_affected_internal_card_id = s.instance->internal_card_id;
  g_affected_card_color = (int)(char)global_cards_data[g_affected_internal_card_id].color;
  g_attacking_card = new_attacking_card;

  switch (event)
  {
  case EVENT_POWER:
    if ((s.instance->state & 0x800002) == 2)
    {
      s.result = (int)global_cards_data[g_affected_internal_card_id].power & 0xffffbfff;
    }
    else
    {
      s.result = (unsigned int)global_cards_data[g_affected_internal_card_id].power;
    }
    s.result += s.instance->counter_power;
    if ((s.instance->regen_status & 0x04000000) == 0)
    {
      g_event_result = (int)s.instance->power;
      goto post_dispatch;
    }
    else
    {
      s.instance->regen_status &= 0xfbffffff;
    }
    break;

  case EVENT_TOUGHNESS:
    if ((s.instance->state & 0x800002) == 2)
    {
      s.result = (int)global_cards_data[g_affected_internal_card_id].toughness & 0xffffbfff;
    }
    else
    {
      s.result = (unsigned int)global_cards_data[g_affected_internal_card_id].toughness;
    }
    s.result += s.instance->counter_toughness;
    if ((s.instance->regen_status & 0x02000000) == 0)
    {
      g_event_result = (int)s.instance->toughness;
      goto post_dispatch;
    }
    else
    {
      s.instance->regen_status &= 0xfdffffff;
    }
    break;

  case EVENT_ABILITIES:
    s.result = global_cards_data[g_affected_internal_card_id].static_ability | (s.instance->regen_status & 0x07000000);
    if ((s.result & 0x1ff81f) != 0)
    {
      s.current_color = 0;
      for (s.test_card = 0; s.test_card < 5; ++s.test_card)
      {
        if ((s.result & (1 << s.test_card)) != 0)
        {
          s.current_color |= 1 << (get_hacked_color(player, card, s.test_card + 1) - 1);
        }
        if ((s.result & (0x800 << s.test_card)) != 0)
        {
          s.current_color |= 0x800 << (get_sleighted_color(player, card, s.test_card + 1) - 1);
        }
      }
      s.result &= 0xffe007e0;
      s.result |= s.current_color;
    }
    if ((s.instance->regen_status & 0x08000000) == 0)
    {
      g_event_result = s.instance->regen_status;
      goto post_dispatch;
    }
    else
    {
      s.instance->regen_status &= 0xf7ffffff;
    }
    break;

  case EVENT_CHANGE_TYPE:
    if (((s.instance->internal_card_id >= g_damage_card_internal_card_id) &&
         (g_damage_card_internal_card_id + 0x2d > s.instance->internal_card_id)) ||
        s.instance->internal_card_id == -1)
    {
      g_event_result = s.instance->internal_card_id;
      goto post_dispatch;
    }

    s.result = s.instance->original_internal_card_id;
    if ((s.instance->regen_status & 0x01000000) == 0)
    {
      g_event_result = s.instance->internal_card_id;
      goto post_dispatch;
    }
    else
    {
      s.instance->internal_card_id = s.instance->original_internal_card_id;
      s.instance->regen_status &= 0xfeffffff;
      s.instance->mana_color = global_cards_data[s.instance->original_internal_card_id].color;
    }

    s.instance->destroys_if_blocked = 0;
    break;

  case EVENT_RECALC_DAMAGE:
    s.result = (unsigned int)(short)s.instance->damage_on_card;
    break;

  case EVENT_SET_COLOR:
    s.result = (unsigned int)(char)global_cards_data[g_affected_internal_card_id].color;
    break;

  default:
    s.result = 0;
  }

  g_event_result = s.result;
  if (g_duel_active != 0)
  {
    C_dispatch_event_raw(event);
    if (((g_land_can_be_played & LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS) != 0) && (event == EVENT_CHANGE_TYPE))
    {
      g_land_can_be_played &= ~LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
      s.instance->internal_card_id = g_event_result;
      g_land_can_be_played |= LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS;
      C_dispatch_event_raw(event);
      g_land_can_be_played &= ~LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS;
    }
  }

  if (event == EVENT_POWER)
  {
    if ((int)g_event_result < 0)
    {
      g_event_result = 0;
    }
    if ((s.instance->token_status & 0x4000) != 0)
    {
      g_event_result <<= 1;
    }
  }

post_dispatch:
  s.result = g_event_result;
  if (is_in_play(player, card) && event == EVENT_TOUGHNESS && (global_cards_data[g_affected_internal_card_id].type & TYPE_CREATURE) != 0 && ((int)s.result <= 0 || (int)s.result <= (int)(short)s.instance->damage_on_card) && ((PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_CANNOT_BE_DESTROYED) == 0) && g_trigger_condition == -1 && (g_land_can_be_played & (LCBP_DAMAGE_PREVENTION | LCBP_REGENERATION)) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
    regenerate_or_graveyard_triggers();
  }

  if (g_duel_active != 0)
  {
    pop_affected_card_stack();
  }
  if (event == EVENT_POWER)
  {
    s.instance->power = (short)s.result;
  }
  if (event == EVENT_TOUGHNESS)
  {
    s.instance->toughness = (short)s.result;
  }
  if (event == EVENT_ABILITIES)
  {
    s.instance->regen_status = s.result;
  }
  if (event == EVENT_CHANGE_TYPE)
  {
    s.instance->internal_card_id = s.result;
    if ((global_cards_data[s.result].extra_ability & 0x1000) != 0)
    {
      switch (global_cards_data[s.result].id)
      {
      case 0x13e:
        break;

      case 0x366:
        s.instance->mana_color = (unsigned char)s.instance->info_slot;
        break;

      case 0x12c:
      case 0x272:
        s.instance->mana_color = 1;
        break;

      case 0x139:
        s.instance->mana_color = 2;
        break;

      case 0x134:
      case 0x27b:
        s.instance->mana_color = 0x10;
        break;

      case 0xf:
      case 0x193:
        s.instance->mana_color = 0x3e;
        break;

      default:
        if (global_cards_data[s.result].subtype == 0x0c && (s.instance->token_status & 2) != 0)
        {
          s.instance->mana_color = (unsigned char)get_card_color_after_hacks(player, card);
        }
        else
        {
          s.instance->mana_color = global_cards_data[s.result].color;
        }
        break;
      }
    }

    if ((s.instance->token_status & 0x40) != 0 && (global_cards_data[s.instance->internal_card_id].type & TYPE_CREATURE) == 0)
    {
      for (s.test_player = 0; s.test_player < 2; ++s.test_player)
      {
        s.test_instance = global_card_instances[s.test_player];
        for (s.cleanup_card = 0; s.cleanup_card < g_active_cards_count[s.test_player]; ++s.cleanup_card, ++s.test_instance)
        {
          if (s.test_instance->internal_card_id == -1 || (s.test_instance->state & (STATE_OUBLIETTED | STATE_IN_PLAY)) != 2)
          {
            continue;
          }

          if (s.test_instance->damage_target_player == player && s.test_instance->damage_target_card == card &&
              (global_cards_data[s.test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
              global_raw_cards_storage[global_cards_data[s.test_instance->internal_card_id].id].subtype == 0x2c)
          {
            kill_card(s.test_player, s.cleanup_card, KILL_SACRIFICE);
          }
        }
      }
    }

    if ((s.instance->state & 2) != 0)
    {
      g_battlefield_extra_ability_flags |= global_cards_data[s.instance->internal_card_id].extra_ability & 0x1ffc0000;
    }
  }

  g_adventure_duel_internal_card_id = s.current_abilities;
  return s.result;
}

// FUNCTION: MAGIC 0x004427d9
// FUNCTION: SHANDALAR 0x0040e073
int count_colored_cards_in_hand(int player)
{
  struct
  {
    int count;
    int card;
    int internal_card_id;
  } s;

  s.count = 0;
  for (s.card = 0; s.card < g_active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      if ((PLAYER_CARD_INSTANCE(player, s.card).state & STATE_IN_PLAY) == 0)
      {
        if ((global_cards_data[s.internal_card_id].type & 1) != 0)
        {
          if ((char)global_cards_data[s.internal_card_id].color != 0)
          {
            ++s.count;
          }
        }
      }
    }
  }

  return s.count;
}

int find_first_visible_window_index(HWND *windows, int count);
int restack_visible_windows_after(HWND previous_window, HWND *windows, int window_count);

// FUNCTION: MAGIC 0x004428be
// FUNCTION: SHANDALAR 0x0040e158
int dispatch_event(int player, int card, event_t event)
{
  struct
  {
    int result;                   // ebp-0x8
    trigger_t saved_trigger_cond; // ebp-0x4
  } s;

  push_affected_card_stack();

  g_event_result = 0;
  g_affected_card_controller = player;
  g_affected_card = card;
  g_attacking_card_controller = 1 - player;
  g_attacking_card = -1;

  s.saved_trigger_cond = g_trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER)
  {
    g_trigger_condition = -1;
  }

  C_dispatch_event_raw(event);

  g_affected_card_controller = -1;
  g_trigger_condition = s.saved_trigger_cond;

  s.result = g_event_result;
  pop_affected_card_stack();
  return s.result;
}

// FUNCTION: MAGIC 0x0044295b
// FUNCTION: SHANDALAR 0x0040e1f5
void C_dispatch_event_raw(event_t event)
{
  struct
  {
    card_instance_t *instance;
    unsigned int timestamp_slot;
    int card;
    unsigned int saved_trigger_condition;
    int player;
  } s;

  s.saved_trigger_condition = g_adventure_duel_internal_card_id;
  g_ai_search_retry_state = event;
  for (s.timestamp_slot = 0; (int)s.timestamp_slot < 500; ++s.timestamp_slot)
  {
    if (TENTATIVE_timestamp_player[s.timestamp_slot] == -1)
    {
      break;
    }

    s.player = TENTATIVE_timestamp_player[s.timestamp_slot];
    s.card = TENTATIVE_timestamp_card[s.timestamp_slot];
    s.instance = &PLAYER_CARD_INSTANCE(s.player, s.card);
    if (s.instance->timestamp != (int)s.timestamp_slot)
    {
      continue;
    }
    if (s.instance->internal_card_id == -1)
    {
      continue;
    }
    if ((s.instance->state & 0x800000) != 0)
    {
      continue;
    }
    if ((s.instance->state & 2) != 0 || (s.instance->state & 0x20) != 0)
    {

      g_opponent_starting_card_id_3 = s.player * 0x80 + s.card;
      (*global_cards_data[s.instance->internal_card_id].code_pointer)(s.player, s.card, event);
      if (event == EVENT_DECLARE_ATTACKERS && s.player == g_current_player && (s.instance->state & 0x14) == 4 && !has_vigilance(s.player, s.card))
      {
        s.instance->state |= 0x10;
        g_produced_mana_color = -1;
        dispatch_event(s.player, s.card, EVENT_TAP_CARD);
        process_damage_prevention(g_current_player);
        regenerate_or_graveyard_triggers();
      }
    }
  }
  if (event == EVENT_DECLARE_ATTACKERS && s.player == g_current_player)
  {
    reassess_all_cards_and_mana();
  }
  if (g_opponent_starting_card_id_2 != -1)
  {
    (*global_cards_data[g_opponent_starting_card_id_2].code_pointer)(0, 0x94, event);
  }
  g_adventure_duel_internal_card_id = s.saved_trigger_condition;
}

// FUNCTION: MAGIC 0x00442b42
// FUNCTION: SHANDALAR 0x0040e3dc
int dispatch_event_to_single_card(int player,
                                  int card,
                                  event_t event,
                                  int new_attacking_card_controller,
                                  int new_attacking_card)
{
  int saved_unk_008b3270;
  int result;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0 ||
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & UPKEEP_UPKEEP_CANTPAY) != 0)
    {
      return 0;
    }
  }

  push_affected_card_stack();

  g_event_result = 0;
  g_affected_card_controller = player;
  g_affected_card = card;
  g_attacking_card_controller = new_attacking_card_controller;
  g_attacking_card = new_attacking_card;

  saved_unk_008b3270 = g_activation_event_flags;

  result = (*global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer)(player, card, event);

  if (result != 0x63 && (g_land_can_be_played & (LCBP_DAMAGE_PREVENTION | LCBP_SPELL_BEING_PLAYED | LCBP_REGENERATION)) != 0 && (event == EVENT_CAN_CAST || event == EVENT_CAN_ACTIVATE))
  {
    if (is_nonactivated_mana_source(player, card) == 0)
    {
      g_activation_event_flags = saved_unk_008b3270;
      pop_affected_card_stack();
      return 0;
    }
  }

  g_last_single_card_event_result = g_event_result;
  pop_affected_card_stack();
  return result;
}

// FUNCTION: MAGIC 0x00442dac
// FUNCTION: SHANDALAR 0x0040e646
void push_affected_card_stack(void)
{
  if (g_affected_card_context_depth >= 0x20)
    return;

  g_affected_card_context_stack[g_affected_card_context_depth][0] = g_affected_card_controller;
  g_affected_card_context_stack[g_affected_card_context_depth][1] = g_affected_card;
  g_affected_card_context_stack[g_affected_card_context_depth][2] = g_affected_internal_card_id;
  g_affected_card_context_stack[g_affected_card_context_depth][3] = g_affected_card_color;
  g_affected_card_context_stack[g_affected_card_context_depth][4] = g_attacking_card_controller;
  g_affected_card_context_stack[g_affected_card_context_depth][5] = g_attacking_card;
  g_affected_card_context_stack[g_affected_card_context_depth][6] = g_event_result;
  g_affected_card_context_depth += 1;
}

// FUNCTION: MAGIC 0x00442e62
// FUNCTION: SHANDALAR 0x0040e6fc
void pop_affected_card_stack(void)
{
  if (g_affected_card_context_depth > 0)
  {
    g_affected_card_context_depth -= 1;
  }

  g_affected_card_controller = g_affected_card_context_stack[g_affected_card_context_depth][0];
  g_affected_card = g_affected_card_context_stack[g_affected_card_context_depth][1];
  g_affected_internal_card_id = g_affected_card_context_stack[g_affected_card_context_depth][2];
  g_affected_card_color = g_affected_card_context_stack[g_affected_card_context_depth][3];
  g_attacking_card_controller = g_affected_card_context_stack[g_affected_card_context_depth][4];
  g_attacking_card = g_affected_card_context_stack[g_affected_card_context_depth][5];
  g_event_result = g_affected_card_context_stack[g_affected_card_context_depth][6];
}

// FUNCTION: MAGIC 0x00443898
// FUNCTION: SHANDALAR 0x0040f132
unsigned int get_current_stack_action(void)
{
  if (g_stack_size == 0)
  {
    return 0xffffffffU;
  }

  return (unsigned int)g_duel_state_table_0093917c[g_stack_size];
}

// FUNCTION: MAGIC 0x004438cb
// FUNCTION: SHANDALAR 0x0040f165
int refresh_stack_proxy_card(int prompt_flag)
{
  struct
  {
    int card;                    /* ebp-0x18 */
    int saved_parent_controller; /* ebp-0x14 */
    int stack_index;             /* ebp-0x10 */
    int saved_parent_card;       /* ebp-0x0c */
    int saved_timestamp;         /* ebp-0x08 */
    int player;                  /* ebp-0x04 */
  } s;

  s.stack_index = g_stack_size - 1;
  s.player = global_stack_cards[s.stack_index].player;
  s.card = global_stack_cards[s.stack_index].card;

  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == g_stack_proxy_internal_card_id)
  {
    s.saved_parent_card = PLAYER_CARD_INSTANCE(s.player, s.card).parent_card;
    s.saved_parent_controller = PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller;
    s.saved_timestamp = PLAYER_CARD_INSTANCE(s.player, s.card).timestamp;

    memcpy(global_card_instances[s.player] + s.card,
           global_card_instances[PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller] +
               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card,
           300);

    PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id = g_stack_proxy_internal_card_id;
    PLAYER_CARD_INSTANCE(s.player, s.card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).kill_code = 0;
    PLAYER_CARD_INSTANCE(s.player, s.card).state |= 2;

    PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller = s.saved_parent_controller;
    PLAYER_CARD_INSTANCE(s.player, s.card).parent_card = s.saved_parent_card;
    PLAYER_CARD_INSTANCE(s.player, s.card).timestamp = s.saved_timestamp;

    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                             PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
              .internal_card_id;
    }

    if ((int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id < g_damage_card_internal_card_id ||
        (g_damage_card_internal_card_id + 0x2d) <= (int)PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(s.player, s.card).display_pic_csv_id =
          (unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                                                               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
                                              .original_internal_card_id]
              .id;
    }
  }

  if (g_duel_ai_mode_state != 1)
  {
    g_stack_prompt_flags[s.stack_index] = prompt_flag;
  }

  global_stack_damage_targets[s.stack_index].player =
      (int)(char)PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_player;
  global_stack_damage_targets[s.stack_index].card =
      PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_card;
  return 0;
}

// FUNCTION: MAGIC 0x00443ee2
// FUNCTION: SHANDALAR 0x0040f77c
int push_card_onto_stack(int player, int card, int event, int extra, int prompt)
{
  int should_push;
  int stack_card;
  int saved_timestamp;

  if (g_stack_size >= 32)
    return 0;

  g_stack_data[g_stack_size] = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  g_stack_data[g_stack_size] |= event << 16;
  g_stack_data[g_stack_size] |= extra << 24;
  if (event != EVENT_RESOLVE_SPELL && event != EVENT_RESOLVE_TRIGGER && PLAYER_CARD_INSTANCE(player, card).internal_card_id > 4)
  {
    stack_card = add_card_to_hand(player, g_stack_proxy_internal_card_id);
    if (stack_card != -1)
    {
      saved_timestamp = PLAYER_CARD_INSTANCE(player, stack_card).timestamp;
      memcpy(&PLAYER_CARD_INSTANCE(player, stack_card), &PLAYER_CARD_INSTANCE(player, card), sizeof(card_instance_t));
      PLAYER_CARD_INSTANCE(player, stack_card).internal_card_id = g_stack_proxy_internal_card_id;
      PLAYER_CARD_INSTANCE(player, stack_card).unknown0x14 = 0;
      PLAYER_CARD_INSTANCE(player, stack_card).kill_code = 0;
      if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1)
        PLAYER_CARD_INSTANCE(player, stack_card).original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      else
        PLAYER_CARD_INSTANCE(player, stack_card).original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
      *(int *)&PLAYER_CARD_INSTANCE(player, stack_card).display_pic_csv_id =
          *(int *)&PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id;
      PLAYER_CARD_INSTANCE(player, stack_card).state |= 2;
      PLAYER_CARD_INSTANCE(player, stack_card).parent_controller = player;
      PLAYER_CARD_INSTANCE(player, stack_card).parent_card = card;
      PLAYER_CARD_INSTANCE(player, stack_card).timestamp = saved_timestamp;
      should_push = 1;
    }
    else
    {
      should_push = 0;
    }
  }
  else
  {
    stack_card = card;
    should_push = 1;
  }

  if (should_push)
  {
    global_stack_cards[g_stack_size].player = player;
    global_stack_cards[g_stack_size].card = stack_card;
    global_stack_damage_targets[g_stack_size].player = PLAYER_CARD_INSTANCE(player, card).damage_target_player;
    global_stack_damage_targets[g_stack_size].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    if (g_trigger_condition == -1)
    {
      g_stack_trigger_condition[g_stack_size] = g_current_phase;
    }
    else
    {
      g_stack_trigger_condition[g_stack_size] = g_trigger_condition;
    }
    if (g_duel_ai_mode_state != 1)
    {
      g_stack_prompt_flags[g_stack_size] = prompt;
    }
    ++g_stack_size;
    global_stack_cards[g_stack_size].player = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00444443
// FUNCTION: SHANDALAR 0x0040fcdd
int set_stack_damage_targets(void)
{
  struct
  {
    int card;
    int stack_index;
    int player;
  } s;

  for (s.stack_index = 0; s.stack_index < g_stack_size; ++s.stack_index)
  {
    s.player = global_stack_cards[s.stack_index].player;
    s.card = global_stack_cards[s.stack_index].card;
    global_stack_damage_targets[s.stack_index].player =
        (int)(char)PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_player;
    global_stack_damage_targets[s.stack_index].card =
        PLAYER_CARD_INSTANCE(s.player, s.card).damage_target_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00444505
// FUNCTION: SHANDALAR 0x0040fd9f
int resolve_top_card_on_stack(void)
{
  struct
  {
    int player;           /* [ebp-0xc] */
    int current_internal; /* [ebp-0x8] */
    int card;             /* [ebp-0x4] */
  } s;

  if (g_stack_size <= 0)
  {
    return 0;
  }

  --g_stack_size;
  s.player = global_stack_cards[g_stack_size].player;
  s.card = global_stack_cards[g_stack_size].card;
  s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id;
  if (s.current_internal == g_stack_proxy_internal_card_id)
  {
    s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id;
  }
  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
  {
    if ((((unsigned int)g_stack_data[g_stack_size] >> 0x10) & 0xff) == 0x7e)
    {
      dispatch_card_trigger_event(s.player,
                                  s.card,
                                  ((unsigned int)g_stack_data[g_stack_size] >> 0x10) & 0xff,
                                  ((int)g_stack_data[g_stack_size]) >> 0x18);
    }
    else
    {
      if ((PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 8) != 0)
      {
        if ((PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 0x200) == 0)
        {
          dispatch_event_to_single_card(s.player, s.card, EVENT_UPKEEP_COSTS_UNPAID, 1 - s.player, -1);
          if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1 &&
              (PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 2) != 0)
          {
            kill_card(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                      PLAYER_CARD_INSTANCE(s.player, s.card).parent_card,
                      KILL_BURY);
          }
        }
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                             PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
            .upkeep_flags &= ~0x208;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                             PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
            .upkeep_flags |= 4;
      }
      else
      {
        if ((PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 0x80) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 0x40) != 0)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                                 PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
                .state &= ~0x10;
            dispatch_event_to_single_card(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                                          PLAYER_CARD_INSTANCE(s.player, s.card).parent_card,
                                          EVENT_UNTAP_CARD,
                                          1 - s.player,
                                          -1);
          }
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(s.player, s.card).parent_controller,
                               PLAYER_CARD_INSTANCE(s.player, s.card).parent_card)
              .upkeep_flags &= ~0x80;
        }
        else
        {
          dispatch_event_to_single_card(s.player,
                                        s.card,
                                        ((unsigned int)g_stack_data[g_stack_size] >> 0x10) & 0xff,
                                        1 - s.player,
                                        -1);
        }
      }
    }
    if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == g_stack_proxy_internal_card_id)
    {
      kill_card(s.player, s.card, KILL_REMOVE);
    }
  }
  global_stack_cards[g_stack_size].player = -1;
  reassess_all_cards_and_mana();
  if (g_nested_trigger_depth <= 1 && ((g_land_can_be_played & LCBP_REGENERATION) == 0 || g_stack_size == 0))
  {
    process_damage_prevention(g_current_player);
    regenerate_or_graveyard_triggers();
  }

  return 0;
}

// FUNCTION: MAGIC 0x00444b74
// FUNCTION: SHANDALAR 0x0041040d
int obliterate_top_card_of_stack(void)
{
  struct
  {
    int card;
    int current_internal;
    int player;
  } s;

  if (g_stack_size <= 0)
  {
    return 0;
  }

  --g_stack_size;
  s.player = global_stack_cards[g_stack_size].player;
  s.card = global_stack_cards[g_stack_size].card;
  s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id;
  if (g_stack_proxy_internal_card_id == s.current_internal)
  {
    PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id = -1;
  }
  global_stack_cards[g_stack_size].player = -1;

  return 0;
}

// FUNCTION: MAGIC 0x00444c43
// FUNCTION: SHANDALAR 0x004104db
char *format_stack_action_text(char *out, int msg, int player, int card)
{
  struct
  {
    char tmp[300];
    char *prefix;
  } s;

  if (out == (char *)0)
  {
    return (char *)0;
  }

  if (msg == 0x71)
  {
    s.prefix = gs_casting_008b4850;
  }
  else if (msg == 0x72)
  {
    s.prefix = gs_activating_007912e0;
  }
  else if (msg == 0x7e)
  {
    s.prefix = gs_processing_008ce290;
  }
  else
  {
    s.prefix = "";
  }

  if (player >= 0 && card >= 0)
  {
    sprintf(s.tmp, "%s: %s", s.prefix, get_displayed_card_name(player, card));
  }
  else
  {
    strcpy(s.tmp, s.prefix);
  }

  strcpy(out, s.tmp);
  return out;
}
// FUNCTION: MAGIC 0x00444d1f
// FUNCTION: SHANDALAR 0x004105b7
void reset_trigger_dispatch_state(void)
{
  g_trigger_dispatch_depth = g_max_trigger_dispatch_depth = 0;
  g_regenerate_or_graveyard_triggers_in_progress = 0;
}

// FUNCTION: MAGIC 0x00444d48
// FUNCTION: SHANDALAR 0x004105e0
int allow_response(int response_player, int phase, char *prompt, int event_code)
{
  int saved_allow_response_flag = g_phase_stop_suppressed;

  if (g_stack_size == 0)
  {
    if (should_skip_phase(g_current_player) != 0 && response_player != -2 && g_duel_ai_mode_state != 1)
    {
      g_phase_stop_suppressed = 1;
    }
  }

  {
    int result;

    do
    {
      g_response_action_taken = 0;
      result = allow_response_once(response_player, phase, prompt, event_code);
    } while (g_response_action_taken != 0 && g_stack_size <= 0);

    if (g_stack_size == 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        g_phase_stop_suppressed = 0;
      }
      {
        g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] = (int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] & ~PHASE_STOP_SUPPRESSED;
      }
    }
    else if (g_duel_ai_mode_state != 1)
    {
      g_phase_stop_suppressed = saved_allow_response_flag;
    }

    return result;
  }
}

// FUNCTION: MAGIC 0x00444e5f
// FUNCTION: SHANDALAR 0x004106f7
int allow_response_once(int response_player, int phase, char *prompt, int event_code)
{
  /* Stack layout is sensitive here; keep locals in one struct to match /Od codegen. */
  struct
  {
    int saved_DAT_007aadec;          /* ebp-0xa4 */
    int saved_unk_008b3270;          /* ebp-0xa0 */
    unsigned int dispatch_result;    /* ebp-0x9c */
    int result;                      /* ebp-0x98 */
    int saved_DAT_00777aa8_in_loop;  /* ebp-0x94 */
    int saved_DAT_00777aa8;          /* ebp-0x90 */
    int saved_phase_stop_suppressed; /* ebp-0x8c */
    char prompt_copy[128];           /* ebp-0x88 */
    int saved_trigger_condition;     /* ebp-0x8 */
    int saved_DAT_00561390;          /* ebp-0x4 */
  } s;

  s.saved_trigger_condition = g_trigger_condition;
  s.saved_DAT_007aadec = g_current_action_event_code;
  g_trigger_condition = -1;
  s.saved_unk_008b3270 = g_activation_event_flags;

  ++g_trigger_dispatch_depth;
  if (g_trigger_dispatch_depth == 1)
  {
    g_max_trigger_dispatch_depth = 0;
  }
  else if (g_max_trigger_dispatch_depth < g_trigger_dispatch_depth && phase != 0x8e && phase != 0x70 && phase != 0xd3)
  {
    g_max_trigger_dispatch_depth = g_trigger_dispatch_depth;
  }

  s.result = 0;
  g_current_action_event_code = event_code;
  s.saved_DAT_00777aa8 = g_ai_decision_parameter;
  s.saved_DAT_00561390 = g_affected_card_context_flags;

  if (response_player == -2 && should_skip_phase(g_current_player) == 0)
  {
    g_affected_card_context_flags = 1;
  }
  else
  {
    g_affected_card_context_flags = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (should_skip_phase(g_current_player) == 0 &&
        ((((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0) ||
         (g_current_player == g_stop_phase_player && g_stop_phase == phase)))
    {
      g_ai_decision_parameter = 1;
    }
    else
    {
      g_ai_decision_parameter = 0;
    }
  }

  if (response_player > -1)
  {
    if (g_ai_decision_parameter == 0)
    {
      goto cleanup;
    }
  }

  strcpy(s.prompt_copy, prompt);

  if (g_current_phase == PHASE_UPKEEP && g_trigger_dispatch_depth == 1)
  {
    g_event_player = g_current_player;
    reset_upkeep_costs();
  }

  while (1)
  {
  restart_response_window:
    do
    {
      g_ai_decision_parameter = can_stop_for_phase(1);

      if ((g_duel_network_flags & 2) != 0)
      {
        g_response_processing_flags = 1;
      }
      else
      {
        if (g_current_player != 0)
        {
          if (response_player > -1)
          {
            g_response_processing_flags = 0;
          }
          else
          {
            g_response_processing_flags = 2;
          }
        }
        else
        {
          g_response_processing_flags = 1;
        }
      }

      if (g_trigger_dispatch_depth <= 1)
      {
        g_response_action_mask = 0xffffffff;
      }

      g_spell_fizzled = 0;
      g_activation_event_flags = g_spell_fizzled;

      s.dispatch_result = (unsigned int)process_response_actions(g_current_player, s.prompt_copy);
      if (s.dispatch_result != 0)
      {
        g_response_action_taken = 1;
      }

      if (((g_active_player == g_current_player) ||
           (g_other_player == g_current_player && (g_duel_network_flags & 2) != 0)) &&
          s.dispatch_result != 0 && g_duel_ai_mode_state != 1)
      {
        s.result = 1;
      }

      if (g_trigger_dispatch_depth < g_max_trigger_dispatch_depth && g_stack_size >= 0)
      {
        s.dispatch_result = 0;
      }

    } while (s.dispatch_result != 0 || ((g_activation_event_flags & 1) != 0 && g_trigger_dispatch_depth == 1));

    if (g_current_phase == PHASE_UPKEEP && g_trigger_dispatch_depth == 1)
    {
      g_event_player = 1 - g_current_player;
      reset_upkeep_costs();
    }

    while (1)
    {
      s.saved_DAT_00777aa8_in_loop = g_ai_decision_parameter;
      s.saved_phase_stop_suppressed = g_phase_stop_suppressed;

      if ((g_duel_network_flags & 2) != 0)
      {
        g_response_processing_flags = 1;
        g_phase_stop_suppressed = 0;

        if (g_stack_size == 0 && should_skip_phase(1 - g_current_player) != 0 && response_player != -2)
        {
          g_phase_stop_suppressed = 1;
        }

        g_ai_decision_parameter = can_stop_for_phase(0);
      }
      else
      {
        if (g_current_player != 0)
        {
          g_response_processing_flags = 1;
        }
        else
        {
          if (response_player > -1)
          {
            g_response_processing_flags = 0;
          }
          else
          {
            g_response_processing_flags = 2;
          }
        }
      }

      if (g_trigger_dispatch_depth <= 1)
      {
        g_response_action_mask = 0xffffffff;
      }

      g_spell_fizzled = 0;
      g_activation_event_flags = g_spell_fizzled;

      if ((g_trigger_dispatch_depth < g_max_trigger_dispatch_depth && g_stack_size >= 0))
      {
        goto cleanup;
      }

      s.dispatch_result = (unsigned int)process_response_actions(1 - g_current_player, s.prompt_copy);
      g_ai_decision_parameter = s.saved_DAT_00777aa8_in_loop;
      g_phase_stop_suppressed = s.saved_phase_stop_suppressed;
      if (s.dispatch_result != 0)
      {
        g_response_action_taken = 1;

        if (g_duel_ai_mode_state != 1)
        {
          goto restart_response_window;
        }

        if (g_active_player == g_current_player)
        {
          goto restart_response_window;
        }
        if ((g_duel_network_flags & 2) != 0)
        {
          goto restart_response_window;
        }
        if ((g_activation_event_flags & 1) != 0)
        {
          if (g_trigger_dispatch_depth == 1)
          {
            goto restart_response_window;
          }
        }
        goto cleanup;
      }
      else
      {
        goto cleanup;
      }
    }
  }

cleanup:
  --g_trigger_dispatch_depth;
  if (g_trigger_dispatch_depth != 0)
  {
    g_response_action_mask = 0xffffffff;
  }
  else
  {
    g_response_card_type_mask = 0xffffffff;
    g_response_action_mask = g_response_card_type_mask;
    if (g_nested_trigger_depth == 0)
    {
      g_response_window_serial = 0;
      if (g_duel_ai_mode_state != 1)
      {
        g_phase_stop_suppressed = 0;
      }
    }
  }

  g_ai_decision_parameter = s.saved_DAT_00777aa8;
  g_affected_card_context_flags = s.saved_DAT_00561390;
  g_activation_event_flags = s.saved_unk_008b3270;
  g_trigger_condition = s.saved_trigger_condition;
  g_current_action_event_code = s.saved_DAT_007aadec;

  if (s.result != 0)
  {
    g_ai_action_replay_available = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0044541f
// FUNCTION: SHANDALAR 0x00410cb8
int can_stop_for_phase(int use_current_player_stops)
{
  int can_respond;

  can_respond = 0;
  if (use_current_player_stops != 0)
  {
    if (g_current_player == g_active_player &&
        (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0)
    {
      can_respond = 1;
    }

    if (g_current_player == g_other_player &&
        (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0)
    {
      can_respond = 1;
    }

    if (g_current_player == g_active_player)
    {
      if (g_current_player == g_stop_phase_player && g_current_phase == g_stop_phase)
      {
        can_respond = 1;
      }
    }
    else if (g_current_player == g_previous_stop_phase_player && g_previous_stop_phase == g_current_phase)
    {
      can_respond = 1;
    }
    if (should_skip_phase(g_current_player) == 0 && can_respond != 0)
    {
      return 1;
    }
    return 0;
  }

  /* use_current_player_stops == 0 */
  if (g_current_player == g_active_player &&
      (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0)
  {
    can_respond = 1;
  }

  if (g_current_player == g_other_player &&
      (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0)
  {
    can_respond = 1;
  }

  if (g_current_player == g_other_player)
  {
    if (g_current_player == g_stop_phase_player && g_current_phase == g_stop_phase)
    {
      can_respond = 1;
    }
  }
  else if (g_current_player == g_previous_stop_phase_player && g_previous_stop_phase == g_current_phase)
  {
    can_respond = 1;
  }

  if (should_skip_phase(1 - g_current_player) == 0 && can_respond != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00445663
// FUNCTION: SHANDALAR 0x00410efc
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4)
{
  dispatch_trigger(reason_for_trig, trig, prompt, a4);
  dispatch_trigger(1 - reason_for_trig, trig, prompt, a4);
  return 1;
}

// FUNCTION: MAGIC 0x004456ad
// FUNCTION: SHANDALAR 0x00410f46
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response)
{
  struct
  {
    int saved_trig;          // ebp-0x24
    int saved_DAT_00789714;  // ebp-0x20
    int saved_DAT_007aadec;  // ebp-0x1c
    int result;              // ebp-0x18
    int loop_card;           // ebp-0x14
    int saved_DAT_007ab2bc;  // ebp-0x10
    unsigned int saved_ui;   // ebp-0xc
    int saved_current_turn;  // ebp-0x8
    int saved_current_phase; // ebp-0x4
  } s;

  ++g_nested_trigger_depth;

  s.saved_current_phase = g_current_phase;
  s.saved_DAT_007ab2bc = g_activation_dispatch_flags;
  s.saved_current_turn = g_current_turn;
  s.saved_DAT_007aadec = g_current_action_event_code;

  g_current_action_event_code = trig;
  g_current_turn = player;

  s.saved_DAT_00789714 = g_response_processing_flags;
  s.saved_trig = trig;

  do
  {
    if ((g_duel_network_flags & 2) != 0)
    {
      g_response_processing_flags = 1;
    }
    else if (player != 0)
    {
      g_response_processing_flags = 2;
    }
    else
    {
      g_response_processing_flags = 1;
    }

    g_trigger_condition = (trigger_t)s.saved_trig;

    s.saved_ui = g_response_action_mask;
    if (TENTATIVE_allow_response != 0)
    {
      g_response_action_mask = 0x30;
    }
    else
    {
      g_response_action_mask = 0;
    }

    g_spell_fizzled = 0;
    g_duel_state_current_card = g_spell_fizzled;
    g_activation_dispatch_flags = g_duel_state_current_card;

    s.result = process_response_actions(player, prompt);

    g_response_action_mask = s.saved_ui;
    g_response_card_type_mask = g_response_action_mask & 0x30;
    g_response_processing_flags = s.saved_DAT_00789714;

    {
      int mask;
      mask = (s.result == 1);
      mask -= 1;
      mask &= ~1;
      mask += 6;

      if ((g_activation_dispatch_flags & mask) != 0)
      {
        continue;
      }
    }

    if (TENTATIVE_allow_response != 0 && s.result != 0)
    {
      continue;
    }

    break;

  } while (1);

  g_trigger_condition = (trigger_t)-1;
  --g_nested_trigger_depth;

  if (g_nested_trigger_depth == 0)
  {
    for (player = 0; player < 2; ++player)
    {
      for (s.loop_card = 0; s.loop_card < g_active_cards_count[player]; ++s.loop_card)
      {
        PLAYER_CARD_INSTANCE(player, s.loop_card).state &= ~0x100;
      }
    }

    if (g_trigger_dispatch_depth == 0)
    {
      g_response_window_serial = 0;
      if (g_duel_ai_mode_state != 1)
      {
        g_phase_stop_suppressed = 0;
      }
    }
  }

  g_activation_dispatch_flags = s.saved_DAT_007ab2bc;
  g_current_turn = s.saved_current_turn;
  g_current_action_event_code = s.saved_DAT_007aadec;
  g_current_phase = s.saved_current_phase;

  return 0;
}

// FUNCTION: MAGIC 0x00445b56
// FUNCTION: SHANDALAR 0x004113ef
int can_pay_untap_cost(int player, int card)
{
  struct
  {
    int total_paid;
    color_t current_color;
  } s;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    return 0;
  }

  s.total_paid = 0;
  s.current_color = COLOR_BLACK;
  for (; s.current_color < 7; ++s.current_color)
  {
    if (PLAYER_CARD_INSTANCE(player, card).mana_to_untap[s.current_color] != 0)
    {
      if (has_mana(player, s.current_color,
                   PLAYER_CARD_INSTANCE(player, card).mana_to_untap[s.current_color]) == 0)
      {
        return 0;
      }
      s.total_paid += PLAYER_CARD_INSTANCE(player, card).mana_to_untap[s.current_color];
    }
  }

  if (has_mana(player, COLOR_ANY, PLAYER_CARD_INSTANCE(player, card).mana_to_untap[0] + s.total_paid) == 0)
  {
    return 0;
  }

  dispatch_event_to_single_card(player, card, EVENT_CHECK_UNTAP_PAYMENT, 1 - player, -1);
  if (g_last_single_card_event_result != 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x004460d3
// FUNCTION: SHANDALAR 0x0041196b
void reset_upkeep_costs(void)
{
  int player;
  int card;
  int i;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < g_active_cards_count[player]; ++card)
    {
      if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & UPKEEP_UPKEEP_DONE) == 0)
      {
        if (is_in_play(player, card))
        {
          for (i = 0; i < 7; ++i)
          {
            PLAYER_CARD_INSTANCE(player, card).mana_to_untap[i] = 0;
            PLAYER_CARD_INSTANCE(player, card).upkeep_cost[i] = PLAYER_CARD_INSTANCE(player, card).mana_to_untap[i];
          }

          PLAYER_CARD_INSTANCE(player, card).upkeep_flags = 0;
          dispatch_event(player, card, 0x85);
          dispatch_event(player, card, 0x84);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00447f80
int get_duel_time_units(void)
{
  int ticks = GetTickCount();
  int diff = ticks - g_affected_card_context_flags_0093f4b4 - g_duel_tick_adjustment;
  int result = diff / 0x37;
  return result;
}

// FUNCTION: MAGIC 0x004482f3
// FUNCTION: SHANDALAR 0x00451e70
int copy_mana_pool_to_display(void)
{
  if (g_duel_ai_mode_state != 1)
  {
    EnterCriticalSection(&g_duel_render_lock);
    memcpy(g_duel_cached_raw_mana_player_0, g_raw_mana_available[0], sizeof(g_duel_cached_raw_mana_player_0));
    memcpy(g_duel_cached_raw_mana_player_1, g_raw_mana_available[1], sizeof(g_duel_cached_raw_mana_player_1));
    LeaveCriticalSection(&g_duel_render_lock);
    SendMessageA(g_duel_active_popup_window, 0x432, 0, 0);
    SendMessageA(g_duel_last_active_window, 0x432, 0, 0);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044aa01
// FUNCTION: SHANDALAR 0x00403381
int should_skip_phase(int player)
{
  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_stop_phase == -1)
    {
      if (g_previous_stop_phase == -1)
      {
        return 0;
      }
    }

    if (g_duel_ai_mode_state == 1)
    {
      return 0;
    }

    return 0;

    /* Dead code (kept for matching): MSVC /Od still emits this after the return. */
    if (g_previous_stop_phase == g_current_phase && g_current_player == g_previous_stop_phase_player)
    {
      return 0;
    }

    if (g_stop_phase == g_current_phase && g_current_player == g_stop_phase_player)
    {
      return 0;
    }

    if (g_previous_stop_phase == -1 && player == g_other_player)
    {
      if (((char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] & PHASE_STOP_OPPONENT) != 0)
      {
        return 0;
      }
    }

    if (g_stop_phase == -1 && player == g_active_player)
    {
      if (((char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] & PHASE_STOP_ENABLED) != 0)
      {
        return 0;
      }
    }

    if (!(g_stop_phase == g_current_phase && g_current_player == g_stop_phase_player))
    {
      if (g_previous_stop_phase == -1)
      {
        return 1;
      }
    }

    if (!(g_previous_stop_phase == g_current_phase && g_current_player == g_previous_stop_phase_player))
    {
      if (g_stop_phase == -1)
      {
        return 1;
      }
    }

    if (g_trigger_condition != -1 && g_stop_phase == g_current_phase)
    {
      return 1;
    }

    if (g_trigger_condition != -1 && g_previous_stop_phase == g_current_phase)
    {
      return 1;
    }

    return 0;
  }

  if (g_stop_phase == -1)
  {
    return 0;
  }
  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  if (g_stop_phase > g_current_phase)
  {
    return 1;
  }
  if (g_current_player != g_stop_phase_player)
  {
    return 1;
  }

  if (g_trigger_condition != -1)
  {
    if (g_stop_phase == g_current_phase)
    {
      return 1;
    }
  }

  if (g_stop_phase < g_current_phase)
  {
    if (g_current_player == g_stop_phase_player)
    {
      g_stop_phase_player = -1;
      g_stop_phase = g_stop_phase_player;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044ac96
// FUNCTION: SHANDALAR 0x00403615
int player_can_stop_at_phase(int player, phase_t phase)
{
  struct
  {
    phase_t mapped_phase; /* ebp-0x8 */
    int result;           /* ebp-0x4 */
  } s;

  s.result = 0;

  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  if (g_phase_stop_suppressed != 0)
  {
    return 0;
  }

  if (is_phase_stop_allowed_for_current_event() == 0)
  {
    return 0;
  }

  if (g_phase_response_window_open == 0)
  {
    return 0;
  }

  if (phase >= PHASE_BEGIN_UPKEEP && phase <= PHASE_UNKOWN5)
  {
    s.mapped_phase = PHASE_UPKEEP;
  }
  else if (phase >= PHASE_CLEANUP2 && phase <= PHASE_DAMAGE_PREVENTION)
  {
    s.mapped_phase = PHASE_CLEANUP2;
  }
  else
  {
    s.mapped_phase = phase;
  }

  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_current_player == player)
    {
      if ((g_active_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_active_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0) ||
          (g_other_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_other_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0))
      {
        s.result = 1;
      }
    }
    else
    {
      if ((g_other_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_active_player].phase_flags[g_current_phase]) & PHASE_STOP_OPPONENT) != 0) ||
          (g_active_player == player &&
           (((int)(char)g_duel_phase_stop_settings[g_other_player].phase_flags[g_current_phase]) & PHASE_STOP_ENABLED) != 0))
      {
        s.result = 1;
      }
    }
  }
  else
  {
    if (g_stop_phase == s.mapped_phase)
    {
      s.result = 1;
    }

    if (g_stop_phase == -1 && g_active_player == player &&
        (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[s.mapped_phase]) & PHASE_STOP_ENABLED) != 0)
    {
      s.result = 1;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0044af2d
// FUNCTION: SHANDALAR 0x004038a8
int prompt_stop_phase_anyway(phase_t phase)
{
  if (g_duel_ai_mode_state != 1 &&
      (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT)) != 0 ||
       (phase == g_stop_phase && g_current_player == g_stop_phase_player) ||
       (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player)))
  {
    load_text(global_ui_strings_filename, "PROMPT_STOPANYWAY");

    if (phase == 4)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[1]);
    }
    else if (phase == 1)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[2]);
    }
    else if (phase == 10)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[3]);
    }
    else if (phase == 0x14)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[4]);
    }
    else if (phase == 0x1f)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[5]);
    }
    else if (phase == 0x22)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[6]);
    }
    else if (phase == 0x19)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[7]);
    }
    else if (phase == 0x1a)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[8]);
    }
    else if (phase == 0x1b)
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[8]);
    }
    else
    {
      strcpy(g_prompt_stop_phase_anyway_text, g_text_lines[0]);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      if (g_active_player == g_current_player)
      {
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == g_stop_phase && g_current_player == g_stop_phase_player))
        {
          prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player))
        {
          prompt_player_for_stop_action(g_other_player, g_prompt_stop_phase_anyway_text);
        }
      }
      else
      {
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_OPPONENT) != 0 ||
            (phase == g_previous_stop_phase && g_current_player == g_previous_stop_phase_player))
        {
          prompt_player_for_stop_action(g_other_player, g_prompt_stop_phase_anyway_text);
        }
        if (((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[phase] & PHASE_STOP_ENABLED) != 0 ||
            (phase == g_stop_phase && g_current_player == g_stop_phase_player))
        {
          prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
        }
      }
    }
    else
    {
      prompt_player_for_stop_action(g_active_player, g_prompt_stop_phase_anyway_text);
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x0044b2b3
// FUNCTION: SHANDALAR 0x00403c34
void prompt_player_for_stop_action(int player, char *prompt)
{
  struct
  {
    int selected_card;
    char prompt_copy[100];
    int done;
  } s;

  strcpy(s.prompt_copy, prompt);
  s.done = 0;

  while (s.done == 0)
  {
    if (g_active_player == g_current_player)
    {
      g_stop_phase = g_stop_phase_player = -1;
    }
    else
    {
      g_previous_stop_phase = g_previous_stop_phase_player = -1;
    }

    s.selected_card = select_card_for_action(player, player, player, 0xff, 0, s.prompt_copy, 2);
    switch (g_target_selection_status_code)
    {
    case 0:
      if (s.selected_card == -1)
      {
      }
      else
      {
        if (can_activate_mana_source_for_stop_prompt(g_target_player_choice, s.selected_card) != 0)
        {
          activate_mana_source_card(g_target_player_choice, s.selected_card);
        }
        else
        {
        }
      }
      break;

    case -2:
      s.done = 1;
      break;

    case -3:
      break;
    }
  }
}

// FUNCTION: MAGIC 0x0044b646
// FUNCTION: SHANDALAR 0x00403fc7
int contains_player_card_pair(int *card_pairs, int card_pair_count, int player, int card)
{
  struct
  {
    int card_pair_index;
    int result;
  } s;

  s.result = 0;
  for (s.card_pair_index = 0; s.card_pair_index < card_pair_count; s.card_pair_index = s.card_pair_index + 1)
  {
    if ((card_pairs[s.card_pair_index * 2] == player) &&
        (card_pairs[s.card_pair_index * 2 + 1] == card))
    {
      s.result = 1;
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x00485060
// FUNCTION: SHANDALAR 0x00488b70
int dispatch_function_to_all_cards_in_play(int player, int card, in_play_card_callback_t callback, int who_to_check)
{
  int saved_event_result;
  int callback_result;
  int test_player;
  int test_card;
  int result;

  saved_event_result = g_event_result;
  callback_result = 0;
  g_event_result = -1;
  for (test_player = player;
       test_player > -1 && test_player < 2 && callback_result == 0;
       test_player += (player == 0) ? 1 : -1)
  {
    if (who_to_check == -1 || test_player == who_to_check)
    {
      for (test_card = 0; test_card < g_active_cards_count[test_player] && callback_result == 0; ++test_card)
      {
        if (is_in_play(test_player, test_card))
        {
          callback_result = callback(player,
                                     card,
                                     test_player,
                                     test_card,
                                     global_card_instances[test_player][test_card].internal_card_id);
        }
      }
    }
  }

  result = g_event_result;
  g_event_result = saved_event_result;
  return result;
}

// FUNCTION: MAGIC 0x004ad7e0
// FUNCTION: SHANDALAR 0x0046a7c0
int process_response_actions(int reason_for_trigger_controller, const char *prompt)
{
  struct
  {
    int selection_done;           // local_29c
    char prompt_format[300];      // local_29c
    unsigned int selected_card;   // local_170
    int saved_unk_00742f60;       // local_16c
    unsigned int maybe_card;      // local_168
    int some_flag;                // local_164
    int response_code;            // local_160
    unsigned int card;            // local_15c
    int saved_land_can_be_played; // local_158
    unsigned int response_mask;   // local_154
    int player;                   // local_150
    unsigned int result_flags;    // local_14c
    char prompt_copy[300];        // local_148
    int internal_card_id;         // local_1c
    int interrupts_available;     // local_18
    int selected_player;          // local_14
    int can_respond;              // local_10
    int maybe_player;             // local_c
    int num_candidates;           // local_8
  } s;

  if (g_life[g_active_player] <= 0)
  {
    g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
        (unsigned char)(((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) | PHASE_STOP_SUPPRESSED);
  }

  s.interrupts_available = 0;
  if (g_duel_ai_mode_state != 1 && g_stop_phase_player == g_current_player && g_current_phase == g_stop_phase)
  {
    g_phase_stop_suppressed = 0;
  }

  s.can_respond = player_can_stop_at_phase(reason_for_trigger_controller, g_current_phase);

  strcpy(s.prompt_copy, prompt);

  s.saved_land_can_be_played = g_land_can_be_played;
  s.saved_unk_00742f60 = g_event_player;
  s.selected_player = reason_for_trigger_controller;
  g_event_player = s.selected_player;
  s.player = g_event_player;

  g_response_selection_in_progress = 1;
  g_response_card_type_mask = g_response_action_mask & 0x30;
  g_response_action_selected = 0;

  ++g_response_window_depth;
  if (g_response_window_serial > g_response_window_depth)
  {
    g_response_window_serial = 0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    g_response_activation_phase_mask = 2;
    g_response_source_phase_mask = 0x20;

    if (g_current_phase >= PHASE_DECLARE_BLOCKERS)
    {
      g_response_activation_phase_mask = 4;
      g_response_source_phase_mask = 0x40;
    }
    if (g_current_phase <= PHASE_MAIN1)
    {
      g_response_activation_phase_mask = 1;
      g_response_source_phase_mask = 0x10;
    }
    if (g_current_phase >= PHASE_MAIN2)
    {
      g_response_activation_phase_mask = 8;
      g_response_source_phase_mask = (int)0xffffff80;
    }
    if (g_current_phase == PHASE_DISCARD)
    {
      g_response_activation_phase_mask = 0xf;
      g_response_source_phase_mask = (int)0xfffffff0;
    }
    if (g_other_player == reason_for_trigger_controller && global_stack_cards[0].player == g_active_player)
    {
      g_response_activation_phase_mask = 0xf;
      g_response_source_phase_mask = (int)0xfffffff0;
    }
  }
  else
  {
    g_response_activation_phase_mask = 0xf;
    g_response_source_phase_mask = (int)0xfffffff0;
  }

  if ((g_duel_network_flags & 2) == 0)
  {
    if (g_duel_ai_mode_state == 1 || g_ai_action_replay_available != 0 ||
        (g_trigger_condition != -1 && g_current_turn == g_other_player) || g_current_action_event_code == 4 ||
        (g_other_player == g_event_player && (g_land_can_be_played & LCBP_REGENERATION) != 0))
    {
      s.selected_card = choose_response_card(s.player);
      s.selected_player = g_target_player_choice;
    }
    else
    {
      s.selected_card = 0xffffffffU;
    }

    s.result_flags = 0;

    if (s.selected_card != 0xffffffffU &&
        dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) == 2)
    {
      resolve_card_immediately(s.selected_player, (int)s.selected_card, s.player);
      s.selected_card = 0xffffffffU;
      TENTATIVE_reassess_all_cards(0, 0xff);
    }

    if (s.selected_card != 0xffffffffU)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;
      g_selected_response_internal_card_id = s.internal_card_id;

      if (is_in_play(s.selected_player, (int)s.selected_card))
      {
        if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)g_trigger_condition &&
            pending_killed_card_handler != NULL && g_trigger_condition != -1)
        {
          if (pending_killed_card_handler != NULL)
          {
            pending_killed_card_handler(s.selected_player, (int)s.selected_card);
          }
        }
        else
        {
          dispatch_event_to_single_card(s.selected_player, (int)s.selected_card, 0x73, 1 - s.selected_player, -1);
          if (activate(s.player, s.selected_player, (int)s.selected_card))
          {
            resolve_activated_ability(s.selected_player, (int)s.selected_card);
          }
          g_spell_fizzled = 0;
        }

        s.result_flags = 1;
        TENTATIVE_reassess_all_cards(0, 0xff);
      }
      else
      {
        if (resolve_cast_card(s.selected_player, (int)s.selected_card))
        {
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            g_response_card_type_mask = g_response_action_mask & 0x20;
          }

          s.result_flags = 1;
          TENTATIVE_reassess_all_cards(0, 0xff);

          if (g_duel_ai_mode_state != 1 && internal_rand(3) == 0)
          {
            show_opponent_taunt("Didn't expect that, did ya?");
          }
        }
      }
    }
  }
  else
  {
    s.result_flags = 0;
  }

  s.num_candidates = s.response_mask = 0;
  s.maybe_card = 0xffffffffU;

  if (g_duel_ai_mode_state != 1 ||
      (g_trigger_condition != -1 &&
       (g_current_turn == g_active_player || (g_current_turn == g_other_player && (g_duel_network_flags & 2) != 0))))
  {
    if (((g_current_turn == g_active_player) || (g_current_turn == g_other_player && (g_duel_network_flags & 2) != 0)) &&
        g_trigger_condition != -1)
    {
      for (s.player = 0; s.player <= 1; ++s.player)
      {
        for (s.card = 0; (int)s.card < g_active_cards_count[s.player]; ++s.card)
        {
          if (is_in_play(s.player, (int)s.card))
          {
            s.response_code = dispatch_card_trigger_event(s.player, (int)s.card, 0x7d, reason_for_trigger_controller);
            if (s.response_code)
            {
              if (s.response_code == 2)
              {
                s.maybe_card = s.card;
                s.maybe_player = s.player;
                s.selected_player = s.player;
                ++s.num_candidates;
              }
              else
              {
                s.response_mask |= 1U << (unsigned char)s.response_code;
              }
            }
          }

          if ((g_response_processing_flags & 1) != 0 &&
              PLAYER_CARD_INSTANCE(s.player, (int)s.card).unknown0x14 == (unsigned int)g_trigger_condition &&
              g_current_turn == s.player && g_trigger_condition != -1 &&
              PLAYER_CARD_INSTANCE(s.player, (int)s.card).internal_card_id != -1)
          {
            s.response_mask |= 4;
            s.maybe_card = s.card;
            s.maybe_player = s.player;
            s.selected_player = s.player;
            ++s.num_candidates;
          }
        }
      }
    }

    if (s.maybe_card == 0xffffffffU)
    {
      if (g_duel_ai_mode_state != 1)
      {
        if (g_trigger_condition == 0xca && (char)g_duel_phase_stop_settings[g_current_player].phase_flags[PHASE_UPKEEP] == 0)
        {
          g_response_processing_flags = 0;
        }
        if (g_trigger_condition == 0xce && (char)g_duel_phase_stop_settings[g_current_player].phase_flags[PHASE_DRAW] == 0)
        {
          g_response_processing_flags = 0;
        }
      }
    }

    if (s.maybe_card != 0xffffffffU || g_duel_ai_mode_state != 1)
    {
      if (s.maybe_card != 0xffffffffU || s.response_mask != 0 || ((g_response_processing_flags & 1) != 0 && g_response_action_mask != 0))
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          s.player = g_active_player;
        }
        else
        {
          s.player = reason_for_trigger_controller;
        }

        g_response_processing_flags = g_activation_event_flags = 0;
        s.some_flag = 0;

        for (s.card = 0; (int)s.card < g_active_cards_count[s.player]; ++s.card)
        {
          if (PLAYER_CARD_INSTANCE(s.player, (int)s.card).internal_card_id != -1 &&
              ((PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 1) != 0 ||
               (PLAYER_CARD_INSTANCE(s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
               (!is_nonactivated_mana_source(s.player, (int)s.card) && s.player == g_event_player)))
          {
            s.response_code = get_card_response_action_type(s.player, (int)s.card);

            if (((s.response_code >= 2) ||
                 (((g_ai_decision_parameter != 0 || s.player != g_current_player) && (s.response_code & 2) != 0) ||
                  (g_activation_event_flags & 2) != 0)) &&
                ((g_current_turn != s.player || g_trigger_condition == -1) ||
                 (s.response_code != 2 || (g_activation_event_flags & 2) != 0)))
            {
              if ((g_activation_event_flags & 2) == 0 && s.response_code != 2)
              {
                if (s.response_code == 2)
                {
                  s.response_mask |= 4;
                }
                else
                {
                  s.response_mask |= 2;
                }
              }
              else
              {
                s.maybe_card = s.card;
                s.maybe_player = s.player;
                ++s.num_candidates;
              }

              g_activation_event_flags &= ~2;
            }
          }
        }

        if (g_current_action_event_code == 4)
        {
          for (s.card = 0; (int)s.card < g_active_cards_count[1 - s.player]; ++s.card)
          {
            if (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).internal_card_id != -1 &&
                ((PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 1) != 0 ||
                 (PLAYER_CARD_INSTANCE(1 - s.player, (int)s.card).upkeep_flags & 0x10) != 0 ||
                 !is_nonactivated_mana_source(1 - s.player, (int)s.card)))
            {
              s.response_code = get_card_response_action_type(1 - s.player, (int)s.card);
              if ((g_activation_event_flags & 2) != 0)
              {
                s.maybe_card = s.card;
                s.maybe_player = 1 - s.player;
                ++s.num_candidates;

                g_activation_event_flags &= ~2;

                if (s.response_code == 2)
                {
                  s.response_mask |= 4;
                }
                else
                {
                  s.response_mask |= 2;
                }

                if ((g_duel_network_flags & 2) == 0)
                {
                  break;
                }
              }
            }
          }
        }

        if ((s.response_mask & 2) != 0)
        {
          s.interrupts_available = 1;
        }
      }
    }
  }

  if (s.interrupts_available || s.can_respond || s.num_candidates)
  {
    strcpy(s.prompt_format, gs_triggered_effects_009396a0);

    if ((g_response_card_type_mask & 0x10) != 0 && g_current_spell_player == -1)
    {
      strcpy(s.prompt_format, gs_fast_effects_00926520);
    }
    else if ((g_response_card_type_mask & 0x20) != 0)
    {
      if (g_duel_state_007a7d78 != 0)
      {
        g_duel_state_007a7d78 = 1;
      }
      strcpy(s.prompt_format, gs_interrupts_0093a850);
    }

    if (g_trigger_condition != -1)
    {
      strcpy(s.prompt_format, gs_triggered_effects_009396a0);
    }

    sprintf(g_ui_message_buffer, s.prompt_format, s.prompt_copy);

    if (g_phase_stop_suppressed == 0 || (s.response_mask & 2) != 0)
    {
      if (((s.can_respond == 0 && (g_ai_decision_parameter == 0 || g_trigger_condition != -1)) &&
           (s.interrupts_available == 0 || g_trigger_condition == -1) &&
           (g_affected_card_context_flags == 0 || (s.response_mask & 6) == 0)) &&
          ((s.num_candidates <= (int)!(s.response_mask & 2) && (s.response_mask & 4) == 0) ||
           ((s.some_flag == 0 && should_skip_phase(reason_for_trigger_controller) != 0) || g_trigger_condition == 0xd6 ||
            (s.maybe_card != 0xffffffffU && g_response_window_serial == g_response_window_depth))))
      {
        s.selected_card = s.maybe_card;
        s.selected_player = s.maybe_player;
        g_response_action_selected = 1;
      }
      else
      {
        if (g_duel_ai_mode_state != 1)
        {
          g_phase_was_skipped = 1;
        }

        g_response_window_serial = 0;

        s.selection_done = g_response_window_serial;
        while (!s.selection_done)
        {
          if (g_duel_ai_mode_state == 1)
          {
            s.selected_card = s.maybe_card;
            s.selection_done = 1;
            g_target_selection_status_code = -1;
          }
          else
          {
            if ((g_duel_network_flags & 2) == 0)
            {
              s.player = g_active_player;
            }
            else
            {
              s.player = reason_for_trigger_controller;
            }

            if (g_active_player == s.player)
            {
              g_stop_phase = g_stop_phase_player = -1;
            }
            else
            {
              g_previous_stop_phase = g_previous_stop_phase_player = -1;
            }

            s.selected_card = select_card_for_action(s.player, -1, s.player, 0xff, 0, g_ui_message_buffer, 2);
            s.selected_player = g_target_player_choice;
            if ((int)s.selected_card >= 0)
            {
              g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
                  (unsigned char)(((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase]) | PHASE_STOP_SUPPRESSED);
            }
          }

          switch (g_target_selection_status_code)
          {
          case 0:
            if (s.selected_player != -1 && s.selected_card != 0xffffffffU)
            {
              s.selection_done = 1;
            }
            else if (s.selected_player != -1 && s.selected_card == 0xffffffffU)
            {
              s.selection_done = 0;
            }
            break;

          case -3:
            s.selection_done = 0;
            break;

          case -2:
            s.selection_done = 1;
            s.selected_card = 0xffffffffU;

            g_activation_dispatch_flags &= ~2;
            if (g_trigger_condition != -1)
            {
              g_target_selection_status_code = 0;
            }

            if (s.num_candidates != 0)
            {
              g_response_window_serial = g_response_window_depth;
              g_response_action_selected = 1;
              if (g_duel_ai_mode_state != 1)
              {
                g_phase_stop_suppressed = 1;
              }
            }
            break;
          }
        }
      }
    }
    else
    {
      s.selected_card = s.maybe_card;
      s.selected_player = s.maybe_player;
      if (s.maybe_card != 0xffffffffU)
      {
        g_response_action_selected = 1;
      }
    }

    strcpy(g_ui_message_buffer, "");

    if ((g_duel_network_flags & 2) == 0)
    {
      s.player = g_active_player;
    }
    else
    {
      s.player = reason_for_trigger_controller;
    }

    if (s.selected_card != 0xffffffffU &&
        (s.selected_player == s.player ||
         (((PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).state & STATE_IN_PLAY) != 0 &&
           dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0) ||
          g_current_action_event_code == 4)))
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).internal_card_id;

      if (get_card_response_action_type(s.selected_player, (int)s.selected_card) != 0)
      {
        if (is_in_play(s.selected_player, (int)s.selected_card))
        {
          if (dispatch_card_trigger_event(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0)
          {
            resolve_card_immediately(s.selected_player, (int)s.selected_card, s.player);
          }
          else
          {
            if (PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).unknown0x14 == (unsigned int)g_trigger_condition &&
                pending_killed_card_handler != NULL && g_trigger_condition != -1)
            {
              if (pending_killed_card_handler != NULL)
              {
                pending_killed_card_handler(s.selected_player, (int)s.selected_card);
              }
            }
            else
            {
              if (activate(s.player, s.selected_player, (int)s.selected_card))
              {
                resolve_activated_ability(s.selected_player, (int)s.selected_card);

                if (g_spell_fizzled != 1 && g_duel_ai_mode_state != 1)
                {
                  play_sound_effect(0x26);
                }
              }

              g_spell_fizzled = 0;
            }
          }
        }
        else
        {
          resolve_cast_card(s.selected_player, (int)s.selected_card);
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            g_response_card_type_mask = g_response_action_mask & 0x20;
          }

          if (internal_rand(3) == 0)
          {
            show_opponent_taunt("I knew that was coming.");
          }
        }

        TENTATIVE_reassess_all_cards(0, 0xff);
        s.result_flags |= 2;
      }
      else
      {
        if (can_activate_mana_source_for_stop_prompt(s.selected_player, (int)s.selected_card) != 0 && s.selected_player == reason_for_trigger_controller)
        {
          activate_mana_source_card(s.selected_player, (int)s.selected_card);
        }
      }

      s.result_flags |= 2;
    }

    g_response_processing_flags = 1;
    g_response_action_selected = 0;
  }

  if (s.result_flags == 0)
  {
    g_response_card_type_mask = g_response_action_mask & 0x30;
  }

  g_response_selection_in_progress = 0;
  g_event_player = s.saved_unk_00742f60;
  g_land_can_be_played = s.saved_land_can_be_played;
  --g_response_window_depth;

  return (int)s.result_flags;
}

// FUNCTION: MAGIC 0x004aec05
// FUNCTION: SHANDALAR 0x0046bbe0
unsigned int choose_response_card(int player)
{
  struct
  {
    unsigned int candidates[20];
    int response_flags;   /* local_18 */
    unsigned int card;    /* local_14 */
    int other_player;     /* local_10 */
    int internal_card_id; /* local_c */
    int num_candidates;   /* local_8 */
  } s;

  s.num_candidates = 0;

  if (g_trigger_condition != -1 && g_current_turn == g_other_player)
  {
    for (s.other_player = 0; s.other_player < 2; ++s.other_player)
    {
      for (s.card = 0; (int)s.card < g_active_cards_count[s.other_player]; ++s.card)
      {
        if (is_in_play(s.other_player, (int)s.card) &&
            dispatch_card_trigger_event(s.other_player, (int)s.card, 0x7d, player) == 2)
        {
          g_activation_dispatch_flags = 4;
          g_target_player_choice = s.other_player;
          return s.card;
        }

        if (player == s.other_player &&
            (int)PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).unknown0x14 == g_trigger_condition &&
            g_current_turn == s.other_player && g_trigger_condition != -1 &&
            PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).internal_card_id != -1)
        {
          g_activation_dispatch_flags |= 4;
          ++g_duel_state_current_card;
          g_target_player_choice = s.other_player;
          return s.card;
        }
      }
    }
  }

  if (((g_response_processing_flags & 2) == 0) || (player == g_active_player) || (g_response_action_mask == 0))
  {
    return 0xffffffffU;
  }

  g_target_player_choice = player;

  for (s.card = 0; (int)s.card < g_active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, (int)s.card).internal_card_id;
    if (s.internal_card_id == -1)
      continue;

    s.response_flags = (int)get_card_response_action_type(player, (int)s.card);
    if (s.response_flags != 0)
    {
      s.candidates[s.num_candidates] = s.card;
      ++s.num_candidates;

      if (s.response_flags == 2)
      {
        return s.card;
      }
    }
  }

  if (g_current_action_event_code == 4)
  {
    for (s.card = 0; (int)s.card < g_active_cards_count[1 - player]; ++s.card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(1 - player, (int)s.card).internal_card_id;
      if (s.internal_card_id == -1)
        continue;

      s.response_flags = (int)get_card_response_action_type(1 - player, (int)s.card);
      if (s.response_flags != 0 && s.response_flags == 2)
      {
        g_target_player_choice = 1 - player;
        return s.card;
      }
    }
  }

  if (g_current_action_event_code == 4)
  {
    return 0xffffffffU;
  }

  s.candidates[s.num_candidates] = 0xffffffffU;
  ++s.num_candidates;

  if (g_duel_ai_mode_state == 1)
  {
    if (internal_rand(2) != 0 && get_recorded_action_count() != 0)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
    }
    else
    {
      g_ai_recorded_choice = internal_rand(s.num_candidates);
    }

    if (g_ai_search_force_pass != 0)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
      if (g_ai_search_force_pass == 1)
      {
        g_ai_search_force_pass = -1;
      }
    }

    g_ai_recorded_action = ((PLAYER_CARD_INSTANCE(player, (int)s.candidates[g_ai_recorded_choice]).state & STATE_IN_PLAY) ? 0x2000 : 0x1000) | s.candidates[g_ai_recorded_choice] | ((player != 0) ? 0x100 : 0);

    g_ai_recorded_action_type = 4;
    record_ai_action_selection();
  }
  else
  {
    g_ai_recorded_action_type = 4;
    replay_ai_action_selection();

    if (s.num_candidates <= g_ai_recorded_choice)
    {
      g_ai_recorded_choice = s.num_candidates - 1;
    }
  }

  if (s.candidates[g_ai_recorded_choice] != 0xffffffffU)
  {
    if (g_duel_state_008a8de4 >= 0x10)
    {
      --g_duel_state_008a8de4;
    }

    g_nested_trigger_stack[g_duel_state_008a8de4] =
        PLAYER_CARD_INSTANCE(player, (int)s.candidates[g_ai_recorded_choice]).internal_card_id;
    g_ai_action_score_table[g_duel_state_008a8de4] = s.candidates[g_ai_recorded_choice];
    ++g_duel_state_008a8de4;
  }

  return s.candidates[g_ai_recorded_choice];
}

// FUNCTION: MAGIC 0x004af0ec
// FUNCTION: SHANDALAR 0x0046c0c8
int get_card_response_action_type(int player, int card)
{
  struct
  {
    int tmp;                 /* ebp-0x0c */
    int response_controller; /* ebp-0x08 */
    int internal_card_id;    /* ebp-0x04 */
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.response_controller = player;
  }
  else
  {
    s.response_controller = g_active_player;
  }

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_IN_PLAY) != 0)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0)
    {
      return 0;
    }

    if ((int)PLAYER_CARD_INSTANCE(player, card).unknown0x14 == g_trigger_condition)
    {
      if (g_trigger_condition != -1)
      {
        if (g_current_turn == player)
        {
          g_activation_dispatch_flags |= 4;
          g_duel_state_current_card += 1;
          return 2;
        }
        else
        {
          return 0;
        }
      }
    }

    if (g_trigger_condition != -1)
    {
      s.tmp = dispatch_card_trigger_event(player, card, 0x7d, player);
      if (s.tmp != 0)
      {
        g_activation_dispatch_flags |= 1 << ((char)s.tmp);
        g_duel_state_current_card += 1;

        if (s.tmp >= 2)
        {
          return 2;
        }
        else
        {
          return 3;
        }
      }
      return 0;
    }

    if (((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) == 0 &&
         (global_cards_data[s.internal_card_id].extra_ability & 1) != 0 &&
         (g_response_card_type_mask & 0x10) != 0) ||
        ((global_cards_data[s.internal_card_id].extra_ability & 2) != 0 &&
         (g_response_card_type_mask & 0x20) != 0))
    {
      if (g_duel_state_00791418 != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 2) == 0)
      {
        return 0;
      }

      if ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0 ||
          ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
          (g_current_action_event_code == 0xd3 &&
           (global_cards_data[s.internal_card_id].extra_ability & 0x4000) != 0))
      {
        g_activation_event_flags &= ~2;

        if (((s.response_controller == player ||
              (g_response_source_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
             (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0) &&
            dispatch_event_to_single_card(player, card, 0x73, 1 - player, -1) != 0)
        {

          if ((g_activation_event_flags & 2U) != 0)
          {
            g_activation_dispatch_flags |= 4;
            return 2;
          }

          g_activation_dispatch_flags |= 2;
          return 3;
        }
      }
    }

    if (g_current_action_event_code == 4 && (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) != 0)
    {
      g_activation_event_flags |= 3;
      g_activation_dispatch_flags |= 4;
      return 2;
    }

    if (g_current_action_event_code == 4 && player == g_event_player &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x10) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x88) == 0 &&
        can_pay_untap_cost(player, card) != 0)
    {
      g_activation_dispatch_flags |= 2;

      if (g_other_player == player && (g_duel_network_flags & 2) == 0)
      {
        return 2;
      }
      else
      {
        return 3;
      }
    }
  }
  else
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_INVISIBLE | STATE_SUMMONSICK)) != 0)
    {
      return 0;
    }

    if (g_trigger_condition != -1 &&
        global_cards_data[s.internal_card_id].code_pointer != card_death_ward)
    {
      return 0;
    }

    if (g_duel_state_00791418 != 0 && ((global_cards_data[s.internal_card_id].type & 0x20) == 0))
    {
      return 0;
    }

    if ((g_response_card_type_mask & (unsigned char)global_cards_data[s.internal_card_id].type) != 0 &&
        can_pay_card_mana_cost(player, player, card) != 0 &&
        ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) == 0 ||
         ((g_land_can_be_played & LCBP_DAMAGE_PREVENTION) != 0 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x1004) != 0) ||
         (g_current_action_event_code == 0xd3 &&
          (global_cards_data[s.internal_card_id].extra_ability & 0x2000) != 0)) &&
        ((s.response_controller == player ||
          (g_response_activation_phase_mask & (int)(char)global_cards_data[s.internal_card_id].act_phases) != 0) &&
         dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) != 0))
    {
      return 3;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004af6f4
// FUNCTION: SHANDALAR 0x0046c6cf
int is_phase_stop_allowed_for_current_event(void)
{
  if (g_current_action_event_code == 0x8e)
  {
    if (g_attacking_creature_count != 0)
      return 1;
    else
      return 0;
  }
  else if (g_current_action_event_code == 0x6a || g_current_action_event_code == 0x6b || g_current_action_event_code == 0x6c ||
           g_current_action_event_code == 0x6d || g_current_action_event_code == 0x6e || g_current_action_event_code == 0x6f ||
           g_current_action_event_code == 0x70 || g_current_action_event_code == 0x71 || g_current_action_event_code == 0x72 ||
           g_current_action_event_code == 0x73 || g_current_action_event_code == 0x74 || g_current_action_event_code == 0x75 ||
           g_current_action_event_code == 0x76 || g_current_action_event_code == 0x77 || g_current_action_event_code == 0x78 ||
           g_current_action_event_code == 0x79 || g_current_action_event_code == 0x7a || g_current_action_event_code == 0x7b ||
           g_current_action_event_code == 0x7c || g_current_action_event_code == 0x7d || g_current_action_event_code == 0x7e ||
           g_current_action_event_code == 0x7f || g_current_action_event_code == 0x80 || g_current_action_event_code == 0x81 ||
           g_current_action_event_code == 0x82 || g_current_action_event_code == 0x83 || g_current_action_event_code == 0x84 ||
           g_current_action_event_code == 0x85 || g_current_action_event_code == 0x86 || g_current_action_event_code == 0x87 ||
           g_current_action_event_code == 0x88 || g_current_action_event_code == 0x89 || g_current_action_event_code == 0x8e ||
           g_current_action_event_code == 0xc7 || g_current_action_event_code == 0xc8 || g_current_action_event_code == 0xc9 ||
           g_current_action_event_code == 0xca || g_current_action_event_code == 0xcb || g_current_action_event_code == 0xcc ||
           g_current_action_event_code == 0xcd || g_current_action_event_code == 0xce || g_current_action_event_code == 0xcf ||
           g_current_action_event_code == 0xd2 || g_current_action_event_code == 0xd3 || g_current_action_event_code == 0xd4 ||
           g_current_action_event_code == 0xd5 || g_current_action_event_code == 0xd6 || g_current_action_event_code == 0xd7 ||
           g_current_action_event_code == 0xd8 || g_current_action_event_code == 0xd9 || g_current_action_event_code == 0xdc ||
           g_current_action_event_code == 0xdb)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: MAGIC 0x004afa4b
// FUNCTION: SHANDALAR 0x0046ca26
void process_damage_prevention(int player)
{
  struct
  {
    int did_setup;    /* ebp-0x18 */
    int test_card;    /* ebp-0x14 */
    int test_player;  /* ebp-0x10 */
    int best_score;   /* ebp-0x0c */
    int saved_777854; /* ebp-0x08 */
    int temp;         /* ebp-0x04 */
  } s;

  if ((g_land_can_be_played & LCBP_PENDING_DAMAGE_CARDS) == 0)
  {
    return;
  }
  g_land_can_be_played &= ~LCBP_PENDING_DAMAGE_CARDS;
  g_land_can_be_played |= LCBP_DAMAGE_PREVENTION;
  TENTATIVE_reassess_all_cards(0, 0xff);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & STATE_TAPPED) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x21);
      }
    }
  }

  s.did_setup = 0;
setup:
  if (((g_duel_network_flags & 2) == 0) && g_duel_ai_mode_state != 1 && g_ai_action_replay_available == 0)
  {
    start_ai_decision_search(9, 0xf);
    s.best_score = -99999;
    s.did_setup = 1;
  }

after_setup:
  if (g_ai_decision_code == 9 && s.did_setup)
  {
    reset_ai_search_trial_state();
    g_ai_side_score_player_1 = 0;
    g_ai_side_score_player_0 = g_ai_side_score_player_1;
    g_duel_state_008a8de4 = g_ai_side_score_player_0;
    g_ai_modifier = g_duel_state_008a8de4;
  }

  if (allow_response(-2, -1, gs_damage_prevention_00939200, 0x8e) != 0)
  {
    goto setup;
  }

  C_dispatch_event_raw(EVENT_DAMAGE_PREVENTION);
  if ((g_battlefield_extra_ability_flags & 0x00040000) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_END_DAMAGE_PREV, gs_end_damage_prevention_00789740, 0);
  }

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & STATE_TAPPED) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x6e);
      }
    }
  }

  dispatch_trigger_twice_once_with_each_player_as_reason(g_current_player, TRIGGER_DEAL_DAMAGE, gs_damage_dealing_00777ab0, 0);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < g_active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0)
      {
        if ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & STATE_TAPPED) != 0)
        {
          kill_card(s.test_player, s.test_card, 1);
        }
        else
        {
          g_land_can_be_played |= LCBP_PENDING_DAMAGE_CARDS;
        }
      }
    }
  }

  kill_creatures_with_lethal_damage();
  g_land_can_be_played &= ~LCBP_DAMAGE_PREVENTION;

  if (g_duel_ai_mode_state == 1 && s.did_setup != 0 && g_ai_decision_code == 9)
  {
    regenerate_or_graveyard_triggers();
    s.temp = g_ai_modifier + ai_opinion_of_gamestate(g_other_player);
    if (s.temp > s.best_score)
    {
      s.best_score = s.temp;
      save_recorded_ai_actions();
      s.saved_777854 = g_ai_search_flags;
    }
    if (g_ai_search_target_depth == 999)
    {
      g_ai_search_target_depth = -1;
    }
    g_ai_search_force_pass = 0;
#ifdef SHANDALAR
    if ((g_game_time_scale * g_ai_search_time_limit) / 5 < GetUiTickCount())
#else
    if (get_duel_time_units_if_available() > (g_game_time_scale * g_ai_search_time_limit) / 5)
#endif
    {
      g_duel_ai_mode_state = 0;
      g_ai_search_target_depth = -1;
      g_ai_search_flags = s.saved_777854;
    }
    g_land_can_be_played |= LCBP_DAMAGE_PREVENTION;
    goto after_setup;
  }

  if (g_duel_ai_mode_state != 1 && s.did_setup != 0)
  {
    g_ai_action_replay_available = 0;
  }
}

// FUNCTION: MAGIC 0x004aff25
// FUNCTION: SHANDALAR 0x0046cf00
void kill_creatures_with_lethal_damage(void)
{
  int player;
  int card;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < g_active_cards_count[player]; ++card)
    {
      if (is_in_play(player, card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE))
      {
        if (C_get_abilities(player, card, EVENT_TOUGHNESS, -1) <= (short)PLAYER_CARD_INSTANCE(player, card).damage_on_card)
        {
          if (g_duel_ai_mode_state != 1)
          {
            play_sound_effect(WAV_KILL);
          }
          kill_card(player, card, 2);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x004b082f
// FUNCTION: SHANDALAR 0x0046d80a
int dispatch_card_trigger_event(int player, int card, event_t event, int extra)
{
  if (event == EVENT_TRIGGER && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_PROCESSING) != 0 || g_ai_search_result != 0))
  {
    return 0;
  }

  if (g_trigger_condition < 200)
  {
    return 0;
  }

  g_event_result = 0;
  g_affected_card_controller = player;
  g_affected_card = card;
  g_duel_state_008cfd20 = extra;
  g_attacking_card = -1;
  C_dispatch_event_raw(event);
  return g_event_result;
}

// FUNCTION: MAGIC 0x004c0a36
// FUNCTION: SHANDALAR 0x004ecec6
int ClampIntToRange(int a1, int a2, int a3)
{
  if (a1 < a2)
  {
    a1 = a2;
  }
  if (a3 < a1)
  {
    a1 = a3;
  }
  return a1;
}

// FUNCTION: MAGIC 0x004e1b6f
int get_duel_time_units_if_available(void)
{
  return get_duel_time_units();
}

// FUNCTION: MAGIC 0x004e1c81
int legacy_clear_graphics_page_stub()
{
}

// FUNCTION: MAGIC 0x004e1d6d
int legacy_blit_graphics_rect_stub()
{
  return 0;
}

// FUNCTION: MAGIC 0x004e1d91
int legacy_load_pcx_into_page_stub()
{
  return 1;
}

// FUNCTION: MAGIC 0x004e1da6
int legacy_load_pcx_into_page_no_palette_stub()
{
  return 1;
}

// FUNCTION: MAGIC 0x0055117d
// FUNCTION: SHANDALAR 0x0049f3c0
void dispatch_three_arg_callback_to_cards_in_play(int(__cdecl *callback)(int, int, int), int who_to_check)
{
  int test_card;
  int test_player;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    if (who_to_check == -1 || test_player == who_to_check)
    {
      for (test_card = 0; test_card < g_active_cards_count[test_player]; ++test_card)
      {
        if (is_in_play(test_player, test_card))
        {
          callback(test_player,
                   test_card,
                   global_card_instances[test_player][test_card].internal_card_id);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x005513d7
// FUNCTION: SHANDALAR 0x0049f61a
void add_special_counters(int player, int card, int amount)
{
  if ((PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffu) == 0xffu)
    return;

  PLAYER_CARD_INSTANCE(player, card).special_counters =
      ((PLAYER_CARD_INSTANCE(player, card).special_counters + amount) & 0xff) |
      (PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffffff00);

  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(ICON_HASTE);
  }
}
