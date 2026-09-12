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

int can_target(target_definition_t *td)
{
  return td != NULL;
}

// FUNCTION: MAGIC 0x0048194e
// FUNCTION: SHANDALAR 0x00485458
int select_best_land_target_by_score(int preferred_player, int only_player, int *target_data)
{
  struct
  {
    card_instance_t *instance; /* [ebp-0x84] */
    int card;                  /* [ebp-0x80] */
    int mana_colors[8];        /* [ebp-0x7c] */
    unsigned int colors;       /* [ebp-0x5c] */
    int card_colors[8];        /* [ebp-0x58] */
    int current_score;         /* [ebp-0x38] */
    int color;                 /* [ebp-0x34] */
    int best_score;            /* [ebp-0x30] */
    int land_scores[8];        /* [ebp-0x2c] */
    int best_card_color;       /* [ebp-0x0c] */
    int player;                /* [ebp-0x08] */
    int best_land_color;       /* [ebp-0x04] */
  } s;

  target_data[1] = -1;
  *target_data = target_data[1];

  for (s.color = 0; s.color <= 5; ++s.color)
  {
    s.card_colors[s.color] = 0;
    s.land_scores[s.color] = s.card_colors[s.color];
    s.mana_colors[s.color] = s.land_scores[s.color];
  }

  for (s.player = 0; s.player < 2; ++s.player)
  {
    if (only_player != -1 && s.player != only_player)
      continue;

    for (s.card = 0; s.card < g_active_cards_count[s.player]; ++s.card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.player, s.card);
      if (s.instance->internal_card_id != -1)
      {
        if ((s.instance->state & 0x800022) == 2)
        {
          if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
          {
            for (s.color = 1; s.color <= 5; ++s.color)
            {
              if ((((int)(char)s.instance->mana_color) & (1 << (unsigned char)s.color)) != 0)
              {
                ++s.mana_colors[s.color];
              }
            }
          }

          if (((int)s.instance->color & 0x3e) != 0)
          {
            for (s.color = 1; s.color <= 5; ++s.color)
            {
              if (((int)s.instance->color & (1 << (unsigned char)s.color)) != 0)
              {
                ++s.land_scores[s.color];
              }
            }
          }
        }
        else if (preferred_player == only_player || only_player == -1)
        {
          s.colors = (unsigned int)(char)global_cards_data[s.instance->internal_card_id].color;
          for (s.color = 1; s.color <= 5; ++s.color)
          {
            if ((s.colors & (1 << (unsigned char)s.color)) != 0)
            {
              ++s.card_colors[s.color];
            }
          }
        }
      }
    }
  }

  s.best_card_color = 0;
  s.best_land_color = s.best_card_color;

  for (s.color = 1; s.color <= 5; ++s.color)
  {
    if (s.land_scores[s.color] > s.land_scores[s.best_land_color])
    {
      s.best_land_color = s.color;
    }
    if (s.card_colors[s.color] > s.card_colors[s.best_card_color])
    {
      s.best_card_color = s.color;
    }
  }

  s.best_score = -1;

  for (s.player = 0; s.player < 2; ++s.player)
  {
    if (only_player != -1 && s.player != only_player)
      continue;

    for (s.card = 0; s.card < g_active_cards_count[s.player]; ++s.card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.player, s.card);
      if (is_in_play(s.player, s.card) != 0)
      {
        if ((global_cards_data[s.instance->internal_card_id].type & TYPE_LAND) != 0)
        {
          s.current_score = 0;

          if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_ACT_ABILITY) != 0)
          {
            s.current_score += 2;
            if ((s.instance->state & STATE_TAPPED) != 0)
            {
              s.current_score += 3;
            }
          }

          if ((global_cards_data[s.instance->internal_card_id].extra_ability & EA_MANA_SOURCE) != 0)
          {
            ++s.current_score;

            s.colors = (unsigned int)(char)s.instance->mana_color;
            for (s.color = 1; s.color <= 5; ++s.color)
            {
              if ((s.colors & (1 << (unsigned char)s.color)) != 0)
              {
                s.current_score += ((4 - s.mana_colors[s.color]) > 0) ? (4 - s.mana_colors[s.color]) : 0;
              }
            }

            if (s.best_land_color != 0 && (s.colors & (1 << (unsigned char)s.best_land_color)) != 0)
            {
              ++s.current_score;
            }
            if (preferred_player == only_player || only_player == -1)
            {
              if (s.best_card_color != 0 && (s.colors & (1 << (unsigned char)s.best_card_color)) != 0)
              {
                ++s.current_score;
              }
            }
          }

          if ((global_cards_data[s.instance->internal_card_id].type & TYPE_CREATURE) != 0)
          {
            ++s.current_score;
          }

          if (s.current_score > s.best_score)
          {
            s.best_score = s.current_score;
            *target_data = s.player;
            target_data[1] = s.card;
          }
        }
      }
    }
  }
  if (*target_data == -1)
    return 0;
  else
    return 1;
}

// FUNCTION: MAGIC 0x00483e3e
// FUNCTION: SHANDALAR 0x0048794a
int choose_best_card_from_library(int player, unsigned int type_mask)
{
  int best_score;
  int best_slot;
  int current_slot;
  int current_internal_id;
  int current_score;
  int mana_score;
  int abilities;
  int bonus;

  best_score = -999;
  best_slot = -1;

  if (player == -1)
  {
    return best_slot;
  }

  for (current_slot = 0; current_slot < 500 && global_library[player][current_slot] != -1; ++current_slot)
  {
    current_internal_id = global_library[player][current_slot];

    if (type_mask == 0xffffffff || (type_mask & (unsigned int)global_cards_data[current_internal_id].type) != 0)
    {
      current_score = (int)global_cards_data[current_internal_id].power + (int)global_cards_data[current_internal_id].toughness;

      if ((int)(char)global_cards_data[current_internal_id].cc[1] == -1)
      {
        mana_score =
            g_basiclandtypes_controlled[player][7] - (int)(char)global_cards_data[current_internal_id].cc[0] - 1;
      }
      else
      {
        mana_score = (int)(char)global_cards_data[current_internal_id].cc[1];
      }
      mana_score = (int)(char)global_cards_data[current_internal_id].cc[0] + mana_score;

      if (mana_score <= g_basiclandtypes_controlled[player][7])
      {
        current_score += mana_score;
      }

      abilities = (int)global_cards_data[current_internal_id].static_ability;
      while (abilities != 0)
      {
        if ((abilities & 1) != 0)
        {
          ++current_score;
        }
        abilities >>= 1;
      }

      if ((global_cards_data[current_internal_id].extra_ability & 0x1000) != 0)
      {
        bonus = 8 - g_basiclandtypes_controlled[player][7];
        if (bonus < 2)
        {
          bonus = 1;
        }
        current_score += bonus;
      }

      if ((global_cards_data[current_internal_id].extra_ability & 1) != 0)
      {
        current_score += mana_score;
      }

      if ((global_cards_data[current_internal_id].type & TYPE_ARTIFACT) != 0)
      {
        ++current_score;
      }

      if (best_score < current_score)
      {
        best_score = current_score;
        best_slot = current_slot;
      }
    }
  }

  return best_slot;
}

// FUNCTION: MAGIC 0x004a62d7
// FUNCTION: SHANDALAR 0x00558457
int select_from_graveyard_with_dialog(int player,
                                      int *graveyard,
                                      void *available,
                                      int count,
                                      int prompt,
                                      int a6,
                                      int selected,
                                      int a8,
                                      int a9)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }
  return select_multiple_cards_from_card_list(player, graveyard, 0, available, count, prompt, a6, selected, a8, a9);
}

int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  (void)player;
  (void)card;
  return orig_color_test;
}

// FUNCTION: MAGIC 0x004a67bd
// FUNCTION: SHANDALAR 0x00558941
int choose_a_number(int player, char *prompt, int maxnum)
{
  int chosen;

  if (g_duel_ai_mode_state == 1)
  {
    return maxnum;
  }

  if (player == g_other_player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xe);
    maxnum = g_dialog_result_network_packet.result;
  }

  chosen = prompt_for_life_total(player, prompt, maxnum);

  if (player == g_active_player && (g_duel_network_flags & 2) != 0)
  {
    g_dialog_result_network_packet.packet_type = 0xe;
    g_dialog_result_network_packet.result = chosen;
    TENTATIVE_send_network_result(player, 0xe);
  }

  return chosen;
}

// FUNCTION: MAGIC 0x004a686a
// FUNCTION: SHANDALAR 0x005589ed
int choose_a_color(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors)
{
  int chosen_color;

  if (g_duel_ai_mode_state == 1)
  {
    return ai_choice;
  }

  if (g_other_player == player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xf);
    return g_dialog_result_network_packet.result;
  }

  chosen_color = choose_a_color_dialog(player, prompt, use_color_names_instead_of_land, ai_choice, available_colors);
  if (player == g_active_player && (g_duel_network_flags & 2) != 0)
  {
    g_dialog_result_network_packet.packet_type = '\x0f';
    g_dialog_result_network_packet.result = chosen_color;
    TENTATIVE_send_network_result(player, 0xf);
  }

  return chosen_color;
}

// FUNCTION: MAGIC 0x004b413c
// FUNCTION: SHANDALAR 0x0056a3a7
int select_damage_card_from_list(int player,
                                 int *internal_card_ids,
                                 int *damage_amounts,
                                 int count,
                                 char *title,
                                 int allow_cancel,
                                 char *prompt)
{
  int selected;

  selected = -1;

  if (player == g_other_player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    selected = g_network_result_packet.result;
  }

  if (player == g_active_player && g_duel_ai_mode_state != 1)
  {
    selected = show_damage_assignment_cardlist(internal_card_ids, damage_amounts, count, title, allow_cancel, prompt);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet.result = selected;
      g_network_result_packet.packet_type = 0x19;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return selected;
}

// FUNCTION: MAGIC 0x004b41f2
// FUNCTION: SHANDALAR 0x0056a45f
int select_card_from_graveyard(int player,
                               int *graveyard,
                               int *allowed_cards,
                               int count,
                               const char *prompt,
                               int allow_cancel,
                               char *title)
{
  int result;

  result = -1;

  if (g_other_player == player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    result = g_network_result_packet.result;
  }

  if (g_active_player == player && g_duel_ai_mode_state != 1)
  {
    result = (int)show_selectable_cardlist(graveyard,
                                           allowed_cards,
                                           count,
                                           (void *)prompt,
                                           (unsigned int)allow_cancel,
                                           title);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet.packet_type = 0x19;
      g_network_result_packet.result = result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return result;
}

#ifndef SHANDALAR
// FUNCTION: MOK 0x0047936d
// FUNCTION: MAGIC 0x004b42aa
int show_deck(int player, int *cards, int count, char *title, int require_selection, char *prompt)
{
  struct
  {
    int y_step;
    int selected_index;
    int display_count;
    int y_positions[500];
    int x_positions[500];
    int y;
    int x;
    int valid_count;
    int valid_graveyards[500];
    int i;
    int result;
  } s;

  if (((player == g_other_player) && ((g_duel_network_flags & 2) != 0)) && g_duel_active != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    return g_network_result_packet.result;
  }
  else if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || g_duel_ai_mode_state == 1)
  {
    s.valid_count = 0;
    for (s.i = 0; s.i < count; ++s.i)
    {
      if (cards[s.i] != -1)
      {
        s.valid_graveyards[s.valid_count] = s.i;
        ++s.valid_count;
      }
    }

    g_ai_recorded_choice = internal_rand(s.valid_count);
    if (player != g_active_player)
    {
      if (g_duel_ai_mode_state == 1)
      {
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }
    }

    return s.valid_graveyards[g_ai_recorded_choice];
  }
  else if (g_duel_active == 0)
  {
    ReadPalette("todpal.tr", (char *)0);
    legacy_clear_graphics_page_stub(0, 0);
    SelectPalette(g_duel_screen_dc, g_duel_palette, 0);
    legacy_load_pcx_into_page_no_palette_stub("advfac64.pic");
    legacy_load_pcx_into_page_stub(1, 0, 0, "seedeck.pic", &g_duel_palette_state);
    legacy_blit_graphics_rect_stub(g_duel_state_0057a75c, 0, 0, 0x280, 0x1e0, g_world_map_state, 0, 0, global_screen_width, global_screen_height);

    s.x = 0;
    s.display_count = 0;
    for (s.i = 0; s.i < count; ++s.i)
    {
      if (cards[s.i] != -1 && (s.i == 0 || cards[s.i - 1] != cards[s.i]))
      {
        ++s.display_count;
      }
    }

    s.valid_count = (s.display_count - 1) / 5;
    if (s.valid_count == 0)
    {
      s.valid_count = 1;
    }
    s.y_step = 0x54 / s.valid_count;

    s.x = 0x60;
    s.valid_count = 0;
    s.y = 0x10;
    for (s.i = 0; s.i < count; ++s.i)
    {
      if (cards[s.i] != -1 && (s.i == 0 || cards[s.i - 1] != cards[s.i]))
      {
        s.x_positions[s.valid_count] = s.x + 4;
        s.y_positions[s.valid_count] = s.y;
        s.valid_graveyards[s.valid_count] = s.i;
        ++s.valid_count;
        s.x += 0x38;
        if (s.x >= 0x140)
        {
          s.x = 0x60;
          s.y += s.y_step;
        }
      }
    }

    return s.selected_index;
  }
  else
  {
    s.result = show_cardlist_if_human(cards, count, title, require_selection, prompt);
    if ((player == g_active_player) && ((g_duel_network_flags & 2) != 0))
    {
      g_network_result_packet.result = s.result;
      g_network_result_packet.packet_type = 0x19;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return s.result;
}
#endif

// FUNCTION: MAGIC 0x004bd7d0
// FUNCTION: SHANDALAR 0x004be650
int real_target_available(int *num_valid_targets,
                          target_scan_mode_t target_source_mode,
                          int who_chooses,
                          unsigned int allowed_controller,
                          unsigned int preferred_controller,
                          target_zone_t zone,
                          type_t required_type,
                          type_t illegal_type,
                          keyword_t required_abilities,
                          keyword_t illegal_abilities,
                          color_test_t required_color,
                          color_test_t illegal_color,
                          int extra,
                          subtype_in_card_data_t required_subtype,
                          int required_power,
                          int required_toughness,
                          target_special_t special,
                          target_state_t required_state,
                          target_state_t illegal_state)
{
  struct
  {
    int target_card;
    int player_index;
    int target_is_valid;
    int found_any;
    int stop_on_first;
    int count;
    int done;
    int target_player;
    int current_card;
    int scan_player;
  } s;

  s.stop_on_first = num_valid_targets == NULL;
  s.count = 0;
  s.found_any = 0;

  if (target_source_mode != 0 && target_source_mode != 1 && target_source_mode != 2)
  {
    return 0;
  }

  s.done = 0;
  for (s.player_index = 0; s.player_index < 2; ++s.player_index)
  {
    if (C_real_validate_target(s.player_index,
                               -1,
                               (char *)0,
                               who_chooses,
                               allowed_controller,
                               preferred_controller,
                               zone,
                               required_type,
                               illegal_type,
                               required_abilities,
                               illegal_abilities,
                               required_color,
                               illegal_color,
                               extra,
                               required_subtype,
                               required_power,
                               required_toughness,
                               special,
                               required_state,
                               illegal_state) != 0)
    {
      s.found_any = 1;
      ++s.count;
      if (s.stop_on_first)
      {
        s.done = 1;
      }
    }
  }

  if (g_active_player == who_chooses || (g_duel_network_flags & 2) != 0)
  {
    if ((allowed_controller & 2) == 0)
    {
      s.scan_player = 1;
    }
    else
    {
      s.scan_player = 0;
    }
  }
  else if ((preferred_controller & 2) != 0 || (preferred_controller & 1) != 0)
  {
    s.scan_player = 1;
  }
  else
  {
    s.scan_player = 0;
  }

  for (s.player_index = 0;
       s.player_index < 2;
       ++s.player_index, s.scan_player = 1 - s.scan_player)
  {
    if (!s.done)
    {
      for (s.current_card = 0;
           s.current_card < (g_active_cards_count[1] > g_active_cards_count[0] ? g_active_cards_count[1]
                                                                               : g_active_cards_count[0]) &&
           !s.done;
           ++s.current_card)
      {
        if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id != -1)
        {
          if (target_source_mode == 0)
          {
            s.target_player = s.scan_player;
            s.target_card = s.current_card;
            s.target_is_valid = 1;
          }
          else if (target_source_mode == 1)
          {
            if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == g_damage_card_internal_card_id)
            {
              s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_player;
              s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_card;
              s.target_is_valid = 1;
            }
            else
            {
              s.target_is_valid = 0;
            }
          }
          else if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == g_damage_card_internal_card_id)
          {
            s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_player;
            s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_card;
            s.target_is_valid = 1;
          }
          else
          {
            s.target_is_valid = 0;
          }

          if (s.target_is_valid && C_real_validate_target(s.target_player,
                                                          s.target_card,
                                                          (char *)0,
                                                          who_chooses,
                                                          allowed_controller,
                                                          preferred_controller,
                                                          zone,
                                                          required_type,
                                                          illegal_type,
                                                          required_abilities,
                                                          illegal_abilities,
                                                          required_color,
                                                          illegal_color,
                                                          extra,
                                                          required_subtype,
                                                          required_power,
                                                          required_toughness,
                                                          special,
                                                          required_state,
                                                          illegal_state) != 0)
          {
            s.found_any = 1;
            ++s.count;
            if (s.stop_on_first)
            {
              s.done = 1;
            }
          }
        }
      }
    }
  }

  if (num_valid_targets != NULL)
  {
    *num_valid_targets = s.count;
  }
  return s.found_any;
}

// FUNCTION: MAGIC 0x004bdc06
// FUNCTION: SHANDALAR 0x004bea85
unsigned int C_real_validate_target(int tgt_player,
                                    int tgt_card,
                                    char *return_error_str,
                                    int who_chooses,
                                    int allowed_controller,
                                    int preferred_controller,
                                    target_zone_t zone,
                                    type_t required_type,
                                    type_t illegal_type,
                                    keyword_t required_abilities,
                                    keyword_t illegal_abilities,
                                    color_test_t required_color,
                                    color_test_t illegal_color,
                                    int extra,
                                    subtype_in_card_data_t required_subtype,
                                    int power_requirement,
                                    int toughness_requirement,
                                    target_special_t special,
                                    target_state_t required_state,
                                    target_state_t illegal_state)
{
  /* Keep locals as a single struct to stabilize MSVC 4.20 /Od stack layout. */
  struct
  {
    int tmp124; /* local_124 */
    int tmp120; /* local_120 */
    int unused_11c;
    int tmp118; /* local_118 */
    int tmp114; /* local_114 */
    int unused_110;
    int is_not_blocking;                /* local_10c */
    int is_not_attacking;               /* local_108 */
    unsigned int toughness_pt_val;      /* local_104 */
    unsigned int toughness_pt_cmp;      /* local_100 */
    unsigned int power_pt_val;          /* local_fc */
    unsigned int power_pt_cmp;          /* local_f8 */
    subtype_in_card_data_t alt_subtype; /* local_f4 */
    int illegal_type_match;             /* local_f0 */
    int illegal_type_iid;               /* local_ec */
    int type_match;                     /* local_e8 */
    int type_iid;                       /* local_e4 */
    int required_controller;            /* local_e0 */
    int required_owner;                 /* local_dc */
    int player_allowed_ai;              /* local_d8 */
    int player_allowed_human;           /* local_d4 */
    int is_illegal;                     /* bVar20 */
    char errbuf[200];                   /* local_d0 */
    unsigned int is_valid;              /* local_4 */
  } s;

  if (tgt_player == -1)
  {
    if (return_error_str != NULL)
    {
      // STRING: SHANDALAR 0x005899a0
      strcpy(return_error_str, "");
    }
    return 0;
  }

  if (tgt_player != -1 && tgt_card != -1)
  {
    if (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id == -1)
    {
      if (return_error_str != NULL)
      {
        // STRING: SHANDALAR 0x005899a4
        strcpy(return_error_str, "");
      }
      return 0;
    }
  }

  s.is_illegal = 0;
  // STRING: SHANDALAR 0x005899a8
  strcpy(s.errbuf, "");

  if (tgt_card == -1)
  {
    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((((g_active_player == who_chooses) || ((g_duel_network_flags & 2) != 0)) && ((allowed_controller & ANYBODY) != 0)) || (who_chooses == AI && ((preferred_controller & ANYBODY) != 0)))
      {
        s.player_allowed_human = AI;
        s.player_allowed_ai = 1;
      }
      else if ((((g_active_player == who_chooses) || ((g_duel_network_flags & 2) != 0)) && ((allowed_controller & AI) != 0)) || (who_chooses == AI && ((preferred_controller & AI) != 0)))
      {
        s.player_allowed_human = HUMAN;
        s.player_allowed_ai = 1;
      }
      else
      {
        if ((g_active_player == who_chooses) || ((g_duel_network_flags & 2) != 0))
        {
        }
        else
        {
          s.player_allowed_human = AI;
          s.player_allowed_ai = 0;
        }
      }
    }
    else
    {
      s.player_allowed_human = HUMAN;
      s.player_allowed_ai = 0;
    }

    if ((tgt_player == HUMAN && s.player_allowed_human == 0) || (tgt_player == AI && s.player_allowed_ai == 0))
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_player_008b3110);
    }
  }
  else
  {
    if ((g_active_player == who_chooses) || ((g_duel_network_flags & 2) != 0))
    {
      if ((allowed_controller & TARGET_PLAYER_OWNER) != 0)
      {
        if ((allowed_controller & ANYBODY) != 0)
        {
          s.required_owner = -1;
        }
        else if ((allowed_controller & AI) != 0)
        {
          s.required_owner = AI;
        }
        else
        {
          s.required_owner = HUMAN;
        }

        if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          s.required_controller = s.required_owner;
        }
        else
        {
          s.required_controller = -1;
        }
      }
      else
      {
        if ((allowed_controller & ANYBODY) != 0)
        {
          s.required_controller = -1;
        }
        else if ((allowed_controller & AI) != 0)
        {
          s.required_controller = AI;
        }
        else
        {
          s.required_controller = HUMAN;
        }

        if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          s.required_owner = s.required_controller;
        }
        else
        {
          s.required_owner = -1;
        }
      }
    }
    else
    {
      if ((preferred_controller & TARGET_PLAYER_OWNER) != 0)
      {
        if ((preferred_controller & ANYBODY) != 0)
        {
          s.required_owner = -1;
        }
        else if ((preferred_controller & AI) != 0)
        {
          s.required_owner = AI;
        }
        else
        {
          s.required_owner = HUMAN;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          s.required_controller = s.required_owner;
        }
        else
        {
          s.required_controller = -1;
        }
      }
      else
      {
        if ((preferred_controller & ANYBODY) != 0)
        {
          s.required_controller = -1;
        }
        else if ((preferred_controller & AI) != 0)
        {
          s.required_controller = AI;
        }
        else
        {
          s.required_controller = HUMAN;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          s.required_owner = s.required_controller;
        }
        else
        {
          s.required_owner = -1;
        }
      }
    }

    if ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_OUBLIETTED) != 0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_cant_target_this_0091a6e0);
    }

    if ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_CANNOT_TARGET) != 0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_cant_target_this_0091a6e0);
    }

    if (zone != 0 && (((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_IN_PLAY) != 0 && (zone & TARGET_ZONE_IN_PLAY) == 0) || ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_IN_PLAY) == 0 && (zone & TARGET_ZONE_HAND) == 0)))
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_where_0091cd00);
    }

    if (s.required_controller != -1 && tgt_player != s.required_controller)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_controller_0091bbe0);
    }

    if (s.required_owner != -1)
    {
      if (((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_OWNED_BY_OPPONENT) != 0 && s.required_owner == HUMAN) ||
          ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_OWNED_BY_OPPONENT) == 0 && s.required_owner != HUMAN))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_owner_007aacc0);
      }
    }

    if (required_type != TYPE_NONE)
    {
      if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) != 0)
      {
        s.type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).original_internal_card_id;
      }
      else
      {
        s.type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id;
      }

      s.type_match = 0;
      if (((required_type & global_cards_data[s.type_iid].type) != TYPE_NONE) || ((required_type & TARGET_TYPE_NONCREATURE_CAN_BLOCK) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_NONCREATURE_CAN_BLOCK) != 0))
      {
        s.type_match = 1;
      }
      if ((required_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.type_iid].id == g_effect_display_internal_card_id || global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id == g_hunting_display_internal_card_id))
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == g_damage_display_internal_card_id)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == g_legacy_display_internal_card_id)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == g_card_back_display_internal_card_id)
      {
        s.type_match = 1;
      }
      if (!s.type_match)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_type_008a9a00);
      }
    }

    if (illegal_type != TYPE_NONE)
    {
      if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) != 0)
      {
        s.illegal_type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).original_internal_card_id;
      }
      else
      {
        s.illegal_type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id;
      }

      s.illegal_type_match = 0;
      if ((illegal_type & global_cards_data[s.illegal_type_iid].type) != TYPE_NONE)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.illegal_type_iid].id == g_effect_display_internal_card_id || global_cards_data[s.illegal_type_iid].id == g_hunting_display_internal_card_id))
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == g_damage_display_internal_card_id)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == g_legacy_display_internal_card_id)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == g_card_back_display_internal_card_id)
      {
        s.illegal_type_match = 1;
      }
      if (s.illegal_type_match)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_type_008a9a00);
      }
    }

    if (required_abilities != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).regen_status & required_abilities) != required_abilities)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_abilities_0091a810);
    }
    if (illegal_abilities != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).regen_status & illegal_abilities) != 0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_abilities_0091a810);
    }
    if (required_color != COLOR_TEST_0 && (required_color & PLAYER_CARD_INSTANCE(tgt_player, tgt_card).color) == COLOR_TEST_0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_color_008ce3c0);
    }
    if (illegal_color != COLOR_TEST_0 && (illegal_color & PLAYER_CARD_INSTANCE(tgt_player, tgt_card).color) != COLOR_TEST_0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_color_008ce3c0);
    }

    if (extra != -1)
    {
      if ((special & TARGET_SPECIAL_NOT_LAND_SUBTYPE) != 0)
      {
        if (card_matches_excluded_land_type(tgt_player, tgt_card, extra) != 0)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
        }
      }
      else if ((special & TARGET_SPECIAL_BASIC_LAND) != 0)
      {
        if (basic_land_matches_type_index(PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id, extra) == 0)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
        }
      }
      else
      {
        if (extra <= 4)
        {
          if (card_has_basic_land_type(tgt_player, tgt_card, extra + 1) == 0)
          {
            s.is_illegal = 1;
            strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
          }
        }
        else if (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id != extra && global_cards_data[extra].id != global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
        }
      }
    }

    if (required_subtype != ~SUB_WALL)
    {
      if ((special & TARGET_SPECIAL_0x10) != 0)
      {
        if (required_subtype == (SUB_DJINN | SUB_MERFOLK))
        {
          s.alt_subtype = SUB_RAT;
        }
        else if (required_subtype == SUB_RAT)
        {
          s.alt_subtype = SUB_DJINN | SUB_MERFOLK;
        }
        else if (required_subtype == SUB_ENCHANT_WORLD)
        {
          s.alt_subtype = SUB_ENCHANT_WORLD | SUB_MERFOLK;
        }
        else if (required_subtype == (SUB_ENCHANT_WORLD | SUB_MERFOLK))
        {
          s.alt_subtype = SUB_ENCHANT_WORLD;
        }
        else if (required_subtype == 0x49)
        {
          s.alt_subtype = 0x4a;
        }
        else if (required_subtype == 0x4a)
        {
          s.alt_subtype = 0x49;
        }
        else if (required_subtype == 0x56)
        {
          s.alt_subtype = 0x57;
        }
        else if (required_subtype == 0x57)
        {
          s.alt_subtype = 0x56;
        }
        else if (required_subtype == 0x59)
        {
          s.alt_subtype = 0x5a;
        }
        else if (required_subtype == 0x5a)
        {
          s.alt_subtype = 0x59;
        }
        else if (required_subtype == 0x69)
        {
          s.alt_subtype = 0x6a;
        }
        else if (required_subtype == 0x6a)
        {
          s.alt_subtype = 0x69;
        }
        else
        {
          s.alt_subtype = required_subtype;
        }
      }
      else
      {
        s.alt_subtype = required_subtype;
      }

      if ((subtype_in_card_data_t)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id].subtype != required_subtype && (subtype_in_card_data_t)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id].subtype != s.alt_subtype)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_subtype_008cea20);
      }
    }

    if (power_requirement != -1)
    {
      s.power_pt_val = power_requirement & TARGET_PT_MASK;
      s.power_pt_cmp = power_requirement & 0xf000;
      if ((s.power_pt_cmp == 0 && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power != (int)s.power_pt_val) || (s.power_pt_cmp == TARGET_PT_GREATER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power < (int)s.power_pt_val) || (s.power_pt_cmp == TARGET_PT_LESSER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power > (int)s.power_pt_val))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_power_00781aa0);
      }
    }

    if (toughness_requirement != -1)
    {
      s.toughness_pt_val = toughness_requirement & TARGET_PT_MASK;
      s.toughness_pt_cmp = toughness_requirement & 0xf000;
      if ((s.toughness_pt_cmp == 0 && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness != (int)s.toughness_pt_val) || (s.toughness_pt_cmp == TARGET_PT_GREATER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness < (int)s.toughness_pt_val) || (s.toughness_pt_cmp == TARGET_PT_LESSER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness > (int)s.toughness_pt_val))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_toughness_007894b0);
      }
    }

    if (special != 0)
    {
      if ((special & TARGET_SPECIAL_WALL) != 0 && (subtype_in_card_data_t)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id].subtype != 0xc5 && global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_walls_008b34c0);
      }
      if ((special & TARGET_SPECIAL_NON_WALL) != 0 && ((subtype_in_card_data_t)global_raw_cards_storage[global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id].subtype == 0xc5 || global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype == 0))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_walls_008b34c0);
      }
      if ((special & TARGET_SPECIAL_SPELL_ON_STACK) != 0 && ((g_current_spell_player == -1 || tgt_player != g_current_spell_player) || tgt_card != g_current_spell_card || (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) != 0))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_spell_0091be40);
      }
      if ((special & TARGET_SPECIAL_BASIC_LAND) != 0 && is_basic_land(tgt_player, tgt_card) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_basic_land_0093d860);
      }
      if ((special & TARGET_SPECIAL_ARTIFACT_CREATURE) != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].type & (TYPE_ARTIFACT | TYPE_CREATURE)) != (TYPE_ARTIFACT | TYPE_CREATURE))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_artifact_creature_008ced40);
      }
      if ((special & TARGET_SPECIAL_DAMAGE_PLAYER) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_player != who_chooses || PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_card != -1))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_target_player_0091c360);
      }
      if ((special & TARGET_SPECIAL_DJINN_OR_EFREET) != 0 && global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype != 6 && global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype != 5)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, ",djinn/efreet");
      }
    }

    if (required_state != 0)
    {
      if ((required_state & TARGET_STATE_TAPPED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_TAPPED) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_tapped_00925080);
      }
      if ((required_state & TARGET_STATE_ATTACKING) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKING) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_attacking_00896540);
      }
      if ((required_state & TARGET_STATE_ATTACKED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKED) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_attacked_0091bfc0);
      }
      if ((required_state & TARGET_STATE_ISBLOCKED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ISBLOCKED) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_blocked_0091caa0);
      }
      if ((required_state & TARGET_STATE_BLOCKING) != 0 && (tgt_player == g_current_player || (int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking == -1))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_blocking_007ab160);
      }
      if ((required_state & TARGET_STATE_IN_COMBAT) != 0)
      {
        s.is_not_blocking = 0;
        s.is_not_attacking = s.is_not_blocking;

        if ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKING) == 0)
        {
          s.is_not_attacking = 1;
        }

        if (g_current_phase > 0x14 && g_current_phase < 0x1e && tgt_player != g_current_player)
        {
          if (tgt_player == g_current_player || (int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking == -1)
          {
            s.is_not_blocking = 1;
          }
        }
        else
        {
          s.is_not_blocking = 1;
        }

        if (s.is_not_attacking != 0)
        {
          if (s.is_not_blocking != 0)
          {
            s.is_illegal = 1;
            strcat(s.errbuf, gs_illegal_target_why_attacking_blocking_008cc850);
          }
        }
      }
      if ((required_state & TARGET_STATE_ENCHANTED) != 0)
      {
        s.tmp118 = 0;
        for (s.unused_110 = 0; s.unused_110 < 2; ++s.unused_110)
        {
          for (s.tmp114 = 0; s.tmp114 < g_active_cards_count[s.unused_110]; ++s.tmp114)
          {
            if (global_card_instances[s.unused_110][s.tmp114].internal_card_id != -1 &&
                (global_cards_data[global_card_instances[s.unused_110][s.tmp114].internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
                global_card_instances[s.unused_110][s.tmp114].damage_target_player == tgt_player &&
                global_card_instances[s.unused_110][s.tmp114].damage_target_card == tgt_card)
            {
              s.tmp118 = 1;
            }
          }
        }
        if (!s.tmp118)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_enchanted_008cf3c0);
        }
      }
      if ((required_state & TARGET_STATE_JUST_CAST) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_casted_00939050);
      }
      if ((required_state & TARGET_STATE_SPELL_RESOLVED) != 0 && ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) == 0 || (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_INVISIBLE) != 0))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_cast_resolved_008ce6a0);
      }
      if ((required_state & TARGET_STATE_DAMAGED) != 0 && has_pending_damage(tgt_player, tgt_card) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_damaged_007911b0);
      }
      if ((required_state & TARGET_STATE_COULD_UNTAP) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).untap_status & UNTAP_STATUS_COULD_UNTAP) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_can_untap_008b3be0);
      }
      if ((required_state & TARGET_STATE_WILL_UNTAP) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).untap_status & UNTAP_STATUS_WILL_UNTAP) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_will_untap_008ceb50);
      }
      if ((required_state & TARGET_STATE_SUMMONING_SICK) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & 0x30000) == 0x30000)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, ",summoning sickness");
      }
    }

    if (illegal_state != 0)
    {
      if ((illegal_state & TARGET_STATE_TAPPED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_TAPPED) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_tapped_00925080);
      }
      if ((((illegal_state & TARGET_STATE_ATTACKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0)) && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKING) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_attacking_00896540);
      }
      if ((illegal_state & TARGET_STATE_ATTACKED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKED) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_attacked_0091bfc0);
      }
      if ((illegal_state & TARGET_STATE_ISBLOCKED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ISBLOCKED) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_blocked_0091caa0);
      }
      if (((illegal_state & TARGET_STATE_BLOCKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0))
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking != -1 && tgt_player != g_current_player)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_blocking_007ab160);
        }
      }
      if ((illegal_state & TARGET_STATE_ENCHANTED) != 0)
      {
        s.tmp124 = 0;
        for (s.unused_11c = 0; s.unused_11c < 2; ++s.unused_11c)
        {
          for (s.tmp120 = 0; s.tmp120 < g_active_cards_count[s.unused_11c]; ++s.tmp120)
          {
            if (global_card_instances[s.unused_11c][s.tmp120].internal_card_id != -1 &&
                (global_cards_data[global_card_instances[s.unused_11c][s.tmp120].internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
                global_card_instances[s.unused_11c][s.tmp120].damage_target_player == tgt_player &&
                global_card_instances[s.unused_11c][s.tmp120].damage_target_card == tgt_card)
            {
              s.tmp124 = 1;
            }
          }
        }
        if (s.tmp124)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_enchanted_008cf3c0);
        }
      }
      if ((illegal_state & TARGET_STATE_JUST_CAST) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_casted_00939050);
      }
      if ((illegal_state & TARGET_STATE_SPELL_RESOLVED) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_INVISIBLE) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_cast_resolved_008ce6a0);
      }
      if ((illegal_state & TARGET_STATE_DAMAGED) != 0 && has_pending_damage(tgt_player, tgt_card) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_damaged_007911b0);
      }
      if ((illegal_state & TARGET_STATE_COULD_UNTAP) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).untap_status & UNTAP_STATUS_COULD_UNTAP) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_can_untap_008b3be0);
      }
      if ((illegal_state & TARGET_STATE_WILL_UNTAP) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).untap_status & UNTAP_STATUS_WILL_UNTAP) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_will_untap_008ceb50);
      }
      if ((illegal_state & TARGET_STATE_SUMMONING_SICK) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & 0x30000) != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, ",summoning sickness");
      }
    }
  }

  if (s.is_illegal != 0)
  {
    s.is_valid = 0;
  }
  else
  {
    s.is_valid = 1;
  }
  if (return_error_str != NULL)
  {
    if (s.errbuf[0] == '\0')
    {
      *return_error_str = '\0';
    }
    else
    {
      strcpy(return_error_str, s.errbuf + 1);
    }
  }

  return s.is_valid;
}

// FUNCTION: MAGIC 0x004e9c50
// FUNCTION: SHANDALAR 0x00507a50
int select_card_for_action(int player,
                           int arg_2,
                           int player_to_check,
                           unsigned int required_type,
                           unsigned int required_color,
                           char *prompt,
                           int dialog_mode)
{
  struct
  {
    unsigned int modal_required_type; /* ebp - 0x20c */
    int stack_card;                   /* ebp - 0x208 */
    int action_msg;                   /* ebp - 0x204 */
    int stack_player;                 /* ebp - 0x200 */
    int candidate_cards[60];          /* ebp - 0x1fc */
    int candidate_count;              /* ebp - 0x10c (reused as random candidate index) */
    target_t selected_target;         /* ebp - 0x108 */
    int action;                       /* ebp - 0x100 (get_current_stack_action() result) */
    int current_card;                 /* ebp - 0xfc */
    int current_player;               /* ebp - 0xf8 */
    int selected_internal_id;         /* ebp - 0xf4 */
    int candidate_players[60];        /* ebp - 0xf0 */
  } s;

  if (g_spell_fizzled == 1 || (g_duel_ai_mode_state == 1 && g_active_player == player))
  {
    return -1;
  }

  if ((g_ai_search_flags & 1) != 0)
  {
    player_to_check = arg_2;
  }

  if ((player == g_other_player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
  {
    s.candidate_count = 0;

    for (s.current_player = 0; s.current_player < 2; s.current_player++)
    {
      if (player_to_check != -1 && player_to_check != s.current_player)
      {
        continue;
      }

      for (s.current_card = 0; s.current_card < g_active_cards_count[s.current_player]; s.current_card++)
      {
        if (required_type == (unsigned int)-2 || PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id == -1)
        {
          continue;
        }

        if (((PLAYER_CARD_INSTANCE(s.current_player, s.current_card).state & 0x800002) != 2) &&
            !(g_active_player == player && g_duel_network_state != 0))
        {
          continue;
        }

        if (!(((int)required_type <= 0) ||
              ((required_type &
                (unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type) != 0)))
        {
          continue;
        }

        if (!((required_color == 1 || required_color == 0) ||
              (((int)required_color & (int)PLAYER_CARD_INSTANCE(s.current_player, s.current_card).mana_color) != 0)))
        {
          continue;
        }

        s.candidate_players[s.candidate_count] = s.current_player;
        s.candidate_cards[s.candidate_count] = s.current_card;
        s.candidate_count++;
      }

      if (((int)required_type <= 0) && (required_color == 1 || required_color == 0))
      {
        s.candidate_players[s.candidate_count] = s.current_player;
        s.candidate_cards[s.candidate_count] = -1;
        s.candidate_count++;
      }
    }

    if (s.candidate_count == 0)
    {
      return -1;
    }

    if (g_active_player == player)
    {
    select_card_for_action_retry:
      s.candidate_count = internal_rand(s.candidate_count);
      g_target_player_choice = s.candidate_players[s.candidate_count];

      if (g_duel_network_state != 0)
      {
        g_target_selection_status_code = 0;
        if (internal_rand(0x20) == 0 || g_response_selection_in_progress != 0)
        {
          g_target_selection_status_code = (int)0xfffffffeU;
          g_stop_phase = -1;
          g_stop_phase_player = g_stop_phase;
          return -1;
        }

        if (g_active_player != g_target_player_choice)
        {
          goto select_card_for_action_retry;
        }

        s.selected_internal_id = PLAYER_CARD_INSTANCE(g_target_player_choice, s.candidate_cards[s.candidate_count]).internal_card_id;
        if (((global_cards_data[s.selected_internal_id].type & 1) != 0) &&
            ((PLAYER_CARD_INSTANCE(g_target_player_choice, s.candidate_cards[s.candidate_count]).state & 2) != 0))
        {
          goto select_card_for_action_retry;
        }
        if (((global_cards_data[s.selected_internal_id].type & 2) != 0) &&
            ((PLAYER_CARD_INSTANCE(g_target_player_choice, s.candidate_cards[s.candidate_count]).state & 4) != 0))
        {
          goto select_card_for_action_retry;
        }

        if (g_current_phase > 0x14 && g_current_phase < 0x1e)
        {
          if ((global_cards_data[s.selected_internal_id].type & 2) == 0 || (PLAYER_CARD_INSTANCE(g_target_player_choice, s.candidate_cards[s.candidate_count]).state & 2) == 0)
          {
            goto select_card_for_action_retry;
          }
          goto select_card_for_action_return_candidate;
        }

        if ((global_cards_data[s.selected_internal_id].type & 0x4b) == 0 || (PLAYER_CARD_INSTANCE(g_target_player_choice, s.candidate_cards[s.candidate_count]).state & 2) != 0)
        {
          goto select_card_for_action_retry;
        }
      }

    select_card_for_action_return_candidate:
      return s.candidate_cards[s.candidate_count];
    }

    if (g_duel_ai_mode_state == 1)
    {
      g_ai_recorded_choice = internal_rand(s.candidate_count);
      g_ai_recorded_action = ((((s.candidate_players[g_ai_recorded_choice] == 0) - 1) & 0x100) |
                              (s.candidate_cards[g_ai_recorded_choice] & 0xff) |
                              0x4000);
      record_ai_action_selection();
    }
    else
    {
      replay_ai_action_selection();
      if (g_ai_recorded_choice == 99)
      {
        g_ai_recorded_choice = internal_rand(s.candidate_count);
      }
    }

    g_target_player_choice = s.candidate_players[g_ai_recorded_choice];
    return s.candidate_cards[g_ai_recorded_choice];
  }

  if (required_type != 0 && required_type != 0xff)
  {
    strcpy(g_ui_message_buffer, "");
    s.action = get_current_stack_action();
    if (s.action != -1)
    {
      s.action_msg = ((unsigned int)s.action >> 0x10) & 0xff;
      s.stack_player = global_stack_cards[g_stack_size - 1].player;
      s.stack_card = global_stack_cards[g_stack_size - 1].card;
      format_stack_action_text(g_ui_message_buffer,
                               s.action_msg,
                               s.stack_player,
                               s.stack_card);
    }
  }

  if (required_type == 0 || required_type == 0xff || required_type == (unsigned int)-2)
  {
    s.modal_required_type = 0xffffffffU;
  }
  else
  {
    s.modal_required_type = required_type;
  }

  run_target_selection_modal(player,
                             arg_2,
                             prompt,
                             dialog_mode,
                             s.modal_required_type,
                             required_color,
                             0xffffffff,
                             0xffffffff,
                             &g_target_selection_status_code,
                             &s.selected_target,
                             0,
                             0);

  strcpy(g_text_lines[0], "");
  if (s.selected_target.player != -1)
  {
    g_ai_action_replay_available = 0;
  }

  g_target_player_choice = s.selected_target.player;
  return s.selected_target.card;
}

// FUNCTION: MAGIC 0x00551638
// FUNCTION: SHANDALAR 0x0049f87b
int select_target_creature_and_store(int player, unsigned int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  if (C_real_select_target(player,
                           2,
                           preferred_controller,
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
                           0,
                           0,
                           g_text_lines[0],
                           1,
                           &selected_target) != 0)
  {
    global_card_instances[player][card].targets[(char)global_card_instances[player][card].number_of_targets].card = selected_target.card;
    global_card_instances[player][card].targets[(char)global_card_instances[player][card].number_of_targets].player = selected_target.player;
    ++global_card_instances[player][card].number_of_targets;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x005517b4
// FUNCTION: SHANDALAR 0x0049f9f7
int select_target_creature_and_store_without_protection(int player, unsigned int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  if (C_real_select_target(player,
                           2,
                           preferred_controller,
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
                           0,
                           g_text_lines[0],
                           1,
                           &selected_target) != 0)
  {
    SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[(char)PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
    ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00551921
// FUNCTION: SHANDALAR 0x0049fb64
int choose_creature_to_sacrifice(int player)
{
  struct
  {
    int best_card;
    target_t target;
    int current_card;
    int best_value;
    int internal_card_id;
    int card_value;
  } s;

  if ((player == g_active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
  {
    if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                             g_text_lines[0], 0, &s.target))
    {
      s.best_card = s.target.card;
    }
    else
    {
      s.best_card = -1;
    }
  }
  else
  {
    s.best_card = -1;
    s.best_value = 0x7fff;
    for (s.current_card = 0; s.current_card < g_active_cards_count[player]; ++s.current_card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id;
      if (s.internal_card_id != -1 &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x800002) == 2 &&
          (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
          PLAYER_CARD_INSTANCE(player, s.current_card).kill_code != 3 &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & STATE_CANNOT_TARGET) == 0)
      {
        s.card_value = C_get_abilities(player, s.current_card, EVENT_POWER, -1) + 2;
        s.card_value *= C_get_abilities(player, s.current_card, EVENT_TOUGHNESS, -1) + 2;
        if (s.card_value < s.best_value)
        {
          s.best_value = s.card_value;
          s.best_card = s.current_card;
        }
      }
    }
  }

  if (s.best_card != -1 && g_duel_ai_mode_state != 1)
  {
    play_sound_effect(WAV_SACRFICE);
  }
  return s.best_card;
}

// FUNCTION: MAGIC 0x00551b60
// FUNCTION: SHANDALAR 0x0049fda3
int select_target_land_and_store(int player, int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  if (C_real_select_target(player,
                           2,
                           preferred_controller,
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
                           g_text_lines[0],
                           1,
                           &selected_target) != 0)
  {
    SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets], selected_target);
    ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
    return 1;
  }

  return 0;
}

/* target_source_mode:
 *   0 = scan direct player/card targets
 *   1 = scan damage cards by damage_target_player/card
 *   2 = scan damage cards by damage_source_player/card
 */
// FUNCTION: MAGIC 0x00551e49
// FUNCTION: SHANDALAR 0x004a008c
int sacrifice_a_land(int player)
{
  target_t target;

  if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0,
                           COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                           g_text_lines[0], 0, &target))
  {

    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
    kill_card(target.player, target.card, KILL_SACRIFICE);

    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00551ed7
// FUNCTION: SHANDALAR 0x004a011a
int select_target_artifact_and_store(int player, unsigned int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  if (C_real_select_target(player,
                           2,
                           preferred_controller,
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
                           g_text_lines[0],
                           1,
                           &selected_target))
  {
    global_card_instances[player][card].targets[global_card_instances[player][card].number_of_targets].player = selected_target.player;
    global_card_instances[player][card].targets[global_card_instances[player][card].number_of_targets].card = selected_target.card;
    ++global_card_instances[player][card].number_of_targets;

    return 1;
  }
  else
    return 0;
}
