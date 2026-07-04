#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "global_strings.h"

typedef ptrdiff_t INT_PTR;

typedef int(__cdecl *in_play_card_callback_t)(int source_player,
                                              int source_card,
                                              int player,
                                              int card,
                                              int internal_card_id);

extern card_ptr_t global_raw_cards_storage[2000];
extern int DAT_00777854;
extern int DAT_00896714;
extern int DAT_0091c0f0;
extern int DAT_008cff0c;

int GetNextManaSymbol(char **param_1);
int choose_a_color_dialog(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors);

int FUN_00435c1f(void);
int FUN_00435cdd(void);
int FUN_00435d11(int player);
int FUN_004438cb(int param_1);
int set_stack_damage_targets(void);

int FUN_004ad7e0(int reason_for_trigger_controller, const char *prompt);
void FUN_004d91c9(char *out, int csvid, int field, const char *csv_name);
void FUN_004e1c8c(unsigned int internal_card_id, int unk1, char *prompt, int unk2);
void FUN_004480a6(char *text);
int FUN_004b0c12(int player, int card);
char *FUN_0044a3bf(int player, int card);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int allow_response(int param_1, int param_2, char *param_3, int param_4);

void FUN_004ec616(HWND param_1);
void FUN_004ec6c3(HWND param_1, HDC param_2, int *param_3);
void FUN_00495fa1(void);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
void FUN_004b4720(int player, int internal_card_id, int card);
void FUN_004b6002(const char *param_1);
void FUN_004e698f(unsigned int *param_1, unsigned int *param_2);
int FUN_004418c6(int param_1, int param_2, int param_3, int param_4, unsigned int param_5, unsigned int param_6);
void *FUN_0055dc4c(void *dest, const void *src, unsigned int count);

// GLOBAL: MAGIC 0x00573080
const unsigned char DAT_00573080[18] = {
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

// GLOBAL: MAGIC 0x00561784
const char s_PROMPT_DRAWACARD_00561784[] = "PROMPT_DRAWACARD";

// GLOBAL: MAGIC 0x00561798
const char s_PROMPT_DRAWACARD_00561798[] = "PROMPT_DRAWACARD";

// GLOBAL: MAGIC 0x005617ac
const char DAT_005617ac[4] = {0, 0, 0, 0};

// GLOBAL: MAGIC 0x005617b0
const char s_PROMPT_DRAWACARD_005617b0[] = "PROMPT_DRAWACARD";

// GLOBAL: MAGIC 0x005617c4
const char DAT_005617c4[4] = {0, 0, 0, 0};

int can_target(target_definition_t *td)
{
  return td != NULL;
}

// FUNCTION: MAGIC 0x0043edab
int put_card_on_stack(int player, int card, int mode)
{
  struct
  {
    int rand_choice;        // ebp-0x34c
    int trace_counter;      // ebp-0x348
    char prompt_text[300];  // ebp-0x344
    int tmp_mana;           // ebp-0x218
    char trace_text[500];   // ebp-0x214
    int saved_unk_008ce508; // ebp-0x20
    int saved_max_x_value;  // ebp-0x1c
    int local_18;           // ebp-0x18
    int color_index;        // ebp-0x14
    int saved_DAT_00789b7c; // ebp-0x10
    int internal_card_id;   // ebp-0xc
    card_data_t *card_data; // ebp-0x8
    int saved_unk_008ce4f4; // ebp-0x4
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.color_index = (int)single_color_test_bit_to_color_t((int)(char)global_cards_data[s.internal_card_id].color);
  s.card_data = &global_cards_data[s.internal_card_id];

  if (mode == 0)
  {
    if ((unk_00926804 & 2) != 0)
    {
      s.trace_counter = DAT_00777aa0;
      ++DAT_00777aa0;
      sprintf(s.trace_text, "%d: Player #%d is casting %s(%d).\n", s.trace_counter, player, s.card_data->name, card);
      append_to_trace_txt(s.trace_text);
    }

    if (!FUN_0043fdb3(player, player, card))
    {
      return 0;
    }

    unk_00925bb8 = -1;
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

    s.saved_DAT_00789b7c = DAT_00789b7c;
    s.saved_unk_008ce508 = unk_008ce508;
    s.saved_unk_008ce4f4 = unk_008ce4f4;

    DAT_00789b7c = s.internal_card_id;
    unk_008ce508 = player;
    unk_008ce4f4 = card;

    FUN_00443ee2(player, card, 0x71, player, 0);
    FUN_00435c1f();

    if (unk_0091bfb4 == 0)
    {
      if ((player == unk_008b35ec || (unk_00926804 & 2) != 0) && unk_008a9000 != 1)
      {
        if ((s.card_data->type & 0x40) != 0)
        {
          unk_008ce510[6] = (int)(char)s.card_data->cc[0] + (int)(char)s.card_data->cc[1];
        }
        else
        {
          unk_008ce510[s.color_index] = (int)(char)s.card_data->cc[0];
          unk_008ce510[0] += (int)(char)s.card_data->cc[1];
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
          unk_008ce510[s.color_index] = (int)(char)s.card_data->cc[0];
        }
        if (0 < (char)s.card_data->cc[1])
        {
          unk_008ce510[0] = (int)(char)s.card_data->cc[1];
        }

        charge_mana_w_global_cost_mod(player, card, 0, 0);

        if ((char)s.card_data->cc[1] == -1)
        {
          if (player == unk_008b35ec)
          {
            s.saved_max_x_value = max_x_value;
            max_x_value = has_mana(player, COLOR_ANY, 1);
            charge_mana(player, 0, -1);
            max_x_value = s.saved_max_x_value;
          }
          else
          {
            if (unk_008a9000 == 1)
            {
              s.rand_choice = internal_rand(2);
              if (s.rand_choice == 0)
              {
                s.tmp_mana = has_mana(player, COLOR_ANY, 1);
                if (s.tmp_mana == 0)
                {
                  s.tmp_mana = has_mana(player, COLOR_ANY, 1);
                  unk_00939340 = internal_rand(s.tmp_mana + 1);
                  s.local_18 = unk_00939340;
                }
                else
                {
                  unk_00939340 = internal_rand(s.tmp_mana) + 1;
                  s.local_18 = unk_00939340;
                }
              }
              else if (s.rand_choice == 1)
              {
                unk_00939340 = has_mana(player, COLOR_ANY, 1);
                s.local_18 = unk_00939340;
                if (life[0] < unk_00939340 && internal_rand(3) == 0)
                {
                  s.local_18 = life[0];
                  unk_00939340 = life[0];
                }
                if (max_x_value != -1 && max_x_value < unk_00939340)
                {
                  s.local_18 = max_x_value;
                  unk_00939340 = max_x_value;
                }
              }

              FUN_004e4f11();
            }
            else
            {
              FUN_004e5089();
              s.local_18 = unk_00939340;
            }

            s.saved_max_x_value = max_x_value;
            max_x_value = s.local_18;
            charge_mana(player, 0, -1);
            max_x_value = s.saved_max_x_value;
          }
        }
      }
    }

    if (x_value == 0)
    {
      ai_modifier -= 100;
    }

    max_x_value = -1;

    if (s.internal_card_id != -1)
    {
      DAT_0078930c = player;
      unk_008b2880 = 1;
      --hand_count[player];

      if ((*(unsigned char *)((char *)s.card_data + 0x28) & 2) != 0)
      {
        ++creature_cards_in_play[player];
      }
      if ((*(unsigned char *)((char *)s.card_data + 0x28) & 0x40) != 0)
      {
        ++artifact_cards_in_play[player];
      }
      if ((*(unsigned char *)((char *)s.card_data + 0x28) & 4) != 0)
      {
        ++enchantments_in_play[player];
      }

      card_types_in_play[player] |= *(unsigned char *)((char *)s.card_data + 0x28);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_INVISIBLE;
      unk_008b4278 |= 0x20;

      if (player == unk_008b35ec || (unk_00926804 & 2) != 0 || unk_008a9000 != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_BOTH;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_SUMMONSICK_NOATTACK;
      }

      DAT_00789b7c = s.saved_DAT_00789b7c;
      unk_008ce508 = s.saved_unk_008ce508;
      unk_008ce4f4 = s.saved_unk_008ce4f4;

      if (spell_fizzled == 1)
      {
        FUN_00435cdd();
      }
      else
      {
        if (dispatch_event(player, card, 0x6c) != 0)
        {
          spell_fizzled = 1;
        }
        if (spell_fizzled == 1)
        {
          FUN_00435d11(player);
        }
        FUN_00435cdd();

        if (spell_fizzled != 1)
        {
          if (player == active_player && (unk_00926804 & 2) == 0 && unk_008a9000 == 1 && *(char *)((char *)s.card_data + 0x2c) == -1)
          {
            ai_modifier -= FUN_004427d9(player) * 12;
          }
          return 1;
        }
      }
    }
  }
  else
  {
    s.saved_DAT_00789b7c = DAT_00789b7c;
    s.saved_unk_008ce508 = unk_008ce508;
    s.saved_unk_008ce4f4 = unk_008ce4f4;

    DAT_00789b7c = s.internal_card_id;
    unk_008ce508 = player;
    unk_008ce4f4 = card;

    if ((*(unsigned char *)((char *)s.card_data + 0x28) & 1) == 0)
    {
      FUN_004438cb(1);
    }

    if ((*(unsigned char *)((char *)s.card_data + 0x28) != 1) &&
        ((*(unsigned char *)((char *)s.card_data + 0x28) != 0x20) || ((*(unsigned char *)((char *)s.card_data + 0x3d) & 0x10) == 0)))
    {
      if (DAT_007a7d78 != 0)
      {
        DAT_007a7d78 |= 2;
      }

      sprintf(unk_00748770, (const char *)gs_trying_to_cast_007a79c0, FUN_0044a3bf(unk_008ce508, unk_008ce4f4));
      allow_response(-2, current_phase, unk_00748770, 0xd3);
      if ((DAT_007a7d78 & 2) != 0)
      {
        FUN_004a61a1(player, card, 2, 1);
        Sleep(3000);
      }
    }

    DAT_00789b7c = s.saved_DAT_00789b7c;
    unk_008ce508 = s.saved_unk_008ce508;
    unk_008ce4f4 = s.saved_unk_008ce4f4;

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
    {
      spell_fizzled = 1;
    }

    unk_008b4278 &= ~0x20;
    PLAYER_CARD_INSTANCE(player, card).state |= (player == 1) ? 0x400080 : 0x80;

    if (unk_008a9000 != 1)
    {
      set_stack_damage_targets();
    }

    if (spell_fizzled != 1)
    {
      if (player == active_player && unk_008a9000 != 1 && DAT_007abc80 == 0 && (((unsigned char)s.card_data->type & 0x7e) != 0))
      {
        LoadTextSectionLines(global_ui_strings_filename, "PROMPT_CAST1");
        sprintf(s.prompt_text, text_lines[0], DAT_007a7c60);
        if ((char)s.card_data->cc[1] == -1)
        {
          sprintf(s.prompt_text, text_lines[1], DAT_007a7c60, x_value);
        }

        if (PLAYER_CARD_INSTANCE(player, card).mana_to_untap[2] == 0)
        {
          goto cast_dialog_default_jump;
        }

        if (PLAYER_CARD_INSTANCE(player, card).mana_to_untap[2] == 1)
        {
          raw_do_dialog(player,
                        card,
                        PLAYER_CARD_INSTANCE(player, card).internal_card_id,
                        PLAYER_CARD_INSTANCE(player, card).unknown0x70,
                        s.prompt_text,
                        0);
          goto cast_dialog_done;
        }

        raw_do_dialog(player, card, -1, -1, s.prompt_text, 0);
        goto cast_dialog_done;

      cast_dialog_default_jump:
        goto cast_dialog_default;

        // Unreachable in practice, but present in the original binary.
        raw_do_dialog(player, card, (unk_00925bb8 >> 8), (unk_00925bb8 & 0xff), s.prompt_text, 0);
        goto cast_dialog_done;

      cast_dialog_default:
        raw_do_dialog(player, card, -1, -1, s.prompt_text, 0);

      cast_dialog_done:;
      }

      if (unk_008a9000 != 1)
      {
        FUN_004a61a1(player, card, 2, 1);
      }
    }
  }

  if (spell_fizzled == 1)
  {
    ++hand_count[player];

    if ((*(unsigned char *)((char *)s.card_data + 0x28) & 2) != 0)
    {
      --creature_cards_in_play[player];
    }
    if ((*(unsigned char *)((char *)s.card_data + 0x28) & 0x40) != 0)
    {
      --artifact_cards_in_play[player];
    }
    if ((*(unsigned char *)((char *)s.card_data + 0x28) & 4) != 0)
    {
      --enchantments_in_play[player];
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xffffff5d;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= 0xfffcffff;

    if (player == active_player && (unk_00926804 & 2) == 0)
    {
      DAT_0093d850 = 1;
    }
    spell_fizzled = 0;
    obliterate_top_card_of_stack();
    unk_008b4278 &= ~0x20;
    return 0;
  }

  TENTATIVE_set_timestamps(player, card);
  return 1;
}

// FUNCTION: MAGIC 0x0043f9ed
int FUN_0043f9ed(int player, int card)
{
  int internal_card_id;
  int saved_trigger_cause;
  int saved_trigger_cause_controller;
  char *card_name;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  single_color_test_bit_to_color_t((int)(char)global_cards_data[internal_card_id].color);

  saved_trigger_cause = trigger_cause;
  saved_trigger_cause_controller = trigger_cause_controller;

  if (internal_card_id == -1)
  {
    obliterate_top_card_of_stack();
    return 0;
  }

  trigger_cause_controller = player;
  trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_SPELL_CAST, gs_casting_00896400, 0);
  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;

  if (global_cards_data[internal_card_id].type != 0x01 &&
      (global_cards_data[internal_card_id].type != 0x20 || (global_cards_data[internal_card_id].extra_ability & 0x1000) == 0))
  {
    card_name = FUN_0044a3bf(player, card);
    sprintf(unk_00748770, gs_cast_008b4720, card_name);
    allow_response(-2, current_phase, unk_00748770, 0x6c);
  }

  PLAYER_CARD_INSTANCE(player, card).state &= ~0x20;
  PLAYER_CARD_INSTANCE(player, card).state |= 2;
  unk_008b4278 &= ~0x20;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id)
  {
    card_types_in_play[player] |= (unsigned char)global_cards_data[internal_card_id].type;

    if (unk_008a9000 != 1)
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
    FUN_004afa4b(human_player);
    unk_007abc78 &= ~1;
    DAT_007ab2cc = FUN_004b0c12;
    regenerate_or_graveyard_triggers();
    FUN_004afa4b(human_player);

    DAT_0078930c = -1;
    DAT_00789b7c = -1;

    if ((global_cards_data[internal_card_id].type & 1) != 0)
    {
      ++unk_008cfdb0;
    }

    if (spell_fizzled == 1)
    {
      if (unk_008a9000 != 1)
      {
        load_text("prompts.txt", "PROMPT_FIZZLE");
        FUN_004a61d6(unk_00748770);
        Sleep(2000);
        FUN_004a61d6("");
      }
      DAT_0093d850 = 1;
      spell_fizzled = 0;
      return 0;
    }

    spell_fizzled = 0;
    TENTATIVE_reassess_all_cards();
    return 1;
  }

  obliterate_top_card_of_stack();
  return 0;
}

// FUNCTION: MAGIC 0x004456ad
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

  ++unk_0091a6d0;

  s.saved_current_phase = current_phase;
  s.saved_DAT_007ab2bc = DAT_007ab2bc;
  s.saved_current_turn = current_turn;
  s.saved_DAT_007aadec = DAT_007aadec;

  DAT_007aadec = trig;
  current_turn = player;

  s.saved_DAT_00789714 = DAT_00789714;
  s.saved_trig = trig;

  do
  {
    if ((unk_00926804 & 2) != 0)
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

    s.result = FUN_004ad7e0(player, prompt);

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
  --unk_0091a6d0;

  if (unk_0091a6d0 == 0)
  {
    for (player = 0; player < 2; ++player)
    {
      for (s.loop_card = 0; s.loop_card < active_cards_count[player]; ++s.loop_card)
      {
        PLAYER_CARD_INSTANCE(player, s.loop_card).state &= ~0x100;
      }
    }

    if (DAT_00939510 == 0)
    {
      DAT_00742f64 = 0;
      if (unk_008a9000 != 1)
      {
        DAT_00742fc8 = 0;
      }
    }
  }

  DAT_007ab2bc = s.saved_DAT_007ab2bc;
  current_turn = s.saved_current_turn;
  DAT_007aadec = s.saved_DAT_007aadec;
  current_phase = s.saved_current_phase;

  return 0;
}

// FUNCTION: MAGIC 0x00446da7
void FUN_00446da7(unsigned int internal_card_id, int player, int card, int unk1, char *prompt, int unk2)
{
  (void)unk1;
  (void)prompt;
  (void)unk2;

  // The original stores these params to locals even if it won't use them.
  {
    int local_c;
    int local_8;

    local_c = player;
    local_8 = card;

    if (IsWindowVisible((HWND)DAT_00896714))
    {
      if (player != -1 && card != -1)
      {
        SendMessageA((HWND)DAT_00896714, 0x401, CardIDFromType(internal_card_id), (LPARAM)&local_c);
      }
      else
      {
        SendMessageA((HWND)DAT_00896714, 0x401, CardIDFromType(internal_card_id), 0);
      }
    }
  }
}

// FUNCTION: MAGIC 0x0046489e
int FUN_0046489e(int card_id)
{
  struct
  {
    int rand_pick;    // ebp-0x14
    int chosen_csvid; // ebp-0x10
    int total_cards;  // ebp-0xc
    int running_pick; // ebp-0x8
    int i;            // ebp-0x4
  } s;

  if (card_id == -1)
  {
    return -1;
  }

  s.total_cards = 0;
  for (s.i = 0; s.i < 200; ++s.i)
  {
    s.total_cards += initial_library[card_id][s.i].numcards;
  }

  if (s.total_cards == 0)
  {
    return -1;
  }

  s.rand_pick = internal_rand(s.total_cards);
  s.running_pick = s.rand_pick;

  for (s.i = 0; s.i < 200; ++s.i)
  {
    s.running_pick -= initial_library[card_id][s.i].numcards;
    if (s.running_pick < 0)
    {
      s.chosen_csvid = initial_library[card_id][s.i].csvid;
      if (unk_008a9000 != 1)
      {
        --initial_library[card_id][s.i].numcards;
      }
      break;
    }
  }

  for (s.i = 0; s.i < unk_0093f4b8; ++s.i)
  {
    if ((int)global_cards_data[s.i].id == s.chosen_csvid)
    {
      return s.i;
    }
  }
}

// FUNCTION: MAGIC 0x004a6355
void FUN_004a6355(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  if (unk_008a9000 == 1)
  {
    return;
  }

  if (unk_00742fc4 != 0)
  {
    FUN_00446da7(internal_card_id, -1, -1, unk1, prompt, unk2);
  }
  else
  {
    FUN_004e1c8c(internal_card_id, unk1, prompt, unk2);
  }
}

// FUNCTION: MAGIC 0x004b58e2
int FUN_004b58e2(int color, int unk1, int unk2)
{
  if (color == 1 || unk1 == 1)
  {
    return 1;
  }

  color = (int)single_color_test_bit_to_color_t(color);
  unk1 = (int)single_color_test_bit_to_color_t(unk1);

  if ((char)DAT_00573080[unk1 * 3] == color)
  {
    return 1;
  }

  if (unk2 > 1 && (char)DAT_00573080[unk1 * 3 + 1] == color)
  {
    return 1;
  }

  if (unk2 > 2 && (char)DAT_00573080[unk1 * 3 + 2] == color)
  {
    return 1;
  }

  if (unk2 > 3)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b5de8
int FUN_004b5de8(int internal_card_id)
{
  struct
  {
    char rarity_str[32]; // ebp-0x24
    int rarity;          // ebp-0x4
  } s;

  if ((global_cards_data[internal_card_id].extra_ability & 0x180) != 0 || global_cards_data[internal_card_id].expansion == '@')
  {
    global_cards_data[internal_card_id].rarity = 4;
  }

  if ((char)global_cards_data[internal_card_id].rarity != -1)
  {
    s.rarity = (int)(char)global_cards_data[internal_card_id].rarity;
    return s.rarity;
  }

  FUN_004d91c9(s.rarity_str, (int)global_cards_data[internal_card_id].id, 9, "info.csv");

  s.rarity = 1;
  if (strcmp(s.rarity_str, "Special") == 0)
  {
    s.rarity = 3;
  }
  if (strcmp(s.rarity_str, "Rare") == 0)
  {
    s.rarity = 3;
  }
  if (strcmp(s.rarity_str, "Uncommon") == 0)
  {
    s.rarity = 2;
  }

  global_cards_data[internal_card_id].rarity = (unsigned char)s.rarity;

  return s.rarity;
}

// FUNCTION: MAGIC 0x00551ed7
int FUN_00551ed7(int player, unsigned int preferred_controller, int card)
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
void FUN_00449bef(char *name)
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
int is_in_play(int player, int card)
{
  if (global_card_instances[player][card].internal_card_id == -1)
    return 0;

  return (global_card_instances[player][card].state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY)) == STATE_IN_PLAY;
}

// FUNCTION: MAGIC 0x005001e0
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
int FUN_004832f4(int player, unsigned char type)
{
  int graveyard_index;
  int result;

  result = 0;
  if (player == -1 || player == unk_008b35ec)
  {
    graveyard_index = 0;
    while (graveyard_index < 500)
    {
      if (global_graveyard_slots[unk_008b35ec][graveyard_index] == -1)
      {
        break;
      }
      else if ((type & global_cards_data[global_graveyard_slots[unk_008b35ec][graveyard_index]].type) != 0)
      {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  if ((player == -1 || player == active_player) && result == 0)
  {
    graveyard_index = 0;
    while (graveyard_index < 500)
    {
      if (global_graveyard_slots[active_player][graveyard_index] == -1)
      {
        break;
      }
      else if ((type & global_cards_data[global_graveyard_slots[active_player][graveyard_index]].type) != 0)
      {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00484581
int FUN_00484581(int internal_card_id, color_t color)
{
  int result;

  result = 0;
  if ((char)global_cards_data[internal_card_id].subtype == 13 && global_cards_data[internal_card_id].id == unk_0092664c[color])
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
int FUN_004a62d7(int player,
                 int *graveyard,
                 void *available,
                 int count,
                 int prompt,
                 int a6,
                 int selected,
                 int a8,
                 int a9)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }
  return FUN_0049e8bb(player, graveyard, 0, available, count, prompt, a6, selected, a8, a9);
}

int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  (void)player;
  (void)card;
  return orig_color_test;
}

// FUNCTION: MAGIC 0x004a6245
INT_PTR FUN_004a6245(int *internal_card_ids, int *damage_amounts, int count, int title, int allow_cancel, char *prompt)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }

  return show_cardlist(internal_card_ids, damage_amounts, 0, count, (void *)title, (unsigned int)allow_cancel, prompt);
}

// FUNCTION: MAGIC 0x004a628e
INT_PTR FUN_004a628e(int *graveyard, int *available, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }

  return show_cardlist(graveyard, 0, available, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x004b5f03
int FUN_004b5f03(int card_id)
{
  int internal_card_id;

  if (card_id != -1)
  {
    for (internal_card_id = 0; internal_card_id < unk_0093f4b8 + 0x10; ++internal_card_id)
    {
      if (global_cards_data[internal_card_id].id == card_id)
      {
        return internal_card_id;
      }
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x004b41f2
int FUN_004b41f2(int player,
                 int *graveyard,
                 int *allowed_cards,
                 int count,
                 const char *prompt,
                 int allow_cancel,
                 int *title)
{
  int result;

  result = -1;

  if (active_player == player && (unk_00926804 & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    result = unk_007a7d0c;
  }

  if (unk_008b35ec == player && unk_008a9000 != 1)
  {
    result = (int)FUN_004a628e(graveyard,
                               allowed_cards,
                               count,
                               (void *)prompt,
                               (unsigned int)allow_cancel,
                               (char *)title);
    if ((unk_00926804 & 2) != 0)
    {
      unk_007a7d08 = 0x19;
      unk_007a7d0c = result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00483e3e
int FUN_00483e3e(int player, unsigned int type_mask)
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
int FUN_00481e25(int player, int card, int event)
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

  dispatch_function_to_all_cards_in_play(player, card, FUN_004821f5, -1);

  if (trigger_condition == 0xdf && card == affected_card && player == affected_card_controller && current_turn == player && player != human_player && player == trigger_cause_controller && card == trigger_cause)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      unk_007abc78 |= 4;
      PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .unknown0x14 |= 0x8000000;
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
      unk_007abc78 &= 0xfffffffb;

      PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .unknown0x14 &= 0xf7ffffff;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0048194e
int FUN_0048194e(int preferred_player, int only_player, int *target_data)
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
    if (only_player == -1 || s.player == only_player)
    {
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
    if (only_player == -1 || s.player == only_player)
    {
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
  }

  return *target_data != -1;
}

// FUNCTION: MAGIC 0x004821f5
int FUN_004821f5(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
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
void FUN_0044837e(int deck_owner)
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
void FUN_004b5b7a(int deck_owner)
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
void FUN_004b5c19(int deck_owner)
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
void FUN_004b59b2(int player, int deck_owner)
{
  struct
  {
    int swap_slot;    /* [ebp-0x14] */
    int temp;         /* [ebp-0x10] */
    int unused;       /* [ebp-0x0c] */
    int current_slot; /* [ebp-0x08] */
    int deck_size;    /* [ebp-0x04] */
  } s;

  if (unk_008a9000 != 1)
  {
    play_sound_effect(WAV_SHUFFLE);
    FUN_0044837e(deck_owner);
  }

  if ((unk_00926804 & 2) == 0 || unk_008b35ec == player)
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

    if ((unk_00926804 & 2) != 0)
    {
      FUN_004b5b7a(deck_owner);
    }
  }
  else
  {
    FUN_004b5c19(deck_owner);
  }
}

// FUNCTION: MAGIC 0x004b5c94
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
int regenerate_or_graveyard_triggers(void)
{
  if (DAT_007ab2cc == 0 || regenerate_or_graveyard_triggers_in_progress != 0)
  {
    return 0;
  }

  regenerate_or_graveyard_triggers_in_progress = 1;

  unk_008b4278 |= 0x200;
  allow_response(-2, current_phase, gs_use_regeneration_effects_0091c680, 0x70);
  unk_008b4278 &= ~0x200;

  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_GRAVEYARD_ORDER, gs_graveyard_order_0091cbd0, 0);
  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);

  DAT_007ab2cc = 0;
  regenerate_or_graveyard_triggers_in_progress = 0;

  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: MAGIC 0x005510dc
int FUN_005510dc(int player, int type_mask)
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
void FUN_004b15f7(int player, int graveyard_index)
{
  for (; graveyard_index < 499; ++graveyard_index)
  {
    global_graveyard_slots[player][graveyard_index] = global_graveyard_slots[player][graveyard_index + 1];
  }
  global_graveyard_slots[player][499] = -1;
}

// FUNCTION: MOK 0x0047936d
// FUNCTION: MAGIC 0x004b42aa
int show_deck(int player, int *cards, int count, void *context, int suppress_done_txt, char *prompt)
{
  struct
  {
    int result;
    int display_count;
    int y_positions[500];
    int x_positions[500];
    int y;
    int x;
    int valid_count;
    int valid_graveyards[488];
    int draw_args[6];
    int padding[6];
    int local_c;
    int local_8;
  } s;

  if (((active_player == player) && ((unk_00926804 & 2) != 0)) && unk_00742fc4 != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    s.result = unk_007a7d0c;
  }
  else if (((active_player == player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1)
  {
    s.valid_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1)
      {
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
      }
    }

    unk_00939340 = internal_rand(s.valid_count);
    if (unk_008b35ec != player)
    {
      if (unk_008a9000 == 1)
      {
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
      }
    }

    s.result = s.valid_graveyards[unk_00939340];
  }
  else if (unk_00742fc4 == 0)
  {
    ReadPalette(unk_00573364, (char *)0);
    FUN_004e1c81(0, 0);
    SelectPalette(unk_0074309c, unk_007463dc, 0);
    FUN_004e1da6(unk_00573370);
    FUN_004e1d91(1, 0, 0, unk_00573380, &unk_007462a0);
    FUN_004e1d6d(unk_0057a75c, 0, 0, 0x280, 0x1e0, unk_007497b0, 0, 0, unk_0057d9f0, unk_0057d9f4);

    s.display_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8]))
      {
        ++s.display_count;
      }
    }

    s.valid_count = (s.display_count - 1) / 5;
    if (s.valid_count == 0)
    {
      s.valid_count = 1;
    }

    s.x = 0x60;
    s.valid_count = 0;
    s.y = 0x10;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8]))
      {
        s.x_positions[s.valid_count] = s.x + 4;
        s.y_positions[s.valid_count] = s.y;
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
        s.x += 0x38;
        if (s.x > 0x13f)
        {
          s.x = 0x60;
          s.y += 0x54 / s.display_count;
        }
      }
    }

    if (s.valid_count > 0)
    {
      s.result = s.valid_graveyards[0];
    }
    else
    {
      s.result = -1;
    }
  }
  else
  {
    s.result = FUN_004a61fe(cards, count, context, suppress_done_txt, prompt);
    if ((unk_008b35ec == player) && ((unk_00926804 & 2) != 0))
    {
      unk_007a7d08 = 0x19;
      unk_007a7d0c = s.result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return s.result;
}

// FUNCTION: MOK 0x00459AA0
// FUNCTION: MAGIC 0x00485060
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
int FUN_004964dd(int card_id, int player, int card)
{
  if (card_id == -1)
  {
    return 0;
  }
  if (player == -1 || card == -1)
  {
    return 0;
  }
  if (global_raw_cards_storage[card_id].num_pics < 2)
  {
    return 0;
  }

  return (player + card) % global_raw_cards_storage[card_id].num_pics;
}

// FUNCTION: MAGIC 0x0050047c
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card)
{
  int source_internal_card_id;
  int damage_player;
  int result;
  int display_pic_num;

  if (target_player == -1 || source_player == -1 || amount < 1)
  {
    return -1;
  }

  if (target_card == -1)
  {
    damage_player = target_player;
  }
  else
  {
    damage_player = source_player;
  }

  result = add_card_to_hand(damage_player, unk_009266a4);
  if (result != -1)
  {
    PLAYER_CARD_INSTANCE(damage_player, result).state |= (damage_player == 0 ? 0x1002 : 2);
    PLAYER_CARD_INSTANCE(damage_player, result).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(damage_player, result).damage_target_card = target_card;
    PLAYER_CARD_INSTANCE(damage_player, result).info_slot = amount;
    PLAYER_CARD_INSTANCE(damage_player, result).damage_source_player = (char)source_player;
    PLAYER_CARD_INSTANCE(damage_player, result).damage_source_card = source_card;
    if (source_card == -1)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(damage_player, result).display_pic_csv_id = 0xef;
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == -1 ||
          PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == unk_0091a80c)
      {
        source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).original_internal_card_id;
      }
      else
      {
        source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id;
      }

      PLAYER_CARD_INSTANCE(damage_player, result).color = PLAYER_CARD_INSTANCE(source_player, source_card).color;
      if ((global_cards_data[source_internal_card_id].type & TYPE_INTERRUPT) != 0)
      {
        PLAYER_CARD_INSTANCE(damage_player, result).color |= 0x40;
      }

      PLAYER_CARD_INSTANCE(damage_player, result).eot_toughness =
          (unsigned int)(unsigned char)global_cards_data[source_internal_card_id].type;

      if ((PLAYER_CARD_INSTANCE(source_player, source_card).state & 4) != 0)
      {
        if (current_phase == 0x19)
        {
          PLAYER_CARD_INSTANCE(damage_player, result).token_status |= 0x100000;
        }
        if (current_phase == 0x1a)
        {
          PLAYER_CARD_INSTANCE(damage_player, result).token_status |= 0x40000;
        }
      }
      if (global_cards_data[source_internal_card_id].id == unk_00789734 || global_cards_data[source_internal_card_id].id == unk_008a8de8 || global_cards_data[source_internal_card_id].id == unk_008cf1ac)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(damage_player, result).display_pic_csv_id =
            *(unsigned int *)&PLAYER_CARD_INSTANCE(source_player, source_card).display_pic_csv_id;
      }
      else
      {
        display_pic_num = FUN_004964dd(global_cards_data[source_internal_card_id].id, source_player, source_card);
        *(unsigned int *)&PLAYER_CARD_INSTANCE(damage_player, result).display_pic_csv_id =
            (display_pic_num << 16) | (unsigned int)(unsigned short)global_cards_data[source_internal_card_id].id;
      }
    }
    unk_008b4278 |= 2;
  }

  return result;
}

// FUNCTION: MAGIC 0x004b4654
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
      FUN_004b4720(player, internal_card_id, card);
      if (active_cards_count[player] <= card)
      {
        active_cards_count[player] = card + 1;
      }
      return card;
    }
  }

  FUN_004b6002("AddCard error: No more room in cd to add a card");
  return -1;
}

int FUN_004b5274(int player, int card);

// FUNCTION: MAGIC 0x004b4720
void FUN_004b4720(int player, int internal_card_id, int card)
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

  PLAYER_CARD_INSTANCE(player, card).attack_rating = FUN_004b5274(player, card);

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
  PLAYER_CARD_INSTANCE(player, card).counters5 = 0;
  PLAYER_CARD_INSTANCE(player, card).unknown0x122 = 0;

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
int FUN_004b5274(int player, int card)
{
  int internal_card_id;
  keyword_t static_ability;
  int power2;
  int toughness_masked;
  int score;
  int scaled;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  static_ability = global_cards_data[internal_card_id].static_ability;

  power2 = (((int)global_cards_data[internal_card_id].power & 0xffffbfffU) * 2);
  if (global_cards_data[internal_card_id].subtype == 0)
  {
    power2 = 0;
  }

  toughness_masked = ((int)global_cards_data[internal_card_id].toughness & 0xffffbfffU);
  score = (int)((power2 + 2) * (toughness_masked + 1)) / 2;

  if (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) && (human_player == player))
  {
    score -= 1;
  }

  if ((static_ability & 0x80) != 0)
  {
    score = (score * 3) / 2;
  }
  if ((static_ability & 0x100) != 0)
  {
    score = (score * 3) / 2;
  }
  if ((global_cards_data[internal_card_id].extra_ability & 3) != 0)
  {
    score = (score * 3) / 2;
  }
  if ((static_ability & 0x40) != 0)
  {
    score = (int)((toughness_masked + 1) * score) / 2;
  }
  if ((static_ability & 0x200) != 0)
  {
    score = (score * 3) / 2;
  }

  scaled = unk_007a7d18[player] * score;
  return (scaled + ((scaled >> 0x1f) & 7U)) >> 3;
}

// FUNCTION: MAGIC 0x00500135
int create_a_card_type(int internal_card_id)
{
  int new_internal_card_id;

  for (new_internal_card_id = unk_0093f4b8; new_internal_card_id < unk_0093f4b8 + 0x10; ++new_internal_card_id)
  {
    if (*(int *)&global_cards_data[new_internal_card_id].id == -1)
    {
      memcpy(&global_cards_data[new_internal_card_id], &global_cards_data[internal_card_id], 0x48);
      return new_internal_card_id;
    }
  }

  FUN_004b6002("AddType error: No room in ci to add another type");
  return -1;
}

// FUNCTION: MAGIC 0x00442b42
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
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0)
    {
      return 0;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & UPKEEP_UPKEEP_CANTPAY) != 0)
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

  if (result != 0x63 && (unk_008b4278 & 0x224) != 0 && (event == EVENT_CAN_CAST || event == EVENT_CAN_ACTIVATE))
  {
    if (FUN_00442cf9(player, card) == 0)
    {
      unk_008b3270 = saved_unk_008b3270;
      pop_affected_card_stack();
      return 0;
    }
  }

  DAT_008cf1b8 = event_result;
  pop_affected_card_stack();
  return result;
}

// FUNCTION: MAGIC 0x004428be
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
int FUN_0043e18b(int player)
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

  if (player == unk_008b35ec)
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
      s.drawn_card = add_card_to_hand(player, FUN_0046489e(g_selected_wizard_color));
    }

    if (s.drawn_card != -1 && PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id != -1)
    {
      TENTATIVE_reassess_all_cards(0, 0x30);
      if (unk_008a9000 != 1)
      {
        load_text(global_ui_strings_filename, s_PROMPT_DRAWACARD_00561784);
        if (unk_00742fc4 != 0)
        {
          FUN_00446da7(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                       player,
                       s.drawn_card,
                       0xf6,
                       text_lines[0],
                       1);
        }
        else
        {
          FUN_004a6355(PLAYER_CARD_INSTANCE(player, s.drawn_card).internal_card_id,
                       0xf6,
                       text_lines[0],
                       1);
        }
      }
    }
    else if (unk_008a9000 != 1)
    {
      load_text(global_ui_strings_filename, s_PROMPT_DRAWACARD_00561798);
      FUN_004a61d6(text_lines[1]);
      Sleep(0x9c4);
      FUN_004a61d6((char *)DAT_005617ac);
      FUN_004b4110(0);
    }
    else
    {
      life[unk_008b35ec] = -99;
      life[1 - unk_008b35ec] = 0x14;
    }
  }
  else
  {
    if (DAT_0057a750 == -1)
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
        s.drawn_card = internal_rand(unk_0093f4b8);
        s.candidate_ok = FUN_004b58e2((int)(char)global_cards_data[s.drawn_card].color,
                                      DAT_008ce538,
                                      DAT_008951c8);
        if (s.candidate_ok != 0 && FUN_004b5de8(s.drawn_card) > DAT_007a7874)
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
      if (unk_008a9000 != 1)
      {
        load_text(global_ui_strings_filename, s_PROMPT_DRAWACARD_005617b0);
        FUN_004a61d6(text_lines[2]);
        Sleep(0x9c4);
        FUN_004a61d6((char *)DAT_005617c4);
        FUN_004b4110(1);
      }
      else
      {
        life[1 - unk_008b35ec] = -99;
        life[unk_008b35ec] = 0x14;
      }
    }

    TENTATIVE_reassess_all_cards(0, 0x30);
  }

  ++hand_count[player];
  ++DAT_008cfda8;
  if (unk_008a9000 != 1 && s.drawn_card != -1)
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
int FUN_004b0047(int player, int card)
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

  if (s.in_play != 0 && current_phase == 0x15 && player == human_player && (s.state & 0x8000) != 0 && (s.state & 4) == 0 && FUN_0044125c(player, card) != 0)
  {
    s.result = 2;
  }
  else if (s.in_play != 0 && trigger_condition != -1)
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
      s.trigger_result = FUN_004c0a36(FUN_004b082f(player, card, EVENT_TRIGGER, player), 0, 2);
    }

    if (s.trigger_result != 0)
    {
      DAT_007ab2bc |= 1 << ((unsigned char)s.trigger_result & 0x1f);
      DAT_00791410 += 1;
      s.result = s.trigger_result;
    }
    else
    {
      s.result = 0;
    }
  }
  else
  {
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
    else if (player == active_player)
    {
      if ((DAT_007aadec == 4 && (s.upkeep_flags & 1) != 0) || (DAT_007aadec == 10 && unk_008b28f8 == s.internal_card_id))
      {
        unk_008b3270 |= 3;
        DAT_007ab2bc |= 4;
        s.result = 2;
      }
      else
      {
        s.instance->state &= ~0x800;
        s.result = 0;
      }
    }
    else if (_DAT_00742fbc == 0 && (current_phase == 0x15 || current_phase == 0x17))
    {
      if (s.in_play != 0 && (s.state & 0x10) == 0 && ((s.type & TYPE_CREATURE) != 0 || (s.state & 0x3000000) != 0))
      {
        if (player == human_player && FUN_0044125c(player, card) != 0 && (s.state & 0x10000) == 0)
        {
          unk_00743038 = 0;
          return 0x10;
        }
        if (player != human_player && unk_008b60e0 != 0 && (s.state & 8) == 0)
        {
          unk_00743038 = 0;
          return 0x20;
        }
      }

      s.instance->state &= ~0x800;
      s.result = 0;
    }
    else
    {
      if (s.in_play == 0)
      {
        if ((s.state & 0xa0) != 0)
        {
          s.instance->state &= ~0x800;
          unk_00743038 = 0;
          return 0;
        }

        single_color_test_bit_to_color_t((int)(char)global_cards_data[s.internal_card_id].color);
        if (_DAT_00742fbc == 0 || (s.type & DAT_00742f68) != 0)
        {
          if ((s.type & TYPE_LAND) != 0)
          {
            if (player == human_player && (unk_008b4278 & 1) == 0 && (current_phase == 0x14 || current_phase == 0x1e))
            {
              unk_00743038 = 0;
              return 4;
            }

            s.instance->state &= ~0x800;
            unk_00743038 = 0;
            return 0;
          }

          if ((((unk_008b35ec == human_player && (((_DAT_00742fbc != 0 && (s.type & 0x30) != 0) || current_phase == 0x14) || current_phase == 0x1e)) || (unk_008b35ec != human_player && _DAT_00742fbc != 0 && ((s.type & 0x10) != 0 || (s.type & 0x20) != 0))) && FUN_0043fdb3(player, player, card) != 0 && (((unk_008b4278 & 4) == 0 || (s.extra_ability & 0x3004) != 0) && ((s.type & 0x42) != 0 || dispatch_event_to_single_card(player, card, EVENT_CAN_CAST, 1 - player, -1) != 0))))
          {
            unk_00743038 = 0;
            return 4;
          }
        }
      }
      else
      {
        if (DAT_007aadec == 4 && (s.upkeep_flags & 1) != 0)
        {
          unk_008b3270 |= 3;
          DAT_007ab2bc |= 4;
          unk_00743038 = 0;
          return 2;
        }

        if (DAT_007aadec == 4 && (s.upkeep_flags & 0x10) != 0 && (s.upkeep_flags & 0x88) == 0 && FUN_00445b56(player, card) != 0)
        {
          DAT_007ab2bc |= 2;
          unk_00743038 = 0;
          return 8;
        }

        if ((s.state & 0x10) == 0 && (s.type & TYPE_CREATURE) != 0 && _DAT_00742fbc == 0 && player == human_player && current_phase < 0x1b && FUN_0044125c(player, card) != 0 && (((s.instance->token_status & 3) == 0) || (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0))
        {
          _DAT_00743024 = 1;
        }

        if ((((s.extra_ability & 0x1000) != 0 && (s.state & 0x10) == 0 && (((s.instance->token_status & 3) == 0) || (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0)) || ((s.extra_ability & 1) != 0 && (DAT_00742f68 & 0x10) != 0) || ((s.extra_ability & 2) != 0 && (DAT_00742f68 & 0x20) != 0)) && ((unk_008b4278 & 4) == 0 || (s.extra_ability & 0x5004) != 0))
        {
          unk_008b3270 &= ~2;
          if ((s.state & 0x20) == 0 && dispatch_event_to_single_card(player, card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
          {
            if ((unk_008b3270 & 2) != 0)
            {
              DAT_007ab2bc |= 4;
              unk_00743038 = 0;
              return 2;
            }

            DAT_007ab2bc |= 2;
            unk_00743038 = 0;
            return 8;
          }
        }
      }

      s.instance->state &= ~0x800;
      s.result = 0;
    }
  }

  unk_00743038 = 0;
  return s.result;
}

// FUNCTION: MAGIC 0x004e1cd1
void FUN_004e1cd1(void)
{
}

// FUNCTION: MOK 0x0049d510
// FUNCTION: MAGIC 0x004eaf09
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
  for (s.current_color = 0; s.current_color < 10 && unk_007161e0[player][s.current_color] != -1; ++s.current_color)
  {
    s.mana_source_color = unk_007161e0[player][s.current_color] & 0xffff;
    s.mana_color = HIWORD(unk_007161e0[player][s.current_color]);
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

  s.special_mana = unk_00742f70[player][s.effective_color];
  for (s.current_color = COLOR_COLORLESS; s.current_color < 7; ++s.current_color)
  {
    if (s.effective_color != (color_t)s.current_color && (s.interchangeable & (1 << (unsigned char)s.current_color)) != 0)
    {
      s.special_mana += unk_00742f70[player][s.current_color];
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

  s.artifact_special_mana = unk_00742f80[player][2];

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
int FUN_0043fdb3(int player, int target_player, int target_card)
{
  int internal_card_id;
  int color;
  int colored_cost;
  int colorless_cost;

  if (player == -1 || target_player == -1 || target_card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
  if (internal_card_id == -1)
  {
    return 0;
  }

  colored_cost = (int)(char)global_cards_data[internal_card_id].cc[0];
  colorless_cost = (int)(char)global_cards_data[internal_card_id].cc[1];
  if ((global_cards_data[internal_card_id].type & TYPE_ARTIFACT) == 0)
  {
    color = single_color_test_bit_to_color_t((int)(char)global_cards_data[internal_card_id].color);
    if (has_mana_w_global_cost_mod(player, target_card, color, colored_cost) == 0)
    {
      return 0;
    }

    if (colorless_cost > 0 && has_mana_w_global_cost_mod(player, target_card, COLOR_ANY, colored_cost + colorless_cost) == 0)
    {
      return 0;
    }
  }
  else if (has_mana(player, COLOR_ARTIFACT, colored_cost + colorless_cost) == 0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00442dac
void push_affected_card_stack(void)
{
  if (DAT_00561268 >= 0x20)
    return;

  DAT_0093f4c0[DAT_00561268][0] = affected_card_controller;
  DAT_0093f4c0[DAT_00561268][1] = affected_card;
  DAT_0093f4c0[DAT_00561268][2] = unk_008b4dd0;
  DAT_0093f4c0[DAT_00561268][3] = unk_008cf6d4;
  DAT_0093f4c0[DAT_00561268][4] = attacking_card_controller;
  DAT_0093f4c0[DAT_00561268][5] = attacking_card;
  DAT_0093f4c0[DAT_00561268][6] = event_result;
  DAT_00561268 += 1;
}

// FUNCTION: MAGIC 0x00442e62
void pop_affected_card_stack(void)
{
  if (DAT_00561268 > 0)
  {
    DAT_00561268 -= 1;
  }

  affected_card_controller = DAT_0093f4c0[DAT_00561268][0];
  affected_card = DAT_0093f4c0[DAT_00561268][1];
  unk_008b4dd0 = DAT_0093f4c0[DAT_00561268][2];
  unk_008cf6d4 = DAT_0093f4c0[DAT_00561268][3];
  attacking_card_controller = DAT_0093f4c0[DAT_00561268][4];
  attacking_card = DAT_0093f4c0[DAT_00561268][5];
  event_result = DAT_0093f4c0[DAT_00561268][6];
}

// FUNCTION: MAGIC 0x00445b56
int FUN_00445b56(int player, int card)
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
  if (DAT_008cf1b8 != 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x00445918
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
void FUN_0055117d(int(__cdecl *callback)(int, int, int), int who_to_check)
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
int FUN_00551638(int player, unsigned int preferred_controller, int card)
{
  int result;
  unsigned char target_number;
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  result = C_real_select_target(player,
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
                                &selected_target);
  if (result != 0)
  {
    target_number = global_card_instances[player][card].number_of_targets;
    global_card_instances[player][card].targets[target_number] = selected_target;
    ++global_card_instances[player][card].number_of_targets;
  }

  return result != 0;
}

// FUNCTION: MAGIC 0x004eab5c
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
void declare_mana_available(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
}

// FUNCTION: MAGIC 0x004eadeb
int undeclare_mana_available(int player, color_t color, int amount)
{
  unk_00742f70[player][color] -= amount;
  unk_00742f80[player][3] -= amount;
  return unk_00742f70[player][color];
}

// FUNCTION: MAGIC 0x004eaec1
int undeclare_mana_available_and_produce_it(int player, color_t color, int amount)
{
  undeclare_mana_available(player, color, amount);
  produce_mana(player, color, amount);
  return raw_mana_available[player][color];
}

// FUNCTION: MAGIC 0x005513d7
void FUN_005513d7(int player, int card, int amount)
{
  if ((PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffu) == 0xffu)
    return;

  PLAYER_CARD_INSTANCE(player, card).special_counters =
      ((PLAYER_CARD_INSTANCE(player, card).special_counters + amount) & 0xff) |
      (PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffffff00);

  if (unk_008a9000 != 1)
  {
    play_sound_effect(ICON_HASTE);
  }
}

// FUNCTION: MAGIC 0x004a686a
int choose_a_color(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors)
{
  int chosen_color;

  if (unk_008a9000 == 1)
  {
    return ai_choice;
  }

  if (active_player == player && (unk_00926804 & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xf);
    return DAT_008b293c;
  }

  chosen_color = choose_a_color_dialog(player, prompt, use_color_names_instead_of_land, ai_choice, available_colors);
  if (player == unk_008b35ec && (unk_00926804 & 2) != 0)
  {
    unk_008b2938 = '\x0f';
    DAT_008b293c = chosen_color;
    TENTATIVE_send_network_result(player, 0xf);
  }

  return chosen_color;
}

// FUNCTION: MOK 0x0044e050
// FUNCTION: MAGIC 0x004e1d6d
int FUN_004e1d6d()
{
  return 0;
}

// FUNCTION: MAGIC 0x004e1d91
int FUN_004e1d91()
{
  return 1;
}

// FUNCTION: MAGIC 0x004e1da6
int FUN_004e1da6()
{
  return 1;
}

// FUNCTION: MAGIC 0x004e1c81
int FUN_004e1c81()
{
}

// FUNCTION: MAGIC 0x00464a57
int internal_rand(int maximum)
{
  return (maximum > 1) ? rand() % maximum : 0;
}

// FUNCTION: MAGIC 0x00464a84
int FUN_00464a84(int player, int maximum)
{
  if (player == unk_008b35ec)
  {
    if (maximum > 1)
    {
      unk_007a7d0c = rand() % maximum;
    }
    else
    {
      unk_007a7d0c = 0;
    }

    unk_007a7d08 = 0x18;
    TENTATIVE_send_network_result(player, 0x18);
  }
  else
  {
    TENTATIVE_wait_for_network_result(player, 0x18);
  }

  return unk_007a7d0c;
}

// FUNCTION: MAGIC 0x004e4f11
void FUN_004e4f11(void)
{
  if (unk_00743094 < 0x100)
  {
    unk_006a21b8[unk_00743094] = unk_00925bb8;
    unk_006a8258[unk_00743094] = PLAYER_CARD_INSTANCE((unk_00925bb8 & 0x100) >> 8, unk_00925bb8 & 0xff).internal_card_id;
    unk_006a8660[unk_00743094] = unk_0057aae8;
    unk_006a6ee8[unk_00743094] = unk_00939340;
    ++unk_00743094;
    if (unk_006a6ee8[0] == 99 || unk_006a6ae8[0] == 99)
    {
      unk_00925bb8 = -1;
    }
  }
  else
  {
    spell_fizzled = 1;
  }

  unk_0057aae8 = 0;
}

// FUNCTION: MAGIC 0x004e5089
void FUN_004e5089(void)
{
  if (unk_006a5f18[unk_00743094] != unk_0057aae8)
  {
    unk_0057aae8 |= 0x100;
  }

  unk_00925bb8 = unk_006a1db8[unk_00743094];
  unk_00939340 = unk_006a6ae8[unk_00743094];
  if (unk_00939340 != 99)
  {
    ++unk_00743094;
  }

  unk_0057aae8 = 0;
}

// FUNCTION: MAGIC 0x0048463d
int FUN_0048463d(int player, int card, int amount)
{
  struct
  {
    int result;
    int internal_card_id;
  } s;
  card_instance_t *player_instances;

  s.result = 0;
  player_instances = global_card_instances[player];

  if (player_instances[card].internal_card_id == -1)
  {
    s.internal_card_id = player_instances[card].original_internal_card_id;
  }
  else
  {
    s.internal_card_id = player_instances[card].internal_card_id;
  }

  if (player == -1 || card == -1)
  {
    s.result = 0;
  }
  else
  {
    if ((char)global_cards_data[s.internal_card_id].subtype == 13 && global_cards_data[s.internal_card_id].id == unk_0092664c[amount])
    {
      s.result = 1;
    }
    if ((char)global_cards_data[s.internal_card_id].subtype == 12 && ((1 << (unsigned char)amount) & (int)(char)player_instances[card].mana_color) != 0)
    {
      s.result = 1;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004817fd
void FUN_004817fd(int player)
{
  int ante_slot;
  int card_in_hand;

  for (ante_slot = 0; ante_slot < 0x10 && *(int *)&global_ante_cards[player][ante_slot * 4] != -1; ++ante_slot)
  {
  }

  if (ante_slot < 0x10)
  {
    card_in_hand = FUN_0043e18b(player);
    *(int *)&global_ante_cards[player][ante_slot * 4] = PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id;
    --hand_count[player];
    load_text("promptsX1.txt", "ANTE_A_CARD");
    if (unk_008b35ec == player)
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[0], 0);
    }
    else
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[1], 0);
    }

    PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id = -1;
  }
}

// FUNCTION: MAGIC 0x00483489
int FUN_00483489(int player, int card, int amount)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if (amount == -1)
  {
    return 0;
  }

  if (global_cards_data[amount].code_pointer == (int)global_cards_data[internal_card_id].code_pointer)
  {
    return 1;
  }

  if ((int)*(char *)&global_cards_data[internal_card_id].subtype == 0xc)
  {
    if ((int)*(char *)&global_cards_data[amount].subtype != 0xc)
    {
      if (((int)*(char *)&global_cards_data[amount].color & (int)*(char *)&global_cards_data[internal_card_id].color) != 0)
      {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004bff5a
int FUN_004bff5a(int player, int card)
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
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_009266a4 &&
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
int FUN_004c081a(int player, int card)
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
int FUN_004c0880(int internal_card_id, int extra)
{
  int result;

  result = 0;
  if (extra == 0 && (extra == internal_card_id || global_cards_data[internal_card_id].id == unk_0092664c[1]))
  {
    result = 1;
  }
  if (extra == 1 && (extra == internal_card_id || global_cards_data[internal_card_id].id == unk_0092664c[2]))
  {
    result = 1;
  }
  if (extra == 2 && (extra == internal_card_id || global_cards_data[internal_card_id].id == unk_0092664c[3]))
  {
    result = 1;
  }
  if (extra == 3 && (extra == internal_card_id || global_cards_data[internal_card_id].id == unk_0092664c[4]))
  {
    result = 1;
  }
  if (extra == 4 && (extra == internal_card_id || global_cards_data[internal_card_id].id == unk_0092664c[5]))
  {
    result = 1;
  }
  return result;
}

// FUNCTION: MAGIC 0x004848a0
int gain_life(int player, int amount)
{
  int saved_trigger_cause_controller;
  int saved_trigger_cause;
  int num_cards_drawn;

  life[player] += amount;

  if ((unk_007abc78 & 0x00200000) != 0)
  {
    saved_trigger_cause_controller = trigger_cause_controller;
    saved_trigger_cause = trigger_cause;
    push_affected_card_stack();

    trigger_cause_controller = player;
    trigger_cause = -1;
    life_gained = amount;
    dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_GAIN_LIFE, &gs_gain_life_007895e0[0], 0);

    trigger_cause_controller = saved_trigger_cause_controller;
    trigger_cause = saved_trigger_cause;
    pop_affected_card_stack();
  }

  if (unk_008b44d0[player] != 0)
  {
    for (num_cards_drawn = 0; num_cards_drawn < amount; ++num_cards_drawn)
    {
      FUN_0043e18b(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00445663
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4)
{
  dispatch_trigger(reason_for_trig, trig, prompt, a4);
  dispatch_trigger(1 - reason_for_trig, trig, prompt, a4);
  return 1;
}

// FUNCTION: MAGIC 0x004b1670
void FUN_004b1670(int player, int internal_card_id)
{
  int exile_index;

  if (unk_008a9000 != 1 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
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
int FUN_004b0c12(int player, int card)
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
        FUN_004b1670((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) >> 12,
                     PLAYER_CARD_INSTANCE(player, card).original_internal_card_id);
      }
      else
      {
        if (s.kill_mode != 3 && unk_008a9000 != 1)
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

        FUN_004b14f5(player, card);
        if (s.kill_mode == 3)
        {
          dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_GRAVEYARD_FROM_PLAY, gs_cards_to_graveyard_008a8ed0, 0);
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
    dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_LEAVE_PLAY, gs_card_leaving_play_007aaef0, 0);
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
  if (unk_008a9000 != 1)
  {
    FUN_004a61a1(player, card, 7, 2);
  }

  FUN_004b117e(player, card);
  if (((unsigned int)(unsigned char)global_cards_data[s.internal_card_id].type & 0x1047) != 0)
  {
    FUN_00441d78();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b08e6
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
    FUN_004b0c12(player, card);
  }
  else
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = kill_mode;
    PLAYER_CARD_INSTANCE(player, card).state |= 2;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0xd6;
    DAT_007ab2cc = FUN_004b0c12;
  }
}

// FUNCTION: MAGIC 0x004b117e
void FUN_004b117e(int player, int card)
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

  if ((active_player == player && (unk_00926804 & 2) == 0 && (hand_count[player] + unk_007161d8) <= 0))
    return;

  if (((unk_008b35ec == player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1) && flags == 0)
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
    if ((unk_00926804 & 2) != 0 && active_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x14);
      s.selected_card = unk_007a7d0c;
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

      if ((unk_00926804 & 2) != 0 && unk_008b35ec == player)
      {
        if (s.found == 0)
        {
          s.selected_card = -1;
        }
        unk_007a7d0c = s.selected_card;
        unk_007a7d08 = 0x14;
        TENTATIVE_send_network_result(player, 0x14);
      }
    }
  }

  if ((unk_007a79b0[player] & 2) == 0 || player_who_controls_effect != 0)
  {
    if (active_player == player && (unk_00926804 & 2) == 0 && unk_008a9000 != 1)
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
    FUN_0043ec38(player, s.selected_card);
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
      FUN_004a61a1(player, s.selected_card, 10, 1);
      if (unk_008a9000 != 1)
      {
        play_sound_effect(WAV_DISCARD);
      }
      --hand_count[player];
    }
    else
    {
      FUN_0043ec38(player, s.selected_card);
    }
  }
}

// FUNCTION: MAGIC 0x004eae35
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
int FUN_004c0a36(int a1, int a2, int a3)
{
  if (a1 < a2)
  {
    return a2;
  }
  if (a1 > a3)
  {
    return a3;
  }
  return a1;
}

// FUNCTION: MOK 0x0042de60
// FUNCTION: MAGIC 0x00432f00
int charge_mana(int player, color_t color, int amount)
{
  struct
  {
    int idx8;               /* [ebp-0xd4] */
    int saved_raw_mana[8];  /* [ebp-0xd0] */
    int saved_x_value;      /* [ebp-0xb0] */
    int saved_mana_cost[8]; /* [ebp-0xac] */
    int saved_max_x_value;  /* [ebp-0x8c] */

    card_instance_t *instance;            /* [ebp-0x88] */
    card_data_t *card_data;               /* [ebp-0x84] */
    int available_mana;                   /* [ebp-0x80] */
    int max_variable_mana;                /* [ebp-0x7c] */
    int selected_card;                    /* [ebp-0x78] */
    int current_internal_card_id;         /* [ebp-0x74] */
    int cost_color;                       /* [ebp-0x70] */
    int amount_to_take;                   /* [ebp-0x6c] */
    int slot;                             /* [ebp-0x68] */
    int color_count;                      /* [ebp-0x64] */
    int chosen_color;                     /* [ebp-0x60] */
    int old_tapped_state;                 /* [ebp-0x5c] */
    color_test_t produced_special_colors; /* [ebp-0x58] */
    color_test_t payable_colors;          /* [ebp-0x54] */

    int allow_special_x_payment;            /* [ebp-0x50] */
    int mana_paid_by_color[7];              /* [ebp-0x4c] */
    int allow_generic_payment;              /* [ebp-0x30] */
    int any_variable_costs;                 /* [ebp-0x2c] */
    int allow_mana_sources;                 /* [ebp-0x28] */
    int allow_interrupt_speed_mana_sources; /* [ebp-0x24] */
    int max_colorless;                      /* [ebp-0x20] */
    int done;                               /* [ebp-0x1c] */
    int i;                                  /* [ebp-0x18] */
    int only_variable_costs;                /* [ebp-0x14] */
    int result;                             /* [ebp-0x10] */
    int mana_paid_total;                    /* [ebp-0x0c] */
    int temp;                               /* [ebp-0x08] */
    int include_auto_payment;               /* [ebp-0x04] */
  } s;

  if ((unk_008b4278 & LCBP_CARD_BEING_COPIED) != 0)
  {
    goto skipped_for_copied_card;
  }

  {
    if (DAT_008ce52c > 0)
    {
      if (color == COLOR_COLORLESS)
      {
        if (amount != -1)
        {
          s.result = amount - DAT_008ce52c * 2;
          if (s.result <= 1)
          {
            s.result = 1;
          }
          amount = s.result;
        }
      }
      else if (unk_008ce510[COLOR_COLORLESS] > 0)
      {
        s.result = unk_008ce510[COLOR_COLORLESS] - DAT_008ce52c * 2;
        if (s.result <= 1)
        {
          s.result = 1;
        }
        unk_008ce510[COLOR_COLORLESS] = s.result;
      }
    }

    unk_008ce510[color] += amount;
    s.mana_paid_total = 0;
    for (s.i = 0; s.i < 7; ++s.i)
    {
      s.mana_paid_by_color[s.i] = 0;
    }

    s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
    s.max_colorless -= has_mana(player, COLOR_ANY, 1);

    s.include_auto_payment = 1;
    if (unk_00715fb0 == 0 && ((player != active_player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1 && unk_009252e0 == 0))
    {
      s.allow_interrupt_speed_mana_sources = 0;
      s.allow_mana_sources = 0;
      s.allow_generic_payment = 0;
      s.allow_special_x_payment = 0;
    }
    else
    {
      s.allow_special_x_payment = 1;
      s.allow_generic_payment = 1;
      s.allow_mana_sources = 1;
      if ((player == active_player && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
      {
        s.allow_interrupt_speed_mana_sources = 1;
      }
      else
      {
        s.allow_interrupt_speed_mana_sources = 0;
      }
    }

    s.temp = 1;

    if (player == active_player && (unk_00926804 & 2) == 0)
    {
      s.max_variable_mana = max_x_value;
      for (s.i = COLOR_COLORLESS; s.i < 7; ++s.i)
      {
        if (unk_008ce510[s.i] == -1)
        {
          s.available_mana = has_mana(player, s.i, 1);
          if (unk_008a9000 == 1)
          {
            s.result = internal_rand(3);
            if (s.result == 0 || s.available_mana < 2)
            {
              s.old_tapped_state = s.available_mana;
            }
            else
            {
              s.old_tapped_state = internal_rand(s.available_mana - 1) + 1;
            }

            if (max_x_value != -1 && s.old_tapped_state > max_x_value)
            {
              s.old_tapped_state = max_x_value;
            }

            unk_00939340 = s.old_tapped_state;
            FUN_004e4f11();
          }
          else
          {
            FUN_004e5089();
            if (unk_00939340 == 99)
            {
              unk_00939340 = 0;
            }
            s.old_tapped_state = unk_00939340;
          }

          s.max_variable_mana = s.old_tapped_state;
        }
      }

      max_x_value = s.max_variable_mana;
    }

    if (DAT_008ce52c < 1)
    {
      x_value = 0;
    }
    else
    {
      x_value = DAT_008ce52c * 2;
    }

    if (max_x_value == 0)
    {
      for (s.i = 0; s.i < 7; ++s.i)
      {
        if (unk_008ce510[s.i] == -1)
        {
          unk_008ce510[s.i] = 0;
        }
      }
    }

    s.payable_colors = 0;
    for (s.i = 0; s.i < 7; ++s.i)
    {
      if (unk_008ce510[s.i] == -1)
      {
        s.payable_colors = 1;
      }
    }

    if (s.include_auto_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      FUN_0043410a(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
      copy_mana_pool_to_display();
    }

    if (s.allow_special_x_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0 && s.payable_colors != 0)
    {
      FUN_004342b3(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_generic_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      FUN_004344b0(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_mana_sources != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1e, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1c, s.temp);
      }
    }

    if (s.allow_interrupt_speed_mana_sources != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x14, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x04, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1a, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x18, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x10, s.temp);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x00, s.temp);
      }
    }

    if (s.allow_interrupt_speed_mana_sources != 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0 && s.payable_colors == 0)
      {
        spell_fizzled = 1;
      }
    }
    else if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      s.done = 0;
      while (s.done == 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        s.only_variable_costs = 1;
        for (s.i = 0; s.i < 7; ++s.i)
        {
          if (unk_008ce510[s.i] > 0)
          {
            s.only_variable_costs = 0;
          }
        }

        FUN_0043479e(unk_00748770, unk_008ce510, x_value, max_x_value);
        s.selected_card =
            FUN_004e9c50(player, player, player, 0, 0, unk_00748770, s.only_variable_costs != 0 ? 3 : 1);

        if (unk_00742fcc == -1 && (s.selected_card == -1 || s.selected_card == -2))
        {
          if (_DAT_0074303c == -2)
          {
            if (unk_00716244 == -1 && unk_00716248 == -1)
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
          else if (_DAT_0074303c == -3 && player == DAT_00715fa4 && DAT_0072c8e0 != -1)
          {
            if (raw_mana_available[player][DAT_0072c8e0] > 0 && (unk_008ce510[DAT_0072c8e0] != 0 || unk_008ce510[COLOR_COLORLESS] != 0 || unk_008ce510[COLOR_ARTIFACT] != 0) && (DAT_0072c8e0 != COLOR_ARTIFACT || unk_008ce510[COLOR_ARTIFACT] != 0))
            {
              if (unk_008ce510[DAT_0072c8e0] == 0)
              {
                if (unk_008ce510[COLOR_ARTIFACT] == 0)
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
                  FUN_00435b88(unk_008ce510, s.cost_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
              FUN_00435b18(
                  unk_008ce510, s.cost_color, s.amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
              copy_mana_pool_to_display();
            }

            if (raw_mana_available[player][DAT_0072c8e0] > 0)
            {
              s.produced_special_colors = 0;
              for (s.slot = 0; s.slot < 10 && unk_007161e0[player][s.slot] != -1; ++s.slot)
              {
                if (LOWORD(unk_007161e0[player][s.slot]) == DAT_0072c8e0)
                {
                  s.produced_special_colors |= 1 << HIWORD(unk_007161e0[player][s.slot]);
                }
              }

              s.selected_card = 0;
              for (s.i = 0; s.i < 7; ++s.i)
              {
                if (unk_008ce510[s.i] != 0)
                {
                  s.selected_card |= 1 << ((unsigned char)s.i & 0x1f);
                }
              }

              s.produced_special_colors &= s.selected_card;
              if (s.produced_special_colors != 0)
              {
                s.color_count = 0;
                for (s.i = 0; s.i < 7; ++s.i)
                {
                  if ((s.produced_special_colors & (1 << ((unsigned char)s.i & 0x1f))) != 0)
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
                  load_text("promptsX1.txt", "PROMPT_GRABMANA_DUALUSE");
                  s.chosen_color = choose_a_color(player, text_lines[0], 1, DAT_0072c8e0, s.produced_special_colors);
                }

                s.amount_to_take =
                    FUN_00435b88(unk_008ce510, s.chosen_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
                FUN_00435b18(
                    unk_008ce510, s.chosen_color, s.amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
                copy_mana_pool_to_display();
              }
            }
          }
        }
        else
        {
          s.instance = &PLAYER_CARD_INSTANCE(player, s.selected_card);
          s.current_internal_card_id = s.instance->internal_card_id;
          s.card_data = &global_cards_data[s.current_internal_card_id];

          if ((unk_00742fcc == -1 || s.selected_card != -1) && (s.card_data->extra_ability & EA_MANA_SOURCE) != 0 &&
              ((s.card_data->type & TYPE_INTERRUPT) != 0 ||
               (((s.instance->state & STATE_IN_PLAY) != 0) && (s.instance->color & 0x80) == 0 && (s.instance->state & STATE_TAPPED) == 0 &&
                (((s.instance->color & 3) == 0) || (s.card_data->type & TYPE_CREATURE) == 0))))
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
              s.saved_mana_cost[s.idx8] = unk_008ce510[s.idx8];
              unk_008ce510[s.idx8] = 0;
            }

            if ((s.instance->state & 0x800002) == STATE_IN_PLAY)
            {
              if ((s.card_data->type & TYPE_LAND) != 0 || dispatch_event_to_single_card(player, s.selected_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
              {
                FUN_00443ee2(player, s.selected_card, EVENT_ACTIVATE, player, 0);
                unk_00938e2c = FUN_00434746(s.saved_mana_cost);
                unk_0092664c[6] = -1;
                s.old_tapped_state = s.instance->state & STATE_TAPPED;
                dispatch_event_to_single_card(player, s.selected_card, EVENT_ACTIVATE, 1 - player, -1);
                unk_00938e2c = 0;
                if (spell_fizzled == 1)
                {
                  spell_fizzled = 0;
                  obliterate_top_card_of_stack();
                }
                else
                {
                  if (s.old_tapped_state == 0 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & STATE_TAPPED) != 0)
                  {
                    dispatch_event(player, s.selected_card, EVENT_TAP_CARD);
                  }
                  if (unk_008a9000 != 1)
                  {
                    play_sound_effect(WAV_TAP);
                  }
                  resolve_top_card_on_stack();
                  TENTATIVE_reassess_all_cards();
                }
              }
            }
            else
            {
              FUN_0043ece1(player, s.selected_card);
              TENTATIVE_reassess_all_cards();
            }

            x_value = s.saved_x_value;
            max_x_value = s.saved_max_x_value;
            for (s.idx8 = 0; s.idx8 < 7; ++s.idx8)
            {
              unk_008ce510[s.idx8] = s.saved_mana_cost[s.idx8];
            }

            for (s.idx8 = 0; s.idx8 <= 7; ++s.idx8)
            {
              raw_mana_available[player][s.idx8] -= s.saved_raw_mana[s.idx8];
            }

            s.max_colorless = 0;
            FUN_0043410a(player, s.mana_paid_by_color, &s.mana_paid_total, 0);
            FUN_004342b3(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
            FUN_004344b0(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);

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

skipped_for_copied_card:
  if (spell_fizzled == 1)
  {
    FUN_00435db0(s.mana_paid_by_color);
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
    unk_008ce510[s.i] = 0;
  }
  max_x_value = -1;

  return s.mana_paid_total;
}

// FUNCTION: MAGIC 0x004346ab
int FUN_004346ab(int *mana_cost, int required_colorless, int max_x)
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
unsigned int FUN_00434746(int *mana)
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
int FUN_0043479e(char *prompt, int *mana_cost, int x_paid, int max_x)
{
  struct
  {
    char symbol[8];      /* [ebp-0x1b8] */
    int total_generic;   /* [ebp-0x1b0] */
    char tmp[300];       /* [ebp-0x1ac] */
    char mana_text[100]; /* [ebp-0x80] */
    int stack_card;      /* [ebp-0x1c] */
    size_t local_1c;     /* [ebp-0x18] (intentionally unused, but stabilizes layout) */
    unsigned int action; /* [ebp-0x14] */
    int i;               /* [ebp-0x10] */
    int color;           /* [ebp-0x0c] */
    unsigned int msg;    /* [ebp-0x08] */
    int stack_player;    /* [ebp-0x04] */
  } s;

  (void)prompt;

  strcpy(s.tmp, "");

  s.action = FUN_00443898();
  if (s.action != 0xffffffffU)
  {
    s.msg = (s.action >> 16) & 0xff;
    s.stack_player = DAT_009251d8[unk_008b2934].x;
    s.stack_card = DAT_009251d8[unk_008b2934].y;
    FUN_00444c43(s.tmp, s.msg, s.stack_player, s.stack_card);
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

  strcpy(unk_00748770, s.tmp);
  strcat(unk_00748770, ", ");
  sprintf(s.tmp, gs_tap_for_mana_0091c230, s.mana_text);
  strcat(unk_00748770, s.tmp);

  return 0;
}

// FUNCTION: MAGIC 0x00435c91
int FUN_00435c91(int color, int amount)
{
  if (0 < unk_005607e0)
  {
    unk_00637558[unk_005607e0 - 1][color] += amount;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x004eae77
int unproduce_mana(int player, int color, int amount)
{
  raw_mana_available[player][color] -= amount;
  raw_mana_available[player][7] -= amount;
  return raw_mana_available[player][color];
}

// FUNCTION: MAGIC 0x00435b18
void FUN_00435b18(int *mana_cost,
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
  FUN_00435c91(mana_color, amount);
  *total_mana_paid += amount;
}

// FUNCTION: MAGIC 0x0043410a
void FUN_0043410a(int player, int *mana_paid_by_color, int *total_mana_paid, int max_colorless)
{
  unsigned short produced_color;
  unsigned int special_mana;
  int slot;
  int color;

  for (color = 0; color < 7; ++color)
  {
    while (unk_008ce510[color] > 0 && raw_mana_available[player][color] > 0)
    {
      FUN_00435b18(unk_008ce510, color, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
    }
  }

  while (unk_008ce510[6] > 0 && raw_mana_available[player][0] > 0 && max_colorless < unk_008ce510[6])
  {
    FUN_00435b18(unk_008ce510, 6, 1, (int *)0, 0, player, 0, mana_paid_by_color, total_mana_paid);
  }

  slot = 0;
  while (slot < 10 && unk_007161e0[player][slot] != -1)
  {
    produced_color = (unsigned short)unk_007161e0[player][slot];
    special_mana = (unsigned int)unk_007161e0[player][slot];
    while (raw_mana_available[player][produced_color] > 0 && unk_008ce510[special_mana >> 16] > 0)
    {
      FUN_00435b18(
          unk_008ce510, special_mana >> 16, 1, (int *)0, 0, player, produced_color, mana_paid_by_color, total_mana_paid);
    }
    slot = slot + 1;
  }
}

// FUNCTION: MAGIC 0x004342b3
void FUN_004342b3(int player,
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
    if (unk_008ce510[color] == -1)
    {
      while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
      {
        FUN_00435b18(
            unk_008ce510, color, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
      }
    }
  }

  if (unk_008ce510[6] == -1)
  {
    while (raw_mana_available[player][0] > 0 && max_colorless < unk_008ce510[6] &&
           (x_value < max_x || max_x == -1))
    {
      FUN_00435b18(
          unk_008ce510, 6, 1, special_mana_override, max_x, player, 0, mana_paid_by_color, total_mana_paid);
    }
  }

  slot = 0;
  while (slot < 10 && unk_007161e0[player][slot] != -1)
  {
    produced_color = (unsigned short)unk_007161e0[player][slot];
    special_mana = (unsigned int)unk_007161e0[player][slot];
    if (unk_008ce510[special_mana >> 16] == -1)
    {
      while (raw_mana_available[player][produced_color] > 0 && (x_value < max_x || max_x == -1))
      {
        FUN_00435b18(unk_008ce510,
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
void FUN_004344b0(int player,
                  int *mana_paid_by_color,
                  int *total_mana_paid,
                  int max_colorless,
                  int *special_mana_override,
                  int max_x)
{
  int color;

  if (unk_008ce510[6] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (unk_008ce510[6] < 1)
      {
        while (raw_mana_available[player][color] > 0 && unk_008ce510[6] == -1 &&
               (max_x == -1 || max_colorless < max_x - x_value))
        {
          FUN_00435b18(
              unk_008ce510, 6, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
      else
      {
        while (raw_mana_available[player][color] > 0 && max_colorless < unk_008ce510[6])
        {
          FUN_00435b18(unk_008ce510, 6, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
    }
  }

  if (unk_008ce510[0] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (color != 6)
      {
        if (unk_008ce510[0] == -1)
        {
          while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
          {
            FUN_00435b18(
                unk_008ce510, 0, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
          }
        }
        else
        {
          while (unk_008ce510[0] > 0 && raw_mana_available[player][color] > 0)
          {
            FUN_00435b18(unk_008ce510, 0, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00435b88
int FUN_00435b88(int *mana_cost,
                 int cost_color,
                 int *mana_available,
                 int mana_color,
                 int allow_multi_pay,
                 int max_x,
                 int x_paid)
{
  int amount;

  if (allow_multi_pay == 0)
  {
    amount = 1;
  }
  else if (mana_cost[cost_color] == -1)
  {
    if (max_x == -1)
    {
      amount = mana_available[mana_color];
    }
    else
    {
      amount = mana_available[mana_color];
      if (max_x - x_paid <= amount)
      {
        amount = max_x - x_paid;
      }
    }
  }
  else
  {
    amount = mana_cost[cost_color];
    if (mana_available[mana_color] <= amount)
    {
      amount = mana_available[mana_color];
    }
  }

  return amount;
}

// FUNCTION: MAGIC 0x00435db0
int FUN_00435db0(int *mana_paid_by_color)
{
  int color;

  if (unk_005607e0 > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      unk_00637558[unk_005607e0 - 1][color] -= mana_paid_by_color[color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435643
int FUN_00435643(int player, int card, unsigned char activation_flags)
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
int FUN_00435881(int player, int card)
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

  x_value = 0;
  s.old_max_x_value = max_x_value;
  max_x_value = -1;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    s.saved_mana_cost[s.color] = unk_008ce510[s.color];
    unk_008ce510[s.color] = 0;
  }
  FUN_00443ee2(player, card, 0x72, player, 0);
  unk_00938e2c = FUN_00434746(s.saved_mana_cost);
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
    if (unk_008a9000 != 1)
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
    unk_008ce510[s.color] = s.saved_mana_cost[s.color];
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00434af9
int FUN_00434af9(int player,
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
    if (unk_008ce510[s.color] == -1)
    {
      s.any_variable_costs = 1;
    }
  }

  if (player == active_player && (unk_00926804 & 2) != 0)
  {
    result = 0;
    while (result != -1)
    {
      TENTATIVE_wait_for_network_result(player, 0x10);
      result = unk_007a7d0c;
      if (unk_007a7d0c != -1 && FUN_00435881(player, unk_007a7d0c) != 0)
      {
        s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
        s.current_card = has_mana(player, COLOR_ANY, 1);
        s.max_colorless -= s.current_card;
        FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
        FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
        FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
      }
    }
    copy_mana_pool_to_display();
  }
  else
  {
    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          for (s.color = 0; s.color < 7; ++s.color)
          {
            if (unk_008ce510[s.color] > 0 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
            else if (unk_008ce510[s.color] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
          }
          if (include_special_mana != 0 && !s.can_activate)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && unk_007161e0[player][s.dual_mana_slot] != -1)
            {
              s.produced_color = (unsigned char)unk_007161e0[player][s.dual_mana_slot];
              if (unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] > 0 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              else if (unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.can_activate)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0 && (unk_008ce510[0] != 0 || unk_008ce510[6] != 0))
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          if (unk_008ce510[0] < 1)
          {
            if (unk_008ce510[0] == -1 && x_value < max_x_value && max_x_value != -1)
            {
              s.can_activate = 1;
            }
            else if (unk_008ce510[6] < 1)
            {
              if (unk_008ce510[6] == -1 && x_value < max_x_value && max_x_value != -1)
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
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && unk_008ce510[6] == 0)
          {
            s.can_activate = 0;
          }
          if (s.can_activate)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0 && s.any_variable_costs && max_x_value == -1)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.any_variable_costs = 0;
          for (s.color = 1; s.color < 7; ++s.color)
          {
            if (max_x_value == -1 && unk_008ce510[s.color] == -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && unk_008ce510[0] == -1)
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && unk_008ce510[6] == -1)
            {
              s.any_variable_costs = 1;
            }
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && unk_008ce510[6] != -1)
          {
            s.any_variable_costs = 0;
          }
          if (include_special_mana != 0 && !s.any_variable_costs)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && unk_007161e0[player][s.dual_mana_slot] != -1)
            {
              s.special_color = (unsigned char)unk_007161e0[player][s.dual_mana_slot];
              if (max_x_value == -1 && unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] == -1 && (((unsigned int)(1 << s.special_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.any_variable_costs = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.any_variable_costs)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    if ((unk_00926804 & 2) != 0)
    {
      unk_007a7d08 = 0x10;
      unk_007a7d0c = -1;
      TENTATIVE_send_network_result(player, 0x10);
    }
    copy_mana_pool_to_display();
  }

  return 1;
}

// FUNCTION: MAGIC 0x004e9c50
int FUN_004e9c50(int player,
                 int arg_2,
                 int player_to_check,
                 unsigned int required_type,
                 unsigned int required_color,
                 char *prompt,
                 int dialog_mode)
{
  struct
  {
    unsigned int local_210; /* padding field to stabilize stack layout */
    unsigned int local_20c;
    int local_208;
    int local_204;
    int candidate_card[60];   /* ebp - 0x200 */
    int choice_idx;           /* ebp - 0x110 */
    int num_candidates;       /* ebp - 0x10c */
    int selected_player;      /* ebp - 0x108 */
    int return_value;         /* ebp - 0x104 */
    int internal_id;          /* ebp - 0x100 (also used to hold FUN_00443898() result) */
    int current_card;         /* ebp - 0xfc */
    int current_player;       /* ebp - 0xf8 */
    int candidate_player[60]; /* ebp - 0xf4 */
    int pad_end;
  } s;

  if (spell_fizzled == 1 || (unk_008a9000 == 1 && unk_008b35ec == player))
  {
    return -1;
  }

  if ((DAT_00777854 & 1) != 0)
  {
    player_to_check = arg_2;
  }

  if ((active_player == player && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
  {
    s.num_candidates = 0;

    for (s.current_player = 0; s.current_player < 2; s.current_player++)
    {
      if (player_to_check == -1 || player_to_check == s.current_player)
      {
        for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; s.current_card++)
        {
          if (required_type == (unsigned int)-2)
          {
            continue;
          }

          if (PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id == -1)
          {
            continue;
          }

          if (((PLAYER_CARD_INSTANCE(s.current_player, s.current_card).state & 0x800002) != 2) &&
              !(unk_008b35ec == player && unk_009252e0 != 0))
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

          s.candidate_player[s.num_candidates] = s.current_player;
          s.candidate_card[s.num_candidates] = s.current_card;
          s.num_candidates++;
        }

        if (((int)required_type <= 0) && (required_color == 1 || required_color == 0))
        {
          s.candidate_player[s.num_candidates] = s.current_player;
          s.candidate_card[s.num_candidates] = -1;
          s.num_candidates++;
        }
      }
    }

    if (s.num_candidates == 0)
    {
      return -1;
    }

    if (unk_008b35ec == player)
    {
      while (1)
      {
        while (1)
        {
          do
          {
            do
            {
              s.choice_idx = internal_rand(s.num_candidates);
              unk_00742fcc = s.candidate_player[s.choice_idx];

              if (unk_009252e0 == 0)
              {
                return s.candidate_card[s.choice_idx];
              }

              _DAT_0074303c = 0;
              s.local_208 = internal_rand(0x20);
              if (s.local_208 == 0 || _DAT_00742fbc != 0)
              {
                unk_00716244 = -1;
                unk_00716248 = -1;
                _DAT_0074303c = (int)0xfffffffeU;
                return -1;
              }
            } while (unk_008b35ec != unk_00742fcc);

            s.internal_id = PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_card[s.choice_idx]).internal_card_id;
            s.local_20c = global_cards_data[s.internal_id].type;
            s.local_204 = PLAYER_CARD_INSTANCE(unk_00742fcc, s.candidate_card[s.choice_idx]).state;
          } while ((((s.local_20c & 1) != 0) && ((s.local_204 & 2) != 0)) ||
                   (((s.local_20c & 2) != 0) && ((s.local_204 & 4) != 0)));

          if (current_phase < 0x15 || 0x1d < current_phase)
          {
            break;
          }

          if (((s.local_20c & 2) != 0) && ((s.local_204 & 2) != 0))
          {
            return s.candidate_card[s.choice_idx];
          }
        }

        if (((s.local_20c & 0x4b) != 0) && ((s.local_204 & 2) == 0))
        {
          break;
        }
      }

      return s.candidate_card[s.choice_idx];
    }

    if (unk_008a9000 == 1)
    {
      unk_00939340 = internal_rand(s.num_candidates);
      unk_00925bb8 = ((s.candidate_player[unk_00939340] == 0) - 1) & 0x100;
      unk_00925bb8 |= s.candidate_card[unk_00939340] & 0xff;
      unk_00925bb8 &= 0x1ff;
      unk_00925bb8 |= 0x4000;
      FUN_004e4f11();
    }
    else
    {
      FUN_004e5089();
      if (unk_00939340 == 99)
      {
        unk_00939340 = internal_rand(s.num_candidates);
      }
    }

    unk_00742fcc = s.candidate_player[unk_00939340];
    return s.candidate_card[unk_00939340];
  }

  if (required_type != 0 && required_type != 0xff)
  {
    strcpy(unk_00748770, "");
    s.internal_id = FUN_00443898();
    if (s.internal_id != -1)
    {
      s.local_204 = ((unsigned int)s.internal_id >> 0x10) & 0xff;
      s.candidate_card[0] = DAT_009251d8[unk_008b2934].x;
      s.local_208 = DAT_009251d8[unk_008b2934].y;
      FUN_00444c43(unk_00748770,
                   s.local_204,
                   s.candidate_card[0],
                   s.local_208);
    }
  }

  if (required_type == 0 || required_type == 0xff || required_type == (unsigned int)-2)
  {
    s.local_210 = 0xffffffffU;
  }
  else
  {
    s.local_210 = required_type;
  }

  FUN_004466b5(player,
               arg_2,
               prompt,
               dialog_mode,
               s.local_210,
               required_color,
               0xffffffff,
               0xffffffff,
               &_DAT_0074303c,
               &s.selected_player,
               0,
               0);

  strcpy(text_lines[0], "");
  if (s.selected_player != -1)
  {
    DAT_0072c8e4 = 0;
  }

  unk_00742fcc = s.selected_player;
  return s.return_value;
}

// FUNCTION: MAGIC 0x004e4ff3
int FUN_004e4ff3(int a1)
{
  if (unk_008a9000 != 1)
  {
    unk_00925bb8 = unk_006a1db8[a1 + unk_00743094];
    if (unk_00925bb8 != -1)
    {
      unk_00925bb8 &= 0xfff;
    }
  }
  return 0;
}

// FUNCTION: MOK 0x00499010
// FUNCTION: MAGIC 0x004e503e
int FUN_004e503e(int a1)
{
  if (unk_008a9000 != 1)
  {
    unk_00715fa8 = unk_006a6ae8[a1 + unk_00743094];
    if (unk_00715fa8 == 99)
    {
      unk_00715fa8 = 0;
    }
  }
  return 0;
}

// FUNCTION: MOK 0x0049d710
// FUNCTION: MAGIC 0x004eb23d
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

    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 || PLAYER_CARD_INSTANCE(player, card).internal_card_id == unk_0091a80c)
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    }
    else
    {
      s.source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    }

    *(unsigned int *)&PLAYER_CARD_INSTANCE(player, s.legacy_card).display_pic_csv_id =
        ((unsigned int)FUN_004964dd(global_cards_data[s.source_internal_card_id].id, player, card) << 16) |
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
int FUN_0043ece1(int player, int card)
{
  int force_special_mode;

  force_special_mode = 0;
  if (DAT_007aadec == 0xd3)
  {
    DAT_00791418 = 1;
    force_special_mode = 1;
  }

  if (put_card_on_stack(player, card, 0))
  {
    if (put_card_on_stack(player, card, 1))
    {
      if (FUN_0043f9ed(player, card))
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
int FUN_0043ec38(int player, int card)
{
  dispatch_event_to_single_card(player, card, EVENT_DISCARD, 1 - player, -1);
  FUN_004b14f5(player, card);
  PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  FUN_004a61a1(player, card, 11, 1);
  if (unk_008a9000 != 1)
  {
    play_sound_effect(WAV_DISCARD);
  }
  --hand_count[player];
  return 0;
}

// FUNCTION: MAGIC 0x00446c95
void FUN_00446c95(void)
{
  SendMessageA((HWND)g_main_window_hwnd, 0x464, 0xff, 0);
}

// FUNCTION: MAGIC 0x004a61a1
void FUN_004a61a1(int player, int card, int unk1, int unk2)
{
  if (unk_008a9000 != 1)
  {
    FUN_00446c95();
  }
}

// FUNCTION: MAGIC 0x004b14f5
void FUN_004b14f5(int player, int card)
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
void FUN_004b5cf5(int player, int internal_card_id)
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
void FUN_004e698f(unsigned int *param_1, unsigned int *param_2)
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

  if (param_1 != NULL)
  {
    *param_1 = bits1;
  }
  if (param_2 != NULL)
  {
    *param_2 = bits2;
  }
}

// FUNCTION: MAGIC 0x00441f16
int C_get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  card_instance_t *instance;
  card_instance_t *test_instance;
  unsigned int base_abilities;
  unsigned int current_abilities;
  unsigned int hidden_abilities;
  unsigned int result;
  int current_color;
  int raw_card_data;
  int saved_event_state;
  int test_card;
  int test_player;
  int token_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  saved_event_state = unk_00712938;
  ++unk_0093f9c0;
  affected_card_controller = player;
  affected_card = card;
  unk_008b4dd0 = instance->internal_card_id;
  unk_008cf6d4 = (int)(char)global_cards_data[unk_008b4dd0].color;
  attacking_card = new_attacking_card;

  switch (event)
  {
  case EVENT_POWER:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[unk_008b4dd0].power & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[unk_008b4dd0].power;
    }
    result += instance->counter_power;
    if ((instance->regen_status & 0x04000000) != 0)
    {
      instance->regen_status &= 0xfbffffff;
      event_result = result;
    }
    else
    {
      event_result = (unsigned int)instance->power;
    }
    if ((int)event_result < 0)
    {
      event_result = 0;
    }
    if ((instance->token_status & 0x4000) != 0)
    {
      event_result <<= 1;
    }
    break;

  case EVENT_TOUGHNESS:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[unk_008b4dd0].toughness & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[unk_008b4dd0].toughness;
    }
    result += instance->counter_toughness;
    if ((instance->regen_status & 0x02000000) != 0)
    {
      instance->regen_status &= 0xfdffffff;
      event_result = result;
    }
    else
    {
      event_result = (unsigned int)instance->toughness;
    }
    break;

  case EVENT_ABILITIES:
    base_abilities = global_cards_data[unk_008b4dd0].static_ability;
    hidden_abilities = instance->regen_status & 0x07000000;
    result = base_abilities | hidden_abilities;
    if ((base_abilities & 0x1ff81f) != 0)
    {
      current_abilities = 0;
      for (current_color = 0; current_color < 5; ++current_color)
      {
        if ((result & (1 << current_color)) != 0)
        {
          current_abilities |= 1 << ((char)get_hacked_color(player, card, current_color + 1) - 1);
        }
        if ((result & (0x800 << current_color)) != 0)
        {
          current_abilities |= 0x800 << ((char)get_sleighted_color(player, card, current_color + 1) - 1);
        }
      }
      result = (base_abilities & 0xffe007e0) | hidden_abilities | current_abilities;
    }
    if ((instance->regen_status & 0x08000000) != 0)
    {
      instance->regen_status &= 0xf7ffffff;
      event_result = result;
    }
    else
    {
      event_result = instance->regen_status;
    }
    break;

  case EVENT_RECALC_DAMAGE:
    event_result = (unsigned int)(short)instance->damage_on_card;
    break;

  case EVENT_SET_COLOR:
    event_result = (unsigned int)(char)global_cards_data[unk_008b4dd0].color;
    break;

  case EVENT_CHANGE_TYPE:
    if (((instance->internal_card_id < unk_009266a4) || (unk_009266a4 + 0x2d <= instance->internal_card_id)) && instance->internal_card_id != -1)
    {
      result = instance->original_internal_card_id;
      if ((instance->regen_status & 0x01000000) != 0)
      {
        instance->internal_card_id = instance->original_internal_card_id;
        instance->regen_status &= 0xfeffffff;
        instance->mana_color = global_cards_data[instance->original_internal_card_id].color;
        instance->destroys_if_blocked = 0;
        event_result = result;
      }
      else
      {
        event_result = instance->internal_card_id;
      }
    }
    else
    {
      event_result = instance->internal_card_id;
    }
    break;

  default:
    event_result = 0;
    break;
  }

  result = event_result;
  if (is_in_play(player, card) && event == EVENT_TOUGHNESS && (global_cards_data[unk_008b4dd0].type & TYPE_CREATURE) != 0 && ((int)result < 1 || (int)result <= (int)(short)instance->damage_on_card) && ((*((char *)instance + 0x1b) & 4) == 0) && trigger_condition == -1 && (unk_008b4278 & 0x204) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
    regenerate_or_graveyard_triggers();
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
    unk_00712938 = saved_event_state;
    return result;
  }

  instance->internal_card_id = result;
  if ((global_cards_data[result].extra_ability & 0x1000) != 0)
  {
    raw_card_data = global_cards_data[result].id;
    if (raw_card_data < 0x12d)
    {
      if (raw_card_data == 0x12c)
      {
        instance->mana_color = 1;
      }
      else if (raw_card_data == 0xf)
      {
        instance->mana_color = 0x3e;
      }
      else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
      {
        token_color = global_cards_data[result].color;
        instance->mana_color = (unsigned char)token_color;
      }
      else
      {
        instance->mana_color = global_cards_data[result].color;
      }
    }
    else if (raw_card_data < 0x13a)
    {
      if (raw_card_data == 0x139)
      {
        instance->mana_color = 2;
      }
      else if (raw_card_data == 0x134)
      {
        instance->mana_color = 0x10;
      }
      else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
      {
        token_color = global_cards_data[result].color;
        instance->mana_color = (unsigned char)token_color;
      }
      else
      {
        instance->mana_color = global_cards_data[result].color;
      }
    }
    else if (raw_card_data < 0x194)
    {
      if (raw_card_data == 0x193)
      {
        instance->mana_color = 0x3e;
      }
      else if (raw_card_data != 0x13e)
      {
        if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
        {
          token_color = global_cards_data[result].color;
          instance->mana_color = (unsigned char)token_color;
        }
        else
        {
          instance->mana_color = global_cards_data[result].color;
        }
      }
    }
    else if (raw_card_data == 0x272)
    {
      instance->mana_color = 1;
    }
    else if (raw_card_data == 0x27b)
    {
      instance->mana_color = 0x10;
    }
    else if (raw_card_data == 0x366)
    {
      instance->mana_color = (unsigned char)instance->info_slot;
    }
    else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
    {
      token_color = global_cards_data[result].color;
      instance->mana_color = (unsigned char)token_color;
    }
    else
    {
      instance->mana_color = global_cards_data[result].color;
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
    unk_007abc78 |= global_cards_data[instance->internal_card_id].extra_ability & 0x1ffc0000;
  }

  unk_00712938 = saved_event_state;
  return result;
}

// FUNCTION: MAGIC 0x00500a13
int damage_player(int target_player, int amount, int source_player, int source_card)
{
  damage_creature(target_player, -1, amount, source_player, source_card);
}

// FUNCTION: MOK 0x004302c0
// FUNCTION: MAGIC 0x00435e27
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount)
{
  int color_index;
  int result;

  color_index = single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color);
  unk_008ce510[color_index] += unk_0072c440[color_index];
  result = charge_mana(player, color, amount) - unk_0072c440[color_index];
  if (spell_fizzled == 1)
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0044331f
void play_sound_effect(wav_t sound_id)
{
  static const char *sound_filenames[] = {
      "artifact.wav", "buried.wav", "draw.wav", "enchant.wav", "endphase.wav",
      "endturn.wav", "instant.wav", "interupt.wav", "grey.wav", "black.wav",
      "blue.wav", "green.wav", "red.wav", "white.wav", "lifeloss.wav",
      "sacrfice.wav", "sorcery.wav", "summon.wav", "tap.wav", "untap.wav",
      "attack2.wav", "block2.wav", "damage.wav", "destroy.wav", "discard.wav",
      "kill.wav", "regen.wav", "blackred.wav", "greenblack.wav", "whitered.wav",
      "whitegreen.wav", "blackwhite.wav", "greenred.wav", "greenblue.wav", "whiteblue.wav",
      "blueblack.wav", "redblue.wav", "counter.wav", "fastfx.wav", "changec.wav",
      "changet.wav", "control.wav", "manaburn.wav", "shuffle.wav", "shell_loseduel.wav",
      "shell_winduel.wav", "aswanjag.wav", "callgrav.wav", "faerdrag.wav", "gembazar.wav",
      "necrazar.wav", "polkamix.wav", "pandora.wav", "prsmdrag.wav", "pwrstrgl.wav",
      "catatap.wav", "orcart.wav", "whimsy.wav", "rainbowk.wav", "toss.wav",
      "shell_shandalar.wav", "shell_tooltime.wav", "shell_helpme.wav", "shell_hallofrecords.wav", "shell_duelmenow.wav",
      "exp1_openfoil.wav", "exp1_openbox.wav", "exp1_outofpack.wav", "exp1_backinpack.wav"};
  struct
  {
    char path[264];
    int replacement_result;
    int sound_num;
    Sound sound;
  } s;
  int found_sound;

  s.sound_num = sound_id;
  s.sound.volume = 300;
  s.sound.sampleRate = 0;
  s.sound.pan = 0;
  s.sound.field_C = 0;
  s.sound.field_10 = 0;
  s.sound.field_14 = 0;
  s.sound.loadId = sound_id;
  s.sound.flags = 0;

  if (unk_008a9000 == 1)
  {
    return;
  }

  if (sound_id < 0x14)
  {
    sound_play(sound_id, 0);
    return;
  }

  if (sound_id < 0x27)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x14, 0x16);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x2e)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x27, 0x27);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x3c)
  {
    s.sound.volume = 400;
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (sound_id == WAV_CATATAP)
    {
      s.sound.field_14 = -1;
    }
    else
    {
      s.sound.flags |= 4;
    }

    if (found_sound == 0)
    {
      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, &s.sound);
    return;
  }

  if (sound_id < 0x41)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x3c, 0x40);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x45)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x41, 0x44);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
}

// FUNCTION: MAGIC 0x0044125c
int FUN_0044125c(int player, int card)
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

      if ((player == active_player) && ((unk_00926804 & 2) == 0))
      {
        push_affected_card_stack();
        event_result = 0;
        affected_card_controller = player;
        affected_card = card;
        FUN_0055117d(FUN_0051e631, -1);
        result = event_result;
        pop_affected_card_stack();
        if (result != 0)
        {
          return 0;
        }
      }

      if ((((unk_007a79b0[1 - player] & 1) != 0) || ((unk_007abc78 & 0x04000000) != 0)) &&
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
void FUN_005001c4(int internal_card_id)
{
  global_cards_data[internal_card_id].id = -1;
}

// FUNCTION: MAGIC 0x00483190
int FUN_00483190(int player, int card, int source_player, int source_card, int internal_card_id)
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
int FUN_00483242(int player, int card, int source_player, int source_card, int internal_card_id)
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
int FUN_004b413c(int player,
                 int *internal_card_ids,
                 int *damage_amounts,
                 int count,
                 int title,
                 int allow_cancel,
                 char *prompt)
{
  int selected;

  selected = -1;

  if (player == active_player && (unk_00926804 & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    selected = unk_007a7d0c;
  }

  if (player == unk_008b35ec && unk_008a9000 != 1)
  {
    selected = (int)FUN_004a6245(internal_card_ids, damage_amounts, count, title, allow_cancel, prompt);
    if ((unk_00926804 & 2) != 0)
    {
      unk_007a7d0c = selected;
      unk_007a7d08 = 0x19;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return selected;
}

// FUNCTION: MAGIC 0x0055d802
void FUN_0055d802(char *out, char *in, int choice)
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
void *FUN_0055dc4c(void *dest, const void *src, unsigned int count)
{
  return memcpy(dest, src, count);
}

// FUNCTION: MAGIC 0x00495311
char *FUN_00495311(int value)
{
  if (value < 0 || value >= 0xd4)
  {
    return "";
  }
  else
    return gs_hunting_subtype_name_00926930[value];
}

// FUNCTION: MAGIC 0x004a61fe
int FUN_004a61fe(int *cards, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }

  return show_cardlist(cards, 0, 0, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x004ec616
void FUN_004ec616(HWND param_1)
{
  struct
  {
    RECT rect1; // ebp-0x20
    RECT rect2; // ebp-0x10
  } s;

  if (IsWindowVisible((HWND)DAT_0091c0f0))
  {
    GetWindowRect((HWND)DAT_0091c0f0, &s.rect1);
  }
  else if (IsWindowVisible((HWND)DAT_008cff0c))
  {
    GetWindowRect(DAT_0094ca30, &s.rect1);
  }
  else
  {
    GetWindowRect(DAT_008cf078, &s.rect1);
    GetWindowRect(param_1, &s.rect2);
    s.rect1.bottom -= (s.rect2.bottom - s.rect2.top) / 2;
  }

  SetWindowPos(param_1, (HWND)0, s.rect1.left, s.rect1.bottom, 0, 0, 5);
}

// FUNCTION: MAGIC 0x004ec6c3
void FUN_004ec6c3(HWND param_1, HDC param_2, int *param_3)
{
  struct
  {
    RECT rect;        // ebp-0x4c
    HGDIOBJ wnd_font; // ebp-0x3c
    TEXTMETRICA tm;   // ebp-0x38
  } s;

  s.wnd_font = (HGDIOBJ)GetWindowLongA(param_1, DAT_0055e17c);
  if (s.wnd_font != (HGDIOBJ)0)
  {
    SelectObject(param_2, s.wnd_font);
  }

  GetTextMetricsA(param_2, &s.tm);

  *param_3 += s.tm.tmHeight / 2;
  param_3[1] += 4;
  param_3[3] -= 3;

  if (IsWindowVisible(DAT_00743090))
  {
    GetWindowRect(DAT_00743090, &s.rect);
    MapWindowPoints((HWND)0, param_1, (LPPOINT)&s.rect, 2);
    *param_3 = max(s.rect.right + s.tm.tmHeight / 2, *param_3);
  }

  if (IsWindowVisible(DAT_0074308c))
  {
    GetWindowRect(DAT_0074308c, &s.rect);
    MapWindowPoints((HWND)0, param_1, (LPPOINT)&s.rect, 2);
    *param_3 = max(s.rect.right + s.tm.tmHeight / 2, *param_3);
  }

  SetMapMode(param_2, 8);
  SetWindowExtEx(param_2, param_3[2] - *param_3, 0x14, (LPSIZE)0);
  SetViewportExtEx(param_2, param_3[2] - *param_3, param_3[3] - param_3[1], (LPSIZE)0);
}

// FUNCTION: MAGIC 0x004ec223
void FUN_004ec223(HWND param_1, char *param_2, unsigned int param_3)
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

  if (param_2 == NULL || strlen(param_2) == 0)
  {
    ShowWindow(param_1, 0);
  }

  if (param_3 == 0 || (param_3 & 1) == 0)
  {
    ShowWindow(DAT_00743090, 0);
  }

  if (param_3 == 0 || (param_3 & 2) == 0)
  {
    ShowWindow(DAT_0074308c, 0);
  }

  s.extra_width = 0;
  s.dc = GetDC(param_1);
  GetClientRect(param_1, &s.rect2);

  s.client_h = (s.rect2.bottom - s.rect2.top) - s.padding * 2;
  FUN_004ec6c3(param_1, s.dc, (int *)&s.rect2.left);

  GetTextExtentPoint32A(s.dc, DAT_008ce680, strlen(DAT_008ce680), &s.text_size);
  s.icon_width = s.text_size.cx + s.client_h / 2;

  s.rect2.right = 2000;
  s.text_width = (unsigned short)(*PTR_CalcDrawManaText_00950780)(s.dc, &s.rect2, param_2);
  ReleaseDC(param_1, s.dc);

  if ((param_3 & 2) != 0 || (param_3 & 1) != 0)
  {
    SetWindowPos(DAT_0074308c, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if ((param_3 & 1) != 0)
  {
    SetWindowPos(DAT_00743090, (HWND)0, 0, 0, s.icon_width, s.client_h, 6);
    s.extra_width = s.extra_width + s.icon_width + s.gap;
  }

  if (param_2 != NULL && strlen(param_2) != 0)
  {
    s.dc2 = GetDC(param_1);
    GetClientRect(param_1, &s.rect1);
    FUN_004ec6c3(param_1, s.dc2, (int *)&s.rect1.left);
    s.rect1.right = 2000;
    s.text_width = (unsigned short)(*PTR_CalcDrawManaText_00950780)(s.dc2, &s.rect1, param_2);
    ReleaseDC(param_1, s.dc2);
    SetWindowTextA(param_1, param_2);
  }
  else
  {
    s.text_width = 0;
    SetWindowTextA(param_1, "");
  }

  GetWindowRect(param_1, &s.rect4);
  SetWindowPos(param_1,
               (HWND)0,
               0,
               0,
               s.border * 2 + s.extra_width + s.text_width + 0x19,
               s.rect4.bottom - s.rect4.top,
               6);

  GetClientRect(param_1, &s.rect4);
  s.x = s.rect4.left + s.border;

  if ((param_3 & 2) != 0)
  {
    GetWindowRect(DAT_0074308c, &s.rect3);
    s.x = s.x + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(DAT_0074308c, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  if ((param_3 & 1) != 0)
  {
    GetWindowRect(DAT_00743090, &s.rect3);
    s.x = s.x + (s.rect3.right - s.rect3.left) + s.gap;
    s.y = (s.rect4.bottom - s.rect4.top) / 2 - (s.rect3.bottom - s.rect3.top) / 2;
    SetWindowPos(DAT_00743090, (HWND)0, s.x, s.y, 0, 0, 5);
  }

  FUN_004ec616(param_1);
  InvalidateRect(DAT_00743090, (RECT *)0, 1);
  InvalidateRect(DAT_0074308c, (RECT *)0, 1);
  InvalidateRect(param_1, (RECT *)0, 1);

  if ((param_3 & 1) != 0)
  {
    ShowWindow(DAT_00743090, 5);
  }

  if ((param_3 & 2) != 0)
  {
    ShowWindow(DAT_0074308c, 5);
  }

  if (param_2 != NULL)
  {
    if (strlen(param_2) != 0)
    {
      ShowWindow(param_1, 5);
    }
  }

  FUN_00495fa1();
  UpdateWindow(param_1);
}

// FUNCTION: MAGIC 0x004480a6
void FUN_004480a6(char *text)
{
  char *msg;

  if (text != NULL)
  {
    msg = text;
  }
  else
  {
    msg = DAT_0056e494;
  }

  FUN_004ec223(DAT_0093a7b0, msg, 0);
}

// FUNCTION: MAGIC 0x004a61d6
void FUN_004a61d6(char *text)
{
  strcpy(DAT_0064efa0, text);
  FUN_004480a6(text);
}

// FUNCTION: MAGIC 0x00482a97
int FUN_00482a97(int player, int card, unsigned int flags)
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
      if (s.instance->internal_card_id == unk_009266a4 &&
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

        if (PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id == -1)
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).original_internal_card_id;
        }
        else
        {
          s.source_internal_card_id = PLAYER_CARD_INSTANCE(s.source_player, s.source_card).internal_card_id;
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
int FUN_00551b60(int player, int preferred_controller, int card)
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

// FUNCTION: MAGIC 0x004bd7d0
/* target_source_mode:
 *   0 = scan direct player/card targets
 *   1 = scan damage cards by damage_target_player/card
 *   2 = scan damage cards by damage_source_player/card
 */
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

  if (unk_008b35ec == who_chooses || (unk_00926804 & 2) != 0)
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
            if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4)
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
          else if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4)
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
    int is_not_blocking;  /* local_10c */
    int is_not_attacking; /* local_108 */
    int unused_104;
    int unused_100;
    unsigned int pt_val;                /* local_fc (uVar17) */
    subtype_in_card_data_t alt_subtype; /* local_f8 */
    unsigned int pt_cmp;                /* local_f4 (uVar18) */
    int illegal_type_iid;               /* local_f0 */
    card_instance_t *test_instance;
    int type_iid;            /* local_e8 */
    int required_controller; /* local_e4 */
    int required_owner;      /* local_e0 */
    int tmp_match;           /* bVar19 */
    int tmp_dc;              /* local_dc */
    int tmp_d8;              /* local_d8 */
    int is_illegal;          /* bVar20 */
    char errbuf[200];        /* local_d0 */
    unsigned int is_valid;   /* local_4 */
  } s;

  if (tgt_player == -1)
  {
    if (return_error_str != NULL)
    {
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
        strcpy(return_error_str, "");
      }
      return 0;
    }
  }

  s.is_illegal = 0;
  strcpy(s.errbuf, "");

  if (tgt_card == -1)
  {
    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0)) && ((allowed_controller & ANYBODY) != 0)) || (who_chooses == AI && ((preferred_controller & ANYBODY) != 0)))
      {
        s.tmp_d8 = AI;
        s.tmp_dc = 1;
      }
      else if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0)) && ((allowed_controller & HUMAN) != 0)) || (who_chooses == AI && ((preferred_controller & HUMAN) != 0)))
      {
        s.tmp_d8 = HUMAN;
        s.tmp_dc = 1;
      }
    }
    else
    {
      s.tmp_d8 = HUMAN;
      s.tmp_dc = 0;
    }

    if ((tgt_player == HUMAN && s.tmp_d8 == 0) || (tgt_player == AI && s.tmp_dc == 0))
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_player_008b3110);
    }
  }
  else
  {
    if ((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0))
    {
      if ((allowed_controller & TARGET_PLAYER_OWNER) == 0)
      {
        if ((allowed_controller & ANYBODY) == 0)
        {
          if ((allowed_controller & HUMAN) == 0)
          {
            s.required_controller = HUMAN;
          }
          else
          {
            s.required_controller = AI;
          }
        }
        else
        {
          s.required_controller = -1;
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
      else
      {
        if ((allowed_controller & ANYBODY) == 0)
        {
          if ((allowed_controller & HUMAN) == 0)
          {
            s.required_owner = HUMAN;
          }
          else
          {
            s.required_owner = AI;
          }
        }
        else
        {
          s.required_owner = -1;
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
    }
    else if ((preferred_controller & TARGET_PLAYER_OWNER) == 0)
    {
      if ((preferred_controller & ANYBODY) == 0)
      {
        if ((preferred_controller & HUMAN) == 0)
        {
          s.required_controller = HUMAN;
        }
        else
        {
          s.required_controller = AI;
        }
      }
      else
      {
        s.required_controller = -1;
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
    else
    {
      if ((preferred_controller & ANYBODY) == 0)
      {
        if ((preferred_controller & HUMAN) == 0)
        {
          s.required_owner = HUMAN;
        }
        else
        {
          s.required_owner = AI;
        }
      }
      else
      {
        s.required_owner = -1;
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

    s.tmp_match = (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_OUBLIETTED) != 0;
    if (s.tmp_match)
    {
      strcat(s.errbuf, gs_illegal_target_why_cant_target_this_0091a6e0);
    }

    if ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_CANNOT_TARGET) != 0)
    {
      s.is_illegal = 1;
      strcat(s.errbuf, gs_illegal_target_why_cant_target_this_0091a6e0);
    }

    s.is_illegal = s.is_illegal || s.tmp_match;
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
      s.tmp114 = (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_OWNED_BY_OPPONENT) != 0 ? 1 - tgt_player : tgt_player;
      if (s.tmp114 != s.required_owner)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_owner_007aacc0);
      }
    }

    if (required_type != TYPE_NONE)
    {
      if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0)
      {
        s.type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id;
      }
      else
      {
        s.type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).original_internal_card_id;
      }

      s.tmp_match = 0;
      if (((required_type & global_cards_data[s.type_iid].type) != TYPE_NONE) || ((required_type & TARGET_TYPE_NONCREATURE_CAN_BLOCK) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_NONCREATURE_CAN_BLOCK) != 0))
      {
        s.tmp_match = 1;
      }
      if ((required_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.type_iid].id == unk_00789734 || global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id == unk_00789b80))
      {
        s.tmp_match = 1;
      }
      if ((required_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.tmp_match = 1;
      }
      if ((required_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_007a7d64)
      {
        s.tmp_match = 1;
      }
      if ((required_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_008a8de8)
      {
        s.tmp_match = 1;
      }
      if ((required_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.type_iid].id == unk_009266ac)
      {
        s.tmp_match = 1;
      }
      if (!s.tmp_match)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_type_008a9a00);
      }
    }

    if (illegal_type != TYPE_NONE)
    {
      if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0)
      {
        s.illegal_type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id;
      }
      else
      {
        s.illegal_type_iid = PLAYER_CARD_INSTANCE(tgt_player, tgt_card).original_internal_card_id;
      }

      s.tmp_match = (illegal_type & global_cards_data[s.illegal_type_iid].type) != TYPE_NONE;
      if ((illegal_type & TYPE_EFFECT) != TYPE_NONE && (global_cards_data[s.illegal_type_iid].id == unk_00789734 || global_cards_data[s.illegal_type_iid].id == unk_00789b80))
      {
        s.tmp_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).token_status & STATUS_TOKEN) != 0)
      {
        s.tmp_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_007a7d64)
      {
        s.tmp_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_008a8de8)
      {
        s.tmp_match = 1;
      }
      if ((illegal_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && global_cards_data[s.illegal_type_iid].id == unk_009266ac)
      {
        s.tmp_match = 1;
      }
      if (s.tmp_match)
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
        if (FUN_00483489(tgt_player, tgt_card, extra) != 0)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
        }
      }
      else if ((special & TARGET_SPECIAL_BASIC_LAND) != 0)
      {
        if (FUN_004c0880(PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id, extra) == 0)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_card_type_008cfde0);
        }
      }
      else
      {
        if (extra < 5)
        {
          if (FUN_0048463d(tgt_player, tgt_card, extra + 1) == 0)
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
      if ((special & TARGET_SPECIAL_0x10) == 0)
      {
        s.alt_subtype = required_subtype;
      }
      else if (required_subtype == (SUB_DJINN | SUB_MERFOLK))
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

      s.tmp114 = global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id;
      if ((subtype_in_card_data_t)global_raw_cards_storage[s.tmp114].subtype != required_subtype && (subtype_in_card_data_t)global_raw_cards_storage[s.tmp114].subtype != s.alt_subtype)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_subtype_008cea20);
      }
    }

    if (power_requirement != -1)
    {
      s.pt_val = power_requirement & TARGET_PT_MASK;
      s.pt_cmp = power_requirement & 0xf000;
      if ((s.pt_cmp == 0 && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power != (int)s.pt_val) || (s.pt_cmp == TARGET_PT_GREATER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power < (int)s.pt_val) || (s.pt_cmp == TARGET_PT_LESSER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).power > (int)s.pt_val))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_power_00781aa0);
      }
    }

    if (toughness_requirement != -1)
    {
      s.pt_val = toughness_requirement & TARGET_PT_MASK;
      s.pt_cmp = toughness_requirement & 0xf000;
      if ((s.pt_cmp == 0 && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness != (int)s.pt_val) || (s.pt_cmp == TARGET_PT_GREATER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness < (int)s.pt_val) || (s.pt_cmp == TARGET_PT_LESSER_OR_EQUAL && PLAYER_CARD_INSTANCE(tgt_player, tgt_card).toughness > (int)s.pt_val))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_toughness_007894b0);
      }
    }

    if (special != 0)
    {
      s.tmp114 = global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].id;
      if ((special & TARGET_SPECIAL_WALL) != 0 && (subtype_in_card_data_t)global_raw_cards_storage[s.tmp114].subtype != 0xc5 && global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype != 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_walls_008b34c0);
      }
      if ((special & TARGET_SPECIAL_NON_WALL) != 0 && ((subtype_in_card_data_t)global_raw_cards_storage[s.tmp114].subtype == 0xc5 || global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].subtype == 0))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_walls_008b34c0);
      }
      if ((special & TARGET_SPECIAL_SPELL_ON_STACK) != 0 && ((unk_008ce508 == -1 || tgt_player != unk_008ce508) || unk_008ce4f4 != tgt_card || (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_SUMMONSICK) != 0))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_spell_0091be40);
      }
      if ((special & TARGET_SPECIAL_BASIC_LAND) != 0 && FUN_004c081a(tgt_player, tgt_card) == 0)
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_basic_land_0093d860);
      }
      if ((special & TARGET_SPECIAL_ARTIFACT_CREATURE) != 0 && (global_cards_data[PLAYER_CARD_INSTANCE(tgt_player, tgt_card).internal_card_id].type & (TYPE_ARTIFACT | TYPE_CREATURE)) != (TYPE_ARTIFACT | TYPE_CREATURE))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_artifact_creature_008ced40);
      }
      if ((special & TARGET_SPECIAL_DAMAGE_PLAYER) != 0 && (PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_source_player != who_chooses || PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_card != -1))
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
      if ((required_state & TARGET_STATE_BLOCKING) != 0 && (tgt_player == human_player || (int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_player == -1))
      {
        s.is_illegal = 1;
        strcat(s.errbuf, gs_illegal_target_why_blocking_007ab160);
      }
      if ((required_state & TARGET_STATE_IN_COMBAT) != 0)
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_player == -1)
        {
          s.is_not_blocking = 1;
        }
        else
        {
          s.is_not_blocking = 0;
        }

        if ((PLAYER_CARD_INSTANCE(tgt_player, tgt_card).state & STATE_ATTACKING) == 0)
        {
          s.is_not_attacking = 1;
        }
        else
        {
          s.is_not_attacking = 0;
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
        s.tmp_match = 0;
        for (s.tmp114 = 0; s.tmp114 < 2; ++s.tmp114)
        {
          for (s.tmp118 = 0; s.tmp118 < active_cards_count[s.tmp114]; ++s.tmp118)
          {
            s.test_instance = &global_card_instances[s.tmp114][s.tmp118];
            if (s.test_instance->internal_card_id != -1 && (global_cards_data[s.test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && s.test_instance->damage_target_player == tgt_player && s.test_instance->damage_target_card == tgt_card)
            {
              s.tmp_match = 1;
            }
          }
        }
        if (!s.tmp_match)
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
      if ((required_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) == 0)
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
        if ((int)(char)PLAYER_CARD_INSTANCE(tgt_player, tgt_card).damage_target_player != -1 && tgt_player != human_player)
        {
          s.is_illegal = 1;
          strcat(s.errbuf, gs_illegal_target_why_blocking_007ab160);
        }
      }
      if ((illegal_state & TARGET_STATE_ENCHANTED) != 0)
      {
        s.tmp_match = 0;
        for (s.tmp120 = 0; s.tmp120 < 2; ++s.tmp120)
        {
          for (s.tmp124 = 0; s.tmp124 < active_cards_count[s.tmp120]; ++s.tmp124)
          {
            s.test_instance = &global_card_instances[s.tmp120][s.tmp124];
            if (s.test_instance->internal_card_id != -1 && (global_cards_data[s.test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && s.test_instance->damage_target_player == tgt_player && s.test_instance->damage_target_card == tgt_card)
            {
              s.tmp_match = 1;
            }
          }
        }
        if (s.tmp_match)
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
      if ((illegal_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) != 0)
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
void FUN_004e51bb(void)
{
  if (unk_00743094 < 1)
  {
    unk_00743094 = 0;
  }
  else
  {
    --unk_00743094;
  }
}

// FUNCTION: MAGIC 0x00551240
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

  if (unk_008a9000 != 1)
  {
    play_sound_effect(ICON_HASTE);
  }
}

// FUNCTION: MAGIC 0x005514cd
void FUN_005514cd(int player, int card, int amount)
{
  global_card_instances[player][card].special_counters =
      (unsigned char)(global_card_instances[player][card].special_counters - amount);
}

// FUNCTION: MAGIC 0x00551572
void FUN_00551572(int player, int card, int amount)
{
  if (amount > 0xff)
  {
    amount = 0xff;
  }

  global_card_instances[player][card].special_counters = (unsigned char)amount;
}

// FUNCTION: MAGIC 0x005515f5
unsigned int C_get_special_counters(int player, int card)
{
  return global_card_instances[player][card].special_counters & 0xff;
}

// FUNCTION: MAGIC 0x00441514
int has_vigilance(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).state & STATE_VIGILANCE)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x004418c6
int FUN_004418c6(int blocker_player,
                 int blocker_card,
                 int attacker_player,
                 int attacker_card,
                 unsigned int attacker_abilities,
                 unsigned int land_bits)
{
  int internal_card_id;
  unsigned int abilities;
  int color;

  internal_card_id = PLAYER_CARD_INSTANCE(blocker_player, blocker_card).internal_card_id;

  if ((((unsigned char)global_cards_data[internal_card_id].type & TYPE_CREATURE) == 0) &&
      ((PLAYER_CARD_INSTANCE(blocker_player, blocker_card).state & 0x02000000) == 0))
  {
    return 0;
  }

  if (internal_card_id == -1 || (PLAYER_CARD_INSTANCE(blocker_player, blocker_card).state & 0x800018) != 0)
  {
    return 0;
  }

  if (((PLAYER_CARD_INSTANCE(blocker_player, blocker_card).state & 4) != 0) && has_vigilance(blocker_player, blocker_card) == 0)
  {
    return 0;
  }

  if ((attacker_abilities & 0x20) != 0)
  {
    abilities = (unsigned int)C_get_abilities(blocker_player, blocker_card, 0x34, -1);
    if ((abilities & 0x420) == 0)
    {
      return 0;
    }
  }

  if ((attacker_abilities & 0x1ff800) != 0)
  {
    color = (int)single_color_test_bit_to_color_t((int)(char)PLAYER_CARD_INSTANCE(blocker_player, blocker_card).toughness);
    if ((attacker_abilities & (0x800 << (unsigned char)(color - 1))) != 0)
    {
      return 0;
    }
  }

  if ((((unsigned char)global_cards_data[internal_card_id].type & TYPE_ARTIFACT) != 0) && (attacker_abilities & 0x10000) != 0)
  {
    return 0;
  }

  if ((land_bits & attacker_abilities & 0x1f) != 0)
  {
    return 0;
  }

  push_affected_card_stack();
  affected_card_controller = blocker_player;
  affected_card = blocker_card;
  attacking_card_controller = attacker_player;
  attacking_card = attacker_card;
  event_result = 0;
  C_dispatch_event_raw(0x78);
  abilities = event_result < 1;
  pop_affected_card_stack();

  return abilities;
}

// FUNCTION: MAGIC 0x00443ee2
int FUN_00443ee2(int player, int card, int event, int extra, int prompt)
{
  card_instance_t *stack_card_instance;
  int stack_card;
  int saved_timestamp;

  if (unk_008b2934 >= 32)
    return 0;

  unk_00939180[unk_008b2934] = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  unk_00939180[unk_008b2934] |= event << 16;
  unk_00939180[unk_008b2934] |= extra << 24;
  if (event == EVENT_RESOLVE_SPELL || event == EVENT_RESOLVE_TRIGGER || PLAYER_CARD_INSTANCE(player, card).internal_card_id <= 4)
  {
    stack_card = card;
  }
  else
  {
    stack_card = add_card_to_hand(player, unk_0091a80c);
    if (stack_card == -1)
    {
      return 0;
    }

    stack_card_instance = &PLAYER_CARD_INSTANCE(player, stack_card);
    saved_timestamp = stack_card_instance->timestamp;
    memcpy(stack_card_instance, &PLAYER_CARD_INSTANCE(player, card), sizeof(card_instance_t));
    stack_card_instance->internal_card_id = unk_0091a80c;
    stack_card_instance->unknown0x14 = 0;
    stack_card_instance->kill_code = 0;
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
      stack_card_instance->original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    else
      stack_card_instance->original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    *(int *)&stack_card_instance->display_pic_csv_id = *(int *)&PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id;
    stack_card_instance->state |= 2;
    stack_card_instance->parent_controller = player;
    stack_card_instance->parent_card = card;
    stack_card_instance->timestamp = saved_timestamp;
  }

  global_stack_cards[unk_008b2934].player = player;
  global_stack_cards[unk_008b2934].card = stack_card;
  global_stack_damage_targets[unk_008b2934].player = PLAYER_CARD_INSTANCE(player, card).damage_target_player;
  global_stack_damage_targets[unk_008b2934].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
  if (trigger_condition == -1)
  {
    unk_00895030[unk_008b2934] = current_phase;
  }
  else
  {
    unk_00895030[unk_008b2934] = trigger_condition;
  }
  if (unk_008a9000 != 1)
  {
    unk_007a7750[unk_008b2934] = prompt;
  }
  ++unk_008b2934;
  global_stack_cards[unk_008b2934].player = -1;

  return 0;
}

// FUNCTION: MAGIC 0x004eaceb
void FUN_004eaceb(int player, int color_to_produce, int color_to_consume)
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
      if (unk_007161e0[player][s.slot] == -1)
      {
        s.found = 1;
        unk_007161e0[player][s.slot] = MAKELONG(color_to_consume, color_to_produce);
        unk_007161e0[player][s.slot + 1] = -1;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00551334
void FUN_00551334(int player, int card)
{
  /* The exe only decrements the low byte of special_counters and preserves the upper 3 bytes. */
  PLAYER_CARD_INSTANCE(player, card).special_counters =
      (PLAYER_CARD_INSTANCE(player, card).special_counters & 0xffffff00) |
      ((PLAYER_CARD_INSTANCE(player, card).special_counters - 1) & 0xff);
}

// FUNCTION: MAGIC 0x00444b74
int obliterate_top_card_of_stack(void)
{
  if (unk_008b2934 > 0)
  {
    --unk_008b2934;
    if (unk_0091a80c == PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
                            .internal_card_id)
    {
      PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
          .internal_card_id = -1;
    }
    global_stack_cards[unk_008b2934].player = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00444505
int resolve_top_card_on_stack(void)
{
  struct
  {
    int player;           /* [ebp-0xc] */
    int current_internal; /* [ebp-0x8] */
    int card;             /* [ebp-0x4] */
  } s;

  if (unk_008b2934 <= 0)
  {
    return 0;
  }

  --unk_008b2934;
  s.player = global_stack_cards[unk_008b2934].player;
  s.card = global_stack_cards[unk_008b2934].card;
  s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id;
  if (s.current_internal == unk_0091a80c)
  {
    s.current_internal = PLAYER_CARD_INSTANCE(s.player, s.card).original_internal_card_id;
  }
  if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
  {
    if ((((unsigned int)unk_00939180[unk_008b2934] >> 0x10) & 0xff) == 0x7e)
    {
      FUN_004b082f(s.player,
                   s.card,
                   ((unsigned int)unk_00939180[unk_008b2934] >> 0x10) & 0xff,
                   ((int)unk_00939180[unk_008b2934]) >> 0x18);
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
        if ((PLAYER_CARD_INSTANCE(s.player, s.card).upkeep_flags & 0x80) == 0)
        {
          dispatch_event_to_single_card(s.player,
                                        s.card,
                                        ((unsigned int)unk_00939180[unk_008b2934] >> 0x10) & 0xff,
                                        1 - s.player,
                                        -1);
        }
        else
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
      }
    }
    if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id == unk_0091a80c)
    {
      kill_card(s.player, s.card, KILL_REMOVE);
    }
  }
  global_stack_cards[unk_008b2934].player = -1;
  FUN_00441d78();
  if (unk_0091a6d0 < 2 && ((unk_008b4278 & 0x200) == 0 || unk_008b2934 == 0))
  {
    FUN_004afa4b(human_player);
    regenerate_or_graveyard_triggers();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004482f3
int copy_mana_pool_to_display(void)
{
  if (unk_008a9000 != 1)
  {
    EnterCriticalSection(&unk_00789110);
    memcpy(unk_00896670, raw_mana_available[0], 0x1c);
    memcpy(unk_007ab290, raw_mana_available[1], 0x1c);
    LeaveCriticalSection(&unk_00789110);
    SendMessageA(unk_008ce534, 0x432, 0, 0);
    SendMessageA(unk_00939344, 0x432, 0, 0);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00441d78
void FUN_00441d78(void)
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
void FUN_004afa4b(int player)
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

  if ((unk_008b4278 & 2) == 0)
  {
    return;
  }
  unk_008b4278 &= ~2;
  unk_008b4278 |= 4;
  TENTATIVE_reassess_all_cards(0, 0xff);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_009266a4 &&
          is_in_play(s.test_player, s.test_card) != 0 &&
          (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x21);
      }
    }
  }

  s.did_setup = 0;
  while (1)
  {
    if (((unk_00926804 & 2) == 0) && unk_008a9000 != 1 && DAT_0072c8e4 == 0)
    {
      FUN_00441cf2(9, 0xf);
      s.best_score = -99999;
      s.did_setup = 1;
    }

    if (DAT_00777a9c == 9 && s.did_setup)
    {
      FUN_004e4e9a();
      DAT_008cdab4 = 0;
      s.temp = DAT_008cdab4;
      DAT_008cdab0 = s.temp;
      s.temp = DAT_008cdab0;
      DAT_008a8de4 = s.temp;
      s.temp = DAT_008a8de4;
      ai_modifier = s.temp;
    }

    if (allow_response(-2, -1, gs_damage_prevention_00939200, 0x8e) != 0)
    {
      continue;
    }

    C_dispatch_event_raw(0x25);
    if ((unk_007abc78 & 0x00040000) != 0)
    {
      dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_END_DAMAGE_PREV, &gs_end_damage_prevention_00789740, 0);
    }

    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
      {
        if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_009266a4 &&
            is_in_play(s.test_player, s.test_card) != 0 &&
            (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) == 0)
        {
          dispatch_event(s.test_player, s.test_card, 0x6e);
        }
      }
    }

    dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_DEAL_DAMAGE, &gs_damage_dealing_00777ab0, 0);

    for (s.test_player = 0; s.test_player < 2; ++s.test_player)
    {
      for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
      {
        if (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == unk_009266a4 &&
            is_in_play(s.test_player, s.test_card) != 0)
        {
          if ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).state & 0x10) == 0)
          {
            unk_008b4278 |= 2;
          }
          else
          {
            kill_card(s.test_player, s.test_card, 1);
          }
        }
      }
    }

    FUN_004aff25();
    unk_008b4278 &= ~4;

    if (unk_008a9000 != 1 || s.did_setup == 0 || DAT_00777a9c != 9)
    {
      if (unk_008a9000 == 1)
      {
        return;
      }
      if (s.did_setup == 0)
      {
        return;
      }
      DAT_0072c8e4 = 0;
      return;
    }

    regenerate_or_graveyard_triggers();
    s.temp = ai_opinion_of_gamestate(active_player);
    s.temp = ai_modifier + s.temp;
    if (s.best_score < s.temp)
    {
      FUN_004e50f5();
      s.saved_777854 = DAT_00777854;
      s.best_score = s.temp;
    }
    if (DAT_008a8d7c == 999)
    {
      DAT_008a8d7c = -1;
    }
    DAT_008a8d74 = 0;
    s.temp = FUN_004e1b6f();
    if (((DAT_0057d9e4 * DAT_0091c500) / 5) < s.temp)
    {
      unk_008a9000 = 0;
      DAT_008a8d7c = -1;
      DAT_00777854 = s.saved_777854;
    }
    unk_008b4278 |= 4;
  }
}

// FUNCTION: MAGIC 0x004aff25
void FUN_004aff25(void)
{
  int player;
  int card;
  int toughness;
  card_instance_t *instance;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, card);
      if (is_in_play(player, card) && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE))
      {
        toughness = (short)instance->damage_on_card;
        if (C_get_abilities(player, card, EVENT_TOUGHNESS, -1) <= toughness)
        {
          if (unk_008a9000 != 1)
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
int FUN_0044aa01(int player)
{
  if ((unk_00926804 & 2) != 0)
  {
    if (unk_00716248 == -1)
    {
      if (unk_007161d4 == -1)
      {
        return 0;
      }
    }

    if (unk_008a9000 == 1)
    {
      return 0;
    }

    return 0;

    /* Dead code (kept for matching): MSVC /Od still emits this after the return. */
    if (unk_007161d4 == current_phase && human_player == _DAT_00743034)
    {
      return 0;
    }

    if (unk_00716248 == current_phase && human_player == unk_00716244)
    {
      return 0;
    }

    if (unk_007161d4 == -1 && player == active_player)
    {
      if (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0)
      {
        return 0;
      }
    }

    if (unk_00716248 == -1 && player == unk_008b35ec)
    {
      if (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0)
      {
        return 0;
      }
    }

    if (!(unk_00716248 == current_phase && human_player == unk_00716244))
    {
      if (unk_007161d4 == -1)
      {
        return 1;
      }
    }

    if (!(unk_007161d4 == current_phase && human_player == _DAT_00743034))
    {
      if (unk_00716248 == -1)
      {
        return 1;
      }
    }

    if (trigger_condition != -1 && unk_00716248 == current_phase)
    {
      return 1;
    }

    if (trigger_condition != -1 && unk_007161d4 == current_phase)
    {
      return 1;
    }

    return 0;
  }

  if (unk_00716248 == -1)
  {
    return 0;
  }
  if (unk_008a9000 == 1)
  {
    return 0;
  }

  if (unk_00716248 > current_phase)
  {
    return 1;
  }
  if (human_player != unk_00716244)
  {
    return 1;
  }

  if (trigger_condition != -1)
  {
    if (unk_00716248 == current_phase)
    {
      return 1;
    }
  }

  if (unk_00716248 < current_phase)
  {
    if (human_player == unk_00716244)
    {
      unk_00716244 = -1;
      unk_00716248 = unk_00716244;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044541f
int FUN_0044541f(int param_1)
{
  int can_respond;

  can_respond = 0;
  if (param_1 != 0)
  {
    if (human_player == unk_008b35ec &&
        (((int)(char)DAT_007abc90[human_player * 0x26 + current_phase]) & 1) != 0)
    {
      can_respond = 1;
    }

    if (human_player == active_player &&
        (((int)(char)DAT_007abc90[human_player * 0x26 + current_phase]) & 4) != 0)
    {
      can_respond = 1;
    }

    if (human_player == unk_008b35ec)
    {
      if (human_player == unk_00716244 && current_phase == unk_00716248)
      {
        can_respond = 1;
      }
    }
    else if (human_player == _DAT_00743034 && unk_007161d4 == current_phase)
    {
      can_respond = 1;
    }
    if (FUN_0044aa01(human_player) == 0 && can_respond != 0)
    {
      return 1;
    }
    return 0;
  }

  /* param_1 == 0 */
  if (human_player == unk_008b35ec &&
      (((int)(char)DAT_007abc90[human_player * 0x26 + current_phase]) & 4) != 0)
  {
    can_respond = 1;
  }

  if (human_player == active_player &&
      (((int)(char)DAT_007abc90[human_player * 0x26 + current_phase]) & 1) != 0)
  {
    can_respond = 1;
  }

  if (human_player == active_player)
  {
    if (human_player == unk_00716244 && current_phase == unk_00716248)
    {
      can_respond = 1;
    }
  }
  else if (human_player == _DAT_00743034 && unk_007161d4 == current_phase)
  {
    can_respond = 1;
  }

  if (FUN_0044aa01(1 - human_player) == 0 && can_respond != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004460d3
void FUN_004460d3(void)
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
unsigned int FUN_00447f80(void)
{
  unsigned int ticks = GetTickCount();
  unsigned int diff = ticks - unk_0093f4b4 - unk_007abc84;
  unsigned int result = diff / 0x37;
  return result;
}

// FUNCTION: MAGIC 0x004e1b6f
unsigned int FUN_004e1b6f(void)
{
  return FUN_00447f80();
}

// FUNCTION: MAGIC 0x004b082f
int FUN_004b082f(int player, int card, event_t event, int extra)
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
void C_dispatch_event_raw(event_t event)
{
  struct
  {
    card_instance_t *instance;
    unsigned int timestamp_slot;
    int card;
    unsigned int saved_trigger_condition;
    int player;
  } locals;

  locals.saved_trigger_condition = unk_00712938;
  unk_008cc844 = event;
  for (locals.timestamp_slot = 0; (int)locals.timestamp_slot < 500; ++locals.timestamp_slot)
  {
    if (TENTATIVE_timestamp_player[locals.timestamp_slot] == -1)
    {
      break;
    }

    locals.player = TENTATIVE_timestamp_player[locals.timestamp_slot];
    locals.card = TENTATIVE_timestamp_card[locals.timestamp_slot];
    locals.instance = &PLAYER_CARD_INSTANCE(locals.player, locals.card);
    if (locals.instance->timestamp != (int)locals.timestamp_slot)
    {
      continue;
    }
    if (locals.instance->internal_card_id == -1)
    {
      continue;
    }
    if ((locals.instance->state & 0x800000) != 0)
    {
      continue;
    }
    if ((locals.instance->state & 2) != 0 || (locals.instance->state & 0x20) != 0)
    {

      unk_00789a48 = locals.player * 0x80 + locals.card;
      (*global_cards_data[locals.instance->internal_card_id].code_pointer)(locals.player, locals.card, event);
      if (event == EVENT_DECLARE_ATTACKERS && locals.player == human_player && (locals.instance->state & 0x14) == 4 && !has_vigilance(locals.player, locals.card))
      {
        locals.instance->state |= 0x10;
        produced_mana_color = -1;
        dispatch_event(locals.player, locals.card, EVENT_TAP_CARD);
        FUN_004afa4b(human_player);
        regenerate_or_graveyard_triggers();
      }
    }
  }
  if (event == EVENT_DECLARE_ATTACKERS && locals.player == human_player)
  {
    FUN_00441d78();
  }
  if (unk_00789308 != -1)
  {
    (*global_cards_data[unk_00789308].code_pointer)(0, 0x94, event);
  }
  unk_00712938 = locals.saved_trigger_condition;
}

// FUNCTION: MAGIC 0x004a6d0e
void count_mana(void)
{
  struct
  {
    card_instance_t *instance;
    int current_card;
    int player;
  } locals;

  for (locals.current_card = 0; locals.current_card <= 7; ++locals.current_card)
  {
    unk_0072c440[locals.current_card] = 0;
    unk_00742f90[locals.current_card] = unk_0072c440[locals.current_card];
    unk_00742f70[0][locals.current_card] = unk_00742f90[locals.current_card];
  }

  unk_007160fc = -1;
  raw_mana_available_hex[0][0] = unk_007160fc;
  unk_0071620c = -1;
  unk_007161e0[0][0] = unk_0071620c;
  for (locals.player = 0; locals.player < 2; ++locals.player)
  {
    locals.instance = global_card_instances[locals.player];
    for (locals.current_card = 0; locals.current_card < active_cards_count[locals.player];
         ++locals.current_card, ++locals.instance)
    {
      if (locals.instance->internal_card_id == -1)
      {
        continue;
      }

      if ((global_cards_data[locals.instance->internal_card_id].extra_ability & 0x1000) != 0 && (locals.instance->state & 0x800012) == 2)
      {
        dispatch_event(locals.player, locals.current_card, EVENT_COUNT_MANA);
      }
      if (global_cards_data[locals.instance->internal_card_id].id == 0xee && (locals.instance->state & 0x800012) == 2)
      {
        dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_COUNT_MANA, -1, -1);
      }
      if (global_cards_data[locals.instance->internal_card_id].id == 100 && (locals.instance->state & 0x800002) == 2)
      {
        dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_COUNT_MANA, -1, -1);
      }
    }
  }
  if ((unk_007abc78 & 0x08000000) != 0)
  {
    for (locals.player = 0; locals.player < 2; ++locals.player)
    {
      locals.instance = global_card_instances[locals.player];
      for (locals.current_card = 0; locals.current_card < active_cards_count[locals.player];
           ++locals.current_card, ++locals.instance)
      {
        if (locals.instance->internal_card_id == -1)
        {
          continue;
        }

        if (global_cards_data[locals.instance->internal_card_id].code_pointer == card_fellwar_stone && (locals.instance->state & 0x800012) == 2)
        {
          dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_VARIABLE_MANA_SRC, -1, -1);
        }
      }
    }
  }
}
// FUNCTION: MAGIC 0x004a6faa
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
        else if (internal_card_id < 0xf || unk_0093f4b8 <= internal_card_id)
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
int sacrifice_a_land(int player)
{
  target_t target;

  if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0,
                           COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                           text_lines[0], 0, &target))
  {

    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
    kill_card(target.player, target.card, KILL_SACRIFICE);

    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00551921
int FUN_00551921(int player)
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

  if ((player == unk_008b35ec || (unk_00926804 & 2) != 0) && unk_008a9000 != 1)
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

  if (s.best_card != -1 && unk_008a9000 != 1)
  {
    play_sound_effect(WAV_SACRFICE);
  }
  return s.best_card;
}

// FUNCTION: MAGIC 0x004823a5
int FUN_004823a5(int player, int card)
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
        s.instance->internal_card_id == unk_008cefb8 &&
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
           s.instance->internal_card_id == unk_008d0340) &&
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
int choose_a_number(int player, char *prompt, int maxnum)
{
  int chosen;

  if (unk_008a9000 == 1)
  {
    return maxnum;
  }

  if (player == active_player && (unk_00926804 & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0xe);
    maxnum = DAT_008b293c;
  }

  chosen = FUN_004a09c6(player, prompt, maxnum);

  if (player == unk_008b35ec && (unk_00926804 & 2) != 0)
  {
    unk_008b2938 = 0xe;
    DAT_008b293c = chosen;
    TENTATIVE_send_network_result(player, 0xe);
  }

  return chosen;
}

// FUNCTION: MAGIC 0x004b4110
void FUN_004b4110(int player)
{
  PostMessageA((void *)g_main_window_hwnd, 0x401, (unsigned int)player, 0);
  ExitThread((unsigned int)player);
}

// FUNCTION: MAGIC 0x004b5d72
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
void FUN_004b6002(const char *param_1)
{
  char buf[500];

  if (unk_008a9000 != 1)
  {
    LoadTextSectionLines(global_ui_strings_filename, "PROMPT_SYSERR");
    sprintf(buf, text_lines[1], param_1);
    MessageBoxA((HWND)g_main_window_hwnd, buf, text_lines[0], 0);
  }
}

// FUNCTION: MAGIC 0x004a6968
void TENTATIVE_reassess_all_cards(void)
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

    if (unk_008a9000 != 1)
    {
      unk_008b3270 = 0;
      for (s.player = 0; s.player < 2; ++s.player)
      {
        for (s.card = 0; s.card < active_cards_count[s.player]; ++s.card)
        {
          if (PLAYER_CARD_INSTANCE(s.player, s.card).internal_card_id != -1)
          {
            PLAYER_CARD_INSTANCE(s.player, s.card).unknown0x70 = FUN_004b0047(s.player, s.card);
          }
        }
      }

      if (unk_00742fc4 == 0)
      {
        FUN_004e1cd1();
      }
      else
      {
        SendMessageA((HWND)g_main_window_hwnd, 0x464, 0xffff, 0);
      }
    }
  }
}
