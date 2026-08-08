#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "game_support.h"
#include "global_strings.h"

typedef ptrdiff_t INT_PTR;

typedef int(__cdecl *in_play_card_callback_t)(int source_player,
                                              int source_card,
                                              int player,
                                              int card,
                                              int internal_card_id);

extern card_ptr_t global_raw_cards_storage[2000];
extern int ai_search_flags;
extern const char *g_duel_sound_filenames[];
extern char global_duelsounds_path[0x110];

int GetNextManaSymbol(char **text);
int choose_a_color_dialog(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors);

int begin_mana_payment_record(void);
int end_mana_payment_record(void);
int restore_recorded_mana_to_pool(int player);
int refresh_stack_proxy_card(int prompt_flag);
int set_stack_damage_targets(void);
int count_colored_cards_in_hand(int player);

int process_response_actions(int reason_for_trigger_controller, const char *prompt);
void ReadCsvFieldByCsvid(char *out, int csvid, int field, const char *csv_name);
void legacy_show_card_preview_stub(unsigned int internal_card_id, int unk1, char *prompt, int unk2);
int update_duel_selection_display(int player, int phase);
void display_duel_prompt_text(char *text);
int GetCardRarity(int internal_card_id);
int process_killed_card(int player, int card);
char *get_displayed_card_name(int player, int card);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int allow_response(int response_player, int phase, char *prompt, int event_code);

void position_duel_prompt_context_window(HWND prompt_hwnd);
void setup_duel_prompt_context_text_dc(HWND prompt_hwnd, HDC dc, int *rect_values);
void restack_duel_child_windows(void);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
void initialize_card_instance(int player, int internal_card_id, int card);
void show_duel_system_error(const char *err_msg);
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask);
int can_block_attacker_with_abilities(int blocker_player,
                                      int blocker_card,
                                      int attacker_player,
                                      int attacker_card,
                                      unsigned int attacker_abilities,
                                      unsigned int evasion_mask);
void *copy_bytes(void *dest, const void *src, unsigned int count);

// GLOBAL: MAGIC 0x00573080
const unsigned char wizard_card_color_preference_table[18] = {
    0x00,
    0x00,
    0x00,
    0x01,
    0x04,
    0x02,
    0x02,
    0x01,
    0x05,
    0x03,
    0x05,
    0x04,
    0x04,
    0x03,
    0x01,
    0x05,
    0x02,
    0x05,
};

int can_target(target_definition_t *td)
{
  return td != NULL;
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
    s.trace_counter = duel_trace_counter;
    ++duel_trace_counter;
    sprintf(s.trace_text, "%d: Player #%d is casting %s(%d).\n", s.trace_counter, player, global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name, card);
    append_to_trace_txt(s.trace_text);
  }

  if (!can_pay_card_mana_cost(player, player, card))
  {
    return 0;
  }

  ai_recorded_action = -1;
  unk_0091bfb4 = 0;
  max_x_value = -1;

  if ((((unsigned char)s.card_data->type & 0x3c) != 0 ||
       (int)s.card_data->id == 0x107 ||
       (int)s.card_data->id == 0x27) &&
      dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) == 0)
  {
    return 0;
  }

  spell_fizzled = -1;

  s.saved_current_casting_internal_card_id = current_casting_internal_card_id;
  s.saved_unk_008ce508 = current_spell_player;
  s.saved_unk_008ce4f4 = current_spell_card;

  current_casting_internal_card_id = s.internal_card_id;
  current_spell_player = player;
  current_spell_card = card;

  push_card_onto_stack(player, card, 0x71, player, 0);
  begin_mana_payment_record();

  if (unk_0091bfb4 != 0)
  {
  }
  else
  {
    if ((player == active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
    {
      if ((s.card_data->type & 0x40) != 0)
      {
        mana_charge[6] = (int)(char)s.card_data->cc[0];
        mana_charge[6] += (int)(char)s.card_data->cc[1];
      }
      else
      {
        mana_charge[s.color_index] = (int)(char)s.card_data->cc[0];
        mana_charge[0] += (int)(char)s.card_data->cc[1];
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
        mana_charge[s.color_index] = (int)(char)s.card_data->cc[0];
      }
      if (0 < (char)s.card_data->cc[1])
      {
        mana_charge[0] = (int)(char)s.card_data->cc[1];
      }

      charge_mana_w_global_cost_mod(player, card, 0, 0);

      if ((char)s.card_data->cc[1] == -1)
      {
        if (player != active_player)
        {
          if (g_duel_ai_mode_state == 1)
          {
            switch (internal_rand(2))
            {
            case 0:
              s.tmp_mana = has_mana(player, COLOR_ANY, 1);
              if (s.tmp_mana != 0)
              {
                ai_recorded_choice = s.chosen_x_value = internal_rand(s.tmp_mana) + 1;
              }
              else
              {
                ai_recorded_choice = s.chosen_x_value = internal_rand(has_mana(player, COLOR_ANY, 1) + 1);
              }
              break;

            case 1:
              ai_recorded_choice = s.chosen_x_value = has_mana(player, COLOR_ANY, 1);
              if (life[0] < ai_recorded_choice && internal_rand(3) == 0)
              {
                ai_recorded_choice = s.chosen_x_value = life[0];
              }
              if (max_x_value != -1 && max_x_value < ai_recorded_choice)
              {
                ai_recorded_choice = s.chosen_x_value = max_x_value;
              }
              break;
            }

            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            s.chosen_x_value = ai_recorded_choice;
          }

          s.saved_max_x_value = max_x_value;
          max_x_value = s.chosen_x_value;
          charge_mana(player, 0, -1);
          max_x_value = s.saved_max_x_value;
        }
        else
        {
          s.saved_max_x_value = max_x_value;
          max_x_value = has_mana(player, COLOR_ANY, 1);
          charge_mana(player, 0, -1);
          max_x_value = s.saved_max_x_value;
        }

        if (x_value == 0)
        {
          ai_modifier -= 100;
        }
      }
    }
  }

  max_x_value = -1;

  if (s.internal_card_id != -1)
  {
    current_casting_player = player;
    unk_008b2880 = 1;
    --hand_count[player];

    if ((s.card_data->type & 2) != 0)
    {
      ++creature_cards_in_play[player];
    }
    if ((s.card_data->type & 0x40) != 0)
    {
      ++artifact_cards_in_play[player];
    }
    if ((s.card_data->type & 4) != 0)
    {
      ++enchantments_in_play[player];
    }

    card_types_in_play[player] |= s.card_data->type;
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_INVISIBLE;
    land_can_be_played |= 0x20;

    if (player == active_player || (g_duel_network_flags & 2) != 0 || g_duel_ai_mode_state != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_BOTH;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_NOATTACK;
    }

    current_casting_internal_card_id = s.saved_current_casting_internal_card_id;
    current_spell_player = s.saved_unk_008ce508;
    current_spell_card = s.saved_unk_008ce4f4;

    if (spell_fizzled == 1)
    {
      end_mana_payment_record();
      goto finish_put_card_on_stack;
    }

    if (dispatch_event(player, card, 0x6c) != 0)
    {
      spell_fizzled = 1;
    }
    if (spell_fizzled == 1)
    {
      restore_recorded_mana_to_pool(player);
    }
    end_mana_payment_record();

    if (spell_fizzled == 1)
    {
      goto finish_put_card_on_stack;
    }

    if (player == other_player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state == 1 && s.card_data->cc[1] == -1)
    {
      ai_modifier -= count_colored_cards_in_hand(player) * 12;
    }
    return 1;

  respond_to_card_on_stack:
    s.saved_current_casting_internal_card_id = current_casting_internal_card_id;
    s.saved_unk_008ce508 = current_spell_player;
    s.saved_unk_008ce4f4 = current_spell_card;

    current_casting_internal_card_id = s.internal_card_id;
    current_spell_player = player;
    current_spell_card = card;

    if ((s.card_data->type & 1) == 0)
    {
      refresh_stack_proxy_card(1);
    }

    if ((s.card_data->type != 1) &&
        ((s.card_data->type != 0x20) || ((s.card_data->extra_ability & EA_MANA_SOURCE) == 0)))
    {
      if (DAT_007a7d78 != 0)
      {
        DAT_007a7d78 |= 2;
      }

      sprintf(g_ui_message_buffer, (const char *)gs_trying_to_cast_007a79c0, get_displayed_card_name(current_spell_player, current_spell_card));
      allow_response(-2, current_phase, g_ui_message_buffer, 0xd3);
      if ((DAT_007a7d78 & 2) != 0)
      {
        request_duel_display_refresh_if_human(player, card, 2, 1);
        Sleep(3000);
      }
    }

    current_casting_internal_card_id = s.saved_current_casting_internal_card_id;
    current_spell_player = s.saved_unk_008ce508;
    current_spell_card = s.saved_unk_008ce4f4;

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
    {
      spell_fizzled = 1;
    }

    land_can_be_played &= ~0x20;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state |= (((unsigned int)player < 1 ? 0 : 0x400000) | 0x80);

    if (g_duel_ai_mode_state != 1)
    {
      set_stack_damage_targets();
    }

    if (spell_fizzled == 1)
    {
      goto finish_put_card_on_stack;
    }

    if (player == other_player && g_duel_ai_mode_state != 1 && DAT_007abc80 == 0 && (((unsigned char)s.card_data->type & 0x7e) != 0))
    {
      LoadTextSectionLines(global_ui_strings_filename, "PROMPT_CAST1");
      sprintf(s.prompt_text, text_lines[0], DAT_007a7c60);
      if ((char)s.card_data->cc[1] == -1)
      {
        sprintf(s.prompt_text, text_lines[1], DAT_007a7c60, x_value);
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
        raw_do_dialog(player, card, (ai_recorded_action >> 8), (ai_recorded_action & 0xff), s.prompt_text, 0);
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
  if (spell_fizzled == 1)
  {
    ++hand_count[player];

    if ((s.card_data->type & 2) != 0)
    {
      --creature_cards_in_play[player];
    }
    if ((s.card_data->type & 0x40) != 0)
    {
      --artifact_cards_in_play[player];
    }
    if ((s.card_data->type & 4) != 0)
    {
      --enchantments_in_play[player];
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xffffff5d;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;

    if (player == other_player && (g_duel_network_flags & 2) == 0)
    {
      DAT_0093d850 = 1;
    }
    spell_fizzled = 0;
    obliterate_top_card_of_stack();
    land_can_be_played &= ~0x20;
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

  saved_trigger_cause_controller = trigger_cause_controller;
  saved_trigger_cause = trigger_cause;

  trigger_cause_controller = player;
  trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_SPELL_CAST, gs_casting_00896400, 0);
  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;

  if (global_cards_data[internal_card_id].type != 0x01 &&
      (global_cards_data[internal_card_id].type != 0x20 || (global_cards_data[internal_card_id].extra_ability & 0x1000) == 0))
  {
    sprintf(g_ui_message_buffer, gs_cast_008b4720, get_displayed_card_name(player, card));
    allow_response(-2, current_phase, g_ui_message_buffer, 0x6c);
  }

  PLAYER_CARD_INSTANCE(player, card).state &= ~0x20;
  PLAYER_CARD_INSTANCE(player, card).state |= 2;
  land_can_be_played &= ~0x20;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != internal_card_id)
  {
    obliterate_top_card_of_stack();
    return 0;
  }

  card_types_in_play[player] |= (unsigned char)global_cards_data[internal_card_id].type;

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
  battlefield_extra_ability_flags &= ~1;
  pending_killed_card_handler = process_killed_card;
  regenerate_or_graveyard_triggers();
  process_damage_prevention(player);

  current_casting_player = -1;
  current_casting_internal_card_id = current_casting_player;

  if ((global_cards_data[internal_card_id].type & 1) != 0)
  {
    ++unk_008cfdb0;
  }

  if (spell_fizzled == 1)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PROMPT_FIZZLE");
      set_duel_prompt_text(g_ui_message_buffer);
      Sleep(2000);
      set_duel_prompt_text("");
    }
    DAT_0093d850 = 1;
    spell_fizzled = 0;
    return 0;
  }

  spell_fizzled = 0;
  TENTATIVE_reassess_all_cards(0, 0xff);
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

  ++nested_trigger_depth;

  s.saved_current_phase = current_phase;
  s.saved_DAT_007ab2bc = DAT_007ab2bc;
  s.saved_current_turn = current_turn;
  s.saved_DAT_007aadec = current_action_event_code;

  current_action_event_code = trig;
  current_turn = player;

  s.saved_DAT_00789714 = DAT_00789714;
  s.saved_trig = trig;

  do
  {
    if ((g_duel_network_flags & 2) != 0)
    {
      DAT_00789714 = 1;
    }
    else if (player != 0)
    {
      DAT_00789714 = 2;
    }
    else
    {
      DAT_00789714 = 1;
    }

    trigger_condition = (trigger_t)s.saved_trig;

    s.saved_ui = _DAT_00743020;
    if (TENTATIVE_allow_response != 0)
    {
      _DAT_00743020 = 0x30;
    }
    else
    {
      _DAT_00743020 = 0;
    }

    spell_fizzled = 0;
    DAT_00791410 = spell_fizzled;
    DAT_007ab2bc = DAT_00791410;

    s.result = process_response_actions(player, prompt);

    _DAT_00743020 = s.saved_ui;
    DAT_00742f68 = _DAT_00743020 & 0x30;
    DAT_00789714 = s.saved_DAT_00789714;

    {
      int mask;
      mask = (s.result == 1);
      mask -= 1;
      mask &= ~1;
      mask += 6;

      if ((DAT_007ab2bc & mask) != 0)
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

  trigger_condition = (trigger_t)-1;
  --nested_trigger_depth;

  if (nested_trigger_depth == 0)
  {
    for (player = 0; player < 2; ++player)
    {
      for (s.loop_card = 0; s.loop_card < active_cards_count[player]; ++s.loop_card)
      {
        PLAYER_CARD_INSTANCE(player, s.loop_card).state &= ~0x100;
      }
    }

    if (trigger_dispatch_depth == 0)
    {
      DAT_00742f64 = 0;
      if (g_duel_ai_mode_state != 1)
      {
        phase_stop_suppressed = 0;
      }
    }
  }

  DAT_007ab2bc = s.saved_DAT_007ab2bc;
  current_turn = s.saved_current_turn;
  current_action_event_code = s.saved_DAT_007aadec;
  current_phase = s.saved_current_phase;

  return 0;
}

// FUNCTION: MAGIC 0x00446da7
void show_duel_card_preview(unsigned int internal_card_id, int player, int card, int unk1, char *prompt, int unk2)
{
  (void)unk1;
  (void)prompt;
  (void)unk2;

  // The original stores these params to locals even if it won't use them.
  {
    int preview_player;
    int preview_card;

    preview_player = player;
    preview_card = card;

    if (IsWindowVisible((HWND)g_duel_card_preview_window_hwnd))
    {
      if (player != -1 && card != -1)
      {
        SendMessageA((HWND)g_duel_card_preview_window_hwnd, 0x401, CardIDFromType(internal_card_id), (LPARAM)&preview_player);
      }
      else
      {
        SendMessageA((HWND)g_duel_card_preview_window_hwnd, 0x401, CardIDFromType(internal_card_id), 0);
      }
    }
  }
}

// FUNCTION: MAGIC 0x0046489e
// FUNCTION: SHANDALAR 0x0052234e
int DrawRandomCardFromInitialLibrary(int library_index)
{
  struct
  {
    int rand_pick;    // ebp-0x14
    int chosen_csvid; // ebp-0x10
    int total_cards;  // ebp-0xc
    int running_pick; // ebp-0x8
    int i;            // ebp-0x4
  } s;

  if (library_index == -1)
  {
    return -1;
  }

  s.total_cards = 0;
  for (s.i = 0; s.i < 200; ++s.i)
  {
    s.total_cards += initial_library[library_index][s.i].numcards;
  }

  if (s.total_cards == 0)
  {
    return -1;
  }

  s.rand_pick = internal_rand(s.total_cards);
  s.running_pick = s.rand_pick;

  for (s.i = 0; s.i < 200; ++s.i)
  {
    s.running_pick -= initial_library[library_index][s.i].numcards;
    if (s.running_pick < 0)
    {
      s.chosen_csvid = initial_library[library_index][s.i].csvid;
      if (g_duel_ai_mode_state != 1)
      {
        --initial_library[library_index][s.i].numcards;
      }
      break;
    }
  }

  for (s.i = 0; s.i < g_card_count; ++s.i)
  {
    if ((int)global_cards_data[s.i].id == s.chosen_csvid)
    {
      return s.i;
    }
  }
}

// FUNCTION: MAGIC 0x004a616d
// FUNCTION: SHANDALAR 0x005582ed
int update_duel_selection_display_if_human(int player, int phase)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }
  return update_duel_selection_display(player, phase);
}

// FUNCTION: MAGIC 0x004a6355
void show_card_preview_if_human(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  if (g_duel_ai_mode_state == 1)
  {
    return;
  }

  if (duel_active != 0)
  {
    show_duel_card_preview(internal_card_id, -1, -1, unk1, prompt, unk2);
  }
  else
  {
    legacy_show_card_preview_stub(internal_card_id, unk1, prompt, unk2);
  }
}

// FUNCTION: MAGIC 0x004b58e2
int is_opponent_random_draw_color_allowed(int color, int preferred_color, int color_count)
{
  if (color == 1 || preferred_color == 1)
  {
    return 1;
  }

  color = (int)single_color_test_bit_to_color_t(color);
  preferred_color = (int)single_color_test_bit_to_color_t(preferred_color);

  if ((char)wizard_card_color_preference_table[preferred_color * 3] == color)
  {
    return 1;
  }

  if (color_count > 1 && (char)wizard_card_color_preference_table[preferred_color * 3 + 1] == color)
  {
    return 1;
  }

  if (color_count > 2 && (char)wizard_card_color_preference_table[preferred_color * 3 + 2] == color)
  {
    return 1;
  }

  if (color_count > 3)
  {
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
                           text_lines[0],
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

// FUNCTION: MAGIC 0x00449bef
// FUNCTION: SHANDALAR 0x00453770
void copy_opponent_name_prefix(char *name)
{
  char *source;

  if (name == 0)
  {
    return;
  }

  source = unk_009266d0;
  while (*source != '\0' && *source != '-')
  {
    *name = *source;
    ++source;
    ++name;
  }
  *name = '\0';
}

void default_target_definition(int player, int card, target_definition_t *td, int type)
{
  memset(td, 0, sizeof(*td));
  td->who_chooses = player;
  td->allowed_controller = ANYBODY;
  td->preferred_controller = 1 - player;
  td->zone = TARGET_ZONE_IN_PLAY;
  td->required_type = type;
  td->player = player;
  td->card = card;
}

// FUNCTION: MAGIC 0x004464f0
// FUNCTION: SHANDALAR 0x00411d90
int is_in_play(int player, int card)
{
  if (global_card_instances[player][card].internal_card_id == -1)
    return 0;

  return (global_card_instances[player][card].state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) == STATE_IN_PLAY;
}

// FUNCTION: MAGIC 0x005001e0
// FUNCTION: SHANDALAR 0x004ba309
int get_hacked_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].hack_mode[orig_color] == 0)
  {
    return orig_color;
  }
  else
  {
    return (char)global_card_instances[player][card].hack_mode[orig_color];
  }
}

// FUNCTION: MAGIC 0x0050026d
// FUNCTION: SHANDALAR 0x004ba392
int get_sleighted_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].color_id[orig_color] == 0)
  {
    return orig_color;
  }
  else
  {
    return (char)global_card_instances[player][card].color_id[orig_color];
  }
}

// FUNCTION: MOK 0x004358f0
// FUNCTION: MAGIC 0x00442763
// FUNCTION: SHANDALAR 0x0040dffd
color_t single_color_test_bit_to_color_t(color_test_t color_test)
{
  if ((color_test & COLOR_TEST_BLACK) != 0)
  {
    return COLOR_BLACK;
  }
  if ((color_test & COLOR_TEST_BLUE) != 0)
  {
    return COLOR_BLUE;
  }
  if ((color_test & COLOR_TEST_GREEN) != 0)
  {
    return COLOR_GREEN;
  }
  if ((color_test & COLOR_TEST_RED) != 0)
  {
    return COLOR_RED;
  }
  if ((color_test & COLOR_TEST_WHITE) != 0)
  {
    return COLOR_WHITE;
  }

  return COLOR_COLORLESS;
}

// FUNCTION: MAGIC 0x004832f4
// FUNCTION: SHANDALAR 0x00486e05
int graveyard_has_type(int player, int type)
{
  struct
  {
    int result;
    int graveyard_index;
    int done;
  } s;

  s.result = 0;
  if (player == -1 || player == active_player)
  {
    for (s.graveyard_index = 0, s.done = 0; s.graveyard_index < 500 && s.done == 0; ++s.graveyard_index)
    {
      if (global_graveyard_slots[active_player][s.graveyard_index] != -1)
      {
        if ((type & global_cards_data[global_graveyard_slots[active_player][s.graveyard_index]].type) != 0)
        {
          s.result = 1;
          s.done = s.result;
        }
      }
      else
      {
        s.done = 1;
      }
    }
  }

  if ((player == -1 || player == other_player) && s.result == 0)
  {
    for (s.graveyard_index = 0, s.done = 0; s.graveyard_index < 500 && s.done == 0; ++s.graveyard_index)
    {
      if (global_graveyard_slots[other_player][s.graveyard_index] != -1)
      {
        if ((type & global_cards_data[global_graveyard_slots[other_player][s.graveyard_index]].type) != 0)
        {
          s.result = 1;
          s.done = s.result;
        }
      }
      else
      {
        s.done = 1;
      }
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00484581
// FUNCTION: SHANDALAR 0x0048808b
int is_basic_land_internal_card_id_of_color(int internal_card_id, color_t color)
{
  int result;

  result = 0;
  if ((char)global_cards_data[internal_card_id].subtype == 13 && global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[color])
  {
    result = 1;
  }
  if ((char)global_cards_data[internal_card_id].subtype == 12 && ((1 << color) & (int)*(char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).mana_color) != 0)
  {
    result = 1;
  }

  return result;
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

// FUNCTION: MAGIC 0x004a6245
INT_PTR show_damage_assignment_cardlist(int *internal_card_ids, int *damage_amounts, int count, int title, int allow_cancel, char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(internal_card_ids, damage_amounts, 0, count, (void *)title, (unsigned int)allow_cancel, prompt);
}

// FUNCTION: MAGIC 0x004a628e
// FUNCTION: SHANDALAR 0x0055840e
INT_PTR show_selectable_cardlist(int *graveyard, int *available, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(graveyard, 0, available, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x004b5f03
int find_internal_card_id_by_csv_id(int card_id)
{
  int i;
  int result;

  if (card_id == -1)
  {
    return -1;
  }

  result = -1;
  for (i = 0; i < g_card_count + 0x10; ++i)
  {
    if (global_cards_data[i].id == card_id)
    {
      return i;
    }
  }

  return result;
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

  if (other_player == player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    result = g_network_result_value;
  }

  if (active_player == player && g_duel_ai_mode_state != 1)
  {
    result = (int)show_selectable_cardlist(graveyard,
                                           allowed_cards,
                                           count,
                                           (void *)prompt,
                                           (unsigned int)allow_cancel,
                                           title);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet_type = 0x19;
      g_network_result_value = result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return result;
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
            basiclandtypes_controlled[player][7] - (int)(char)global_cards_data[current_internal_id].cc[0] - 1;
      }
      else
      {
        mana_score = (int)(char)global_cards_data[current_internal_id].cc[1];
      }
      mana_score = (int)(char)global_cards_data[current_internal_id].cc[0] + mana_score;

      if (mana_score <= basiclandtypes_controlled[player][7])
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
        bonus = 8 - basiclandtypes_controlled[player][7];
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

      if ((global_cards_data[current_internal_id].type & TYPE_CREATURE) != 0)
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

// FUNCTION: MAGIC 0x00481e25
int card_two_headed_giant_of_foriys_legacy(int player, int card, int event)
{
  if (event == 0x78 && card == affected_card && player == affected_card_controller)
  {
    if ((int)(char)PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                        PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .blocking == attacking_card ||
        (int)(char)PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                        PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .blocking ==
            (int)(char)PLAYER_CARD_INSTANCE(attacking_card_controller, attacking_card).blocking)
    {
      event_result = 1;
    }
  }

  dispatch_function_to_all_cards_in_play(player, card, reattach_if_attached_to_source, -1);

  if (trigger_condition == 0xdf && card == affected_card && player == affected_card_controller && current_turn == player && player != current_player && player == trigger_cause_controller && card == trigger_cause)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      battlefield_extra_ability_flags |= 4;
      PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .token_status |= 0x8000000;
    }
  }

  if ((trigger_condition == 0xcc || event == 199) && card == affected_card && player == affected_card_controller && current_turn == player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      kill_card(player, card, KILL_BURY);
      battlefield_extra_ability_flags &= 0xfffffffb;

      *(unsigned int *)((char *)&PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                                      PLAYER_CARD_INSTANCE(player, card).damage_source_card) +
                        0x18) &= 0xf7ffffff;
    }
  }

  return 0;
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

    for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
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

    for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
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

// FUNCTION: MAGIC 0x004821f5
int reattach_if_attached_to_source(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  (void)internal_card_id;

  if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == source_player &&
      PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == source_card)
  {
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player =
        PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_player;
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card =
        PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044837e
// FUNCTION: SHANDALAR 0x00451efb
void notify_library_shuffled(int deck_owner)
{
  HWND hwnd;

  if (deck_owner == 0)
  {
    hwnd = DAT_0091ce30;
  }
  else
  {
    hwnd = DAT_0092680c;
  }

  SendMessageA(hwnd, 0x400, 0, 0);
}

// FUNCTION: MAGIC 0x004b5b7a
// FUNCTION: SHANDALAR 0x0056c37c
void send_library_contents(int deck_owner)
{
  int done;
  int index;

  done = 0;
  index = 0;
  while (done == 0)
  {
    if (global_library[deck_owner][index] != -1 && index < 500)
      AddCardToCLPacket(global_library[deck_owner][index]);
    else
      done = 1;
    ++index;
  }

  AddCardToCLPacket(-1);
  TENTATIVE_send_network_result(0, 4);
}

// FUNCTION: MAGIC 0x004b5c19
// FUNCTION: SHANDALAR 0x0056c41b
void receive_library_contents(int deck_owner)
{
  struct
  {
    int index;
    int done;
    int packet_card;
  } s;

  s.done = 0;
  s.index = 0;
  TENTATIVE_wait_for_network_result(1, 4);
  while (s.done == 0)
  {
    s.packet_card = GetCardFromCLPacket(s.index);
    if (s.packet_card == -1)
    {
      s.done = 1;
    }
    else
    {
      global_library[deck_owner][s.index] = s.packet_card;
      ++s.index;
    }
  }
}

// FUNCTION: MAGIC 0x004b59b2
// FUNCTION: SHANDALAR 0x0056c1b5
void shuffle_duel_library(int player, int deck_owner)
{
  struct
  {
    int swap_slot;    /* [ebp-0x14] */
    int temp;         /* [ebp-0x10] */
    int unused;       /* [ebp-0x0c] */
    int current_slot; /* [ebp-0x08] */
    int deck_size;    /* [ebp-0x04] */
  } s;

  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(WAV_SHUFFLE);
    notify_library_shuffled(deck_owner);
  }

  if ((g_duel_network_flags & 2) == 0 || active_player == player)
  {
    s.deck_size = 500;
    for (s.current_slot = 0; s.current_slot < 500; ++s.current_slot)
    {
      if (global_library[deck_owner][s.current_slot] == -1)
      {
        s.deck_size = s.current_slot;
        break;
      }
    }

    for (s.current_slot = 0; s.current_slot < s.deck_size; ++s.current_slot)
    {
      s.swap_slot = s.current_slot + internal_rand(s.deck_size - s.current_slot);
      if (global_library[deck_owner][s.swap_slot] != -1)
      {
        s.temp = global_library[deck_owner][s.swap_slot];
        global_library[deck_owner][s.swap_slot] = global_library[deck_owner][s.current_slot];
        global_library[deck_owner][s.current_slot] = s.temp;
      }
    }

    for (s.current_slot = s.deck_size; s.current_slot < 500; ++s.current_slot)
    {
      global_library[deck_owner][s.current_slot] = -1;
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      send_library_contents(deck_owner);
    }
  }
  else
  {
    receive_library_contents(deck_owner);
  }
}

// FUNCTION: MAGIC 0x004b5c94
// FUNCTION: SHANDALAR 0x0056c496
void remove_card_from_deck(int player, int position)
{
  int current_slot;

  for (current_slot = position; current_slot < 499; ++current_slot)
  {
    global_library[player][current_slot] = global_library[player][current_slot + 1];
  }

  global_library[player][499] = -1;
}

// FUNCTION: MAGIC 0x004b0b53
// FUNCTION: SHANDALAR 0x0046db2e
int regenerate_or_graveyard_triggers(void)
{
  if (pending_killed_card_handler == 0 || regenerate_or_graveyard_triggers_in_progress != 0)
  {
    return 0;
  }

  regenerate_or_graveyard_triggers_in_progress = 1;

  land_can_be_played |= 0x200;
  allow_response(-2, current_phase, gs_use_regeneration_effects_0091c680, 0x70);
  land_can_be_played &= ~0x200;

  dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_GRAVEYARD_ORDER, gs_graveyard_order_0091cbd0, 0);
  dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);

  pending_killed_card_handler = 0;
  regenerate_or_graveyard_triggers_in_progress = 0;

  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: MAGIC 0x005510dc
// FUNCTION: SHANDALAR 0x0049f31f
int has_permanent_of_type(int player, int type_mask)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) && (type_mask & global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b15f7
// FUNCTION: SHANDALAR 0x0046e5d2
void remove_card_from_graveyard(int player, int graveyard_index)
{
  int i = graveyard_index;
  for (; i < 499; ++i)
  {
    global_graveyard_slots[player][i] = global_graveyard_slots[player][i + 1];
  }
  global_graveyard_slots[player][499] = -1;
}

// FUNCTION: MOK 0x0047936d
// FUNCTION: MAGIC 0x004b42aa
int show_deck(int player, int *cards, int count, void *context, int suppress_done_txt, char *prompt)
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
    int valid_graveyards[488];
    int draw_args[6];
    int padding[5];
    int unused_padding_c;
    int i;
    int result;
  } s;

  if (((player == other_player) && ((g_duel_network_flags & 2) != 0)) && duel_active != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    return g_network_result_value;
  }
  else if (((player == other_player) && ((g_duel_network_flags & 2) == 0)) || g_duel_ai_mode_state == 1)
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

    ai_recorded_choice = internal_rand(s.valid_count);
    if (player != active_player)
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

    return s.valid_graveyards[ai_recorded_choice];
  }
  else if (duel_active == 0)
  {
    ReadPalette("todpal.tr", (char *)0);
    legacy_clear_graphics_page_stub(0, 0);
    SelectPalette(unk_0074309c, unk_007463dc, 0);
    legacy_load_pcx_into_page_no_palette_stub("advfac64.pic");
    legacy_load_pcx_into_page_stub(1, 0, 0, "seedeck.pic", &unk_007462a0);
    legacy_blit_graphics_rect_stub(unk_0057a75c, 0, 0, 0x280, 0x1e0, unk_007497b0, 0, 0, global_screen_width, global_screen_height);

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
    s.result = show_cardlist_if_human(cards, count, context, suppress_done_txt, prompt);
    if ((player == active_player) && ((g_duel_network_flags & 2) != 0))
    {
      g_network_result_value = s.result;
      g_network_result_packet_type = 0x19;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return s.result;
}

// FUNCTION: MOK 0x00459AA0
// FUNCTION: MAGIC 0x00485060
// FUNCTION: SHANDALAR 0x00488b70
int dispatch_function_to_all_cards_in_play(int player, int card, in_play_card_callback_t callback, int who_to_check)
{
  int saved_event_result;
  int callback_result;
  int test_player;
  int test_card;
  int result;

  saved_event_result = event_result;
  callback_result = 0;
  event_result = -1;
  for (test_player = player;
       test_player > -1 && test_player < 2 && callback_result == 0;
       test_player += (player == 0) ? 1 : -1)
  {
    if (who_to_check == -1 || test_player == who_to_check)
    {
      for (test_card = 0; test_card < active_cards_count[test_player] && callback_result == 0; ++test_card)
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

  result = event_result;
  event_result = saved_event_result;
  return result;
}

// FUNCTION: MAGIC 0x004964dd
// FUNCTION: SHANDALAR 0x00466f70
int get_card_display_pic_num(card_id_t card_id, int player, int card)
{
  int num_pics;

  if (card_id == -1)
  {
    return 0;
  }
  if (player == -1 || card == -1)
  {
    return 0;
  }
  num_pics = global_raw_cards_storage[card_id].num_pics;
  if (num_pics <= 1)
  {
    return 0;
  }
  return (player + card) % num_pics;
}

// FUNCTION: MAGIC 0x0050047c
// FUNCTION: SHANDALAR 0x004ba5a1
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card)
{
  struct
  {
    int source_internal_card_id;
    int result;
    int damage_player;
  } s;

  if (target_player == -1 || source_player == -1 || amount <= 0)
  {
    return -1;
  }

  if (target_card == -1)
  {
    s.damage_player = target_player;
  }
  else
  {
    s.damage_player = source_player;
  }

  s.result = add_card_to_hand(s.damage_player, damage_card_internal_card_id);
  if (s.result != -1)
  {
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).state |=
        ((-(s.damage_player != 0)) & 0x1000) | 2;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_target_card = target_card;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).info_slot = amount;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_source_player = (char)source_player;
    PLAYER_CARD_INSTANCE(s.damage_player, s.result).damage_source_card = source_card;
    if (source_card != -1)
    {
      if (PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == -1 ||
          PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == stack_proxy_internal_card_id)
      {
        s.source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).original_internal_card_id;
      }
      else
      {
        s.source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id;
      }

      PLAYER_CARD_INSTANCE(s.damage_player, s.result).color = PLAYER_CARD_INSTANCE(source_player, source_card).color;
      if ((global_cards_data[s.source_internal_card_id].type & TYPE_ARTIFACT) != 0)
      {
        PLAYER_CARD_INSTANCE(s.damage_player, s.result).color |= 0x40;
      }

      PLAYER_CARD_INSTANCE(s.damage_player, s.result).eot_toughness =
          (unsigned int)(unsigned char)global_cards_data[s.source_internal_card_id].type;

      if ((PLAYER_CARD_INSTANCE(source_player, source_card).state & 4) != 0)
      {
        if (current_phase == 0x19)
        {
          PLAYER_CARD_INSTANCE(s.damage_player, s.result).token_status |= 0x100000;
        }
        if (current_phase == 0x1a)
        {
          PLAYER_CARD_INSTANCE(s.damage_player, s.result).token_status |= 0x40000;
        }
      }
      if (global_cards_data[s.source_internal_card_id].id == unk_00789734 || global_cards_data[s.source_internal_card_id].id == unk_008a8de8 || global_cards_data[s.source_internal_card_id].id == unk_008cf1ac)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id =
            *(unsigned int *)&PLAYER_CARD_INSTANCE(source_player, source_card).display_pic_csv_id;
      }
      else
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id =
            (get_card_display_pic_num(global_cards_data[s.source_internal_card_id].id, source_player, source_card) << 16) |
            global_cards_data[s.source_internal_card_id].id;
      }
    }
    else
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(s.damage_player, s.result).display_pic_csv_id = 0xef;
    }
    land_can_be_played |= 2;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004b4654
// FUNCTION: SHANDALAR 0x0056ae5d
int add_card_to_hand(int player, int internal_card_id)
{
  int card;

  if (internal_card_id == -1)
  {
    return -1;
  }

  for (card = 0; card < 0x96; ++card)
  {
    if (PLAYER_CARD_INSTANCE(player, card).original_internal_card_id == -1)
    {
      initialize_card_instance(player, internal_card_id, card);
      if (active_cards_count[player] <= card)
      {
        active_cards_count[player] = card + 1;
      }
      return card;
    }
  }

  show_duel_system_error("AddCard error: No more room in cd to add a card");
  return -1;
}

int calculate_attack_rating(int player, int card);

// FUNCTION: MAGIC 0x004b4720
// FUNCTION: SHANDALAR 0x0056af29
void initialize_card_instance(int player, int internal_card_id, int card)
{
  int i;

  PLAYER_CARD_INSTANCE(player, card).original_internal_card_id = internal_card_id;
  PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  PLAYER_CARD_INSTANCE(player, card).dummy3 = 0;

  if (player == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).state = 0;
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).state = 0x1000;
  }

  PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
  PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)-1;
  PLAYER_CARD_INSTANCE(player, card).damage_target_card = -1;
  PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)-1;
  PLAYER_CARD_INSTANCE(player, card).damage_source_card = -1;

  PLAYER_CARD_INSTANCE(player, card).power = global_cards_data[internal_card_id].power;
  PLAYER_CARD_INSTANCE(player, card).toughness = global_cards_data[internal_card_id].toughness;
  PLAYER_CARD_INSTANCE(player, card).counter_power = 0;
  PLAYER_CARD_INSTANCE(player, card).counter_toughness = 0;

  PLAYER_CARD_INSTANCE(player, card).color = (char)global_cards_data[internal_card_id].color;
  PLAYER_CARD_INSTANCE(player, card).mana_color = PLAYER_CARD_INSTANCE(player, card).color;

  PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
  PLAYER_CARD_INSTANCE(player, card).unknown0x37 = 0;
  PLAYER_CARD_INSTANCE(player, card).kill_code = 0;

  PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  PLAYER_CARD_INSTANCE(player, card).timestamp = -1;

  PLAYER_CARD_INSTANCE(player, card).token_status = 0;
  PLAYER_CARD_INSTANCE(player, card).regen_status = 0x08000000;

  PLAYER_CARD_INSTANCE(player, card).attack_rating = calculate_attack_rating(player, card);

  PLAYER_CARD_INSTANCE(player, card).special_counters = 0;
  PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
  PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
  PLAYER_CARD_INSTANCE(player, card).untap_status = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_flags = PLAYER_CARD_INSTANCE(player, card).untap_status;

  for (i = 0; i < 7; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).mana_to_untap[i] = 0;
  }

  PLAYER_CARD_INSTANCE(player, card).upkeep_colorless = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_black = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_blue = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_green = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_red = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_white = 0;
  PLAYER_CARD_INSTANCE(player, card).upkeep_artmana = 0;

  for (i = 0; i < 6; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).color_id[i] = 0;
    PLAYER_CARD_INSTANCE(player, card).hack_mode[i] = 0;
  }

  for (i = 0; i < 19; ++i)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[i].player = (char)-1;
    PLAYER_CARD_INSTANCE(player, card).targets[i].card = -1;
  }

  PLAYER_CARD_INSTANCE(player, card).parent_controller = -1;
  PLAYER_CARD_INSTANCE(player, card).parent_card = -1;

  PLAYER_CARD_INSTANCE(player, card).counters = 0;

  if ((global_cards_data[internal_card_id].cc[2] & 0x10) != 0)
  {
    if (global_cards_data[internal_card_id].type == 1 || global_cards_data[internal_card_id].type == 0x40)
    {
      PLAYER_CARD_INSTANCE(player, card).color = 1;
    }

    switch (global_cards_data[internal_card_id].id)
    {
    case 300:
    case 0x272:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 1;
      break;

    case 0xf:
    case 0x193:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 0x3e;
      break;

    case 0x139:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 2;
      break;

    case 0x134:
    case 0x27b:
      PLAYER_CARD_INSTANCE(player, card).mana_color = 0x10;
      break;
    }
  }

  TENTATIVE_set_timestamps(player, card);
}

// FUNCTION: MAGIC 0x004b5274
// FUNCTION: SHANDALAR 0x0056ba7d
int calculate_attack_rating(int player, int card)
{
  struct
  {
    int power2;
    int toughness_masked;
    keyword_t static_ability;
    int score;
    int internal_card_id;
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.static_ability = global_cards_data[s.internal_card_id].static_ability;

  s.power2 = (((int)global_cards_data[s.internal_card_id].power & 0xffffbfffU) * 2);
  if (global_cards_data[s.internal_card_id].subtype == 0)
  {
    s.power2 = 0;
  }

  s.toughness_masked = ((int)global_cards_data[s.internal_card_id].toughness & 0xffffbfffU);
  s.score = (int)((s.power2 + 2) * (s.toughness_masked + 1)) / 2;

  if (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) && (current_player == player))
  {
    s.score -= 1;
  }

  if ((s.static_ability & 0x80) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((s.static_ability & 0x100) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((global_cards_data[s.internal_card_id].extra_ability & 3) != 0)
  {
    s.score = (s.score * 3) / 2;
  }
  if ((s.static_ability & 0x40) != 0)
  {
    s.score = (int)((s.toughness_masked + 1) * s.score) / 2;
  }
  if ((s.static_ability & 0x200) != 0)
  {
    s.score = (s.score * 3) / 2;
  }

  s.score = unk_007a7d18[player] * s.score;
  s.score = (s.score + ((s.score >> 0x1f) & 7)) >> 3;
  return s.score;
}

// FUNCTION: MAGIC 0x00500135
// FUNCTION: SHANDALAR 0x004ba25e
int create_a_card_type(int internal_card_id)
{
  int new_internal_card_id;

  for (new_internal_card_id = g_card_count; new_internal_card_id < g_card_count + 0x10; ++new_internal_card_id)
  {
    if (*(int *)&global_cards_data[new_internal_card_id].id == -1)
    {
      memcpy(&global_cards_data[new_internal_card_id], &global_cards_data[internal_card_id], 0x48);
      return new_internal_card_id;
    }
  }

  show_duel_system_error("AddType error: No room in ci to add another type");
  return -1;
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

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  attacking_card_controller = new_attacking_card_controller;
  attacking_card = new_attacking_card;

  saved_unk_008b3270 = unk_008b3270;

  result = (*global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer)(player, card, event);

  if (result != 0x63 && (land_can_be_played & 0x224) != 0 && (event == EVENT_CAN_CAST || event == EVENT_CAN_ACTIVATE))
  {
    if (is_nonactivated_mana_source(player, card) == 0)
    {
      unk_008b3270 = saved_unk_008b3270;
      pop_affected_card_stack();
      return 0;
    }
  }

  last_single_card_event_result = event_result;
  pop_affected_card_stack();
  return result;
}

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

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  attacking_card_controller = 1 - player;
  attacking_card = -1;

  s.saved_trigger_cond = trigger_condition;
  if (event != EVENT_TRIGGER && event != EVENT_RESOLVE_TRIGGER)
  {
    trigger_condition = -1;
  }

  C_dispatch_event_raw(event);

  affected_card_controller = -1;
  trigger_condition = s.saved_trigger_cond;

  s.result = event_result;
  pop_affected_card_stack();
  return s.result;
}

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

  unk_0091c4fc = 0;
  dispatch_trigger(player, TRIGGER_REPLACE_CARD_DRAW, gs_draw_a_card_008961c0, 1);
  if (unk_0091c4fc != 0)
  {
    return 0;
  }

  if (player == active_player)
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
        if (duel_active != 0)
        {
          show_duel_card_preview(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                                 player,
                                 s.drawn_card,
                                 0xf6,
                                 text_lines[0],
                                 1);
        }
        else
        {
          show_card_preview_if_human(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                                     0xf6,
                                     text_lines[0],
                                     1);
        }
      }
    }
    else if (g_duel_ai_mode_state != 1)
    {
      load_text(global_ui_strings_filename, "PROMPT_DRAWACARD");
      set_duel_prompt_text(text_lines[1]);
      Sleep(0x9c4);
      set_duel_prompt_text("");
      exit_duel_thread(0);
    }
    else
    {
      life[active_player] = -99;
      life[1 - active_player] = 0x14;
    }
  }
  else
  {
    if (opponent_initial_library_index == -1)
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
        s.candidate_ok = is_opponent_random_draw_color_allowed((int)(char)global_cards_data[s.drawn_card].color,
                                                               current_encounter_color,
                                                               DAT_008951c8);
        if (s.candidate_ok != 0 && GetCardRarity(s.drawn_card) > current_encounter_strength)
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
        set_duel_prompt_text(text_lines[2]);
        Sleep(0x9c4);
        set_duel_prompt_text("");
        exit_duel_thread(1);
      }
      else
      {
        life[1 - active_player] = -99;
        life[active_player] = 0x14;
      }
    }

    TENTATIVE_reassess_all_cards(0, 0x30);
  }

  ++hand_count[player];
  ++cards_drawn_count;
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

// FUNCTION: MAGIC 0x004b0047
int get_available_card_action(int player, int card)
{
  struct
  {
    card_instance_t *instance;  /* ebp-0x2c */
    unsigned int unused_28;     /* ebp-0x28 */
    unsigned int state;         /* ebp-0x24 */
    unsigned int type;          /* ebp-0x20 */
    int trigger_result;         /* ebp-0x1c */
    unsigned int unused_18;     /* ebp-0x18 */
    unsigned int upkeep_flags;  /* ebp-0x14 */
    int result;                 /* ebp-0x10 */
    int internal_card_id;       /* ebp-0xc */
    int in_play;                /* ebp-0x8 */
    unsigned int extra_ability; /* ebp-0x4 */
  } s;

  s.instance = &PLAYER_CARD_INSTANCE(player, card);
  s.state = s.instance->state;
  s.upkeep_flags = s.instance->upkeep_flags;
  s.in_play = is_in_play(player, card);
  s.internal_card_id = s.instance->internal_card_id;
  s.type = (unsigned int)global_cards_data[s.internal_card_id].type;
  s.extra_ability = (unsigned int)global_cards_data[s.internal_card_id].extra_ability;

  unk_00743038 = 1;
  s.instance->state |= 0x800;

  if (s.in_play != 0 && current_phase == 0x15 && player == current_player && (s.state & 0x8000) != 0 && (s.state & 4) == 0 && can_attack(player, card) != 0)
  {
    s.result = 2;
    goto finish_get_available_card_action;
  }

  if (s.in_play != 0 && trigger_condition != -1)
  {
    if (s.instance->unknown0x14 == trigger_condition)
    {
      if (current_turn == player)
      {
        s.trigger_result = 2;
      }
      else
      {
        s.trigger_result = 0;
      }
    }
    else
    {
      s.trigger_result = ClampIntToRange(dispatch_card_trigger_event(player, card, EVENT_TRIGGER, player), 0, 2);
    }

    if (s.trigger_result != 0)
    {
      DAT_007ab2bc |= 1 << ((unsigned char)s.trigger_result);
      DAT_00791410 += 1;
      s.result = s.trigger_result;
      goto finish_get_available_card_action;
    }
    else
    {
      s.result = 0;
      goto finish_get_available_card_action;
    }
  }
  if (s.in_play == 0 && trigger_condition != -1 && global_cards_data[s.internal_card_id].code_pointer != card_death_ward)
  {
    s.result = 0;
  }
  else if (s.in_play != 0 && current_phase == 1)
  {
    push_affected_card_stack();
    affected_card_controller = player;
    affected_card = card;
    event_result = 0;
    C_dispatch_event_raw(EVENT_TRIGGER);
    s.result = event_result;
    pop_affected_card_stack();
  }
  else if (player == other_player)
  {
    if ((current_action_event_code == 4 && (s.upkeep_flags & 1) != 0) || (current_action_event_code == 10 && s.internal_card_id == draw_card_placeholder_internal_card_id))
    {
      unk_008b3270 |= 3;
      DAT_007ab2bc |= 4;
      s.result = 2;
      goto finish_get_available_card_action;
    }

    s.instance->state &= ~0x800;
    s.result = 0;
  }
  else if (_DAT_00742fbc == 0 && (current_phase == 0x15 || current_phase == 0x17))
  {
    if (s.in_play != 0 && (s.state & 0x10) == 0 && ((s.type & TYPE_CREATURE) != 0 || (s.state & 0x3000000) != 0))
    {
      if (player == current_player && can_attack(player, card) != 0 && (s.state & 0x10000) == 0)
      {
        s.result = 0x10;
        goto finish_get_available_card_action;
      }
      if (player != current_player && attacking_creature_count != 0 && (s.state & 8) == 0)
      {
        s.result = 0x20;
        goto finish_get_available_card_action;
      }
    }

    s.instance->state &= ~0x800;
    s.result = 0;
  }
  else
  {
    if (s.in_play)
    {
      if (current_action_event_code == 4 && (s.upkeep_flags & 1) != 0)
      {
        unk_008b3270 |= 3;
        DAT_007ab2bc |= 4;
        s.result = 2;
        goto finish_get_available_card_action;
      }

      if (current_action_event_code == 4 && (s.upkeep_flags & 0x10) != 0 && (s.upkeep_flags & 0x88) == 0 && can_pay_untap_cost(player, card) != 0)
      {
        DAT_007ab2bc |= 2;
        s.result = 8;
        goto finish_get_available_card_action;
      }

      if ((s.state & 0x10) == 0 && (s.type & TYPE_CREATURE) != 0 && _DAT_00742fbc == 0 && player == current_player && current_phase <= 0x1a && can_attack(player, card) != 0 && (((PLAYER_CARD_INSTANCE(player, card).state & 0x30000) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
      {
        _DAT_00743024 = 1;
      }

      if ((((s.extra_ability & 0x1000) != 0 && (s.state & 0x10) == 0 && (((PLAYER_CARD_INSTANCE(player, card).state & 0x30000) == 0) || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0)) || ((s.extra_ability & 1) != 0 && (DAT_00742f68 & 0x10) != 0) || ((s.extra_ability & 2) != 0 && (DAT_00742f68 & 0x20) != 0)) && ((land_can_be_played & 4) == 0 || (s.extra_ability & 0x5004) != 0))
      {
        unk_008b3270 &= ~2;
        if ((s.state & 0x20) == 0 && dispatch_event_to_single_card(player, card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          if ((unk_008b3270 & 2) != 0)
          {
            DAT_007ab2bc |= 4;
            s.result = 2;
            goto finish_get_available_card_action;
          }

          DAT_007ab2bc |= 2;
          s.result = 8;
          goto finish_get_available_card_action;
        }
      }
    }
    else
    {
      if ((s.state & 0xa0) != 0)
      {
        s.instance->state &= ~0x800;
        s.result = 0;
        goto finish_get_available_card_action;
      }

      s.unused_28 = single_color_test_bit_to_color_t((int)(char)global_cards_data[s.internal_card_id].color);
      if (_DAT_00742fbc == 0 || (DAT_00742f68 & s.type) != 0)
      {
        if ((s.type & TYPE_LAND) != 0)
        {
          if (player == current_player && (land_can_be_played & 1) == 0 && (current_phase == 0x14 || current_phase == 0x1e))
          {
            s.result = 4;
            goto finish_get_available_card_action;
          }
          else
          {
            s.instance->state &= ~0x800;
            s.result = 0;
            goto finish_get_available_card_action;
          }
        }

        if ((((active_player == current_player && (((_DAT_00742fbc != 0 && (s.type & 0x30) != 0) || current_phase == 0x14) || current_phase == 0x1e)) || (active_player != current_player && _DAT_00742fbc != 0 && ((s.type & 0x10) != 0 || (s.type & 0x20) != 0))) && (s.unused_18 = can_pay_card_mana_cost(player, player, card)) != 0 && (((land_can_be_played & 4) == 0 || (s.extra_ability & 0x3004) != 0) && ((s.type & 0x42) != 0 || dispatch_event_to_single_card(player, card, EVENT_CAN_CAST, 1 - player, -1) != 0))))
        {
          s.result = 4;
          goto finish_get_available_card_action;
        }
      }
    }

    s.instance->state &= ~0x800;
    s.result = 0;
  }

finish_get_available_card_action:
  unk_00743038 = 0;
  return s.result;
}

// FUNCTION: MAGIC 0x004e1cd1
// FUNCTION: SHANDALAR 0x00426f3c
void redraw_shandalar_duel_screen(int view_player, int present_after_draw)
{
  /*
   * MAGIC keeps Shandalar's two-argument screen redraw hook, but compiles it
   * as a no-op.  In SHANDALAR, view_player selects the bottom/hero side and
   * present_after_draw controls whether the freshly drawn page is presented.
   */
  (void)view_player;
  (void)present_after_draw;
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
  for (s.current_color = 0; s.current_color < 10 && mana_color_conversions[player][s.current_color] != -1; ++s.current_color)
  {
    s.mana_source_color = (unsigned short)mana_color_conversions[player][s.current_color];
    s.mana_color = HIWORD(mana_color_conversions[player][s.current_color]);
    if (s.effective_color == (color_t)s.mana_color)
    {
      s.interchangeable |= 1 << (unsigned char)s.mana_source_color;
    }
  }

  s.available_mana = raw_mana_available[player][s.effective_color];
  for (s.current_color = COLOR_COLORLESS; s.current_color < 7; ++s.current_color)
  {
    if (s.effective_color != (color_t)s.current_color && (s.interchangeable & (1 << (unsigned char)s.current_color)) != 0)
    {
      s.available_mana += raw_mana_available[player][s.current_color];
    }
  }

  s.artifact_mana = raw_mana_available[player][COLOR_ARTIFACT];

  s.special_mana = special_mana_pool[player][s.effective_color];
  for (s.current_color = COLOR_COLORLESS; s.current_color < 7; ++s.current_color)
  {
    if (s.effective_color != (color_t)s.current_color && (s.interchangeable & (1 << (unsigned char)s.current_color)) != 0)
    {
      s.special_mana += special_mana_pool[player][s.current_color];
    }
  }

  s.xmana = 0;
  for (s.current_color = 0; s.current_color < 0x32 && raw_mana_available_hex[player][s.current_color] != -1; ++s.current_color)
  {
    if (s.effective_color == COLOR_ANY)
    {
      s.xmana += raw_mana_available_hex[player][s.current_color] >> 16;
    }
    else if ((raw_mana_available_hex[player][s.current_color] & (1 << (unsigned char)s.effective_color)) != 0)
    {
      s.xmana += raw_mana_available_hex[player][s.current_color] >> 16;
    }
    else
    {
      if ((s.interchangeable & raw_mana_available_hex[player][s.current_color]) != 0)
      {
        s.xmana += raw_mana_available_hex[player][s.current_color] >> 16;
      }
    }
  }

  s.artifact_special_mana = special_mana_pool[player][COLOR_ARTIFACT];

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

// FUNCTION: MAGIC 0x00442dac
// FUNCTION: SHANDALAR 0x0040e646
void push_affected_card_stack(void)
{
  if (affected_card_context_depth >= 0x20)
    return;

  affected_card_context_stack[affected_card_context_depth][0] = affected_card_controller;
  affected_card_context_stack[affected_card_context_depth][1] = affected_card;
  affected_card_context_stack[affected_card_context_depth][2] = affected_internal_card_id;
  affected_card_context_stack[affected_card_context_depth][3] = affected_card_color;
  affected_card_context_stack[affected_card_context_depth][4] = attacking_card_controller;
  affected_card_context_stack[affected_card_context_depth][5] = attacking_card;
  affected_card_context_stack[affected_card_context_depth][6] = event_result;
  affected_card_context_depth += 1;
}

// FUNCTION: MAGIC 0x00442e62
// FUNCTION: SHANDALAR 0x0040e6fc
void pop_affected_card_stack(void)
{
  if (affected_card_context_depth > 0)
  {
    affected_card_context_depth -= 1;
  }

  affected_card_controller = affected_card_context_stack[affected_card_context_depth][0];
  affected_card = affected_card_context_stack[affected_card_context_depth][1];
  affected_internal_card_id = affected_card_context_stack[affected_card_context_depth][2];
  affected_card_color = affected_card_context_stack[affected_card_context_depth][3];
  attacking_card_controller = affected_card_context_stack[affected_card_context_depth][4];
  attacking_card = affected_card_context_stack[affected_card_context_depth][5];
  event_result = affected_card_context_stack[affected_card_context_depth][6];
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
  if (last_single_card_event_result != 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x00445918
// FUNCTION: SHANDALAR 0x004111b1
int TENTATIVE_set_timestamps(int player, int card)
{
  int slot;

  for (slot = 0; slot < 500; slot++)
  {
    if (TENTATIVE_timestamp_player[slot] == -1)
    {
      TENTATIVE_timestamp_player[slot] = player;
      TENTATIVE_timestamp_card[slot] = card;
      global_card_instances[player][card].timestamp = slot;
      return slot;
    }
  }
  return -1;
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
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
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
                           text_lines[0],
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

// FUNCTION: MAGIC 0x004eab5c
// FUNCTION: SHANDALAR 0x004412ec
void declare_mana_available_hex(int player, color_test_t colors, int amount)
{
  int i;

  if ((int)colors > 0)
  {
    for (i = 0; i < 0x32; ++i)
    {
      if (raw_mana_available_hex[player][i] == -1)
      {
        raw_mana_available_hex[player][i] = (amount << 16) | colors;
        raw_mana_available_hex[player][i + 1] = -1;
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
    if (raw_mana_available_hex[player][s.i] == -1)
    {
      break;
    }

    if (s.found != 0)
    {
      break;
    }

    if (raw_mana_available_hex[player][s.i] == ((amount << 16) | color))
    {
      s.found = 1;
      s.j = s.i;
      while (s.j < 0x31)
      {
        raw_mana_available_hex[player][s.j] = raw_mana_available_hex[player][s.j + 1];
        ++s.j;
      }
    }

    ++s.i;
  }
}

// FUNCTION: MAGIC 0x004eada9
// FUNCTION: SHANDALAR 0x00441539
int declare_mana_available(int player, color_t color, int amount)
{
  special_mana_pool[player][color] += amount;
  special_mana_pool[player][COLOR_ANY] += amount;
  return special_mana_pool[player][color];
}

// FUNCTION: MAGIC 0x004eadeb
// FUNCTION: SHANDALAR 0x0044157b
int undeclare_mana_available(int player, color_t color, int amount)
{
  special_mana_pool[player][color] -= amount;
  special_mana_pool[player][COLOR_ANY] -= amount;
  return special_mana_pool[player][color];
}

// FUNCTION: MAGIC 0x004eaec1
// FUNCTION: SHANDALAR 0x00441651
int undeclare_mana_available_and_produce_it(int player, color_t color, int amount)
{
  undeclare_mana_available(player, color, amount);
  produce_mana(player, color, amount);
  return raw_mana_available[player][color];
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

// FUNCTION: MAGIC 0x004a686a
// FUNCTION: SHANDALAR 0x005589ed
int choose_a_color(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors)
{
  int chosen_color;

  if (g_duel_ai_mode_state == 1)
  {
    return ai_choice;
  }

  if (other_player == player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xf);
    return DAT_008b293c;
  }

  chosen_color = choose_a_color_dialog(player, prompt, use_color_names_instead_of_land, ai_choice, available_colors);
  if (player == active_player && (g_duel_network_flags & 2) != 0)
  {
    unk_008b2938 = '\x0f';
    DAT_008b293c = chosen_color;
    TENTATIVE_send_network_result(player, 0xf);
  }

  return chosen_color;
}

// FUNCTION: MOK 0x0044e050
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

// FUNCTION: MAGIC 0x004e1c81
int legacy_clear_graphics_page_stub()
{
}

// FUNCTION: MAGIC 0x00464a57
// FUNCTION: SHANDALAR 0x00522508
int internal_rand(int maximum)
{
  return (maximum > 1) ? rand() % maximum : 0;
}

// FUNCTION: MAGIC 0x00464a84
// FUNCTION: SHANDALAR 0x00522535
int network_random(int player, int maximum)
{
  if (player == active_player)
  {
    if (maximum > 1)
    {
      g_network_result_value = rand() % maximum;
    }
    else
    {
      g_network_result_value = 0;
    }

    g_network_result_packet_type = 0x18;
    TENTATIVE_send_network_result(player, 0x18);
  }
  else
  {
    TENTATIVE_wait_for_network_result(player, 0x18);
  }

  return g_network_result_value;
}

// FUNCTION: MAGIC 0x0048463d
// FUNCTION: SHANDALAR 0x00488147
int card_has_basic_land_type(int player, int card, int land_color)
{
  struct
  {
    int result;
    int internal_card_id;
  } s;

  s.result = 0;

  if (global_card_instances[player][card].internal_card_id != -1)
  {
    s.internal_card_id = global_card_instances[player][card].internal_card_id;
  }
  else
  {
    s.internal_card_id = global_card_instances[player][card].original_internal_card_id;
  }

  if (player == -1 || card == -1)
  {
    return 0;
  }

  if ((char)global_cards_data[s.internal_card_id].subtype == 13 && global_cards_data[s.internal_card_id].id == g_duel_special_land_card_ids[land_color])
  {
    s.result = 1;
  }
  if ((char)global_cards_data[s.internal_card_id].subtype == 12 && ((1 << (unsigned char)land_color) & (int)(char)global_card_instances[player][card].mana_color) != 0)
  {
    s.result = 1;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004817fd
// FUNCTION: SHANDALAR 0x00485308
int ante_drawn_card(int player)
{
  int ante_slot = 0;
  int card_in_hand;

  while (global_ante_cards[player][ante_slot] != -1 && ante_slot < 0x10)
  {
    ++ante_slot;
  }

  if (ante_slot < 0x10)
  {
    card_in_hand = draw_card_for_player(player);
    global_ante_cards[player][ante_slot] = PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id;
    --hand_count[player];
    load_text("promptsX1.txt", "ANTE_A_CARD");
    if (active_player == player)
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[0], 0);
    }
    else
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[1], 0);
    }

    PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id = -1;

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00483489
// FUNCTION: SHANDALAR 0x00486f9a
int card_matches_excluded_land_type(int player, int card, int excluded_internal_card_id)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if (excluded_internal_card_id == -1)
  {
    return 0;
  }

  if (global_cards_data[excluded_internal_card_id].code_pointer == (int)global_cards_data[internal_card_id].code_pointer)
  {
    return 1;
  }

  if ((int)*(char *)&global_cards_data[internal_card_id].subtype == 0xc)
  {
    if ((int)*(char *)&global_cards_data[excluded_internal_card_id].subtype != 0xc)
    {
      if (((int)*(char *)&global_cards_data[excluded_internal_card_id].color & (int)*(char *)&global_cards_data[internal_card_id].color) != 0)
      {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004bff5a
// FUNCTION: SHANDALAR 0x004c0dd4
int has_pending_damage(int player, int card)
{
  struct
  {
    int result;
    int test_card;
    int test_player;
  } s;

  s.result = 0;
  for (s.test_player = 0; s.test_player < 2 && s.result == 0; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player] && s.result == 0; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == damage_card_internal_card_id &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card)
      {
        s.result = 1;
      }
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004c081a
// FUNCTION: SHANDALAR 0x004c1695
int is_basic_land(int player, int card)
{
  int result;

  result = 0;
  if ((int)(char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].subtype == 0xd)
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x004c0880
// FUNCTION: SHANDALAR 0x004c16fb
int basic_land_matches_type_index(int internal_card_id, int extra)
{
  int result;

  result = 0;
  if (extra == 0 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[1]))
  {
    result = 1;
  }
  if (extra == 1 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[2]))
  {
    result = 1;
  }
  if (extra == 2 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[3]))
  {
    result = 1;
  }
  if (extra == 3 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[4]))
  {
    result = 1;
  }
  if (extra == 4 && (extra == internal_card_id || global_cards_data[internal_card_id].id == g_duel_special_land_card_ids[5]))
  {
    result = 1;
  }
  return result;
}

// FUNCTION: MAGIC 0x004848a0
// FUNCTION: SHANDALAR 0x004883aa
int gain_life(int player, int amount, ...)
{
  int saved_trigger_cause_controller;
  int saved_trigger_cause;
  int num_cards_drawn;

  life[player] += amount;

  if ((battlefield_extra_ability_flags & 0x00200000) != 0)
  {
    saved_trigger_cause_controller = trigger_cause_controller;
    saved_trigger_cause = trigger_cause;
    push_affected_card_stack();

    trigger_cause_controller = player;
    trigger_cause = -1;
    life_gained = amount;
    dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_GAIN_LIFE, &gs_gain_life_007895e0[0], 0);

    trigger_cause_controller = saved_trigger_cause_controller;
    trigger_cause = saved_trigger_cause;
    pop_affected_card_stack();
  }

  if (unk_008b44d0[player] != 0)
  {
    for (num_cards_drawn = 0; num_cards_drawn < amount; ++num_cards_drawn)
    {
      draw_card_for_player(player);
    }
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

// FUNCTION: MAGIC 0x004b1670
void move_card_to_exile(int player, int internal_card_id)
{
  int exile_index;

  if (g_duel_ai_mode_state != 1 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    play_sound_effect(WAV_DESTROY);
  }

  for (exile_index = 0; exile_index < 500; exile_index++)
  {
    if (global_exile[player][exile_index] == -1)
    {
      global_exile[player][exile_index] = internal_card_id;
      return;
    }
  }
}

// FUNCTION: MAGIC 0x004b0c12
int process_killed_card(int player, int card)
{
  struct
  {
    int saved_trigger_cause_controller;
    int saved_trigger_cause;
    int kill_mode;
    int internal_card_id;
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.kill_mode = (int)(char)PLAYER_CARD_INSTANCE(player, card).kill_code;
  if (s.kill_mode == 0)
  {
    return 0;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 8) == 0)
  {
    if (s.kill_mode != 4 &&
        (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0) &&
        (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0)
    {
      ++unk_008cfdac;
    }

    if (((unsigned char)global_cards_data[s.internal_card_id].type & 0xc7) != 0)
    {
      push_affected_card_stack();
      event_result = 0;
      affected_card_controller = player;
      affected_card = card;
      attacking_card_controller = 1 - player;
      attacking_card = -1;
      C_dispatch_event_raw(0x77);
      if (event_result > 0)
      {
        PLAYER_CARD_INSTANCE(player, card).token_status &= 0xffffff7f;
        pop_affected_card_stack();
        return 0;
      }

      s.kill_mode = (int)(char)PLAYER_CARD_INSTANCE(player, card).kill_code;
      pop_affected_card_stack();
    }

    if ((((unsigned char)global_cards_data[s.internal_card_id].type & 0x80) == 0) &&
        ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x10) == 0))
    {
      if (s.kill_mode == 4)
      {
        move_card_to_exile((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) >> 12,
                           PLAYER_CARD_INSTANCE(player, card).original_internal_card_id);
      }
      else
      {
        if (s.kill_mode != 3 && g_duel_ai_mode_state != 1)
        {
          if (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type & TYPE_CREATURE) == 0)
          {
            if (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type & 0x38) == 0)
            {
              play_sound_effect(WAV_BURIED);
            }
          }
          else
          {
            play_sound_effect(WAV_KILL);
          }
        }

        move_card_to_graveyard(player, card);
        if (s.kill_mode == 3)
        {
          dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);
        }
      }
    }
  }

  push_affected_card_stack();
  s.saved_trigger_cause = trigger_cause;
  s.saved_trigger_cause_controller = trigger_cause_controller;
  trigger_cause_controller = player;
  trigger_cause = card;
  if (((unsigned int)(unsigned char)global_cards_data[s.internal_card_id].type & 0x1047) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_LEAVE_PLAY, gs_card_leaving_play_007aaef0, 0);
  }
  trigger_cause_controller = s.saved_trigger_cause_controller;
  trigger_cause = s.saved_trigger_cause;
  pop_affected_card_stack();

  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
  {
    --creature_cards_in_play[player];
  }
  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_ARTIFACT) != 0)
  {
    --artifact_cards_in_play[player];
  }
  if (((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_ENCHANTMENT) != 0)
  {
    --enchantments_in_play[player];
  }

  PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
  PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
  if (g_duel_ai_mode_state != 1)
  {
    request_duel_display_refresh_if_human(player, card, 7, 2);
  }

  clear_card_attachments(player, card);
  if (((unsigned int)(unsigned char)global_cards_data[s.internal_card_id].type & 0x1047) != 0)
  {
    reassess_all_cards_and_mana();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b08e6
// FUNCTION: SHANDALAR 0x0046d8c1
void kill_card(int player, int card, kill_t kill_mode)
{
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
  {
    return;
  }

  PLAYER_CARD_INSTANCE(player, card).token_status |= 0x80;
  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (internal_card_id == -1)
  {
    return;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).state & 2) == 0)
  {
    kill_mode = 3;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).token_status & 8) != 0 || kill_mode == 3 || kill_mode == 4 ||
      (global_cards_data[internal_card_id].type & 0x43) == 0 || global_cards_data[internal_card_id].type == 0x80)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = kill_mode;
    process_killed_card(player, card);
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = kill_mode;
    PLAYER_CARD_INSTANCE(player, card).state |= 2;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0xd6;
    pending_killed_card_handler = process_killed_card;
  }
}

// FUNCTION: MAGIC 0x004b117e
void clear_card_attachments(int player, int card)
{
  struct
  {
    int current_card;   /* ebp - 0x970 */
    int linked_count;   /* ebp - 0x96c */
    int linked_type;    /* ebp - 0x968 */
    int current_player; /* ebp - 0x964 */
    int linked_cards[600];
  } s;

  s.linked_count = 0;
  for (s.current_player = 0; s.current_player < 2; ++s.current_player)
  {
    for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; ++s.current_card)
    {
      if (is_in_play(s.current_player, s.current_card) &&
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_card == card &&
          (s.current_player != player || s.current_card != card))
      {
        s.linked_type = global_cards_data[PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id].type;
        if ((s.linked_type & 0x43) != 0)
        {
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_player = -1;
          PLAYER_CARD_INSTANCE(s.current_player, s.current_card).damage_target_card = -1;
        }
        else
        {
          s.linked_cards[s.linked_count * 2] = s.current_player;
          s.linked_cards[s.linked_count * 2 + 1] = s.current_card;
          ++s.linked_count;
        }
      }
    }
  }

  while (s.linked_count != 0)
  {
    --s.linked_count;
    kill_card(s.linked_cards[s.linked_count * 2], s.linked_cards[s.linked_count * 2 + 1], 2);
  }

  PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
  PLAYER_CARD_INSTANCE(player, card).counter_toughness = PLAYER_CARD_INSTANCE(player, card).damage_on_card;
  PLAYER_CARD_INSTANCE(player, card).counter_power = PLAYER_CARD_INSTANCE(player, card).counter_toughness;
  PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
}

// FUNCTION: MAGIC 0x0043e668
// FUNCTION: SHANDALAR 0x00409ee8
void discard(int player, int flags, int player_who_controls_effect)
{
  struct
  {
    int tries;         // ebp - 0x4cc
    int selected_card; // ebp - 0x4c8
    target_t target;
    char prompt[600];
    char prompt_line_2[300];
    int dialog_result;
    char prompt_line_1[300];
    int current_card;
    unsigned int found;
  } s;

  s.tries = 0;

  if (hand_count[player] <= 0)
    return;

  if ((other_player == player && (g_duel_network_flags & 2) == 0 && (hand_count[player] + unk_007161d8) <= 0))
    return;

  if (((active_player == player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1) && flags == 0)
  {
    load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
    C_real_select_target(player,
                         player,
                         player,
                         TARGET_ZONE_HAND,
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
                         0,
                         &s.target);
    s.selected_card = s.target.card;
  }
  else
  {
    s.found = 0;
    if ((g_duel_network_flags & 2) != 0 && other_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x14);
      s.selected_card = g_network_result_value;
      if (s.selected_card == -1)
        s.found = 0;
      else
        s.found = 1;
    }
    else
    {
      s.tries = 0;
      do
      {
        s.selected_card = internal_rand(active_cards_count[player]);
        if (PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 0x20) == 0)
        {
          s.found = 1;
        }
      } while (s.found == 0 && ++s.tries < 999);

      if (s.found == 0)
      {
        for (s.current_card = 0; s.current_card < active_cards_count[player] && s.found == 0; s.current_card++)
        {
          if (PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x20) == 0)
          {
            s.found = 1;
            s.selected_card = s.current_card;
          }
        }
      }

      if ((g_duel_network_flags & 2) != 0 && active_player == player)
      {
        if (s.found == 0)
        {
          s.selected_card = -1;
        }
        g_network_result_value = s.selected_card;
        g_network_result_packet_type = 0x14;
        TENTATIVE_send_network_result(player, 0x14);
      }
    }
  }

  if ((unk_007a79b0[player] & 2) == 0 || player_who_controls_effect != 0)
  {
    if (other_player == player && (g_duel_network_flags & 2) == 0 && g_duel_ai_mode_state != 1)
    {
      if (flags)
      {
        load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
        do_dialog(player, player, s.selected_card, -1, -1, text_lines[1], 0);
      }
      else
      {
        load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
        do_dialog(player, player, s.selected_card, -1, -1, text_lines[2], 0);
      }
    }
    discard_card_from_hand(player, s.selected_card);
  }
  else
  {
    load_text(global_ui_strings_filename, "PROMPT_DISCARDACARD");
    strcpy(s.prompt_line_1, text_lines[3]);
    strcpy(s.prompt_line_2, text_lines[4]);
    sprintf(s.prompt, " %s\n %s", s.prompt_line_1, s.prompt_line_2);
    s.dialog_result = do_dialog(player, player, s.selected_card, -1, -1, s.prompt, 0);
    if (s.dialog_result == 0)
    {
      real_put_on_top_of_deck(player, PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id);
      PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id = -1;
      request_duel_display_refresh_if_human(player, s.selected_card, 10, 1);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_DISCARD);
      }
      --hand_count[player];
    }
    else
    {
      discard_card_from_hand(player, s.selected_card);
    }
  }
}

// FUNCTION: MAGIC 0x004eae35
// FUNCTION: SHANDALAR 0x004415c5
int produce_mana(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
  raw_mana_available[player][7] += amount;
  return raw_mana_available[player][color];
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

  if ((land_can_be_played & LCBP_CARD_BEING_COPIED) != 0)
  {
    goto skipped_for_copied_card;
  }

  {
    if (mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] > 0)
    {
      if (color == COLOR_COLORLESS)
      {
        if (amount != -1)
        {
          amount = MAX(amount - mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2, 1);
        }
      }
      else if (mana_charge[COLOR_COLORLESS] > 0)
      {
        mana_charge[COLOR_COLORLESS] = MAX(mana_charge[COLOR_COLORLESS] - mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2, 1);
      }
    }

    mana_charge[color] += amount;
    s.mana_paid_total = 0;
    for (s.i = 0; s.i < 7; ++s.i)
    {
      s.mana_paid_by_color[s.i] = 0;
    }

    s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);

    s.include_auto_payment = 1;
    if (unk_00715fb0 != 0 || (other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
    {
      s.allow_special_x_payment = 1;
      s.allow_generic_payment = 1;
      s.allow_mana_sources = 1;
      if ((other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
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

    if (other_player == player && (g_duel_network_flags & 2) == 0)
    {
      for (s.i = COLOR_COLORLESS; s.i < 7; ++s.i)
      {
        if (mana_charge[s.i] == -1)
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

            if (max_x_value != -1)
            {
              s.old_tapped_state = MIN(max_x_value, s.old_tapped_state);
            }

            ai_recorded_choice = s.old_tapped_state;
            record_ai_action_selection();
          }
          else
          {
            replay_ai_action_selection();
            if (ai_recorded_choice == 99)
            {
              ai_recorded_choice = 0;
            }
            s.old_tapped_state = ai_recorded_choice;
          }
        }
      }

      max_x_value = s.old_tapped_state;
    }

    if (mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] > 0)
    {
      x_value = mana_charge[MANA_CHARGE_POWER_ARTIFACT_REDUCTION] * 2;
    }
    else
    {
      x_value = 0;
    }

    if (max_x_value == 0)
    {
      for (s.i = 0; s.i < 7; ++s.i)
      {
        if (mana_charge[s.i] == -1)
        {
          mana_charge[s.i] = 0;
        }
      }
    }

    for (s.i = 0, s.payable_colors = 0; s.i < 7; ++s.i)
    {
      if (mana_charge[s.i] == -1)
      {
        s.payable_colors = 1;
      }
    }

    if (s.include_auto_payment != 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
    {
      auto_pay_colored_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
      copy_mana_pool_to_display();
    }

    if (s.allow_special_x_payment != 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0 && s.payable_colors != 0)
    {
      auto_pay_variable_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_generic_payment != 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
    {
      auto_pay_generic_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_mana_sources != 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
    {
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1e, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1c, s.temp);
      }
    }

    if (s.allow_interrupt_speed_mana_sources != 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
    {
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x14, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x04, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1a, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x18, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x10, s.temp);
      }
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        activate_mana_sources_for_payment(player, s.mana_paid_by_color, &s.mana_paid_total, 0x00, s.temp);
      }
    }

    if ((other_player == player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
    {
      if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0 && s.payable_colors == 0)
      {
        spell_fizzled = 1;
      }
    }
    else if (is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
    {
      s.done = 0;
      while (s.done == 0 && is_mana_cost_paid(mana_charge, x_value, max_x_value) == 0)
      {
        s.only_variable_costs = 1;
        for (s.i = 0; s.i < 7; ++s.i)
        {
          if (mana_charge[s.i] > 0)
          {
            s.only_variable_costs = 0;
          }
        }

        format_mana_payment_prompt(g_ui_message_buffer, mana_charge, x_value, max_x_value);
        s.selected_card =
            select_card_for_action(player, player, player, 0, 0, g_ui_message_buffer, s.only_variable_costs != 0 ? 3 : 1);

        if (unk_00742fcc == -1 && (s.selected_card == -1 || s.selected_card == -2))
        {
          if (g_target_selection_status_code == -2)
          {
            if (stop_phase_player == -1 && stop_phase == -1)
            {
              if (s.selected_card == -1)
              {
                spell_fizzled = 1;
              }
              s.done = 1;
            }
            else if (s.only_variable_costs != 0)
            {
              s.done = 1;
            }
          }
          else if (g_target_selection_status_code == -3 && player == DAT_00715fa4 && DAT_0072c8e0 != -1)
          {
            if (raw_mana_available[player][DAT_0072c8e0] > 0 && (mana_charge[DAT_0072c8e0] != 0 || mana_charge[COLOR_COLORLESS] != 0 || mana_charge[COLOR_ARTIFACT] != 0) && (DAT_0072c8e0 != COLOR_ARTIFACT || mana_charge[COLOR_ARTIFACT] != 0))
            {
              if (mana_charge[DAT_0072c8e0] == 0)
              {
                if (mana_charge[COLOR_ARTIFACT] == 0)
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
                s.cost_color = DAT_0072c8e0;
              }

              s.amount_to_take =
                  get_mana_payment_amount(mana_charge, s.cost_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
              spend_mana_for_cost(
                  mana_charge, s.cost_color, s.amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
              copy_mana_pool_to_display();
            }

            if (raw_mana_available[player][DAT_0072c8e0] > 0)
            {
              s.produced_special_colors = 0;
              for (s.slot = 0; s.slot < 10 && mana_color_conversions[player][s.slot] != -1; ++s.slot)
              {
                s.produced_color = (unsigned short)mana_color_conversions[player][s.slot];
                s.special_color = ((unsigned int)mana_color_conversions[player][s.slot] >> 16) & 0xffff;
                if (s.produced_color == DAT_0072c8e0)
                {
                  s.produced_special_colors |= 1 << (unsigned char)s.special_color;
                }
              }

              s.payable_color_mask = 0;
              for (s.i = 0; s.i < 7; ++s.i)
              {
                if (mana_charge[s.i] != 0)
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
                  s.chosen_color = choose_a_color(player, text_lines[0], 1, DAT_0072c8e0, s.produced_special_colors);
                }

                s.special_amount_to_take =
                    get_mana_payment_amount(mana_charge, s.chosen_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
                spend_mana_for_cost(
                    mana_charge, s.chosen_color, s.special_amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
                copy_mana_pool_to_display();
              }
            }
          }
        }
        else
        {
          if (unk_00742fcc == -1 || s.selected_card != -1)
          {
            s.current_internal_card_id = (global_card_instances[player] + s.selected_card)->internal_card_id;
            if ((global_cards_data[s.current_internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 &&
                ((global_cards_data[s.current_internal_card_id].type & TYPE_INTERRUPT) != 0 ||
                 ((((global_card_instances[player] + s.selected_card)->state & STATE_IN_PLAY) != 0) &&
                  ((unsigned char)(global_card_instances[player] + s.selected_card)->color & 0x80) == 0 &&
                  ((global_card_instances[player] + s.selected_card)->state & STATE_TAPPED) == 0 &&
                  ((((unsigned char)(global_card_instances[player] + s.selected_card)->color & 3) == 0) ||
                   (global_cards_data[(global_card_instances[player] + s.selected_card)->internal_card_id].type & TYPE_CREATURE) == 0))))
            {
              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                s.saved_raw_mana[s.idx8] = raw_mana_available[player][s.idx8];
              }

              s.saved_x_value = x_value;
              x_value = 0;
              s.saved_max_x_value = max_x_value;
              max_x_value = -1;
              for (s.idx8 = 0; s.idx8 < 7; ++s.idx8)
              {
                s.saved_mana_cost[s.idx8] = mana_charge[s.idx8];
                mana_charge[s.idx8] = 0;
              }

              if (((global_card_instances[player] + s.selected_card)->state & 0x800002) == STATE_IN_PLAY)
              {
                if ((global_cards_data[s.current_internal_card_id].type & TYPE_LAND) != 0 ||
                    dispatch_event_to_single_card(player, s.selected_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
                {
                  push_card_onto_stack(player, s.selected_card, EVENT_RESOLVE_ACTIVATION, player, 0);
                  unk_00938e2c = get_required_mana_color_mask(s.saved_mana_cost);
                  produced_mana_color = -1;
                  s.old_tapped_state_before_activation = (global_card_instances[player] + s.selected_card)->state & STATE_TAPPED;
                  dispatch_event_to_single_card(player, s.selected_card, EVENT_ACTIVATE, 1 - player, -1);
                  unk_00938e2c = 0;
                  if (spell_fizzled == 1)
                  {
                    spell_fizzled = 0;
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

              x_value = s.saved_x_value;
              max_x_value = s.saved_max_x_value;
              for (s.idx8 = 0; s.idx8 < 7; ++s.idx8)
              {
                mana_charge[s.idx8] = s.saved_mana_cost[s.idx8];
              }

              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                raw_mana_available[player][s.idx8] -= s.saved_raw_mana[s.idx8];
              }

              s.max_colorless = 0;
              auto_pay_colored_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
              auto_pay_variable_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
              auto_pay_generic_mana(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);

              for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
              {
                raw_mana_available[player][s.idx8] += s.saved_raw_mana[s.idx8];
              }

              copy_mana_pool_to_display();
            }
          }
        }
      }
    }
  }

skipped_for_copied_card:
  if (spell_fizzled == 1)
  {
    refund_paid_mana(s.mana_paid_by_color);
    for (s.i = 0; s.i < 7; ++s.i)
    {
      produce_mana(player, s.i, s.mana_paid_by_color[s.i]);
      s.mana_paid_by_color[s.i] = 0;
    }
    s.mana_paid_total = 0;
    x_value = 0;
  }

  for (s.i = 0; s.i < 8; ++s.i)
  {
    mana_charge[s.i] = 0;
  }
  max_x_value = -1;

  return s.mana_paid_total;
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
    s.stack_player = global_stack_cards[stack_size - 1].player;
    s.stack_card = global_stack_cards[stack_size - 1].card;
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

// FUNCTION: MAGIC 0x00435c91
// FUNCTION: SHANDALAR 0x0041716b
int record_paid_mana(int color, int amount)
{
  if (0 < mana_payment_record_depth)
  {
    recorded_mana_payments[mana_payment_record_depth - 1][color] += amount;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x004eae77
// FUNCTION: SHANDALAR 0x00441607
int unproduce_mana(int player, int color, int amount)
{
  raw_mana_available[player][color] -= amount;
  raw_mana_available[player][7] -= amount;
  return raw_mana_available[player][color];
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
    while (mana_charge[color] > 0 && raw_mana_available[player][color] > 0)
    {
      spend_mana_for_cost(mana_charge, color, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
    }
  }

  while (mana_charge[6] > 0 && raw_mana_available[player][0] > 0 && max_colorless < mana_charge[6])
  {
    spend_mana_for_cost(mana_charge, 6, 1, (int *)0, 0, player, 0, mana_paid_by_color, total_mana_paid);
  }

  slot = 0;
  while (slot < 10 && mana_color_conversions[player][slot] != -1)
  {
    produced_color = (unsigned short)mana_color_conversions[player][slot];
    special_mana = (unsigned int)mana_color_conversions[player][slot];
    while (raw_mana_available[player][produced_color] > 0 && mana_charge[special_mana >> 16] > 0)
    {
      spend_mana_for_cost(
          mana_charge, special_mana >> 16, 1, (int *)0, 0, player, produced_color, mana_paid_by_color, total_mana_paid);
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
    if (mana_charge[color] == -1)
    {
      while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
      {
        spend_mana_for_cost(
            mana_charge, color, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
      }
    }
  }

  if (mana_charge[6] == -1)
  {
    while (raw_mana_available[player][0] > 0 && max_colorless < mana_charge[6] &&
           (x_value < max_x || max_x == -1))
    {
      spend_mana_for_cost(
          mana_charge, 6, 1, special_mana_override, max_x, player, 0, mana_paid_by_color, total_mana_paid);
    }
  }

  slot = 0;
  while (slot < 10 && mana_color_conversions[player][slot] != -1)
  {
    produced_color = (unsigned short)mana_color_conversions[player][slot];
    special_mana = (unsigned int)mana_color_conversions[player][slot];
    if (mana_charge[special_mana >> 16] == -1)
    {
      while (raw_mana_available[player][produced_color] > 0 && (x_value < max_x || max_x == -1))
      {
        spend_mana_for_cost(mana_charge,
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

  if (mana_charge[COLOR_ARTIFACT] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (mana_charge[COLOR_ARTIFACT] > 0)
      {
        while (raw_mana_available[player][color] > 0 && max_colorless < mana_charge[COLOR_ARTIFACT])
        {
          spend_mana_for_cost(mana_charge, COLOR_ARTIFACT, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
      else
      {
        while (raw_mana_available[player][color] > 0 && mana_charge[COLOR_ARTIFACT] == -1 &&
               (max_x == -1 || max_colorless < max_x - x_value))
        {
          spend_mana_for_cost(mana_charge,
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

  if (mana_charge[COLOR_COLORLESS] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (color != COLOR_ARTIFACT)
      {
        if (mana_charge[COLOR_COLORLESS] == -1)
        {
          while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
          {
            spend_mana_for_cost(mana_charge,
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
          while (mana_charge[COLOR_COLORLESS] > 0 && raw_mana_available[player][color] > 0)
          {
            spend_mana_for_cost(mana_charge,
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

// FUNCTION: MAGIC 0x00435db0
// FUNCTION: SHANDALAR 0x0041728a
int refund_paid_mana(int *mana_paid_by_color)
{
  int color;

  if (mana_payment_record_depth > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      recorded_mana_payments[mana_payment_record_depth - 1][color] -= mana_paid_by_color[color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
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

  s.old_x_value = x_value;
  x_value = 0;
  s.old_max_x_value = max_x_value;
  max_x_value = -1;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    s.saved_mana_cost[s.color] = mana_charge[s.color];
    mana_charge[s.color] = 0;
  }
  push_card_onto_stack(player, card, 0x72, player, 0);
  unk_00938e2c = get_required_mana_color_mask(s.saved_mana_cost);
  produced_mana_color = -1;
  original_state = PLAYER_CARD_INSTANCE(player, card).state;
  dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
  unk_00938e2c = 0;
  if (spell_fizzled == 1)
  {
    spell_fizzled = 0;
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
  max_x_value = s.old_max_x_value;
  x_value = s.old_x_value;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    mana_charge[s.color] = s.saved_mana_cost[s.color];
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00434af9
// FUNCTION: SHANDALAR 0x00415fd7
int activate_mana_sources_for_payment(int player,
                                      int *mana_paid_by_color,
                                      int *total_mana_paid,
                                      unsigned char activation_flags,
                                      int include_special_mana)
{
  struct
  {
    int current_card;
    int color;
    int dual_mana_slot;
    int max_colorless;
    int can_activate;
    unsigned char produced_color;
    unsigned char special_color;
    int any_variable_costs;
  } s;
  int result;

  s.any_variable_costs = 0;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    if (mana_charge[s.color] == -1)
    {
      s.any_variable_costs = 1;
    }
  }

  if (player == other_player && (g_duel_network_flags & 2) != 0)
  {
    result = 0;
    while (result != -1)
    {
      TENTATIVE_wait_for_network_result(player, 0x10);
      result = g_network_result_value;
      if (g_network_result_value != -1 && activate_mana_source_for_payment(player, g_network_result_value) != 0)
      {
        s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
        s.current_card = has_mana(player, COLOR_ANY, 1);
        s.max_colorless -= s.current_card;
        auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
        auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
        auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
      }
    }
    copy_mana_pool_to_display();
  }
  else
  {
    result = is_mana_cost_paid(mana_charge, x_value, max_x_value);
    if (result == 0)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = can_activate_mana_source(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          for (s.color = 0; s.color < 7; ++s.color)
          {
            if (mana_charge[s.color] > 0 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
            else if (mana_charge[s.color] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
          }
          if (include_special_mana != 0 && !s.can_activate)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && mana_color_conversions[player][s.dual_mana_slot] != -1)
            {
              s.produced_color = (unsigned char)mana_color_conversions[player][s.dual_mana_slot];
              if (mana_charge[mana_color_conversions[player][s.dual_mana_slot] >> 16] > 0 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              else if (mana_charge[mana_color_conversions[player][s.dual_mana_slot] >> 16] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
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
              g_network_result_packet_type = 0x10;
              g_network_result_value = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = is_mana_cost_paid(mana_charge, x_value, max_x_value);
    if (result == 0 && (mana_charge[0] != 0 || mana_charge[6] != 0))
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = can_activate_mana_source(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          if (mana_charge[0] < 1)
          {
            if (mana_charge[0] == -1 && x_value < max_x_value && max_x_value != -1)
            {
              s.can_activate = 1;
            }
            else if (mana_charge[6] < 1)
            {
              if (mana_charge[6] == -1 && x_value < max_x_value && max_x_value != -1)
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
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && mana_charge[6] == 0)
          {
            s.can_activate = 0;
          }
          if (s.can_activate)
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              g_network_result_packet_type = 0x10;
              g_network_result_value = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = is_mana_cost_paid(mana_charge, x_value, max_x_value);
    if (result == 0 && s.any_variable_costs && max_x_value == -1)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = can_activate_mana_source(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.any_variable_costs = 0;
          for (s.color = 1; s.color < 7; ++s.color)
          {
            if (max_x_value == -1 && mana_charge[s.color] == -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && mana_charge[0] == -1)
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && mana_charge[6] == -1)
            {
              s.any_variable_costs = 1;
            }
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && mana_charge[6] != -1)
          {
            s.any_variable_costs = 0;
          }
          if (include_special_mana != 0 && !s.any_variable_costs)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && mana_color_conversions[player][s.dual_mana_slot] != -1)
            {
              s.special_color = (unsigned char)mana_color_conversions[player][s.dual_mana_slot];
              if (max_x_value == -1 && mana_charge[mana_color_conversions[player][s.dual_mana_slot] >> 16] == -1 && (((unsigned int)(1 << s.special_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.any_variable_costs = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.any_variable_costs)
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              g_network_result_packet_type = 0x10;
              g_network_result_value = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (activate_mana_source_for_payment(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              auto_pay_colored_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              auto_pay_variable_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              auto_pay_generic_mana(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_packet_type = 0x10;
      g_network_result_value = -1;
      TENTATIVE_send_network_result(player, 0x10);
    }
    copy_mana_pool_to_display();
  }

  return 1;
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
    int selected_player;              /* ebp - 0x108 */
    int selected_card;                /* ebp - 0x104 */
    int action;                       /* ebp - 0x100 (get_current_stack_action() result) */
    int current_card;                 /* ebp - 0xfc */
    int current_player;               /* ebp - 0xf8 */
    int selected_internal_id;         /* ebp - 0xf4 */
    int candidate_players[60];        /* ebp - 0xf0 */
  } s;

  if (spell_fizzled == 1 || (g_duel_ai_mode_state == 1 && active_player == player))
  {
    return -1;
  }

  if ((ai_search_flags & 1) != 0)
  {
    player_to_check = arg_2;
  }

  if ((player == other_player && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1 || g_duel_network_state != 0)
  {
    s.candidate_count = 0;

    for (s.current_player = 0; s.current_player < 2; s.current_player++)
    {
      if (player_to_check != -1 && player_to_check != s.current_player)
      {
        continue;
      }

      for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; s.current_card++)
      {
        if (required_type == (unsigned int)-2 || PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id == -1)
        {
          continue;
        }

        if (((PLAYER_CARD_INSTANCE(s.current_player, s.current_card).state & 0x800002) != 2) &&
            !(active_player == player && g_duel_network_state != 0))
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

    if (active_player == player)
    {
    select_card_for_action_retry:
      s.candidate_count = internal_rand(s.candidate_count);
      unk_00742fcc = s.candidate_players[s.candidate_count];

      if (g_duel_network_state != 0)
      {
        g_target_selection_status_code = 0;
        if (internal_rand(0x20) == 0 || _DAT_00742fbc != 0)
        {
          g_target_selection_status_code = (int)0xfffffffeU;
          stop_phase = -1;
          stop_phase_player = stop_phase;
          return -1;
        }

        if (active_player != unk_00742fcc)
        {
          goto select_card_for_action_retry;
        }

        s.selected_internal_id = PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_cards[s.candidate_count]).internal_card_id;
        if (((global_cards_data[s.selected_internal_id].type & 1) != 0) &&
            ((PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_cards[s.candidate_count]).state & 2) != 0))
        {
          goto select_card_for_action_retry;
        }
        if (((global_cards_data[s.selected_internal_id].type & 2) != 0) &&
            ((PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_cards[s.candidate_count]).state & 4) != 0))
        {
          goto select_card_for_action_retry;
        }

        if (current_phase > 0x14 && current_phase < 0x1e)
        {
          if ((global_cards_data[s.selected_internal_id].type & 2) == 0 || (PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_cards[s.candidate_count]).state & 2) == 0)
          {
            goto select_card_for_action_retry;
          }
          goto select_card_for_action_return_candidate;
        }

        if ((global_cards_data[s.selected_internal_id].type & 0x4b) == 0 || (PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_cards[s.candidate_count]).state & 2) != 0)
        {
          goto select_card_for_action_retry;
        }
      }

    select_card_for_action_return_candidate:
      return s.candidate_cards[s.candidate_count];
    }

    if (g_duel_ai_mode_state == 1)
    {
      ai_recorded_choice = internal_rand(s.candidate_count);
      ai_recorded_action = ((((s.candidate_players[ai_recorded_choice] == 0) - 1) & 0x100) |
                            (s.candidate_cards[ai_recorded_choice] & 0xff) |
                            0x4000);
      record_ai_action_selection();
    }
    else
    {
      replay_ai_action_selection();
      if (ai_recorded_choice == 99)
      {
        ai_recorded_choice = internal_rand(s.candidate_count);
      }
    }

    unk_00742fcc = s.candidate_players[ai_recorded_choice];
    return s.candidate_cards[ai_recorded_choice];
  }

  if (required_type != 0 && required_type != 0xff)
  {
    strcpy(g_ui_message_buffer, "");
    s.action = get_current_stack_action();
    if (s.action != -1)
    {
      s.action_msg = ((unsigned int)s.action >> 0x10) & 0xff;
      s.stack_player = global_stack_cards[stack_size - 1].player;
      s.stack_card = global_stack_cards[stack_size - 1].card;
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
                             &s.selected_player,
                             0,
                             0);

  strcpy(text_lines[0], "");
  if (s.selected_player != -1)
  {
    ai_action_replay_available = 0;
  }

  unk_00742fcc = s.selected_player;
  return s.selected_card;
}

// FUNCTION: MAGIC 0x004e4ff3
// FUNCTION: SHANDALAR 0x004c4263
int load_recorded_action_target(int a1)
{
  if (g_duel_ai_mode_state != 1)
  {
    ai_recorded_action = unk_006a1db8[a1 + recorded_action_count];
    if (ai_recorded_action != -1)
    {
      ai_recorded_action &= 0xfff;
    }
  }
  return 0;
}

// FUNCTION: MOK 0x00499010
// FUNCTION: MAGIC 0x004e503e
// FUNCTION: SHANDALAR 0x004c42ae
int load_recorded_action_code(int a1)
{
  if (g_duel_ai_mode_state != 1)
  {
    unk_00715fa8 = recorded_action_codes[a1 + recorded_action_count];
    if (unk_00715fa8 == 99)
    {
      unk_00715fa8 = 0;
    }
  }
  return 0;
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
    if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
    {
      return has_mana(
          player, 7, unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)]);
    }

    return 1;
  }
  else
  {
    result = has_mana(player, color, amount);
    if (result != 0)
    {
      if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
      {
        return has_mana(
            player,
            7,
            unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] + amount);
      }
      else
      {
        return result;
      }
    }
  }

  return 0;
}

// FUNCTION: MOK 0x004a09a0
// FUNCTION: MAGIC 0x004ef850
// FUNCTION: SHANDALAR 0x004a9910
int create_legacy_effect(int player, int card, int legacy_iid, int target_player, int target_card)
{
  /* Group locals to force the original /Od stack slots: i @ -0xc, legacy_card @ -8, source_internal_card_id @ -4. */
  struct
  {
    int i;
    int legacy_card;
    int source_internal_card_id;
  } s;

  s.legacy_card = add_card_to_hand(player, legacy_iid);
  if (s.legacy_card != -1 && card != -1)
  {
    PLAYER_CARD_INSTANCE(player, s.legacy_card).state = ((((unsigned int)player < 1U) - 1) & 0x1000) | 2;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).mana_color = PLAYER_CARD_INSTANCE(player, card).mana_color;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).color = PLAYER_CARD_INSTANCE(player, card).color;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_card = card;

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 || PLAYER_CARD_INSTANCE(player, card).internal_card_id == stack_proxy_internal_card_id)
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    }
    else
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, s.legacy_card).display_pic_csv_id =
        ((unsigned int)get_card_display_pic_num(global_cards_data[s.source_internal_card_id].id, player, card) << 16) |
        (unsigned int)global_cards_data[s.source_internal_card_id].id;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card = target_card;

    if (target_player != -1 && target_card != -1)
    {
      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status |= 0x0f000000;
    }

    PLAYER_CARD_INSTANCE(player, s.legacy_card).token_status |= PLAYER_CARD_INSTANCE(player, card).token_status & 6;
    for (s.i = 0; s.i < 6; ++s.i)
    {
      PLAYER_CARD_INSTANCE(player, s.legacy_card).color_id[s.i] = PLAYER_CARD_INSTANCE(player, card).color_id[s.i];
      PLAYER_CARD_INSTANCE(player, s.legacy_card).hack_mode[s.i] = PLAYER_CARD_INSTANCE(player, card).hack_mode[s.i];
    }
  }

  return s.legacy_card;
}

// FUNCTION: MAGIC 0x0043ece1
// FUNCTION: SHANDALAR 0x0040a561
int resolve_cast_card(int player, int card)
{
  int force_special_mode;

  force_special_mode = 0;
  if (current_action_event_code == 0xd3)
  {
    DAT_00791418 = 1;
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
          DAT_00791418 = 0;
        }
        return 1;
      }
    }
  }

  if (force_special_mode != 0)
  {
    DAT_00791418 = 0;
  }
  return 0;
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
  --hand_count[player];
  return 0;
}

// FUNCTION: MAGIC 0x00446c95
// FUNCTION: SHANDALAR 0x00450813
void request_duel_display_refresh(int player, int card, int unk1, int unk2)
{
  (void)player;
  (void)card;
  (void)unk1;
  (void)unk2;
  SendMessageA((HWND)g_duel_window_hwnd, 0x464, 0xff, 0);
}

// FUNCTION: MAGIC 0x004a61a1
// FUNCTION: SHANDALAR 0x00558321
void request_duel_display_refresh_if_human(int player, int card, int unk1, int unk2)
{
  if (g_duel_ai_mode_state == 1)
    return;

  request_duel_display_refresh(player, card, unk1, unk2);
}

// FUNCTION: MAGIC 0x004b14f5
// FUNCTION: SHANDALAR 0x0046e4d0
void move_card_to_graveyard(int player, int card)
{
  int graveyard_index;
  int original_internal_card_id;
  unsigned int graveyard_player;

  original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  graveyard_player = (PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0;
  unk_007a7c58[graveyard_player] |= global_cards_data[original_internal_card_id].type;

  graveyard_index = 0;
  while (graveyard_index < 500)
  {
    if (global_graveyard_slots[graveyard_player][graveyard_index] == -1)
    {
      break;
    }
    ++graveyard_index;
  }

  if (graveyard_index < 500)
  {
    global_graveyard_slots[graveyard_player][graveyard_index] = original_internal_card_id;
  }
}

// FUNCTION: MAGIC 0x004b5cf5
// FUNCTION: SHANDALAR 0x0056c4f7
void put_card_on_bottom_of_library(int player, int internal_card_id)
{
  int deck_position;

  deck_position = 0;
  while (deck_position < 500 && global_library[player][deck_position] != -1)
  {
    ++deck_position;
  }
  if (deck_position < 500)
  {
    global_library[player][deck_position] = internal_card_id;
  }
}

// FUNCTION: MAGIC 0x004e698f
// FUNCTION: SHANDALAR 0x004c5bf6
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask)
{
  int color;
  unsigned int bits2;
  unsigned int bits1;

  bits2 = 0;
  bits1 = bits2;

  for (color = 1; color <= 5; ++color)
  {
    if (unk_00743000[0][color] > 0)
    {
      bits1 |= 1u << ((unsigned char)color - 1);
    }
    if (basiclandtypes_controlled[0][color] > 0)
    {
      bits2 |= 1u << ((unsigned char)color - 1);
    }
  }

  if (out_landwalk_mask != NULL)
  {
    *out_landwalk_mask = bits1;
  }
  if (out_basic_land_mask != NULL)
  {
    *out_basic_land_mask = bits2;
  }
}

// FUNCTION: MAGIC 0x004827ad
// FUNCTION: SHANDALAR 0x004862c0
unsigned int get_card_color_after_hacks(int player, int card)
{
  unsigned int result;
  int base_color;

  result = 0;
  base_color = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color;
  if ((base_color & 2) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 1);
  }
  if ((base_color & 4) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 2);
  }
  if ((base_color & 8) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 3);
  }
  if ((base_color & 0x10) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 4);
  }
  if ((base_color & 0x20) != 0)
  {
    result |= 1 << (unsigned char)get_hacked_color(player, card, 5);
  }
  return result;
}

// FUNCTION: MAGIC 0x00441f16
// FUNCTION: SHANDALAR 0x0040d7b2
int C_get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  card_instance_t *instance;
  card_instance_t *test_instance;
  unsigned int current_abilities;
  unsigned int result;
  int current_color;
  int saved_event_state;
  int test_card;
  int test_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  ++unk_0093f9c0;
  current_abilities = unk_00712938;
  if (duel_active != 0)
  {
    push_affected_card_stack();
  }
  affected_card_controller = player;
  affected_card = card;
  affected_internal_card_id = instance->internal_card_id;
  affected_card_color = (int)(char)global_cards_data[affected_internal_card_id].color;
  attacking_card = new_attacking_card;

  switch (event)
  {
  case EVENT_POWER:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[affected_internal_card_id].power & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[affected_internal_card_id].power;
    }
    result += instance->counter_power;
    if ((instance->regen_status & 0x04000000) == 0)
    {
      result = (unsigned int)instance->power;
    }
    else
    {
      instance->regen_status &= 0xfbffffff;
    }
    break;

  case EVENT_TOUGHNESS:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[affected_internal_card_id].toughness & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[affected_internal_card_id].toughness;
    }
    result += instance->counter_toughness;
    if ((instance->regen_status & 0x02000000) == 0)
    {
      result = (unsigned int)instance->toughness;
    }
    else
    {
      instance->regen_status &= 0xfdffffff;
    }
    break;

  case EVENT_ABILITIES:
    result = global_cards_data[affected_internal_card_id].static_ability | (instance->regen_status & 0x07000000);
    if ((result & 0x1ff81f) != 0)
    {
      current_color = 0;
      for (test_card = 0; test_card < 5; ++test_card)
      {
        if ((result & (1 << test_card)) != 0)
        {
          current_color |= 1 << ((char)get_hacked_color(player, card, test_card + 1) - 1);
        }
        if ((result & (0x800 << test_card)) != 0)
        {
          current_color |= 0x800 << ((char)get_sleighted_color(player, card, test_card + 1) - 1);
        }
      }
      result &= 0xffe007e0;
      result |= current_color;
    }
    if ((instance->regen_status & 0x08000000) == 0)
    {
      result = instance->regen_status;
    }
    else
    {
      instance->regen_status &= 0xf7ffffff;
    }
    break;

  case EVENT_RECALC_DAMAGE:
    result = (unsigned int)(short)instance->damage_on_card;
    break;

  case EVENT_SET_COLOR:
    result = (unsigned int)(char)global_cards_data[affected_internal_card_id].color;
    break;

  case EVENT_CHANGE_TYPE:
    if (((instance->internal_card_id < damage_card_internal_card_id) || (damage_card_internal_card_id + 0x2d <= instance->internal_card_id)) && instance->internal_card_id != -1)
    {
      result = instance->original_internal_card_id;
      if ((instance->regen_status & 0x01000000) != 0)
      {
        instance->internal_card_id = instance->original_internal_card_id;
        instance->regen_status &= 0xfeffffff;
        instance->mana_color = global_cards_data[instance->original_internal_card_id].color;
        instance->destroys_if_blocked = 0;
      }
      else
      {
        result = instance->internal_card_id;
      }
    }
    else
    {
      result = instance->internal_card_id;
    }
    break;

  default:
    result = 0;
    break;
  }

  event_result = result;
  if (duel_active != 0)
  {
    C_dispatch_event_raw(event);
    if (((land_can_be_played & 0x10000) != 0) && (event == EVENT_CHANGE_TYPE))
    {
      land_can_be_played &= ~0x10000;
      instance->internal_card_id = event_result;
      land_can_be_played |= 0x20000;
      C_dispatch_event_raw(event);
      land_can_be_played &= ~0x20000;
    }
  }
  if (event == EVENT_POWER)
  {
    if ((int)event_result < 0)
    {
      event_result = 0;
    }
    if ((instance->token_status & 0x4000) != 0)
    {
      event_result <<= 1;
    }
  }

  result = event_result;
  if (is_in_play(player, card) && event == EVENT_TOUGHNESS && (global_cards_data[affected_internal_card_id].type & TYPE_CREATURE) != 0 && ((int)result <= 0 || (int)result <= (int)(short)instance->damage_on_card) && ((*((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x1b) & 4) == 0) && trigger_condition == -1 && (land_can_be_played & 0x204) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
    regenerate_or_graveyard_triggers();
  }

  if (duel_active != 0)
  {
    pop_affected_card_stack();
  }
  if (event == EVENT_POWER)
  {
    instance->power = (short)result;
  }
  if (event == EVENT_TOUGHNESS)
  {
    instance->toughness = (short)result;
  }
  if (event == EVENT_ABILITIES)
  {
    instance->regen_status = result;
  }
  if (event != EVENT_CHANGE_TYPE)
  {
    unk_00712938 = current_abilities;
    return result;
  }

  instance->internal_card_id = result;
  if ((global_cards_data[result].extra_ability & 0x1000) != 0)
  {
    switch (global_cards_data[result].id)
    {
    case 0x13e:
      break;

    case 0x366:
      instance->mana_color = (unsigned char)instance->info_slot;
      break;

    case 0x12c:
    case 0x272:
      instance->mana_color = 1;
      break;

    case 0x139:
      instance->mana_color = 2;
      break;

    case 0x134:
    case 0x27b:
      instance->mana_color = 0x10;
      break;

    case 0xf:
    case 0x193:
      instance->mana_color = 0x3e;
      break;

    default:
      if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
      {
        instance->mana_color = (unsigned char)get_card_color_after_hacks(player, card);
      }
      else
      {
        instance->mana_color = global_cards_data[result].color;
      }
      break;
    }
  }

  if ((instance->token_status & 0x40) != 0 && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)
  {
    for (test_player = 0; test_player < 2; ++test_player)
    {
      test_instance = global_card_instances[test_player];
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
      {
        if (test_instance->internal_card_id != -1 && (test_instance->state & 0x800002) == 2 && test_instance->damage_target_player == player && test_instance->damage_target_card == card && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && global_raw_cards_storage[global_cards_data[test_instance->internal_card_id].id].subtype == 0x2c)
        {
          kill_card(test_player, test_card, KILL_SACRIFICE);
        }
        ++test_instance;
      }
    }
  }

  if ((instance->state & 2) != 0)
  {
    battlefield_extra_ability_flags |= global_cards_data[instance->internal_card_id].extra_ability & 0x1ffc0000;
  }

  unk_00712938 = current_abilities;
  return result;
}

// FUNCTION: MAGIC 0x00500a13
// FUNCTION: SHANDALAR 0x004bab38
void damage_player(int target_player, int amount, int source_player, int source_card)
{
  damage_creature(target_player, -1, amount, source_player, source_card);
}

// FUNCTION: MOK 0x004302c0
// FUNCTION: MAGIC 0x00435e27
// FUNCTION: SHANDALAR 0x00417301
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount)
{
  int result;

  mana_charge[0] += unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)];
  result = charge_mana(player, color, amount) - unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)];
  if (spell_fizzled == 1)
  {
    return 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0044331f
// FUNCTION: SHANDALAR 0x0040ebb9
int play_sound_effect(wav_t sound_id)
{
  struct
  {
    char path[264];
    int replacement_result;
    int sound_num;
    Sound sound;
  } s;

  s.sound.volume = 300;
  s.sound.sampleRate = 0;
  s.sound.pan = 0;
  s.sound.field_C = 0;
  s.sound.field_10 = 0;
  s.sound.field_14 = 0;
  s.sound.loadId = sound_id;
  s.sound.flags = 0;

  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  s.sound_num = sound_id;
  if (s.sound_num <= 0x13)
  {
    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x26)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x14, 0x16);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x2d)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x27, 0x27);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 1]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x3b)
  {
    s.sound.volume = 400;
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      if (sound_id != WAV_CATATAP)
      {
        s.sound.flags |= 4U;
      }
      else
      {
        s.sound.field_14 = -1;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 2]);
      sound_load(s.path, s.sound_num, &s.sound);
      sound_play(s.sound_num, &s.sound);
    }
    else
    {
      if (sound_id != WAV_CATATAP)
      {
        s.sound.flags |= 4U;
      }
      else
      {
        s.sound.field_14 = -1;
      }

      sound_play(s.sound_num, &s.sound);
    }
  }
  else if (s.sound_num <= 0x40)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x3c, 0x40);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 2]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else if (s.sound_num <= 0x44)
  {
    if (sound_is_loaded(sound_id, &s.sound_num) == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x41, 0x44);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return 0;
      }

      strcpy(s.path, global_duelsounds_path);
      strcat(s.path, "\\");
      strcat(s.path, g_duel_sound_filenames[sound_id + 3]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
  else
  {
    return 0;
  }

  return 1;
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

  saved_affected_card = affected_card;
  saved_affected_card_controller = affected_card_controller;
  saved_spell_fizzled = spell_fizzled;
  saved_event_result = event_result;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if (((global_cards_data[internal_card_id].subtype == 0) && ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x800) == 0)) ||
      (((global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x01000000) == 0)) ||
      ((PLAYER_CARD_INSTANCE(player, card).state & 0x810010) != 0) || ((PLAYER_CARD_INSTANCE(player, card).token_status & 0x8000) != 0))
  {
    result = 0;
    event_result = saved_event_result;
    spell_fizzled = saved_spell_fizzled;
    affected_card_controller = saved_affected_card_controller;
    affected_card = saved_affected_card;
  }
  else
  {
    event_result = 0;
    affected_card_controller = player;
    affected_card = card;
    global_cards_data[internal_card_id].code_pointer(player, card, 0x79);
    if (event_result == 0)
    {
      event_result = saved_event_result;
      spell_fizzled = saved_spell_fizzled;
      affected_card_controller = saved_affected_card_controller;
      affected_card = saved_affected_card;

      if ((player == other_player) && ((g_duel_network_flags & 2) == 0))
      {
        push_affected_card_stack();
        event_result = 0;
        affected_card_controller = player;
        affected_card = card;
        dispatch_three_arg_callback_to_cards_in_play(check_attached_aura_can_pay_cost, -1);
        result = event_result;
        pop_affected_card_stack();
        if (result != 0)
        {
          return 0;
        }
      }

      if ((((unk_007a79b0[1 - player] & 1) != 0) || ((battlefield_extra_ability_flags & 0x04000000) != 0)) &&
          ((result = dispatch_event(player, card, 0x79)) != 0))
      {
        return 0;
      }

      result = 1;
    }
    else
    {
      result = 0;
      event_result = saved_event_result;
      spell_fizzled = saved_spell_fizzled;
      affected_card_controller = saved_affected_card_controller;
      affected_card = saved_affected_card;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x005001c4
// FUNCTION: SHANDALAR 0x004ba2ed
void invalidate_dynamic_card_type(int internal_card_id)
{
  global_cards_data[internal_card_id].id = -1;
}

// FUNCTION: MAGIC 0x00483190
int find_matching_active_control_effect(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int result = 0;

  if (global_cards_data[internal_card_id].id ==
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id &&
      ((PLAYER_CARD_INSTANCE(source_player, source_card).token_status & 0x01000000) != 0))
  {
    event_result = (source_player << 16) | source_card;
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x00483242
int find_matching_inactive_control_effect(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int result = 0;

  if (global_cards_data[internal_card_id].id ==
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id &&
      ((PLAYER_CARD_INSTANCE(source_player, source_card).token_status & 0x01000000) == 0))
  {
    event_result = (source_player << 16) | source_card;
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x004b413c
int select_damage_card_from_list(int player,
                                 int *internal_card_ids,
                                 int *damage_amounts,
                                 int count,
                                 int title,
                                 int allow_cancel,
                                 char *prompt)
{
  int selected;

  selected = -1;

  if (player == other_player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    selected = g_network_result_value;
  }

  if (player == active_player && g_duel_ai_mode_state != 1)
  {
    selected = (int)show_damage_assignment_cardlist(internal_card_ids, damage_amounts, count, title, allow_cancel, prompt);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_network_result_value = selected;
      g_network_result_packet_type = 0x19;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return selected;
}

// FUNCTION: MAGIC 0x0055d802
// FUNCTION: SHANDALAR 0x00573b1c
void extract_numbered_text_choice(char *out, char *in, int choice)
{
  struct
  {
    char *write_ptr;
    int done;
    int matched;
    char *scan;
  } s;

  if (out == NULL || in == NULL)
  {
    return;
  }

  s.done = 0;
  s.matched = 0;
  s.scan = in;
  while (s.done == 0)
  {
    while (*s.scan != '\0' && strncmp(s.scan, unk_0057f758, 2) != 0)
    {
      ++s.scan;
    }

    if (*s.scan == '\0')
    {
      s.done = 1;
    }
    else
    {
      ++s.scan;
      ++s.scan;
      if (atoi(s.scan) == choice)
      {
        s.matched = 1;
        ++s.scan;
        s.write_ptr = out;
        while (*s.scan != '\0' && strncmp(s.scan, unk_0057f75c, 2) != 0)
        {
          *s.write_ptr = *s.scan;
          ++s.scan;
          ++s.write_ptr;
        }
        *s.write_ptr = '\0';
        s.done = 1;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0055dc4c
void *copy_bytes(void *dest, const void *src, unsigned int count)
{
  return memcpy(dest, src, count);
}

// FUNCTION: MAGIC 0x00495311
// FUNCTION: SHANDALAR 0x00465dac
char *get_hunting_subtype_name(int value)
{
  if (value < 0 || value >= 0xd4)
  {
    return "";
  }
  else
    return gs_hunting_subtype_name_00926930[value];
}

// FUNCTION: MAGIC 0x004a61fe
int show_cardlist_if_human(int *cards, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(cards, 0, 0, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x004ec616
// FUNCTION: SHANDALAR 0x0046a315
void position_duel_prompt_context_window(HWND prompt_hwnd)
{
  struct
  {
    RECT rect1; // ebp-0x20
    RECT rect2; // ebp-0x10
  } s;

  if (IsWindowVisible((HWND)g_duel_phase_display_window_hwnd))
  {
    GetWindowRect((HWND)g_duel_phase_display_window_hwnd, &s.rect1);
  }
  else if (IsWindowVisible((HWND)g_duel_attack_phase_window_hwnd))
  {
    GetWindowRect(DAT_0094ca30, &s.rect1);
  }
  else
  {
    GetWindowRect(g_duel_help_owner_hwnd, &s.rect1);
    GetWindowRect(prompt_hwnd, &s.rect2);
    s.rect1.bottom -= (s.rect2.bottom - s.rect2.top) / 2;
  }

  SetWindowPos(prompt_hwnd, (HWND)0, s.rect1.left, s.rect1.bottom, 0, 0, 5);
}

// FUNCTION: MAGIC 0x004ec6c3
// FUNCTION: SHANDALAR 0x0046a3c1
void setup_duel_prompt_context_text_dc(HWND prompt_hwnd, HDC dc, int *rect_values)
{
  struct
  {
    RECT rect;        // ebp-0x4c
    HGDIOBJ wnd_font; // ebp-0x3c
    TEXTMETRICA tm;   // ebp-0x38
  } s;

  s.wnd_font = (HGDIOBJ)GetWindowLongA(prompt_hwnd, DAT_0055e17c);
  if (s.wnd_font != (HGDIOBJ)0)
  {
    SelectObject(dc, s.wnd_font);
  }

  GetTextMetricsA(dc, &s.tm);

  *rect_values += s.tm.tmHeight / 2;
  rect_values[1] += 4;
  rect_values[3] -= 3;

  if (IsWindowVisible(DAT_00743090))
  {
    GetWindowRect(DAT_00743090, &s.rect);
    MapWindowPoints((HWND)0, prompt_hwnd, (LPPOINT)&s.rect, 2);
    *rect_values = max(s.rect.right + s.tm.tmHeight / 2, *rect_values);
  }

  if (IsWindowVisible(DAT_0074308c))
  {
    GetWindowRect(DAT_0074308c, &s.rect);
    MapWindowPoints((HWND)0, prompt_hwnd, (LPPOINT)&s.rect, 2);
    *rect_values = max(s.rect.right + s.tm.tmHeight / 2, *rect_values);
  }

  SetMapMode(dc, 8);
  SetWindowExtEx(dc, rect_values[2] - *rect_values, 0x14, (LPSIZE)0);
  SetViewportExtEx(dc, rect_values[2] - *rect_values, rect_values[3] - rect_values[1], (LPSIZE)0);
}

// FUNCTION: MAGIC 0x004ec223
// FUNCTION: SHANDALAR 0x00469f23
void set_duel_prompt_context(HWND prompt_hwnd, char *text, unsigned int button_flags)
{
  struct
  {
    HDC dc2;                 // ebp-0x74
    RECT rect1;              // ebp-0x70
    HDC dc;                  // ebp-0x60
    RECT rect2;              // ebp-0x5c
    SIZE text_size;          // ebp-0x4c
    int border;              // ebp-0x44
    int icon_width;          // ebp-0x40
    RECT rect3;              // ebp-0x3c
    int extra_width;         // ebp-0x2c
    int gap;                 // ebp-0x28
    int y;                   // ebp-0x24
    unsigned int text_width; // ebp-0x20
    int padding;             // ebp-0x1c
    int x;                   // ebp-0x18
    int client_h;            // ebp-0x14
    RECT rect4;              // ebp-0x10
  } s;

  s.border = 4;
  s.padding = 4;
  s.gap = 7;

  if (text == NULL || strlen(text) == 0)
  {
    ShowWindow(prompt_hwnd, 0);
  }

  if (button_flags == 0 || (button_flags & 1) == 0)
  {
    ShowWindow(DAT_00743090, 0);
  }

  if (button_flags == 0 || (button_flags & 2) == 0)
  {
    ShowWindow(DAT_0074308c, 0);
  }

  s.extra_width = 0;
  s.dc = GetDC(prompt_hwnd);
  GetClientRect(prompt_hwnd, &s.rect2);

  s.client_h = (s.rect2.bottom - s.rect2.top) - s.padding * 2;
  setup_duel_prompt_context_text_dc(prompt_hwnd, s.dc, (int *)&s.rect2.left);

  GetTextExtentPoint32A(s.dc, DAT_008ce680, strlen(DAT_008ce680), &s.text_size);
  s.icon_width = s.text_size.cx + s.client_h / 2;

  s.rect2.right = 2000;
  s.text_width = (unsigned int)(unsigned short)CalcDrawManaText(s.dc, &s.rect2, text);
  ReleaseDC(prompt_hwnd, s.dc);

  if ((button_flags & 2) != 0 || (button_flags & 1) != 0)
  {
    SetWindowPos(DAT_0074308c, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if ((button_flags & 1) != 0)
  {
    SetWindowPos(DAT_00743090, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if (text != NULL && strlen(text) != 0)
  {
    s.dc2 = GetDC(prompt_hwnd);
    GetClientRect(prompt_hwnd, &s.rect1);
    setup_duel_prompt_context_text_dc(prompt_hwnd, s.dc2, (int *)&s.rect1.left);
    s.rect1.right = 2000;
    s.text_width = (unsigned int)(unsigned short)CalcDrawManaText(s.dc2, &s.rect1, text);
    ReleaseDC(prompt_hwnd, s.dc2);
    SetWindowTextA(prompt_hwnd, text);
  }
  else
  {
    s.text_width = 0;
    // STRING: MAGIC 0x0057b054
    SetWindowTextA(prompt_hwnd, "");
  }

  GetWindowRect(prompt_hwnd, &s.rect4);
  SetWindowPos(prompt_hwnd,
               (HWND)0,
               0,
               0,
               s.border * 2 + s.extra_width + s.text_width + 0x19,
               s.rect4.bottom - s.rect4.top,
               6);

  GetClientRect(prompt_hwnd, &s.rect4);
  s.x = s.rect4.left + s.border;

  if ((button_flags & 2) != 0)
  {
    GetWindowRect(DAT_0074308c, &s.rect3);
    s.x = s.x + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(DAT_0074308c, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  if ((button_flags & 1) != 0)
  {
    GetWindowRect(DAT_00743090, &s.rect3);
    s.x = s.x + (s.rect3.right - s.rect3.left) + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(DAT_00743090, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  position_duel_prompt_context_window(prompt_hwnd);
  InvalidateRect(DAT_00743090, (RECT *)0, 1);
  InvalidateRect(DAT_0074308c, (RECT *)0, 1);
  InvalidateRect(prompt_hwnd, (RECT *)0, 1);

  if ((button_flags & 1) != 0)
  {
    ShowWindow(DAT_00743090, 5);
  }

  if ((button_flags & 2) != 0)
  {
    ShowWindow(DAT_0074308c, 5);
  }

  if (text != NULL)
  {
    if (strlen(text) != 0)
    {
      ShowWindow(prompt_hwnd, 5);
    }
  }

  restack_duel_child_windows();
  UpdateWindow(prompt_hwnd);
}

// FUNCTION: MAGIC 0x004480a6
// FUNCTION: SHANDALAR 0x00451c24
void display_duel_prompt_text(char *text)
{
  char *msg;

  if (text != NULL)
  {
    msg = text;
  }
  else
  {
    msg = empty_duel_prompt_text;
  }

  set_duel_prompt_context(g_duel_prompt_context_hwnd, msg, 0);
}

// FUNCTION: MAGIC 0x004a61d6
// FUNCTION: SHANDALAR 0x00558356
void set_duel_prompt_text(char *text)
{
  strcpy(current_duel_prompt_text, text);
  display_duel_prompt_text(text);
}

// FUNCTION: MAGIC 0x00482a97
// FUNCTION: SHANDALAR 0x004865a8
int has_effect_source_type(int player, int card, unsigned int flags)
{
  struct
  {
    card_instance_t *instance;
    int source_internal_card_id;
    int found;
    int test_card;
    int source_player;
    int source_card;
    int test_player;
  } s;

  if (player == -1 || flags == 0)
  {
    return 0;
  }

  s.found = 0;
  for (s.test_player = 0; s.test_player < 2 && s.found == 0; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player] && s.found == 0; ++s.test_card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (s.instance->internal_card_id == damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (int)(char)s.instance->damage_target_player == player &&
          s.instance->damage_target_card == card &&
          s.instance->info_slot != 0)
      {
        s.source_player = (int)(char)s.instance->damage_source_player;
        s.source_card = s.instance->damage_source_card;

        if (PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id != -1 &&
            global_cards_data[PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id].id == unk_008a8de8)
        {
          s.source_player =
              PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(s.source_player, s.source_card).damage_source_player,
                                   s.source_card)
                  .info_slot;
        }

        if (PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id != -1)
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id;
        }
        else
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).original_internal_card_id;
        }

        if ((flags & (unsigned char)global_cards_data[s.source_internal_card_id].type) != 0)
        {
          s.found = 1;
        }
      }
    }
  }

  return s.found;
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
                           text_lines[0],
                           1,
                           &selected_target) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).number_of_targets] = selected_target;
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

  if (active_player == who_chooses || (g_duel_network_flags & 2) != 0)
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

  s.player_index = 0;
  while (s.player_index < 2)
  {
    if (!s.done)
    {
      for (s.current_card = 0;
           s.current_card < (active_cards_count[1] > active_cards_count[0] ? active_cards_count[1]
                                                                           : active_cards_count[0]);
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
            if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == damage_card_internal_card_id)
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
          else if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == damage_card_internal_card_id)
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

    ++s.player_index;
    s.scan_player = 1 - s.scan_player;
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
      if ((((active_player == who_chooses) || ((g_duel_network_flags & 2) != 0)) && ((allowed_controller & ANYBODY) != 0)) || (who_chooses == AI && ((preferred_controller & ANYBODY) != 0)))
      {
        s.player_allowed_human = AI;
        s.player_allowed_ai = 1;
      }
      else if ((((active_player == who_chooses) || ((g_duel_network_flags & 2) != 0)) && ((allowed_controller & AI) != 0)) || (who_chooses == AI && ((preferred_controller & AI) != 0)))
      {
        s.player_allowed_human = HUMAN;
        s.player_allowed_ai = 1;
      }
      else
      {
        if ((active_player == who_chooses) || ((g_duel_network_flags & 2) != 0))
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
    if ((active_player == who_chooses) || ((g_duel_network_flags & 2) != 0))
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
      if ((required_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.type_iid].id == unk_00789734 || global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id == unk_00789b80))
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_007a7d64)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_008a8de8)
      {
        s.type_match = 1;
      }
      if ((required_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_009266ac)
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
      if ((illegal_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.illegal_type_iid].id == unk_00789734 || global_cards_data[s.illegal_type_iid].id == unk_00789b80))
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_007a7d64)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_008a8de8)
      {
        s.illegal_type_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_009266ac)
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
      if ((special & TARGET_SPECIAL_SPELL_ON_STACK) != 0 && ((current_spell_player == -1 || tgt_player != current_spell_player) || tgt_card != current_spell_card || (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) != 0))
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
      if ((required_state & TARGET_STATE_BLOCKING) != 0 && (tgt_player == current_player || (int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking == -1))
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

        if (current_phase > 0x14 && current_phase < 0x1e && tgt_player != current_player)
        {
          if (tgt_player == current_player || (int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking == -1)
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
          for (s.tmp114 = 0; s.tmp114 < active_cards_count[s.unused_110]; ++s.tmp114)
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
        if ((int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).blocking != -1 && tgt_player != current_player)
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
          for (s.tmp120 = 0; s.tmp120 < active_cards_count[s.unused_11c]; ++s.tmp120)
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

// FUNCTION: MAGIC 0x004e51bb
// FUNCTION: SHANDALAR 0x004c442a
void rewind_recorded_action(void)
{
  if (recorded_action_count > 0)
  {
    --recorded_action_count;
  }
  else
  {
    recorded_action_count = 0;
  }
}

// FUNCTION: MAGIC 0x00551240
// FUNCTION: SHANDALAR 0x0049f483
void add_special_counter(int player, int card)
{
  if ((PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffu) == 0xffu)
  {
    return;
  }

  /* The exe increments only the low byte and preserves the upper 3 bytes. */
  PLAYER_CARD_INSTANCE(player, card).special_counters =
      ((PLAYER_CARD_INSTANCE(player, card).special_counters + 1) & 0xff) |
      (PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffffff00);

  if (g_duel_ai_mode_state != 1)
  {
    play_sound_effect(ICON_HASTE);
  }
}

// FUNCTION: MAGIC 0x005514cd
// FUNCTION: SHANDALAR 0x0049f710
void remove_special_counters(int player, int card, int amount)
{
  global_card_instances[player][card].special_counters =
      (unsigned char)(global_card_instances[player][card].special_counters - amount);
}

// FUNCTION: MAGIC 0x00551572
// FUNCTION: SHANDALAR 0x0049f7b5
void set_special_counters(int player, int card, int amount)
{
  if (amount > 0xff)
  {
    amount = 0xff;
  }

  global_card_instances[player][card].special_counters =
      (global_card_instances[player][card].special_counters & 0xffffff00) | (unsigned char)amount;
}

// FUNCTION: MAGIC 0x005515f5
// FUNCTION: SHANDALAR 0x0049f838
unsigned int C_get_special_counters(int player, int card)
{
  return global_card_instances[player][card].special_counters & 0xff;
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
#define BLOCKER_INSTANCE (*(card_instance_t *)((char *)&global_card_instances[0][blocker_card] + blocker_player * sizeof(global_card_instances[0])))
  struct
  {
    int result;
    int internal_card_id;
  } s;

  s.internal_card_id = BLOCKER_INSTANCE.internal_card_id;

  if ((((unsigned char)global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0) &&
      ((BLOCKER_INSTANCE.state & 0x02000000) == 0))
  {
    return 0;
  }

  if (s.internal_card_id == -1 || (BLOCKER_INSTANCE.state & 0x800018) != 0)
  {
    return 0;
  }

  if (((BLOCKER_INSTANCE.state & 4) != 0) && has_vigilance(blocker_player, blocker_card) == 0)
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
  affected_card_controller = blocker_player;
  affected_card = blocker_card;
  attacking_card_controller = attacker_player;
  attacking_card = attacker_card;
  event_result = 0;
  C_dispatch_event_raw(0x78);
  if (event_result > 0)
  {
    s.result = 0;
  }
  pop_affected_card_stack();

  return s.result;
#undef BLOCKER_INSTANCE
}

// FUNCTION: MAGIC 0x00443ee2
// FUNCTION: SHANDALAR 0x0040f77c
int push_card_onto_stack(int player, int card, int event, int extra, int prompt)
{
  int should_push;
  int stack_card;
  int saved_timestamp;

  if (stack_size >= 32)
    return 0;

  stack_data[stack_size] = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  stack_data[stack_size] |= event << 16;
  stack_data[stack_size] |= extra << 24;
  if (event != EVENT_RESOLVE_SPELL && event != EVENT_RESOLVE_TRIGGER && PLAYER_CARD_INSTANCE(player, card).internal_card_id > 4)
  {
    stack_card = add_card_to_hand(player, stack_proxy_internal_card_id);
    if (stack_card != -1)
    {
      saved_timestamp = PLAYER_CARD_INSTANCE(player, stack_card).timestamp;
      memcpy(&PLAYER_CARD_INSTANCE(player, stack_card), &PLAYER_CARD_INSTANCE(player, card), sizeof(card_instance_t));
      PLAYER_CARD_INSTANCE(player, stack_card).internal_card_id = stack_proxy_internal_card_id;
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
    global_stack_cards[stack_size].player = player;
    global_stack_cards[stack_size].card = stack_card;
    global_stack_damage_targets[stack_size].player = PLAYER_CARD_INSTANCE(player, card).damage_target_player;
    global_stack_damage_targets[stack_size].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    if (trigger_condition == -1)
    {
      stack_trigger_condition[stack_size] = current_phase;
    }
    else
    {
      stack_trigger_condition[stack_size] = trigger_condition;
    }
    if (g_duel_ai_mode_state != 1)
    {
      stack_prompt_flags[stack_size] = prompt;
    }
    ++stack_size;
    global_stack_cards[stack_size].player = -1;
  }

  return 0;
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
      if (mana_color_conversions[player][s.slot] == -1)
      {
        s.found = 1;
        mana_color_conversions[player][s.slot] = MAKELONG(color_to_consume, color_to_produce);
        mana_color_conversions[player][s.slot + 1] = -1;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00551334
// FUNCTION: SHANDALAR 0x0049f577
void remove_special_counter(int player, int card)
{
  /* The exe only decrements the low byte of special_counters and preserves the upper 3 bytes. */
  global_card_instances[player][card].special_counters =
      (global_card_instances[player][card].special_counters & 0xffffff00) |
      ((global_card_instances[player][card].special_counters - 1) & 0xff);
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

  if (stack_size <= 0)
  {
    return 0;
  }

  --stack_size;
  s.player = global_stack_cards[stack_size].player;
  s.card = global_stack_cards[stack_size].card;
  s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id;
  if (stack_proxy_internal_card_id == s.current_internal)
  {
    PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id = -1;
  }
  global_stack_cards[stack_size].player = -1;

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

  if (stack_size <= 0)
  {
    return 0;
  }

  --stack_size;
  s.player = global_stack_cards[stack_size].player;
  s.card = global_stack_cards[stack_size].card;
  s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id;
  if (s.current_internal == stack_proxy_internal_card_id)
  {
    s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id;
  }
  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
  {
    if ((((unsigned int)stack_data[stack_size] >> 0x10) & 0xff) == 0x7e)
    {
      dispatch_card_trigger_event(s.player,
                                  s.card,
                                  ((unsigned int)stack_data[stack_size] >> 0x10) & 0xff,
                                  ((int)stack_data[stack_size]) >> 0x18);
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
                                        ((unsigned int)stack_data[stack_size] >> 0x10) & 0xff,
                                        1 - s.player,
                                        -1);
        }
      }
    }
    if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == stack_proxy_internal_card_id)
    {
      kill_card(s.player, s.card, KILL_REMOVE);
    }
  }
  global_stack_cards[stack_size].player = -1;
  reassess_all_cards_and_mana();
  if (nested_trigger_depth <= 1 && ((land_can_be_played & 0x200) == 0 || stack_size == 0))
  {
    process_damage_prevention(current_player);
    regenerate_or_graveyard_triggers();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004482f3
// FUNCTION: SHANDALAR 0x00451e70
int copy_mana_pool_to_display(void)
{
  if (g_duel_ai_mode_state != 1)
  {
    EnterCriticalSection(&g_duel_render_lock);
    memcpy(g_duel_cached_raw_mana_player_0, raw_mana_available[0], 0x1c);
    memcpy(g_duel_cached_raw_mana_player_1, raw_mana_available[1], 0x1c);
    LeaveCriticalSection(&g_duel_render_lock);
    SendMessageA(unk_008ce534, 0x432, 0, 0);
    SendMessageA(unk_00939344, 0x432, 0, 0);
  }

  return 0;
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
    for (card = 0; card < active_cards_count[player]; ++card)
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
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      if (is_in_play(player, card) && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) && (short)instance->damage_on_card < C_get_abilities(player, card, EVENT_TOUGHNESS, -1))
      {
        C_get_abilities(player, card, EVENT_POWER, -1);
      }
      ++instance;
    }
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

  if ((land_can_be_played & 2) == 0)
  {
    return;
  }
  land_can_be_played &= ~2;
  land_can_be_played |= 4;
  TENTATIVE_reassess_all_cards(0, 0xff);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x21);
      }
    }
  }

  s.did_setup = 0;
setup:
  if (((g_duel_network_flags & 2) == 0) && g_duel_ai_mode_state != 1 && ai_action_replay_available == 0)
  {
    start_ai_decision_search(9, 0xf);
    s.best_score = -99999;
    s.did_setup = 1;
  }

after_setup:
  if (ai_decision_code == 9 && s.did_setup)
  {
    reset_ai_search_trial_state();
    DAT_008cdab4 = 0;
    DAT_008cdab0 = DAT_008cdab4;
    DAT_008a8de4 = DAT_008cdab0;
    ai_modifier = DAT_008a8de4;
  }

  if (allow_response(-2, -1, gs_damage_prevention_00939200, 0x8e) != 0)
  {
    goto setup;
  }

  C_dispatch_event_raw(0x25);
  if ((battlefield_extra_ability_flags & 0x00040000) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_END_DAMAGE_PREV, gs_end_damage_prevention_00789740, 0);
  }

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x6e);
      }
    }
  }

  dispatch_trigger_twice_once_with_each_player_as_reason(current_player, TRIGGER_DEAL_DAMAGE, gs_damage_dealing_00777ab0, 0);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == damage_card_internal_card_id &&
          is_in_play(s.test_player, s.test_card) != 0)
      {
        if ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) != 0)
        {
          kill_card(s.test_player, s.test_card, 1);
        }
        else
        {
          land_can_be_played |= 2;
        }
      }
    }
  }

  kill_creatures_with_lethal_damage();
  land_can_be_played &= ~4;

  if (g_duel_ai_mode_state == 1 && s.did_setup != 0 && ai_decision_code == 9)
  {
    regenerate_or_graveyard_triggers();
    s.temp = ai_modifier + ai_opinion_of_gamestate(other_player);
    if (s.temp > s.best_score)
    {
      s.best_score = s.temp;
      save_recorded_ai_actions();
      s.saved_777854 = ai_search_flags;
    }
    if (ai_search_target_depth == 999)
    {
      ai_search_target_depth = -1;
    }
    ai_search_force_pass = 0;
    if (get_duel_time_units_if_available() > (game_time_scale * ai_search_time_limit) / 5)
    {
      g_duel_ai_mode_state = 0;
      ai_search_target_depth = -1;
      ai_search_flags = s.saved_777854;
    }
    land_can_be_played |= 4;
    goto after_setup;
  }

  if (g_duel_ai_mode_state != 1 && s.did_setup != 0)
  {
    ai_action_replay_available = 0;
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
    for (card = 0; card < active_cards_count[player]; ++card)
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

// FUNCTION: MAGIC 0x0044aa01
// FUNCTION: SHANDALAR 0x00403381
int should_skip_phase(int player)
{
  if ((g_duel_network_flags & 2) != 0)
  {
    if (stop_phase == -1)
    {
      if (previous_stop_phase == -1)
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
    if (previous_stop_phase == current_phase && current_player == previous_stop_phase_player)
    {
      return 0;
    }

    if (stop_phase == current_phase && current_player == stop_phase_player)
    {
      return 0;
    }

    if (previous_stop_phase == -1 && player == other_player)
    {
      if (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_OPPONENT) != 0)
      {
        return 0;
      }
    }

    if (stop_phase == -1 && player == active_player)
    {
      if (((char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase] & PHASE_STOP_ENABLED) != 0)
      {
        return 0;
      }
    }

    if (!(stop_phase == current_phase && current_player == stop_phase_player))
    {
      if (previous_stop_phase == -1)
      {
        return 1;
      }
    }

    if (!(previous_stop_phase == current_phase && current_player == previous_stop_phase_player))
    {
      if (stop_phase == -1)
      {
        return 1;
      }
    }

    if (trigger_condition != -1 && stop_phase == current_phase)
    {
      return 1;
    }

    if (trigger_condition != -1 && previous_stop_phase == current_phase)
    {
      return 1;
    }

    return 0;
  }

  if (stop_phase == -1)
  {
    return 0;
  }
  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  if (stop_phase > current_phase)
  {
    return 1;
  }
  if (current_player != stop_phase_player)
  {
    return 1;
  }

  if (trigger_condition != -1)
  {
    if (stop_phase == current_phase)
    {
      return 1;
    }
  }

  if (stop_phase < current_phase)
  {
    if (current_player == stop_phase_player)
    {
      stop_phase_player = -1;
      stop_phase = stop_phase_player;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044541f
// FUNCTION: SHANDALAR 0x00410cb8
int can_stop_for_phase(int use_current_player_stops)
{
  int can_respond;

  can_respond = 0;
  if (use_current_player_stops != 0)
  {
    if (current_player == active_player &&
        (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0)
    {
      can_respond = 1;
    }

    if (current_player == other_player &&
        (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0)
    {
      can_respond = 1;
    }

    if (current_player == active_player)
    {
      if (current_player == stop_phase_player && current_phase == stop_phase)
      {
        can_respond = 1;
      }
    }
    else if (current_player == previous_stop_phase_player && previous_stop_phase == current_phase)
    {
      can_respond = 1;
    }
    if (should_skip_phase(current_player) == 0 && can_respond != 0)
    {
      return 1;
    }
    return 0;
  }

  /* use_current_player_stops == 0 */
  if (current_player == active_player &&
      (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) & PHASE_STOP_OPPONENT) != 0)
  {
    can_respond = 1;
  }

  if (current_player == other_player &&
      (((int)(char)g_duel_phase_stop_settings[current_player].phase_flags[current_phase]) & PHASE_STOP_ENABLED) != 0)
  {
    can_respond = 1;
  }

  if (current_player == other_player)
  {
    if (current_player == stop_phase_player && current_phase == stop_phase)
    {
      can_respond = 1;
    }
  }
  else if (current_player == previous_stop_phase_player && previous_stop_phase == current_phase)
  {
    can_respond = 1;
  }

  if (should_skip_phase(1 - current_player) == 0 && can_respond != 0)
  {
    return 1;
  }
  return 0;
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
    for (card = 0; card < active_cards_count[player]; ++card)
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
  int diff = ticks - unk_0093f4b4 - g_duel_tick_adjustment;
  int result = diff / 0x37;
  return result;
}

// FUNCTION: MAGIC 0x004e1b6f
int get_duel_time_units_if_available(void)
{
  return get_duel_time_units();
}

// FUNCTION: MAGIC 0x004b082f
// FUNCTION: SHANDALAR 0x0046d80a
int dispatch_card_trigger_event(int player, int card, event_t event, int extra)
{
  if (event == EVENT_TRIGGER && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_PROCESSING) != 0 || unk_00777850 != 0))
  {
    return 0;
  }

  if (trigger_condition < 200)
  {
    return 0;
  }

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  unk_008cfd20 = extra;
  attacking_card = -1;
  C_dispatch_event_raw(event);
  return event_result;
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

  s.saved_trigger_condition = unk_00712938;
  unk_008cc844 = event;
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

      unk_00789a48 = s.player * 0x80 + s.card;
      (*global_cards_data[s.instance->internal_card_id].code_pointer)(s.player, s.card, event);
      if (event == EVENT_DECLARE_ATTACKERS && s.player == current_player && (s.instance->state & 0x14) == 4 && !has_vigilance(s.player, s.card))
      {
        s.instance->state |= 0x10;
        produced_mana_color = -1;
        dispatch_event(s.player, s.card, EVENT_TAP_CARD);
        process_damage_prevention(current_player);
        regenerate_or_graveyard_triggers();
      }
    }
  }
  if (event == EVENT_DECLARE_ATTACKERS && s.player == current_player)
  {
    reassess_all_cards_and_mana();
  }
  if (opponent_starting_card_id_2 != -1)
  {
    (*global_cards_data[opponent_starting_card_id_2].code_pointer)(0, 0x94, event);
  }
  unk_00712938 = s.saved_trigger_condition;
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
    unk_0072c440[s.current_card] = 0;
    special_mana_pool[1][s.current_card] = unk_0072c440[s.current_card];
    special_mana_pool[0][s.current_card] = special_mana_pool[1][s.current_card];
  }

  raw_mana_available_hex[1][0] = -1;
  raw_mana_available_hex[0][0] = raw_mana_available_hex[1][0];
  mana_color_conversions[1][0] = -1;
  mana_color_conversions[0][0] = mana_color_conversions[1][0];
  for (s.player = 0; s.player < 2; ++s.player)
  {
    s.instance = global_card_instances[s.player];
    for (s.current_card = 0; s.current_card < active_cards_count[s.player];
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
  if ((battlefield_extra_ability_flags & 0x08000000) != 0)
  {
    for (s.player = 0; s.player < 2; ++s.player)
    {
      s.instance = global_card_instances[s.player];
      for (s.current_card = 0; s.current_card < active_cards_count[s.player];
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
  int internal_card_id;
  int csvid;

  for (card = 0; card < 8; ++card)
  {
    unk_00743000[0][card] = 0;
    basiclandtypes_controlled[0][card] = unk_00743000[0][card];
    unk_00743000[1][card] = 0;
    basiclandtypes_controlled[1][card] = unk_00743000[1][card];
  }
  unk_00743080[0] = 0;
  unk_00743080[1] = 0;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      if (is_in_play(player, card) && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
      {
        unk_00743080[player] |= (unsigned char)PLAYER_CARD_INSTANCE(player, card).mana_color;
        if (internal_card_id < 5)
        {
          ++basiclandtypes_controlled[player][internal_card_id];
        }
        else if (internal_card_id < 0xf || g_card_count <= internal_card_id)
        {
          csvid = global_cards_data[internal_card_id].id;
          if (csvid < 0xd)
          {
            if (csvid == 0xc)
            {
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
            }
            else if (csvid == 9)
            {
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
            }
            else
            {
              ++basiclandtypes_controlled[player][0];
            }
          }
          else
          {
            switch (csvid)
            {
            case 0x5b:
              ++basiclandtypes_controlled[player][3];
              break;
            case 0x7e:
              ++basiclandtypes_controlled[player][2];
              break;
            case 0xa4:
              ++basiclandtypes_controlled[player][4];
              break;
            case 0xbc:
              ++basiclandtypes_controlled[player][5];
              break;
            case 0xbd:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xd4:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              break;
            case 0xd8:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              break;
            case 0xef:
              ++basiclandtypes_controlled[player][1];
              break;
            case 0xf1:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xfc:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0xfe:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x102:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x10a:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            default:
              ++basiclandtypes_controlled[player][0];
              break;
            }
          }
        }
        else
        {
          ++basiclandtypes_controlled[player][0];
        }
        ++basiclandtypes_controlled[player][7];
      }
    }
  }
}

// FUNCTION: MAGIC 0x00551e49
// FUNCTION: SHANDALAR 0x004a008c
int sacrifice_a_land(int player)
{
  target_t target;

  if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0,
                           COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                           text_lines[0], 0, &target))
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

  if ((player == active_player || (g_duel_network_flags & 2) != 0) && g_duel_ai_mode_state != 1)
  {
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              text_lines[0], 0, &s.target))
    {
      s.best_card = -1;
    }
    else
    {
      s.best_card = s.target.card;
    }
  }
  else
  {
    s.best_card = -1;
    s.best_value = 0x7fff;
    for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id;
      if (s.internal_card_id != -1 &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x800002) == 2 &&
          (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
          PLAYER_CARD_INSTANCE(player, s.current_card).kill_code != 3 &&
          (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x1000) == 0)
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

// FUNCTION: MAGIC 0x004823a5
// FUNCTION: SHANDALAR 0x00485eb8
int resolve_control_aura_conflict(int player, int card)
{
  struct
  {
    card_instance_t *instance; /* [ebp-0x20] */
    int target_player;         /* [ebp-0x1c] */
    int target_card;           /* [ebp-0x18] */
    int loop_player;           /* [ebp-0x14] */
    int loop_card;             /* [ebp-0x10] */
    int found_legacy;          /* [ebp-0x0c] */
    int saved_player;          /* [ebp-0x08] */
    int saved_card;            /* [ebp-0x04] */
  } s;

  s.found_legacy = 0;

  s.target_player = (int)(&PLAYER_CARD_INSTANCE(player, card))->damage_target_player;
  s.target_card = (&PLAYER_CARD_INSTANCE(player, card))->damage_target_card;

  s.loop_player = s.target_player;
  for (s.loop_card = 0; s.loop_card < active_cards_count[s.loop_player]; ++s.loop_card)
  {
    s.instance = &PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card);
    if (is_in_play(s.loop_player, s.loop_card) &&
        s.instance->internal_card_id == control_aura_legacy_internal_card_id &&
        s.instance->damage_target_player == s.target_player &&
        s.instance->damage_target_card == s.target_card &&
        (PLAYER_CARD_INSTANCE((int)s.instance->damage_source_player, s.instance->damage_source_card).state & STATE_TAPPED) == 0)
    {
      s.found_legacy = 1;
      s.saved_player = s.instance->targets[0].player;
      s.saved_card = s.instance->targets[0].card;
      s.instance->targets[0].player = player;
      s.instance->targets[0].card = card;
      s.instance->info_slot = 1;
    }
  }

  for (s.loop_player = 0; s.loop_player < 2; ++s.loop_player)
  {
    for (s.loop_card = 0; s.loop_card < active_cards_count[s.loop_player]; ++s.loop_card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.loop_player, s.loop_card);
      if (is_in_play(s.loop_player, s.loop_card) &&
          (global_cards_data[s.instance->internal_card_id].id == 0x2c ||
           global_cards_data[s.instance->internal_card_id].id == 0xea ||
           s.instance->internal_card_id == control_aura_special_internal_card_id) &&
          s.instance->damage_target_player == s.target_player &&
          s.instance->damage_target_card == s.target_card &&
          (s.instance->token_status & 0x1000000) != 0)
      {
        if (s.found_legacy == 0)
        {
          s.instance->token_status &= ~0x1000000;
        }

        if (s.saved_player == -1)
        {
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_player = (int8_t)s.loop_player;
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_card = s.loop_card;
        }
        else
        {
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_player = (int8_t)s.saved_player;
          (&PLAYER_CARD_INSTANCE(player, card))->damage_source_card = s.saved_card;
        }
      }
    }
  }

  return !s.found_legacy;
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

  if (player == other_player && (g_duel_network_flags & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xe);
    maxnum = DAT_008b293c;
  }

  chosen = prompt_for_life_total(player, prompt, maxnum);

  if (player == active_player && (g_duel_network_flags & 2) != 0)
  {
    unk_008b2938 = 0xe;
    DAT_008b293c = chosen;
    TENTATIVE_send_network_result(player, 0xe);
  }

  return chosen;
}

// FUNCTION: MAGIC 0x004b4110
// FUNCTION: SHANDALAR 0x0056a37b
void exit_duel_thread(int player)
{
  PostMessageA((void *)g_duel_window_hwnd, 0x401, (unsigned int)player, 0);
  ExitThread((unsigned int)player);
}

// FUNCTION: MAGIC 0x004b5d72
// FUNCTION: SHANDALAR 0x0056c574
void real_put_on_top_of_deck(int player, int internal_card_id)
{
  int index;

  for (index = 499; index > 0; --index)
  {
    global_library[player][index] = global_library[player][index - 1];
  }
  global_library[player][0] = internal_card_id;
}

// FUNCTION: MAGIC 0x004b6002
// FUNCTION: SHANDALAR 0x0056c804
void show_duel_system_error(const char *err_msg)
{
#ifdef SHANDALAR
  assert_noabort(0, "D:\\Newmagic\\multiplayer\\sid\\Minit.c", 0x76b, err_msg);
#else
  char buf[500];

  if (g_duel_ai_mode_state != 1)
  {
    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_SYSERR");
    sprintf(buf, text_lines[1], err_msg);
    MessageBoxA((HWND)g_duel_window_hwnd, buf, text_lines[0], 0);
  }
#endif
}

// FUNCTION: MAGIC 0x004a6968
// FUNCTION: SHANDALAR 0x00558aea
void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw)
{
  struct
  {
    int card;   /* ebp-0x0c */
    int flags;  /* ebp-0x08 */
    int player; /* ebp-0x04 */
  } s;

  s.flags = 0xffff;

  if ((s.flags & 1) != 0)
  {
    count_mana();
  }

  if (_DAT_00742fbc == 0 && trigger_condition == -1)
  {
    _DAT_00743024 = 0;
  }

  if ((s.flags & 2) != 0)
  {
    for (s.player = 0; s.player < 2; ++s.player)
    {
      for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
      {
        if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1 &&
            ((unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id].type & 2) != 0)
        {
          C_get_abilities(s.player, s.card, 0x3c, -1);
        }
      }
    }

    C_count_colors_of_lands_in_play();

    for (s.player = 0; s.player < 2; ++s.player)
    {
      for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
      {
        if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
        {
          if (((unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id].type & 2) != 0 ||
              (PLAYER_CARD_INSTANCE(s.player, s.card).token_status & 0x04000000) != 0)
          {
            C_get_abilities(s.player, s.card, 0x34, -1);
          }
          if (((unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id].type & 2) != 0)
          {
            C_get_abilities(s.player, s.card, 0x32, -1);
            C_get_abilities(s.player, s.card, 0x33, -1);
          }
        }
      }
    }
  }

  if (g_duel_ai_mode_state == 1)
    return;

  unk_008b3270 = 0;
  for (s.player = 0; s.player < 2; ++s.player)
  {
    for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
    {
      if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
      {
        PLAYER_CARD_INSTANCE(s.player, s.card).unknown0x70 = get_available_card_action(s.player, s.card);
      }
    }
  }

  if (duel_active != 0)
  {
    SendMessageA((HWND)g_duel_window_hwnd, 0x464, s.flags, 0);
  }
  else
  {
    redraw_shandalar_duel_screen(view_player, present_after_draw);
  }
}
