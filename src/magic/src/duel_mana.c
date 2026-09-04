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

// FUNCTION: MOK 0x0042de60
// FUNCTION: MAGIC 0x00432f00
// FUNCTION: SHANDALAR 0x004143e0
int charge_mana(int player, color_t color, int amount)
{
  struct
  {
    int idx8;               /* [ebp-0xd4] */
    int saved_raw_mana[8];  /* [ebp-0xd0] */
    int saved_x_value;      /* [ebp-0xb0] */
    int saved_mana_cost[8]; /* [ebp-0xac] */
    int saved_max_x_value;  /* [ebp-0x8c] */

    int special_amount_to_take;           /* [ebp-0x88] */
    int chosen_color;                     /* [ebp-0x84] */
    unsigned int special_color;           /* [ebp-0x80] */
    unsigned int produced_color;          /* [ebp-0x7c] */
    color_test_t payable_color_mask;      /* [ebp-0x78] */
    color_test_t produced_special_colors; /* [ebp-0x74] */
    int color_count;                      /* [ebp-0x70] */
    int amount_to_take;                   /* [ebp-0x6c] */
    int cost_color;                       /* [ebp-0x68] */
    int only_variable_costs;              /* [ebp-0x64] */
    int available_mana;                   /* [ebp-0x60] */
    int old_tapped_state;                 /* [ebp-0x5c] */
    int done;                             /* [ebp-0x58] */
    color_test_t payable_colors;          /* [ebp-0x54] */

    int allow_special_x_payment;                     /* [ebp-0x50] */
    int mana_paid_by_color[7];                       /* [ebp-0x4c] */
    int allow_generic_payment;                       /* [ebp-0x30] */
    int selected_card;                               /* [ebp-0x2c] */
    int allow_mana_sources;                          /* [ebp-0x28] */
    int allow_interrupt_speed_mana_sources;          /* [ebp-0x24] */
    int max_colorless;                               /* [ebp-0x20] */
    int slot;                                        /* [ebp-0x1c] */
    int i;                                           /* [ebp-0x18] */
    unsigned int old_tapped_state_before_activation; /* [ebp-0x14] */
    int current_internal_card_id;                    /* [ebp-0x10] */
    int mana_paid_total;                             /* [ebp-0x0c] */
    int temp;                                        /* [ebp-0x08] */
    int include_auto_payment;                        /* [ebp-0x04] */
  } s;

  if ((g_land_can_be_played & LCBP_CARD_BEING_COPIED) != 0)
  {
    goto skipped_for_copied_card;
  }

  {
    if (g_mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] > 0)
    {
      if (color == COLOR_COLORLESS)
      {
        if (amount != -1)
        {
          amount = MAX(amount - g_mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2, 1);
        }
      }
      else if (g_mana_charge[COLOR_COLORLESS] > 0)
      {
        g_mana_charge[COLOR_COLORLESS] = MAX(g_mana_charge[COLOR_COLORLESS] - g_mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2, 1);
      }
    }

    g_mana_charge[color] += amount;
    s.mana_paid_total = 0;
    for (s.i = 0; s.i < 7; ++s.i)
    {
      s.mana_paid_by_color[s.i] = 0;
    }

    s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);

    s.include_auto_payment = 1;
    if (g_recorded_action_player != 0 || (g_other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
    {
      s.allow_special_x_payment = 1;
      s.allow_generic_payment = 1;
      s.allow_mana_sources = 1;
      if ((g_other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
      {
        s.allow_interrupt_speed_mana_sources = 1;
        s.temp = 1;
      }
      else
      {
        s.temp = 0;
        s.allow_interrupt_speed_mana_sources = s.temp;
      }
    }
    else
    {
      s.temp = 0;
      s.allow_interrupt_speed_mana_sources = s.temp;
      s.allow_mana_sources = s.allow_interrupt_speed_mana_sources;
      s.allow_generic_payment = s.allow_mana_sources;
      s.allow_special_x_payment = s.allow_generic_payment;
    }

    s.temp = 1;

    if (g_other_player == player && (g_duel_network_flags & 2) == 0)
    {
      for (s.i = COLOR_COLORLESS; s.i < 7; ++s.i)
      {
        if (g_mana_charge[s.i] == -1)
        {
          s.available_mana = has_mana(player, s.i, 1);
          if (g_duel_ai_mode_state == 1)
          {
            if (internal_rand(3) == 0 || s.available_mana <= 1)
            {
              s.old_tapped_state = s.available_mana;
            }
            else
            {
              s.old_tapped_state = internal_rand(s.available_mana - 1) + 1;
            }

            if (g_max_x_value != -1)
            {
              s.old_tapped_state = MIN(g_max_x_value, s.old_tapped_state);
            }

            g_ai_recorded_choice = s.old_tapped_state;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            if (g_ai_recorded_choice == 99)
            {
              g_ai_recorded_choice = 0;
            }
            s.old_tapped_state = g_ai_recorded_choice;
          }
        }
      }

      g_max_x_value = s.old_tapped_state;
    }

    if (g_mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] > 0)
    {
      g_x_value = g_mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2;
    }
    else
    {
      g_x_value = 0;
    }

    if (g_max_x_value == 0)
    {
      for (s.i = 0; s.i < 7; ++s.i)
      {
        if (g_mana_charge[s.i] == -1)
        {
          g_mana_charge[s.i] = 0;
        }
      }
    }

    for (s.i = 0, s.payable_colors = 0; s.i < 7; ++s.i)
    {
      if (g_mana_charge[s.i] == -1)
      {
        s.payable_colors = 1;
      }
    }

    if (s.include_auto_payment != 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      auto_pay_colored_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
      copy_mana_pool_to_display();
    }

    if (s.allow_special_x_payment != 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0 && s.payable_colors != 0)
    {
      auto_pay_variable_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &g_x_value, g_max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_generic_payment != 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      auto_pay_generic_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &g_x_value, g_max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_mana_sources != 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1e, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1c, s.temp);
      }
    }

    if (s.allow_interrupt_speed_mana_sources != 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x14, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x04, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1a, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x18, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x10, s.temp);
      }
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x00, s.temp);
      }
    }

    if ((g_other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
    {
      if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0 && s.payable_colors == 0)
      {
        g_spell_fizzled = 1;
      }
    }
    else if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      s.done = 0;
      while (s.done == 0 && is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
      {
        s.only_variable_costs = 1;
        for (s.i = 0; s.i < 7; ++s.i)
        {
          if (g_mana_charge[s.i] > 0)
          {
            s.only_variable_costs = 0;
          }
        }

        format_mana_payment_prompt(g_ui_message_buffer, g_mana_charge, g_x_value, g_max_x_value);
        s.selected_card =
            select_card_for_action(player, player, player, 0, 0, g_ui_message_buffer, s.only_variable_costs != 0 ? 3 : 1);

        if (g_target_player_choice == -1 && (s.selected_card == -1 || s.selected_card == -2))
        {
          if (g_target_selection_status_code == -2)
          {
            if (g_stop_phase_player == -1 && g_stop_phase == -1)
            {
              if (s.selected_card == -1)
              {
                g_spell_fizzled = 1;
              }
              s.done = 1;
            }
            else if (s.only_variable_costs != 0)
            {
              s.done = 1;
            }
          }
          else if (g_target_selection_status_code == -3 && player == g_recorded_action_controller && g_recorded_action_phase != -1)
          {
            if (g_raw_mana_available[player][g_recorded_action_phase] > 0 && (g_mana_charge[g_recorded_action_phase] != 0 || g_mana_charge[COLOR_COLORLESS] != 0 || g_mana_charge[COLOR_ARTIFACT] != 0) && (g_recorded_action_phase != COLOR_ARTIFACT || g_mana_charge[COLOR_ARTIFACT] != 0))
            {
              if (g_mana_charge[g_recorded_action_phase] == 0)
              {
                if (g_mana_charge[COLOR_ARTIFACT] == 0)
                {
                  s.cost_color = COLOR_COLORLESS;
                }
                else
                {
                  s.cost_color = COLOR_ARTIFACT;
                }
              }
              else
              {
                s.cost_color = g_recorded_action_phase;
              }

              s.amount_to_take =
                  get_mana_payment_amount(g_mana_charge, s.cost_color, g_raw_mana_available[player], g_recorded_action_phase, g_recorded_action_player, g_max_x_value, g_x_value);
              spend_mana_for_cost(
                  g_mana_charge, s.cost_color, s.amount_to_take, &g_x_value, g_max_x_value, player, g_recorded_action_phase, s.mana_paid_by_color, &s.mana_paid_total);
              copy_mana_pool_to_display();
            }

            if (g_raw_mana_available[player][g_recorded_action_phase] > 0)
            {
              s.produced_special_colors = 0;
              for (s.slot = 0; s.slot < 10 && g_mana_color_conversions[player][s.slot] != -1; ++s.slot)
              {
                s.produced_color = (unsigned short)g_mana_color_conversions[player][s.slot];
                s.special_color = ((unsigned int)g_mana_color_conversions[player][s.slot] >> 16) & 0xffff;
                if (s.produced_color == g_recorded_action_phase)
                {
                  s.produced_special_colors |= 1 << (unsigned char)s.special_color;
                }
              }

              s.payable_color_mask = 0;
              for (s.i = 0; s.i < 7; ++s.i)
              {
                if (g_mana_charge[s.i] != 0)
                {
                  s.payable_color_mask |= 1 << (unsigned char)s.i;
                }
              }

              s.produced_special_colors &= s.payable_color_mask;
              if (s.produced_special_colors != 0)
              {
                s.color_count = 0;
                for (s.i = 0; s.i < 7; ++s.i)
                {
                  if ((s.produced_special_colors & (1 << (unsigned char)s.i)) != 0)
                  {
                    ++s.color_count;
                  }
                }

                if (s.color_count == 1)
                {
                  s.chosen_color = single_color_test_bit_to_color_t(s.produced_special_colors);
                }
                else
                {
                  load_text(global_ui_strings_filename, "PROMPT_GRABMANA_DUALUSE");
                  s.chosen_color = choose_a_color(player, g_text_lines[0], 1, g_recorded_action_phase, s.produced_special_colors);
                }

                s.special_amount_to_take =
                    get_mana_payment_amount(g_mana_charge, s.chosen_color, g_raw_mana_available[player], g_recorded_action_phase, g_recorded_action_player, g_max_x_value, g_x_value);
                spend_mana_for_cost(
                    g_mana_charge, s.chosen_color, s.special_amount_to_take, &g_x_value, g_max_x_value, player, g_recorded_action_phase, s.mana_paid_by_color, &s.mana_paid_total);
                copy_mana_pool_to_display();
              }
            }
          }
        }
        else
        {
          if (g_target_player_choice == -1 || s.selected_card != -1)
          {
            s.current_internal_card_id = (global_card_instances[player] + s.selected_card)->internal_card_id;
            if ((global_cards_data[s.current_internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
                ((global_cards_data[s.current_internal_card_id].type & TYPE_INTERRUPT) != 0 ||
                  ((((global_card_instances[player] + s.selected_card)->state & STATE_IN_PLAY) != 0) &&
                   ((global_card_instances[player] + s.selected_card)->state & STATE_OUBLIETTED) == 0 &&
                   ((global_card_instances[player] + s.selected_card)->state & STATE_TAPPED) == 0 &&
                   ((((global_card_instances[player] + s.selected_card)->state &
                       (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0) ||
                    (global_cards_data[(global_card_instances[player] + s.selected_card)->internal_card_id].type & TYPE_CREATURE) == 0))))
            {
              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                s.saved_raw_mana[s.idx8] = g_raw_mana_available[player][s.idx8];
              }

              s.saved_x_value = g_x_value;
              g_x_value = 0;
              s.saved_max_x_value = g_max_x_value;
              g_max_x_value = -1;
              for (s.idx8 = 0; s.idx8 < 7; ++s.idx8)
              {
                s.saved_mana_cost[s.idx8] = g_mana_charge[s.idx8];
                g_mana_charge[s.idx8] = 0;
              }

              if (((global_card_instances[player] + s.selected_card)->state & 0x800002) == STATE_IN_PLAY)
              {
                if ((global_cards_data[s.current_internal_card_id].type & TYPE_LAND) != 0 ||
                    dispatch_event_to_single_card(player, s.selected_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
                {
                  push_card_onto_stack(player, s.selected_card, EVENT_RESOLVE_ACTIVATION, player, 0);
                  g_required_mana_color_mask = get_required_mana_color_mask(s.saved_mana_cost);
                  g_produced_mana_color = -1;
                  s.old_tapped_state_before_activation = (global_card_instances[player] + s.selected_card)->state & STATE_TAPPED;
                  dispatch_event_to_single_card(player, s.selected_card, EVENT_ACTIVATE, 1 - player, -1);
                  g_required_mana_color_mask = 0;
                  if (g_spell_fizzled == 1)
                  {
                    g_spell_fizzled = 0;
                    obliterate_top_card_of_stack();
                  }
                  else
                  {
                    if (s.old_tapped_state_before_activation == 0 && ((global_card_instances[player] + s.selected_card)->state & STATE_TAPPED) != 0)
                    {
                      dispatch_event(player, s.selected_card, EVENT_TAP_CARD);
                    }
                    if (g_duel_ai_mode_state != 1)
                    {
                      play_sound_effect(WAV_TAP);
                    }
                    resolve_top_card_on_stack();
                    TENTATIVE_reassess_all_cards(0, 0xff);
                  }
                }
              }
              else
              {
                resolve_cast_card(player, s.selected_card);
                TENTATIVE_reassess_all_cards(0, 0xff);
              }

              g_x_value = s.saved_x_value;
              g_max_x_value = s.saved_max_x_value;
              for (s.idx8 = 0; s.idx8 < 7; ++s.idx8)
              {
                g_mana_charge[s.idx8] = s.saved_mana_cost[s.idx8];
              }

              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                g_raw_mana_available[player][s.idx8] -= s.saved_raw_mana[s.idx8];
              }

              s.max_colorless = 0;
              auto_pay_colored_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
              auto_pay_variable_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &g_x_value, g_max_x_value);
              auto_pay_generic_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &g_x_value, g_max_x_value);

              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                g_raw_mana_available[player][s.idx8] += s.saved_raw_mana[s.idx8];
              }

              copy_mana_pool_to_display();
            }
          }
        }
      }
    }
  }

skipped_for_copied_card:
  if (g_spell_fizzled == 1)
  {
    refund_paid_mana(s.mana_paid_by_color);
    for (s.i = 0; s.i < 7; ++s.i)
    {
      produce_mana(player, s.i, s.mana_paid_by_color[s.i]);
      s.mana_paid_by_color[s.i] = 0;
    }
    s.mana_paid_total = 0;
    g_x_value = 0;
  }

  for (s.i = 0; s.i < 8; ++s.i)
  {
    g_mana_charge[s.i] = 0;
  }
  g_max_x_value = -1;

  return s.mana_paid_total;
}

// FUNCTION: MAGIC 0x0043410a
// FUNCTION: SHANDALAR 0x004155ee
void auto_pay_colored_mana(int player, int *mana_paid_by_color, int *total_mana_paid, int max_colorless)
{
  unsigned short produced_color;
  unsigned int special_mana;
  int slot;
  int color;

  for (color = 0; color < 7; ++color)
  {
    while (g_mana_charge[color] > 0 && g_raw_mana_available[player][color] > 0)
    {
      spend_mana_for_cost(g_mana_charge, color, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
    }
  }

  while (g_mana_charge[6] > 0 && g_raw_mana_available[player][0] > 0 && max_colorless < g_mana_charge[6])
  {
    spend_mana_for_cost(g_mana_charge, 6, 1, (int *)0, 0, player, 0, mana_paid_by_color, total_mana_paid);
  }

  slot = 0;
  while (slot < 10 && g_mana_color_conversions[player][slot] != -1)
  {
    produced_color = (unsigned short)g_mana_color_conversions[player][slot];
    special_mana = (unsigned int)g_mana_color_conversions[player][slot];
    while (g_raw_mana_available[player][produced_color] > 0 && g_mana_charge[special_mana >> 16] > 0)
    {
      spend_mana_for_cost(
          g_mana_charge, special_mana >> 16, 1, (int *)0, 0, player, produced_color, mana_paid_by_color, total_mana_paid);
    }
    slot = slot + 1;
  }
}

// FUNCTION: MAGIC 0x004342b3
// FUNCTION: SHANDALAR 0x00415794
void auto_pay_variable_mana(int player,
                            int *mana_paid_by_color,
                            int *total_mana_paid,
                            int max_colorless,
                            int *special_mana_override,
                            int max_x)
{
  unsigned short produced_color;
  unsigned int special_mana;
  int slot;
  int color;

  for (color = 0; color < 7; ++color)
  {
    if (g_mana_charge[color] == -1)
    {
      while (g_raw_mana_available[player][color] > 0 && (g_x_value < max_x || max_x == -1))
      {
        spend_mana_for_cost(
            g_mana_charge, color, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
      }
    }
  }

  if (g_mana_charge[6] == -1)
  {
    while (g_raw_mana_available[player][0] > 0 && max_colorless < g_mana_charge[6] &&
           (g_x_value < max_x || max_x == -1))
    {
      spend_mana_for_cost(
          g_mana_charge, 6, 1, special_mana_override, max_x, player, 0, mana_paid_by_color, total_mana_paid);
    }
  }

  slot = 0;
  while (slot < 10 && g_mana_color_conversions[player][slot] != -1)
  {
    produced_color = (unsigned short)g_mana_color_conversions[player][slot];
    special_mana = (unsigned int)g_mana_color_conversions[player][slot];
    if (g_mana_charge[special_mana >> 16] == -1)
    {
      while (g_raw_mana_available[player][produced_color] > 0 && (g_x_value < max_x || max_x == -1))
      {
        spend_mana_for_cost(g_mana_charge,
                            special_mana >> 16,
                            1,
                            special_mana_override,
                            max_x,
                            player,
                            produced_color,
                            mana_paid_by_color,
                            total_mana_paid);
      }
    }
    slot = slot + 1;
  }
}

// FUNCTION: MAGIC 0x004344b0
// FUNCTION: SHANDALAR 0x0041598e
void auto_pay_generic_mana(int player,
                           int *mana_paid_by_color,
                           int *total_mana_paid,
                           int max_colorless,
                           int *special_mana_override,
                           int max_x)
{
  int color;

  if (g_mana_charge[COLOR_ARTIFACT] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (g_mana_charge[COLOR_ARTIFACT] > 0)
      {
        while (g_raw_mana_available[player][color] > 0 && max_colorless < g_mana_charge[COLOR_ARTIFACT])
        {
          spend_mana_for_cost(g_mana_charge, COLOR_ARTIFACT, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
      else
      {
        while (g_raw_mana_available[player][color] > 0 && g_mana_charge[COLOR_ARTIFACT] == -1 &&
               (max_x == -1 || max_colorless < max_x - g_x_value))
        {
          spend_mana_for_cost(g_mana_charge,
                              COLOR_ARTIFACT,
                              1,
                              special_mana_override,
                              max_x,
                              player,
                              color,
                              mana_paid_by_color,
                              total_mana_paid);
        }
      }
    }
  }

  if (g_mana_charge[COLOR_COLORLESS] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (color != COLOR_ARTIFACT)
      {
        if (g_mana_charge[COLOR_COLORLESS] == -1)
        {
          while (g_raw_mana_available[player][color] > 0 && (g_x_value < max_x || max_x == -1))
          {
            spend_mana_for_cost(g_mana_charge,
                                COLOR_COLORLESS,
                                1,
                                special_mana_override,
                                max_x,
                                player,
                                color,
                                mana_paid_by_color,
                                total_mana_paid);
          }
        }
        else
        {
          while (g_mana_charge[COLOR_COLORLESS] > 0 && g_raw_mana_available[player][color] > 0)
          {
            spend_mana_for_cost(g_mana_charge,
                                COLOR_COLORLESS,
                                1,
                                (int *)0,
                                0,
                                player,
                                color,
                                mana_paid_by_color,
                                total_mana_paid);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x004346ab
// FUNCTION: SHANDALAR 0x00415b89
int is_mana_cost_paid(int *mana_cost, int required_colorless, int max_x)
{
  int color;

  for (color = 0; color < 7; color++)
  {
    if (mana_cost[color] > 0)
    {
      return 0;
    }

    if (mana_cost[color] == -1 && max_x != -1 && required_colorless < max_x)
    {
      return 0;
    }

    if (mana_cost[color] == -1 && max_x == -1)
    {
      return 0;
    }
  }

  return 1;
}

// FUNCTION: MAGIC 0x00434746
// FUNCTION: SHANDALAR 0x00415c24
unsigned int get_required_mana_color_mask(int *mana)
{
  int color;
  unsigned int result;

  result = 0;
  for (color = 0; color < 7; ++color)
  {
    if (mana[color] != 0)
    {
      result = result | (1 << (unsigned char)color);
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0043479e
// FUNCTION: SHANDALAR 0x00415c7c
int format_mana_payment_prompt(char *prompt, int *mana_cost, int x_paid, int max_x)
{
  struct
  {
    char symbol[8];               /* [ebp-0x1b8] */
    int total_generic;            /* [ebp-0x1b0] */
    char tmp[300];                /* [ebp-0x1ac] */
    char mana_text[100];          /* [ebp-0x80] */
    int stack_card;               /* [ebp-0x1c] */
    size_t unused_layout_padding; /* [ebp-0x18] (intentionally unused, but stabilizes layout) */
    unsigned int action;          /* [ebp-0x14] */
    int i;                        /* [ebp-0x10] */
    int color;                    /* [ebp-0x0c] */
    unsigned int msg;             /* [ebp-0x08] */
    int stack_player;             /* [ebp-0x04] */
  } s;

  (void)prompt;

  strcpy(s.tmp, "");

  s.action = get_current_stack_action();
  if (s.action != 0xffffffffU)
  {
    s.msg = (s.action >> 16) & 0xff;
    s.stack_player = global_stack_cards[g_stack_size - 1].player;
    s.stack_card = global_stack_cards[g_stack_size - 1].card;
    format_stack_action_text(s.tmp, s.msg, s.stack_player, s.stack_card);
  }

  s.mana_text[0] = '\0';

  if (mana_cost[COLOR_COLORLESS] == -1 || mana_cost[COLOR_ARTIFACT] == -1)
  {
    if (max_x == -1)
    {
      sprintf(s.mana_text + strlen(s.mana_text), gs_mana_so_far_008a9010, "|X", x_paid);
    }
    else if (x_paid < max_x)
    {
      sprintf(s.mana_text + strlen(s.mana_text), gs_mana_so_far_max_00791420, "|X", x_paid, max_x);
    }
  }
  else if (mana_cost[COLOR_COLORLESS] != 0 || mana_cost[COLOR_ARTIFACT] != 0)
  {
    s.total_generic = mana_cost[COLOR_ARTIFACT] + mana_cost[COLOR_COLORLESS];
    for (; s.total_generic > 9; s.total_generic -= 10)
    {
      strcat(s.mana_text, "|10");
    }
    if (s.total_generic != 0)
    {
      sprintf(s.mana_text + strlen(s.mana_text), "|%d", s.total_generic);
    }
  }

  for (s.color = COLOR_BLACK; s.color < 6; s.color++)
  {
    s.symbol[0] = '|';
    if (s.color == COLOR_BLACK)
    {
      s.symbol[1] = 'B';
    }
    else if (s.color == COLOR_BLUE)
    {
      s.symbol[1] = 'U';
    }
    else if (s.color == COLOR_GREEN)
    {
      s.symbol[1] = 'G';
    }
    else if (s.color == COLOR_RED)
    {
      s.symbol[1] = 'R';
    }
    else
    {
      s.symbol[1] = 'W';
    }
    s.symbol[2] = '\0';

    if (mana_cost[s.color] == -1)
    {
      if (max_x == -1)
      {
        sprintf(s.mana_text + strlen(s.mana_text), gs_mana_so_far_008a9010, s.symbol, x_paid);
      }
      else if (x_paid < max_x)
      {
        sprintf(s.mana_text + strlen(s.mana_text), gs_mana_so_far_max_00791420, s.symbol, x_paid, max_x);
      }
    }
    else if (mana_cost[s.color] != 0)
    {
      for (s.i = 0; s.i < mana_cost[s.color]; s.i++)
      {
        strcat(s.mana_text, s.symbol);
      }
    }
  }

  strcpy(g_ui_message_buffer, s.tmp);
  strcat(g_ui_message_buffer, ", ");
  sprintf(s.tmp, gs_tap_for_mana_0091c230, s.mana_text);
  strcat(g_ui_message_buffer, s.tmp);

  return 0;
}

// FUNCTION: MAGIC 0x00434af9
// FUNCTION: SHANDALAR 0x00415fd7
int activate_mana_sources_for_payment(int player,
                                      int *mana_paid_by_color,
                                      int *total_mana_paid,
                                      int activation_flags,
                                      int include_special_mana)
{
  struct
  {
    int special_color;
    int produced_color;
    int scratch;
    int dual_mana_slot;
    int max_colorless;
    int color_scratch;
    int color;
    int can_activate;
    int current_card;
    int any_variable_costs;
  } s;

  s.any_variable_costs = 0;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    if (g_mana_charge[s.color] == -1)
    {
      s.any_variable_costs = 1;
    }
  }

  if (player == g_other_player && (g_duel_network_flags & 2) != 0)
  {
    s.current_card = 0;
    while (s.current_card != -1)
    {
      TENTATIVE_wait_for_network_result(player, 0x10);
      s.current_card = g_network_result_packet.result;
      if (s.current_card != -1 && activate_mana_source_for_payment(player, s.current_card) != 0)
      {
        s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);
        auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
        auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
        auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
      }
    }
    copy_mana_pool_to_display();
  }
  else
  {
    if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
      {
        if (can_activate_mana_source(player, s.current_card, activation_flags) != 0 &&
            dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          for (s.color = 0; s.color < 7; ++s.color)
          {
            if (g_mana_charge[s.color] > 0 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
            else if (g_mana_charge[s.color] == -1 && g_x_value < g_max_x_value && g_max_x_value != -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
          }
          if (include_special_mana != 0 && !s.can_activate)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && g_mana_color_conversions[player][s.dual_mana_slot] != -1)
            {
              s.produced_color = (unsigned short)g_mana_color_conversions[player][s.dual_mana_slot];
              if (g_mana_charge[g_mana_color_conversions[player][s.dual_mana_slot] >> 16] > 0 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              else if (g_mana_charge[g_mana_color_conversions[player][s.dual_mana_slot] >> 16] == -1 && g_x_value < g_max_x_value && g_max_x_value != -1 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.can_activate)
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              g_network_result_packet.packet_type = 0x10;
              g_network_result_packet.result = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
            }
          }
        }
      }
    }

    if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0 &&
        (g_mana_charge[0] != 0 || g_mana_charge[6] != 0))
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
      {
        if (can_activate_mana_source(player, s.current_card, activation_flags) != 0 &&
            dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          if (g_mana_charge[0] < 1)
          {
            if (g_mana_charge[0] == -1 && g_x_value < g_max_x_value && g_max_x_value != -1)
            {
              s.can_activate = 1;
            }
            else if (g_mana_charge[6] < 1)
            {
              if (g_mana_charge[6] == -1 && g_x_value < g_max_x_value && g_max_x_value != -1)
              {
                s.can_activate = 1;
              }
            }
            else
            {
              s.can_activate = 1;
            }
          }
          else
          {
            s.can_activate = 1;
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && g_mana_charge[6] == 0)
          {
            s.can_activate = 0;
          }
          if (s.can_activate)
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              g_network_result_packet.packet_type = 0x10;
              g_network_result_packet.result = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
            }
          }
        }
      }
    }

    if (is_mana_cost_paid(g_mana_charge, g_x_value, g_max_x_value) == 0 &&
        s.any_variable_costs && g_max_x_value == -1)
    {
      for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
      {
        if (can_activate_mana_source(player, s.current_card, activation_flags) != 0 &&
            dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          for (s.color = 1; s.color < 7; ++s.color)
          {
            if (g_max_x_value == -1 && g_mana_charge[s.color] == -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
            else if (g_max_x_value == -1 && g_mana_charge[0] == -1)
            {
              s.can_activate = 1;
            }
            else if (g_max_x_value == -1 && g_mana_charge[6] == -1)
            {
              s.can_activate = 1;
            }
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && g_mana_charge[6] != -1)
          {
            s.can_activate = 0;
          }
          if (include_special_mana != 0 && !s.can_activate)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && g_mana_color_conversions[player][s.dual_mana_slot] != -1)
            {
              s.produced_color = (unsigned short)g_mana_color_conversions[player][s.dual_mana_slot];
              s.special_color = ((unsigned int)g_mana_color_conversions[player][s.dual_mana_slot] >> 16) & 0xffff;
              if (g_max_x_value == -1 && g_mana_charge[s.special_color] == -1 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.can_activate)
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              g_network_result_packet.packet_type = 0x10;
              g_network_result_packet.result = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &g_x_value, g_max_x_value);
            }
          }
        }
      }
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet.packet_type = 0x10;
      g_network_result_packet.result = -1;
      TENTATIVE_send_network_result(player, 0x10);
    }
    copy_mana_pool_to_display();
  }

  return 1;
}

// FUNCTION: MAGIC 0x00435643
// FUNCTION: SHANDALAR 0x00416b1c
int can_activate_mana_source(int player, int card, int activation_flags)
{
  struct
  {
    int csvid;
    int result;
    int internal_card_id;
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.csvid = global_cards_data[s.internal_card_id].id;
  s.result = 1;

  if ((PLAYER_CARD_INSTANCE(player, card).state & 0x800002) == 2 && (global_cards_data[s.internal_card_id].extra_ability & 0x1000) != 0 && (global_cards_data[s.internal_card_id].extra_ability & 0x10000) == 0)
  {
    if ((activation_flags & 1) && (s.internal_card_id <= 4 || s.csvid == 0x366 || PLAYER_CARD_INSTANCE(player, card).mana_color == '@'))
    {
      s.result = 0;
    }
    if ((activation_flags & 2) && (global_cards_data[s.internal_card_id].type & TYPE_LAND) != 0 && s.internal_card_id > 4 && s.csvid != 0x366 && PLAYER_CARD_INSTANCE(player, card).mana_color != '@')
    {
      s.result = 0;
    }
    if ((activation_flags & 4) && (PLAYER_CARD_INSTANCE(player, card).state & 0x40000) != 0)
    {
      s.result = 0;
    }
    if ((activation_flags & 8) && (global_cards_data[s.internal_card_id].type & TYPE_ARTIFACT) != 0)
    {
      s.result = 0;
    }
    if ((activation_flags & 0x10) && (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
    {
      s.result = 0;
    }
  }
  else
  {
    s.result = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00435881
// FUNCTION: SHANDALAR 0x00416d5a
int activate_mana_source_for_payment(int player, int card)
{
  struct
  {
    int old_max_x_value;
    int result;
    int saved_mana_cost[8];
    int old_x_value;
    int color;
  } s;
  unsigned int original_state;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 || (((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) != 0) && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    return 0;
  }

  s.old_x_value = g_x_value;
  g_x_value = 0;
  s.old_max_x_value = g_max_x_value;
  g_max_x_value = -1;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    s.saved_mana_cost[s.color] = g_mana_charge[s.color];
    g_mana_charge[s.color] = 0;
  }
  push_card_onto_stack(player, card, 0x72, player, 0);
  g_required_mana_color_mask = get_required_mana_color_mask(s.saved_mana_cost);
  g_produced_mana_color = -1;
  original_state = PLAYER_CARD_INSTANCE(player, card).state;
  dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
  g_required_mana_color_mask = 0;
  if (g_spell_fizzled == 1)
  {
    g_spell_fizzled = 0;
    obliterate_top_card_of_stack();
    s.result = 0;
  }
  else
  {
    if ((original_state & STATE_TAPPED) == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_TAP);
    }
    resolve_top_card_on_stack();
    s.result = 1;
  }
  g_max_x_value = s.old_max_x_value;
  g_x_value = s.old_x_value;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    g_mana_charge[s.color] = s.saved_mana_cost[s.color];
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00435b18
// FUNCTION: SHANDALAR 0x00416ff1
void spend_mana_for_cost(int *mana_cost,
                         int cost_color,
                         int amount,
                         int *special_mana,
                         int max_x,
                         int player,
                         int mana_color,
                         int *mana_paid_by_color,
                         int *total_mana_paid)
{
  (void)max_x;

  unproduce_mana(player, mana_color, amount);
  if (mana_cost[cost_color] == -1)
  {
    *special_mana += amount;
  }
  else
  {
    mana_cost[cost_color] -= amount;
  }
  mana_paid_by_color[mana_color] += amount;
  record_paid_mana(mana_color, amount);
  *total_mana_paid += amount;
}

// FUNCTION: MAGIC 0x00435b88
// FUNCTION: SHANDALAR 0x00417061
int get_mana_payment_amount(int *mana_cost,
                            int cost_color,
                            int *mana_available,
                            int mana_color,
                            int allow_multi_pay,
                            int max_x,
                            int x_paid)
{
  int amount;

  if (allow_multi_pay != 0)
  {
    if (mana_cost[cost_color] == -1)
    {
      if (max_x == -1)
      {
        amount = mana_available[mana_color];
      }
      else
      {
        amount = mana_available[mana_color] < max_x - x_paid ? mana_available[mana_color] : max_x - x_paid;
      }
    }
    else
    {
      amount = mana_cost[cost_color] >= mana_available[mana_color] ? mana_available[mana_color] : mana_cost[cost_color];
    }
  }
  else
  {
    amount = 1;
  }

  return amount;
}

// FUNCTION: MAGIC 0x00435c1f
// FUNCTION: SHANDALAR 0x004170f8
int begin_mana_payment_record(void)
{
  int color;

  if (g_mana_payment_record_depth < 10)
  {
    for (color = 0; color < 7; ++color)
    {
      g_recorded_mana_payments[g_mana_payment_record_depth][color] = 0;
    }
    ++g_mana_payment_record_depth;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435c91
// FUNCTION: SHANDALAR 0x0041716b
int record_paid_mana(int color, int amount)
{
  if (0 < g_mana_payment_record_depth)
  {
    g_recorded_mana_payments[g_mana_payment_record_depth - 1][color] += amount;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x00435cdd
// FUNCTION: SHANDALAR 0x004171b7
int end_mana_payment_record(void)
{
  if (g_mana_payment_record_depth > 0)
  {
    --g_mana_payment_record_depth;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435d11
// FUNCTION: SHANDALAR 0x004171eb
int restore_recorded_mana_to_pool(int player)
{
  int color;

  if (g_mana_payment_record_depth > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      g_raw_mana_available[player][color] += g_recorded_mana_payments[g_mana_payment_record_depth - 1][color];
      g_raw_mana_available[player][7] += g_recorded_mana_payments[g_mana_payment_record_depth - 1][color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435db0
// FUNCTION: SHANDALAR 0x0041728a
int refund_paid_mana(int *mana_paid_by_color)
{
  int color;

  if (g_mana_payment_record_depth > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      g_recorded_mana_payments[g_mana_payment_record_depth - 1][color] -= mana_paid_by_color[color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MOK 0x004302c0
// FUNCTION: MAGIC 0x00435e27
// FUNCTION: SHANDALAR 0x00417301
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount)
{
  int result;

  g_mana_charge[0] += g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)];
  result = charge_mana(player, color, amount) - g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)];
  if (g_spell_fizzled == 1)
  {
    return 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0043fdb3
// FUNCTION: SHANDALAR 0x0040b633
int can_pay_card_mana_cost(int player, int target_player, int target_card)
{
  struct
  {
    int color;
    int internal_card_id;
    int result;
  } s;

  s.result = 1;
  if (player == -1 || target_player == -1 || target_card == -1)
  {
    return 0;
  }

  s.internal_card_id = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
  if (s.internal_card_id == -1)
  {
    return 0;
  }

  if ((global_cards_data[s.internal_card_id].type & TYPE_ARTIFACT) != 0)
  {
    if (has_mana(player,
                 COLOR_ARTIFACT,
                 (int)(char)global_cards_data[s.internal_card_id].cc[0] +
                     (int)(char)global_cards_data[s.internal_card_id].cc[1]) == 0)
    {
      s.result = 0;
    }
  }
  else
  {
    s.color = single_color_test_bit_to_color_t((int)(char)global_cards_data[s.internal_card_id].color);
    if (has_mana_w_global_cost_mod(player,
                                   target_card,
                                   s.color,
                                   (int)(char)global_cards_data[s.internal_card_id].cc[0]) == 0)
    {
      s.result = 0;
    }

    if ((int)(char)global_cards_data[s.internal_card_id].cc[1] > 0)
    {
      if (has_mana_w_global_cost_mod(player,
                                     target_card,
                                     COLOR_ANY,
                                     (int)(char)global_cards_data[s.internal_card_id].cc[0] +
                                         (int)(char)global_cards_data[s.internal_card_id].cc[1]) == 0)
      {
        s.result = 0;
      }
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00442cf9
// FUNCTION: SHANDALAR 0x0040e593
int is_nonactivated_mana_source(int player, int card)
{
  int result;

  if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
  {
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & EA_ACT_ABILITY) != 0)
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
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00445f61
// FUNCTION: SHANDALAR 0x004117fa
int card_activation_uses_x(int player, int card)
{
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (g_stack_proxy_internal_card_id == internal_card_id)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  }

  if (internal_card_id == -1)
  {
    return 0;
  }

  if ((global_cards_data[internal_card_id].extra_ability & EA_ACT_USE_X) != 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x0044b3d4
// FUNCTION: SHANDALAR 0x00403d55
void resolve_mana_burn(void)
{
  int current_card;
  int player;

  if ((g_duel_network_flags & 2) == 0)
  {
  loop:
    if (g_raw_mana_available[g_other_player][7] != 0 && g_current_phase > 0x1a)
    {
      for (current_card = 0;
           current_card < g_active_cards_count[g_other_player];
           current_card++)
      {
        if (is_in_play(g_other_player, current_card) != 0)
        {
          dispatch_event_to_single_card(g_other_player, current_card, 0x8f, 1 - g_other_player, -1);
          if (g_last_single_card_event_result != 0)
          {
            if (activate(g_other_player, g_other_player, current_card) != 0)
            {
              resolve_activated_ability(g_other_player, current_card);
            }
            goto loop;
          }
        }
      }
    }
  }

  for (player = 0; player < 2; player = player + 1)
  {
    if (g_raw_mana_available[player][7] > 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x2a);
        show_mana_burn_dialog(player, g_raw_mana_available[player][7]);
      }
      g_life[player] = g_life[player] - g_raw_mana_available[player][7];
      for (current_card = 0; current_card <= 7; current_card = current_card + 1)
      {
        g_raw_mana_available[player][current_card] = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044b6af
// FUNCTION: SHANDALAR 0x00404030
int can_activate_mana_source_for_stop_prompt(int player, int card)
{
  if (is_in_play(player, card) != 0 &&
      (global_cards_data[(global_card_instances[player])[card].internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
      ((global_card_instances[player])[card].state & STATE_TAPPED) == 0 &&
      (((global_card_instances[player])[card].state & STATE_SUMMONSICK_BOTH) == 0 ||
       (global_cards_data[(global_card_instances[player])[card].internal_card_id].type & TYPE_CREATURE) == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x0044b7d4
// FUNCTION: SHANDALAR 0x00404155
int activate_mana_source_card(int player, int card)
{
  g_required_mana_color_mask = 0x3e;
  g_produced_mana_color = -1;

  dispatch_event_to_single_card(player, card, 0x6d, 1 - player, -1);

  if (g_spell_fizzled != 1)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, 0x81);
    }

    process_damage_prevention(player);
    regenerate_or_graveyard_triggers();
  }
  else
  {
    g_spell_fizzled = 0;
  }

  g_required_mana_color_mask = 0;
  return g_produced_mana_color;
}

// FUNCTION: MAGIC 0x004a6d0e
// FUNCTION: SHANDALAR 0x00558e90
void count_mana(void)
{
  struct
  {
    card_instance_t *instance;
    int current_card;
    int player;
  } s;

  for (s.current_card = 0; s.current_card <= 7; ++s.current_card)
  {
    g_mana_cost_modifiers_by_color[s.current_card] = 0;
    g_special_mana_pool[1][s.current_card] = g_mana_cost_modifiers_by_color[s.current_card];
    g_special_mana_pool[0][s.current_card] = g_special_mana_pool[1][s.current_card];
  }

  g_raw_mana_available_hex[1][0] = -1;
  g_raw_mana_available_hex[0][0] = g_raw_mana_available_hex[1][0];
  g_mana_color_conversions[1][0] = -1;
  g_mana_color_conversions[0][0] = g_mana_color_conversions[1][0];
  for (s.player = 0; s.player < 2; ++s.player)
  {
    s.instance = global_card_instances[s.player];
    for (s.current_card = 0; s.current_card < g_active_cards_count[s.player];
         ++s.current_card, ++s.instance)
    {
      if (s.instance->internal_card_id == -1)
      {
        continue;
      }

      if ((global_cards_data[s.instance->internal_card_id].extra_ability & 0x1000) != 0 && (s.instance->state & 0x800012) == 2)
      {
        dispatch_event(s.player, s.current_card, EVENT_COUNT_MANA);
      }
      if (global_cards_data[s.instance->internal_card_id].id == 0xee && (s.instance->state & 0x800012) == 2)
      {
        dispatch_event_to_single_card(s.player, s.current_card, EVENT_COUNT_MANA, -1, -1);
      }
      if (global_cards_data[s.instance->internal_card_id].id == 100 && (s.instance->state & 0x800002) == 2)
      {
        dispatch_event_to_single_card(s.player, s.current_card, EVENT_COUNT_MANA, -1, -1);
      }
    }
  }
  if ((g_battlefield_extra_ability_flags & 0x08000000) != 0)
  {
    for (s.player = 0; s.player < 2; ++s.player)
    {
      s.instance = global_card_instances[s.player];
      for (s.current_card = 0; s.current_card < g_active_cards_count[s.player];
           ++s.current_card, ++s.instance)
      {
        if (s.instance->internal_card_id == -1)
        {
          continue;
        }

        if (global_cards_data[s.instance->internal_card_id].code_pointer == card_fellwar_stone && (s.instance->state & 0x800012) == 2)
        {
          dispatch_event_to_single_card(s.player, s.current_card, EVENT_VARIABLE_MANA_SRC, -1, -1);
        }
      }
    }
  }
}
// FUNCTION: MAGIC 0x004a6faa
// FUNCTION: SHANDALAR 0x0055912c
void C_count_colors_of_lands_in_play(void)
{
  int player;
  int card;
  int color;
  int internal_card_id;

  for (color = 0; color <= 7; ++color)
  {
    g_landwalk_basiclandtypes_controlled[0][color] = 0;
    g_basiclandtypes_controlled[0][color] = g_landwalk_basiclandtypes_controlled[0][color];
  }
  g_mana_pool_colorless_components[1] = 0;
  g_mana_pool_colorless_components[0] = g_mana_pool_colorless_components[1];

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < g_active_cards_count[player]; ++card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      if (is_in_play(player, card) && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
      {
        g_mana_pool_colorless_components[player] |= PLAYER_CARD_INSTANCE(player, card).mana_color;
        if (internal_card_id < 5)
        {
          ++g_basiclandtypes_controlled[player][internal_card_id];
        }
        else if (internal_card_id > 0xe && internal_card_id < g_card_count)
        {
          ++g_basiclandtypes_controlled[player][0];
        }
        else
        {
          switch (global_cards_data[internal_card_id].id)
          {
            case 0xc:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 9:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              break;
            case 0xbd:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xd4:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              break;
            case 0xd8:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              break;
            case 0xf1:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xfc:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0xfe:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x102:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x10a:
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              ++g_basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xef:
              ++g_basiclandtypes_controlled[player][1];
              break;
            case 0x7e:
              ++g_basiclandtypes_controlled[player][2];
              break;
            case 0x5b:
              ++g_basiclandtypes_controlled[player][3];
              break;
            case 0xa4:
              ++g_basiclandtypes_controlled[player][4];
              break;
            case 0xbc:
              ++g_basiclandtypes_controlled[player][5];
              break;
            default:
              ++g_basiclandtypes_controlled[player][0];
              break;
          }
        }
        ++g_basiclandtypes_controlled[player][7];
      }
    }
  }
}

// FUNCTION: MAGIC 0x004eab5c
// FUNCTION: SHANDALAR 0x004412ec
void declare_mana_available_hex(int player, color_test_t colors, int amount)
{
  int i;

  if ((int)colors > 0)
  {
    for (i = 0; i < 0x32; ++i)
    {
      if (g_raw_mana_available_hex[player][i] == -1)
      {
        g_raw_mana_available_hex[player][i] = (amount << 16) | colors;
        g_raw_mana_available_hex[player][i + 1] = -1;
        break;
      }
    }
  }
}

// FUNCTION: MAGIC 0x004eac0c
// FUNCTION: SHANDALAR 0x0044139c
void undeclare_mana_available_hex(int player, color_test_t color, int amount)
{
  struct
  {
    int j;     /* [ebp-0x0c] */
    int i;     /* [ebp-0x08] */
    int found; /* [ebp-0x04] */
  } s;

  s.found = 0;
  s.i = 0;

  while (s.i < 0x32)
  {
    if (g_raw_mana_available_hex[player][s.i] == -1)
    {
      break;
    }

    if (s.found != 0)
    {
      break;
    }

    if (g_raw_mana_available_hex[player][s.i] == ((amount << 16) | color))
    {
      s.found = 1;
      s.j = s.i;
      while (s.j < 0x31)
      {
        g_raw_mana_available_hex[player][s.j] = g_raw_mana_available_hex[player][s.j + 1];
        ++s.j;
      }
    }

    ++s.i;
  }
}

// FUNCTION: MAGIC 0x004eaceb
// FUNCTION: SHANDALAR 0x0044147b
void declare_mana_color_conversion(int player, int color_to_produce, int color_to_consume)
{
  struct
  {
    int slot;
    int found;
  } s;

  if (color_to_produce > 0 && color_to_consume > 0)
  {
    s.found = 0;
    s.slot = 0;
    for (; s.slot < 10 && !s.found; ++s.slot)
    {
      if (g_mana_color_conversions[player][s.slot] == -1)
      {
        s.found = 1;
        g_mana_color_conversions[player][s.slot] = MAKELONG(color_to_produce, color_to_consume);
        g_mana_color_conversions[player][s.slot + 1] = -1;
      }
    }
  }
}

// FUNCTION: MAGIC 0x004eada9
// FUNCTION: SHANDALAR 0x00441539
int declare_mana_available(int player, color_t color, int amount)
{
  g_special_mana_pool[player][color] += amount;
  g_special_mana_pool[player][COLOR_ANY] += amount;
  return g_special_mana_pool[player][color];
}

// FUNCTION: MAGIC 0x004eadeb
// FUNCTION: SHANDALAR 0x0044157b
int undeclare_mana_available(int player, color_t color, int amount)
{
  g_special_mana_pool[player][color] -= amount;
  g_special_mana_pool[player][COLOR_ANY] -= amount;
  return g_special_mana_pool[player][color];
}

// FUNCTION: MAGIC 0x004eae35
// FUNCTION: SHANDALAR 0x004415c5
int produce_mana(int player, color_t color, int amount)
{
  g_raw_mana_available[player][color] += amount;
  g_raw_mana_available[player][7] += amount;
  return g_raw_mana_available[player][color];
}

int select_target(int player, int card, target_definition_t *td, const char *prompt, target_t *ret_location)
{
  (void)player;
  (void)card;
  (void)td;
  (void)prompt;
  if (ret_location != NULL)
  {
    ret_location->player = -1;
    ret_location->card = -1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004eae77
// FUNCTION: SHANDALAR 0x00441607
int unproduce_mana(int player, int color, int amount)
{
  g_raw_mana_available[player][color] -= amount;
  g_raw_mana_available[player][7] -= amount;
  return g_raw_mana_available[player][color];
}

// FUNCTION: MAGIC 0x004eaec1
// FUNCTION: SHANDALAR 0x00441651
int undeclare_mana_available_and_produce_it(int player, color_t color, int amount)
{
  undeclare_mana_available(player, color, amount);
  produce_mana(player, color, amount);
  return g_raw_mana_available[player][color];
}

// FUNCTION: MOK 0x0049d510
// FUNCTION: MAGIC 0x004eaf09
// FUNCTION: SHANDALAR 0x00441699
int has_mana(int player, color_t color, int amount)
{
  struct
  {
    int result;                   // ebp-0x2c
    int available_mana;           // ebp-0x28
    unsigned int interchangeable; // ebp-0x24
    color_t effective_color;      // ebp-0x20
    int current_color;            // ebp-0x1c
    unsigned int mana_color;      // ebp-0x18
    int special_mana;             // ebp-0x14
    int artifact_mana;            // ebp-0x10
    int xmana;                    // ebp-0xc
    int artifact_special_mana;    // ebp-0x8
    int mana_source_color;        // ebp-0x4 (only low byte used)
  } s;

  if (color == COLOR_ARTIFACT)
  {
    s.effective_color = COLOR_ANY;
  }
  else if (color == COLOR_COLORLESS)
  {
    s.effective_color = COLOR_ANY;
  }
  else
  {
    s.effective_color = color;
  }

  if (amount == 0)
  {
    return 1;
  }

  s.interchangeable = 0;
  for (s.current_color = 0; s.current_color < 10 && g_mana_color_conversions[player][s.current_color] != -1; ++s.current_color)
  {
    s.mana_source_color = (unsigned short)g_mana_color_conversions[player][s.current_color];
    s.mana_color = HIWORD(g_mana_color_conversions[player][s.current_color]);
    if (s.effective_color == (color_t)s.mana_color)
    {
      s.interchangeable |= 1 << (unsigned char)s.mana_source_color;
    }
  }

  s.available_mana = g_raw_mana_available[player][s.effective_color];
  for (s.current_color = COLOR_COLORLESS; s.current_color < 7; ++s.current_color)
  {
    if (s.effective_color != (color_t)s.current_color && (s.interchangeable & (1 << (unsigned char)s.current_color)) != 0)
    {
      s.available_mana += g_raw_mana_available[player][s.current_color];
    }
  }

  s.artifact_mana = g_raw_mana_available[player][COLOR_ARTIFACT];

  s.special_mana = g_special_mana_pool[player][s.effective_color];
  for (s.current_color = COLOR_COLORLESS; s.current_color < 7; ++s.current_color)
  {
    if (s.effective_color != (color_t)s.current_color && (s.interchangeable & (1 << (unsigned char)s.current_color)) != 0)
    {
      s.special_mana += g_special_mana_pool[player][s.current_color];
    }
  }

  s.xmana = 0;
  for (s.current_color = 0; s.current_color < 0x32 && g_raw_mana_available_hex[player][s.current_color] != -1; ++s.current_color)
  {
    if (s.effective_color == COLOR_ANY)
    {
      s.xmana += g_raw_mana_available_hex[player][s.current_color] >> 16;
    }
    else if ((g_raw_mana_available_hex[player][s.current_color] & (1 << (unsigned char)s.effective_color)) != 0)
    {
      s.xmana += g_raw_mana_available_hex[player][s.current_color] >> 16;
    }
    else
    {
      if ((s.interchangeable & g_raw_mana_available_hex[player][s.current_color]) != 0)
      {
        s.xmana += g_raw_mana_available_hex[player][s.current_color] >> 16;
      }
    }
  }

  s.artifact_special_mana = g_special_mana_pool[player][COLOR_ARTIFACT];

  if (color == COLOR_ANY || color == COLOR_COLORLESS)
  {
    s.result = ((s.available_mana - s.artifact_mana) + s.special_mana - s.artifact_special_mana) + s.xmana;
  }
  else
  {
    s.result = (s.available_mana + s.special_mana) + s.xmana;
  }

  if (s.result >= amount)
  {
    return s.result;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MOK 0x0049d710
// FUNCTION: MAGIC 0x004eb23d
// FUNCTION: SHANDALAR 0x004419ca
int has_mana_w_global_cost_mod(int player, int card, color_t color, int amount)
{
  int result;

  result = 0;

  if (amount == 0)
  {
    if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
    {
      return has_mana(
          player, 7, g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)]);
    }

    return 1;
  }
  else
  {
    result = has_mana(player, color, amount);
    if (result != 0)
    {
      if (g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
      {
        return has_mana(
            player,
            7,
            g_mana_cost_modifiers_by_color[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] + amount);
      }
      else
      {
        return result;
      }
    }
  }

  return 0;
}
