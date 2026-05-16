#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "game_support.h"
#include "global_strings.h"

// This file holds temporary stub implementations for exe functions that are
// referenced by decompiled code but not yet matched/decompiled elsewhere.

int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int set_stack_damage_targets(void);
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode);

// NOTE: Even though this file is named "exe_stubs.c", functions removed from
// here as they get fully decompiled elsewhere. A few UI helpers live here
// temporarily until their proper home is decided.

// FUNCTION: MAGIC 0x00444e5f
int FUN_00444e5f(int param_1, int param_2, char *param_3, int param_4)
{
  struct
  {
    int saved_trigger_condition; /* ebp-0xc */
    int saved_DAT_00561390;      /* ebp-0x8 */

    /* assorted saved globals */
    int saved_unk_008b3270;
    int saved_DAT_007aadec;
    int saved_DAT_00777aa8;

    /* return value */
    int result; /* local_9c */

    unsigned int dispatch_result; /* local_a0 */
    char prompt_copy[128];        /* local_8c */
  } s;

  int saved_DAT_00742fc8;
  int saved_DAT_00777aa8_in_loop;
  unsigned int other_dispatch_result;

  s.saved_unk_008b3270 = unk_008b3270;
  s.saved_DAT_007aadec = DAT_007aadec;
  s.saved_DAT_00777aa8 = DAT_00777aa8;

  s.saved_trigger_condition = trigger_condition;
  trigger_condition = -1;

  ++DAT_00939510;
  if (DAT_00939510 == 1)
  {
    DAT_008ce4ec = 0;
  }
  else if (DAT_008ce4ec < DAT_00939510 && param_2 != 0x8e && param_2 != 0x70 && param_2 != 0xd3)
  {
    DAT_008ce4ec = DAT_00939510;
  }

  s.result = 0;
  DAT_007aadec = param_4;

  s.saved_DAT_00561390 = DAT_00561390;
  if (param_1 == -2 && FUN_0044aa01(human_player) == 0)
  {
    DAT_00561390 = 1;
  }
  else
  {
    DAT_00561390 = 0;
  }

  if ((unk_00926804 & 2) == 0)
  {
    if (FUN_0044aa01(human_player) == 0 &&
        (((DAT_007abc90[human_player * 0x26 + param_2] & 1) != 0) ||
         (unk_00716244 == human_player && param_2 == unk_00716248)))
    {
      DAT_00777aa8 = 1;
    }
    else
    {
      DAT_00777aa8 = 0;
    }
  }

  if (param_1 >= 0 && DAT_00777aa8 == 0)
  {
    goto cleanup;
  }

  strcpy(s.prompt_copy, param_3);

  if (current_phase == 4 && DAT_00939510 == 1)
  {
    unk_00742f60 = human_player;
    FUN_004460d3();
  }

  while (1)
  {
    do
    {
      DAT_00777aa8 = FUN_0044541f(1);

      if ((unk_00926804 & 2) == 0)
      {
        if (human_player == 0)
        {
          DAT_00789714 = 1;
        }
        else if (param_1 < 0)
        {
          DAT_00789714 = 2;
        }
        else
        {
          DAT_00789714 = 0;
        }
      }
      else
      {
        DAT_00789714 = 1;
      }

      if (DAT_00939510 < 2)
      {
        _DAT_00743020 = 0xffffffff;
      }

      spell_fizzled = 0;
      unk_008b3270 = 0;

      s.dispatch_result = (unsigned int)FUN_004ad7e0(human_player, s.prompt_copy);
      if (s.dispatch_result != 0)
      {
        unk_009266d0[0xc0] = 1;
        unk_009266d0[0xc1] = 0;
        unk_009266d0[0xc2] = 0;
        unk_009266d0[0xc3] = 0;
      }

      if (((unk_008b35ec == human_player) ||
           (active_player == human_player && (unk_00926804 & 2) != 0)) &&
          s.dispatch_result != 0 && unk_008a9000 != 1)
      {
        s.result = 1;
      }

      if (DAT_00939510 < DAT_008ce4ec && unk_008b2934 >= 0)
      {
        s.dispatch_result = 0;
      }

    } while (s.dispatch_result != 0 || ((unk_008b3270 & 1) != 0 && DAT_00939510 == 1));

    if (current_phase == 4 && DAT_00939510 == 1)
    {
      unk_00742f60 = 1 - human_player;
      FUN_004460d3();
    }

    while (1)
    {
      saved_DAT_00777aa8_in_loop = DAT_00777aa8;
      saved_DAT_00742fc8 = DAT_00742fc8;

      if ((unk_00926804 & 2) == 0)
      {
        if (human_player == 0)
        {
          if (param_1 < 0)
          {
            DAT_00789714 = 2;
          }
          else
          {
            DAT_00789714 = 0;
          }
        }
        else
        {
          DAT_00789714 = 1;
        }
      }
      else
      {
        DAT_00789714 = 1;
        DAT_00742fc8 = 0;

        if (unk_008b2934 == 0 && FUN_0044aa01(human_player) != 0 && param_1 != -2)
        {
          DAT_00742fc8 = 1;
        }

        DAT_00777aa8 = FUN_0044541f(0);
      }

      if (DAT_00939510 < 2)
      {
        _DAT_00743020 = 0xffffffff;
      }

      spell_fizzled = 0;
      unk_008b3270 = 0;

      if ((DAT_00939510 < DAT_008ce4ec && unk_008b2934 >= 0))
      {
        goto cleanup;
      }

      other_dispatch_result = (unsigned int)FUN_004ad7e0(1 - human_player, s.prompt_copy);
      DAT_00742fc8 = saved_DAT_00742fc8;
      if (other_dispatch_result == 0)
      {
        goto cleanup;
      }

      unk_009266d0[0xc0] = 1;
      unk_009266d0[0xc1] = 0;
      unk_009266d0[0xc2] = 0;
      unk_009266d0[0xc3] = 0;

      DAT_00777aa8 = saved_DAT_00777aa8_in_loop;

      if (unk_008a9000 != 1)
      {
        break;
      }

      if (unk_008b35ec != human_player && (unk_00926804 & 2) == 0 &&
          ((unk_008b3270 & 1) == 0 || DAT_00939510 != 1))
      {
        goto cleanup;
      }
    }
  }

cleanup:
  --DAT_00939510;
  if (DAT_00939510 == 0)
  {
    DAT_00742f68 = 0xffffffff;
    if (unk_0091a6d0 == 0)
    {
      DAT_00742f64 = 0;
      if (unk_008a9000 != 1)
      {
        DAT_00742fc8 = 0;
      }
    }
  }

  _DAT_00743020 = 0xffffffff;

  DAT_00777aa8 = s.saved_DAT_00777aa8;
  DAT_00561390 = s.saved_DAT_00561390;
  unk_008b3270 = s.saved_unk_008b3270;
  trigger_condition = s.saved_trigger_condition;
  DAT_007aadec = s.saved_DAT_007aadec;

  if (s.result != 0)
  {
    DAT_0072c8e4 = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004af6f4
int FUN_004af6f4(void)
{
  if (DAT_007aadec == 0x8e)
  {
    return unk_008b60e0 != 0;
  }
  else if (DAT_007aadec == 0x6a || DAT_007aadec == 0x6b || DAT_007aadec == 0x6c ||
           DAT_007aadec == 0x6d || DAT_007aadec == 0x6e || DAT_007aadec == 0x6f ||
           DAT_007aadec == 0x70 || DAT_007aadec == 0x71 || DAT_007aadec == 0x72 ||
           DAT_007aadec == 0x73 || DAT_007aadec == 0x74 || DAT_007aadec == 0x75 ||
           DAT_007aadec == 0x76 || DAT_007aadec == 0x77 || DAT_007aadec == 0x78 ||
           DAT_007aadec == 0x79 || DAT_007aadec == 0x7a || DAT_007aadec == 0x7b ||
           DAT_007aadec == 0x7c || DAT_007aadec == 0x7d || DAT_007aadec == 0x7e ||
           DAT_007aadec == 0x7f || DAT_007aadec == 0x80 || DAT_007aadec == 0x81 ||
           DAT_007aadec == 0x82 || DAT_007aadec == 0x83 || DAT_007aadec == 0x84 ||
           DAT_007aadec == 0x85 || DAT_007aadec == 0x86 || DAT_007aadec == 0x87 ||
           DAT_007aadec == 0x88 || DAT_007aadec == 0x89 || DAT_007aadec == 0x8e ||
           DAT_007aadec == 0xc7 || DAT_007aadec == 0xc8 || DAT_007aadec == 0xc9 ||
           DAT_007aadec == 0xca || DAT_007aadec == 0xcb || DAT_007aadec == 0xcc ||
           DAT_007aadec == 0xcd || DAT_007aadec == 0xce || DAT_007aadec == 0xcf ||
           DAT_007aadec == 0xd2 || DAT_007aadec == 0xd3 || DAT_007aadec == 0xd4 ||
           DAT_007aadec == 0xd5 || DAT_007aadec == 0xd6 || DAT_007aadec == 0xd7 ||
           DAT_007aadec == 0xd8 || DAT_007aadec == 0xd9 || DAT_007aadec == 0xdc ||
           DAT_007aadec == 0xdb)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: MAGIC 0x0044ac96
int FUN_0044ac96(int param_1, int param_2)
{
  int mapped_phase;

  if (unk_008a9000 == 1)
  {
    return 0;
  }

  if (DAT_00742fc8 != 0)
  {
    return 0;
  }

  if (FUN_004af6f4() == 0)
  {
    return 0;
  }

  if (DAT_00715fac == 0)
  {
    return 0;
  }

  if (param_2 >= 2 && param_2 <= 5)
  {
    mapped_phase = 4;
  }
  else if (param_2 >= 0x20 && param_2 <= 0x25)
  {
    mapped_phase = 0x20;
  }
  else
  {
    mapped_phase = param_2;
  }

  if ((unk_00926804 & 2) == 0)
  {
    if (unk_00716248 == mapped_phase)
    {
      return 1;
    }

    if (unk_00716248 == -1 && unk_008b35ec == param_1 &&
        (DAT_007abc90[human_player * 0x26 + mapped_phase] & 1) != 0)
    {
      return 1;
    }
  }
  else
  {
    if (human_player == param_1)
    {
      if ((unk_008b35ec == param_1 && (DAT_007abc90[unk_008b35ec * 0x26 + current_phase] & 1) != 0) ||
          (active_player == param_1 && (DAT_007abc90[active_player * 0x26 + current_phase] & 4) != 0))
      {
        return 1;
      }
    }
    else
    {
      if ((active_player == param_1 && (DAT_007abc90[unk_008b35ec * 0x26 + current_phase] & 4) != 0) ||
          (unk_008b35ec == param_1 && (DAT_007abc90[active_player * 0x26 + current_phase] & 1) != 0))
      {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004e51a6
int FUN_004e51a6(void)
{
  return unk_00743094;
}

// FUNCTION: MAGIC 0x004aec05
unsigned int FUN_004aec05(int player)
{
  struct
  {
    unsigned int candidates[20];
    int response_flags; /* local_18 */
    unsigned int card;  /* local_14 */
    int other_player;   /* local_10 */
    int internal_card_id; /* local_c */
    int num_candidates;   /* local_8 */
  } s;

  s.num_candidates = 0;

  if (trigger_condition != -1 && current_turn == active_player)
  {
    for (s.other_player = 0; s.other_player < 2; ++s.other_player)
    {
      for (s.card = 0; (int)s.card < active_cards_count[s.other_player]; ++s.card)
      {
        if (is_in_play(s.other_player, (int)s.card) &&
            FUN_004b082f(s.other_player, (int)s.card, 0x7d, player) == 2)
        {
          unk_00742fcc = s.other_player;
          DAT_007ab2bc = 4;
          return s.card;
        }

        if (player == s.other_player &&
            (int)PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).unknown0x14 == trigger_condition &&
            current_turn == s.other_player && trigger_condition != -1 &&
            PLAYER_CARD_INSTANCE(s.other_player, (int)s.card).internal_card_id != -1)
        {
          DAT_007ab2bc |= 4;
          ++DAT_00791410;
          unk_00742fcc = s.other_player;
          return s.card;
        }
      }
    }
  }

  if (((DAT_00789714 & 2) == 0) || (player == unk_008b35ec) || (_DAT_00743020 == 0))
  {
    return 0xffffffffU;
  }

  unk_00742fcc = player;

  for (s.card = 0; (int)s.card < active_cards_count[player]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(player, (int)s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      s.response_flags = (int)FUN_004af0ec(player, (int)s.card);
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
  }

  if (DAT_007aadec == 4)
  {
    for (s.card = 0; (int)s.card < active_cards_count[1 - player]; ++s.card)
    {
      s.internal_card_id = PLAYER_CARD_INSTANCE(1 - player, (int)s.card).internal_card_id;
      if (s.internal_card_id != -1)
      {
        s.response_flags = (int)FUN_004af0ec(1 - player, (int)s.card);
        if (s.response_flags == 2)
        {
          unk_00742fcc = 1 - player;
          return s.card;
        }
      }
    }
  }

  if (DAT_007aadec == 4)
  {
    return 0xffffffffU;
  }

  s.candidates[s.num_candidates] = 0xffffffffU;
  ++s.num_candidates;

  if (unk_008a9000 == 1)
  {
    if (internal_rand(2) == 0 || FUN_004e51a6() == 0)
    {
      unk_00939340 = internal_rand(s.num_candidates);
    }
    else
    {
      unk_00939340 = s.num_candidates - 1;
    }

    if (DAT_008a8d74 != 0)
    {
      unk_00939340 = s.num_candidates - 1;
      if (DAT_008a8d74 == 1)
      {
        DAT_008a8d74 = -1;
      }
    }

    unk_00925bb8 = (((unsigned int)-(((PLAYER_CARD_INSTANCE(player, (int)s.candidates[unk_00939340]).state & 2) == 0))) &
                    0xfffff000) +
                   0x2000;
    unk_00925bb8 |= s.candidates[unk_00939340];
    unk_00925bb8 |= (((player == 0) - 1) & 0x100);

    unk_0057aae8 = 4;
    FUN_004e4f11();
  }
  else
  {
    unk_0057aae8 = 4;
    FUN_004e5089();

    if (s.num_candidates <= unk_00939340)
    {
      unk_00939340 = s.num_candidates - 1;
    }
  }

  if (s.candidates[unk_00939340] != 0xffffffffU)
  {
    if (DAT_008a8de4 > 0xf)
    {
      --DAT_008a8de4;
    }

    DAT_0091bf70[DAT_008a8de4] =
        PLAYER_CARD_INSTANCE(player, (int)s.candidates[unk_00939340]).internal_card_id;
    DAT_007abc00[DAT_008a8de4] = s.candidates[unk_00939340];
    ++DAT_008a8de4;
  }

  return s.candidates[unk_00939340];
}

// FUNCTION: MAGIC 0x00440ad2
int FUN_00440ad2(int player, int card, int arg_3)
{
  struct
  {
    int trace_counter;       /* iVar2 / local_324 */
    char dialog_prompt[300]; /* local_320 */
    char trace_text[500];    /* local_1f4 */
  } locals;

  if ((unk_00926804 & 2) != 0)
  {
    locals.trace_counter = DAT_00777aa0;
    ++DAT_00777aa0;
    sprintf(locals.trace_text, "%d: Player #%d is processing %s(%d).\n",
            locals.trace_counter, player,
            global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].name,
            card);
    append_to_trace_txt(locals.trace_text);
  }

  unk_00777850 = 1;
  FUN_00443ee2(player, card, 0x7e, arg_3, 0);

  if (spell_fizzled == 1)
  {
    obliterate_top_card_of_stack();
    unk_00777850 = 0;
    return 0;
  }

  if (unk_008a9000 != 1)
  {
    set_stack_damage_targets();
    if (current_turn != unk_008b35ec)
    {
      FUN_004eca6d(global_ui_strings_filename, "PROMPT_PROC1");
      sprintf(locals.dialog_prompt, text_lines[0], DAT_007a7c60);
      raw_do_dialog(player, card, -1, -1, locals.dialog_prompt, 0);
    }
  }

  PLAYER_CARD_INSTANCE(player, card).state |= STATE_PROCESSING;
  locals.trace_counter = resolve_top_card_on_stack();

  unk_00777850 = 0;
  return locals.trace_counter;
}

// FUNCTION: MAGIC 0x0044096f
int FUN_0044096f(int player, int card)
{
  int internal_card_id;
  int has_mana_source_and_color;
  int response_player;
  int saved_trigger_cause_controller;
  int saved_trigger_cause;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  has_mana_source_and_color =
      ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) != 0 && produced_mana_color != -1);

  if (spell_fizzled == 1)
  {
    return 1;
  }

  if (!has_mana_source_and_color && unk_007a7c1c == 0)
  {
    sprintf(unk_00748770, gs_activate_0091b2d0, FUN_0044a3bf(player, card));

    if (DAT_00742fc8 != 0 && (unk_008b35ec == player || (unk_00926804 & 2) != 0))
    {
      response_player = -1;
    }
    else
    {
      response_player = -2;
    }

    allow_response(response_player, current_phase, unk_00748770, 0x6d);
  }

  resolve_top_card_on_stack();

  saved_trigger_cause_controller = trigger_cause_controller;
  saved_trigger_cause = trigger_cause;

  trigger_cause_controller = player;
  trigger_cause = card;
  dispatch_trigger_twice_once_with_each_player_as_reason(human_player, 0xd2, gs_tapping_00925d40, 0);

  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;

  return 1;
}

// FUNCTION: MAGIC 0x00442cf9
int FUN_00442cf9(int player, int card)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) == 0)
  {
    return 0;
  }

  return (global_cards_data[internal_card_id].extra_ability & EA_ACT_ABILITY) == 0;
}

// FUNCTION: MAGIC 0x004af0ec
int FUN_004af0ec(int player, int card)
{
  int response_controller;
  int internal_card_id;

  response_controller = player;
  if ((unk_00926804 & 2) == 0)
  {
    response_controller = unk_008b35ec;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_IN_PLAY) == 0)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_INVISIBLE | STATE_SUMMONSICK)) != 0)
    {
      return 0;
    }

    if (trigger_condition != -1 &&
        global_cards_data[internal_card_id].code_pointer != card_death_ward)
    {
      return 0;
    }

    if (DAT_00791418 != 0 && ((global_cards_data[internal_card_id].type & 0x20) == 0))
    {
      return 0;
    }

    if ((DAT_00742f68 & (unsigned char)global_cards_data[internal_card_id].type) != 0 &&
        FUN_0043fdb3(player, player, card) != 0 &&
        (((unk_008b4278 & 4) == 0 ||
          (global_cards_data[internal_card_id].extra_ability & 0x1004) != 0) ||
         (DAT_007aadec == 0xd3 &&
          (global_cards_data[internal_card_id].extra_ability & 0x2000) != 0)) &&
        ((response_controller == player ||
          (_DAT_0064f0a4 & (int)(char)global_cards_data[internal_card_id].act_phases) != 0) &&
         dispatch_event_to_single_card(player, card, 0x74, 1 - player, -1) != 0))
    {
      return 3;
    }
  }
  else
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) != 0)
    {
      return 0;
    }

    if (trigger_condition != -1 &&
        (int)PLAYER_CARD_INSTANCE(player, card).unknown0x14 == trigger_condition)
    {
      if (current_turn == player)
      {
        DAT_007ab2bc |= 4;
        DAT_00791410 += 1;
        return 2;
      }
      return 0;
    }

    if (trigger_condition != -1)
    {
      int trigger_result;
      unsigned char trigger_result_byte;

      trigger_result = FUN_004b082f(player, card, 0x7d, player);
      if (trigger_result == 0)
      {
        return 0;
      }

      trigger_result_byte = (unsigned char)trigger_result;
      DAT_007ab2bc |= 1 << (trigger_result_byte & 0x1f);
      DAT_00791410 += 1;

      if (trigger_result > 1)
      {
        return 2;
      }

      return 3;
    }

    if (((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) == 0 &&
         (global_cards_data[internal_card_id].extra_ability & 1) != 0 &&
         (DAT_00742f68 & 0x10) != 0) ||
        ((global_cards_data[internal_card_id].extra_ability & 2) != 0 &&
         (DAT_00742f68 & 0x20) != 0))
    {
      if (DAT_00791418 != 0 &&
          (global_cards_data[internal_card_id].extra_ability & 2) == 0)
      {
        return 0;
      }

      if ((((unk_008b4278 & 4) == 0 ||
            (global_cards_data[internal_card_id].extra_ability & 0x1004) != 0) ||
           (DAT_007aadec == 0xd3 &&
            (global_cards_data[internal_card_id].extra_ability & 0x4000) != 0)) &&
          ((unk_008b3270 = unk_008b3270 & 0xfffffffd,
            response_controller == player ||
            (_DAT_0064f0a8 & (int)(char)global_cards_data[internal_card_id].act_phases) != 0) &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0) &&
          dispatch_event_to_single_card(player, card, 0x73, 1 - player, -1) != 0)
      {
        if ((unk_008b3270 & 2U) != 0)
        {
          DAT_007ab2bc |= 4;
          return 2;
        }

        DAT_007ab2bc |= 2;
        return 3;
      }
    }

    if (DAT_007aadec == 4 && (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 1) != 0)
    {
      unk_008b3270 |= 3;
      DAT_007ab2bc |= 4;
      return 2;
    }

    if (DAT_007aadec == 4 && player == unk_00742f60 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x10) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x88) == 0 &&
        FUN_00445b56(player, card) != 0)
    {
      DAT_007ab2bc |= 2;

      if (active_player == player && (unk_00926804 & 2) == 0)
      {
        return 2;
      }

      return 3;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044b6af
int FUN_0044b6af(int player, int card)
{
  int internal_card_id;

  if (!is_in_play(player, card))
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  if ((global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE) == 0)
  {
    return 0;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    return 0;
  }

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_SUMMONSICK_BOTH) != 0 &&
      (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MAGIC 0x0044b7d4
int FUN_0044b7d4(int player, int card)
{
  unk_00938e2c = 0x3e;
  produced_mana_color = -1;

  dispatch_event_to_single_card(player, card, 0x6d, 1 - player, -1);

  if (spell_fizzled == 1)
  {
    spell_fizzled = 0;
  }
  else
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, 0x81);
    }

    FUN_004afa4b(player);
    regenerate_or_graveyard_triggers();
  }

  unk_00938e2c = 0;
  return produced_mana_color;
}

// FUNCTION: MAGIC 0x0043ff32
int activate(int who_activates, int player, int card)
{
  (void)who_activates;
  (void)player;
  (void)card;
  return 0;
}

// FUNCTION: MAGIC 0x0050aa68
void FUN_0050aa68(void)
{
}

// FUNCTION: MAGIC 0x00446cb8
void FUN_00446cb8(char *text)
{
  (void)text;
}

// FUNCTION: MAGIC 0x0050aa5d
void FUN_0050aa5d(void)
{
}

// FUNCTION: MAGIC 0x004b5fc9
void FUN_004b5fc9(char *text)
{
  if (unk_008a9000 != 1)
  {
    FUN_0050aa68();
    FUN_00446cb8(text);
    text[0] = '\0';
    FUN_0050aa5d();
  }
}

// FUNCTION: MAGIC 0x00444d48
int allow_response(int param_1, int param_2, char *param_3, int param_4)
{
  int saved_allow_response_flag;
  int result;

  saved_allow_response_flag = DAT_00742fc8;

  if (unk_008b2934 == 0)
  {
    if (FUN_0044aa01(human_player) != 0 && param_1 != -2 && unk_008a9000 != 1)
    {
      DAT_00742fc8 = 1;
    }
  }

  do
  {
    unk_00926790 = 0;
    result = FUN_00444e5f(param_1, param_2, param_3, param_4);
  } while (unk_00926790 != 0 && unk_008b2934 <= 0);

  if (unk_008b2934 == 0)
  {
    if (unk_008a9000 != 1)
    {
      DAT_00742fc8 = 0;
    }
    {
      int zone_index;
      unsigned char tmp;

      zone_index = human_player * 0x26 + current_phase;
      tmp = DAT_007abc90[zone_index];
      tmp &= 0xfd;
      DAT_007abc90[zone_index] = tmp;
    }
  }
  else if (unk_008a9000 != 1)
  {
    DAT_00742fc8 = saved_allow_response_flag;
  }

  return result;
}

// FUNCTION: MAGIC 0x004ad7e0
int FUN_004ad7e0(int reason_for_trigger_controller, const char *prompt)
{
  struct
  {
    char prompt_format[300]; // local_29c
    unsigned int selected_card; // local_170
    int saved_unk_00742f60; // local_16c
    unsigned int maybe_card; // local_168
    int some_flag; // local_164
    unsigned int response_code; // local_160
    unsigned int card; // local_15c
    int saved_unk_008b4278; // local_158
    unsigned int response_mask; // local_154
    int player; // local_150
    unsigned int result_flags; // local_14c
    char prompt_copy[300]; // local_148
    int internal_card_id; // local_1c
    int interrupts_available; // local_18
    int selected_player; // local_14
    int can_respond; // local_10
    int maybe_player; // local_c
    int num_candidates; // local_8
  } s;

  card_instance_t *instance;
  int tmp;

  if (life[unk_008b35ec] < 1)
  {
    DAT_007abc90[human_player * 0x26 + current_phase] |= 2;
  }

  s.interrupts_available = 0;
  if (unk_008a9000 != 1 && unk_00716244 == human_player && current_phase == unk_00716248)
  {
    DAT_00742fc8 = 0;
  }

  s.can_respond = FUN_0044ac96(reason_for_trigger_controller, current_phase);

  strcpy(s.prompt_copy, prompt);

  s.saved_unk_008b4278 = unk_008b4278;
  s.saved_unk_00742f60 = unk_00742f60;
  s.selected_player = reason_for_trigger_controller;
  unk_00742f60 = reason_for_trigger_controller;

  s.player = reason_for_trigger_controller;

  _DAT_00742fbc = 1;
  DAT_00742f68 = _DAT_00743020 & 0x30;
  _DAT_00743088 = 0;

  ++DAT_007161d0;
  if (DAT_007161d0 < DAT_00742f64)
  {
    DAT_00742f64 = 0;
  }

  if ((unk_00926804 & 2) == 0)
  {
    _DAT_0064f0a4 = 2;
    _DAT_0064f0a8 = 0x20;

    if (current_phase > 0x16)
    {
      _DAT_0064f0a4 = 4;
      _DAT_0064f0a8 = 0x40;
    }
    if (current_phase < 0x15)
    {
      _DAT_0064f0a4 = 1;
      _DAT_0064f0a8 = 0x10;
    }
    if (current_phase > 0x1d)
    {
      _DAT_0064f0a4 = 8;
      _DAT_0064f0a8 = (int)0xffffff80;
    }
    if (current_phase == 0x1f)
    {
      _DAT_0064f0a4 = 0xf;
      _DAT_0064f0a8 = (int)0xfffffff0;
    }
    if (active_player == reason_for_trigger_controller && global_stack_cards[0].player == unk_008b35ec)
    {
      _DAT_0064f0a4 = 0xf;
      _DAT_0064f0a8 = (int)0xfffffff0;
    }
  }
  else
  {
    _DAT_0064f0a4 = 0xf;
    _DAT_0064f0a8 = (int)0xfffffff0;
  }

  if ((unk_00926804 & 2) == 0)
  {
    if (unk_008a9000 == 1 || DAT_0072c8e4 != 0 ||
        (trigger_condition != -1 && current_turn == active_player) || DAT_007aadec == 4 ||
        (active_player == reason_for_trigger_controller && (unk_008b4278 & 0x200) != 0))
    {
      s.selected_card = FUN_004aec05(reason_for_trigger_controller);
      s.selected_player = unk_00742fcc;
    }
    else
    {
      s.selected_card = 0xffffffffU;
    }

    s.result_flags = 0;

    if (s.selected_card != 0xffffffffU &&
        FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player) == 2)
    {
      FUN_00440ad2(s.selected_player, (int)s.selected_card, s.player);
      s.selected_card = 0xffffffffU;
      TENTATIVE_reassess_all_cards();
    }

    if (s.selected_card != 0xffffffffU)
    {
      instance = &PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card);
      _DAT_00743030 = instance->internal_card_id;
      s.internal_card_id = _DAT_00743030;

      if (!is_in_play(s.selected_player, (int)s.selected_card))
      {
        tmp = FUN_0043ece1(s.selected_player, (int)s.selected_card);
        if (tmp)
        {
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            DAT_00742f68 = _DAT_00743020 & 0x20;
          }

          s.result_flags = 1;
          TENTATIVE_reassess_all_cards();

          if (unk_008a9000 != 1 && internal_rand(3) == 0)
          {
            strcpy(unk_00748770, "Didn't expect that, did ya?");
            FUN_004b5fc9(unk_00748770);
          }
        }
      }
      else
      {
        if (instance->unknown0x14 == (unsigned int)trigger_condition && DAT_007ab2cc != NULL && trigger_condition != -1)
        {
          DAT_007ab2cc(s.selected_player, (int)s.selected_card);
        }
        else
        {
          dispatch_event_to_single_card(s.selected_player, (int)s.selected_card, 0x73, 1 - s.selected_player, -1);
          if (activate(s.player, s.selected_player, (int)s.selected_card))
          {
            FUN_0044096f(s.selected_player, (int)s.selected_card);
          }
          spell_fizzled = 0;
        }

        s.result_flags = 1;
        TENTATIVE_reassess_all_cards();
      }
    }
  }
  else
  {
    s.result_flags = 0;
  }

  s.response_mask = 0;
  s.num_candidates = 0;
  s.maybe_card = 0xffffffffU;

  if (unk_008a9000 != 1 ||
      (trigger_condition != -1 &&
       (current_turn == unk_008b35ec || (current_turn == active_player && (unk_00926804 & 2) != 0))))
  {
    if (((current_turn == unk_008b35ec) || (current_turn == active_player && (unk_00926804 & 2) != 0)) &&
        trigger_condition != -1)
    {
      for (s.player = 0; s.player < 2; ++s.player)
      {
        for (s.card = 0; (int)s.card < active_cards_count[s.player]; ++s.card)
        {
          if (is_in_play(s.player, (int)s.card))
          {
            s.response_code = FUN_004b082f(s.player, (int)s.card, 0x7d, reason_for_trigger_controller);
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
                s.response_mask |= 1U << ((unsigned char)s.response_code & 0x1f);
              }
            }
          }

          instance = &PLAYER_CARD_INSTANCE(s.player, (int)s.card);
          if ((DAT_00789714 & 1) != 0 && instance->unknown0x14 == (unsigned int)trigger_condition &&
              current_turn == s.player && trigger_condition != -1 && instance->internal_card_id != -1)
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
      int skip_to_cleanup;
      skip_to_cleanup = 0;

      if (unk_008a9000 == 1)
      {
        skip_to_cleanup = 1;
      }
      else
      {
        if (trigger_condition == 0xca && DAT_007abc90[human_player * 0x26 + 4] == 0)
        {
          DAT_00789714 = 0;
        }
        if (trigger_condition == 0xce && DAT_007abc90[human_player * 0x26 + 0xa] == 0)
        {
          DAT_00789714 = 0;
        }
      }

      if (!skip_to_cleanup)
      {
        if (s.maybe_card != 0xffffffffU || s.response_mask != 0 || ((DAT_00789714 & 1) != 0 && _DAT_00743020 != 0))
        {
          if ((unk_00926804 & 2) == 0)
          {
            s.player = unk_008b35ec;
          }
          else
          {
            s.player = reason_for_trigger_controller;
          }

          unk_008b3270 = 0;
          DAT_00789714 = 0;
          s.some_flag = 0;

          for (s.card = 0; (int)s.card < active_cards_count[s.player]; ++s.card)
          {
            instance = &PLAYER_CARD_INSTANCE(s.player, (int)s.card);
            if (instance->internal_card_id != -1 &&
                ((instance->upkeep_flags & 1) != 0 || (instance->upkeep_flags & 0x10) != 0 ||
                 (!FUN_00442cf9(s.player, (int)s.card) && s.player == unk_00742f60)) &&
                (((s.response_code = FUN_004af0ec(s.player, (int)s.card), s.response_code > 1) ||
                  (((DAT_00777aa8 != 0 || s.player != human_player) && (s.response_code & 2) != 0) ||
                   (unk_008b3270 & 2) != 0)) &&
                 ((current_turn != s.player || trigger_condition == -1) ||
                  (s.response_code != 2 || (unk_008b3270 & 2) != 0))))
            {
              if ((unk_008b3270 & 2) == 0 && s.response_code != 2)
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

              unk_008b3270 &= ~2;
            }
          }

          if (DAT_007aadec == 4)
          {
            int other_player;
            other_player = 1 - s.player;
            for (s.card = 0; (int)s.card < active_cards_count[other_player]; ++s.card)
            {
              instance = &PLAYER_CARD_INSTANCE(other_player, (int)s.card);
              if (instance->internal_card_id != -1 &&
                  ((instance->upkeep_flags & 1) != 0 || (instance->upkeep_flags & 0x10) != 0 || !FUN_00442cf9(other_player, (int)s.card)) &&
                  (s.response_code = FUN_004af0ec(other_player, (int)s.card), (unk_008b3270 & 2) != 0))
              {
                s.maybe_card = s.card;
                s.maybe_player = other_player;
                ++s.num_candidates;

                unk_008b3270 &= ~2;

                if (s.response_code == 2)
                {
                  s.response_mask |= 4;
                }
                else
                {
                  s.response_mask |= 2;
                }

                if ((unk_00926804 & 2) == 0)
                {
                  break;
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
  }

  if (s.interrupts_available || s.can_respond || s.num_candidates)
  {
    strcpy(s.prompt_format, gs_triggered_effects_009396a0);

    if ((DAT_00742f68 & 0x10) == 0 || unk_008ce508 != -1)
    {
      if ((DAT_00742f68 & 0x20) != 0)
      {
        if (DAT_007a7d78 != 0)
        {
          DAT_007a7d78 = 1;
        }
        strcpy(s.prompt_format, gs_interrupts_0093a850);
      }
    }
    else
    {
      strcpy(s.prompt_format, gs_fast_effects_00926520);
    }

    if (trigger_condition != -1)
    {
      strcpy(s.prompt_format, gs_triggered_effects_009396a0);
    }

    sprintf(unk_00748770, s.prompt_format, s.prompt_copy);

    if (DAT_00742fc8 == 0 || (s.response_mask & 2) != 0)
    {
      if (((s.can_respond == 0 && (DAT_00777aa8 == 0 || trigger_condition != -1)) &&
           (s.interrupts_available == 0 || trigger_condition == -1) &&
           (DAT_00561390 == 0 || (s.response_mask & 6) == 0)) &&
          (s.num_candidates <= (int)((s.response_mask & 2) == 0 && (s.response_mask & 4) == 0) ||
           ((s.some_flag == 0 && FUN_0044aa01(reason_for_trigger_controller) != 0) || trigger_condition == 0xd6 ||
            (s.maybe_card != 0xffffffffU && DAT_00742f64 == DAT_007161d0))))
      {
        s.selected_card = s.maybe_card;
        s.selected_player = s.maybe_player;
        _DAT_00743088 = 1;
      }
      else
      {
        if (unk_008a9000 != 1)
        {
          DAT_0093a844 = 1;
        }

        DAT_00742f64 = 0;

        tmp = 0;
        while (!tmp)
        {
          if (unk_008a9000 == 1)
          {
            s.selected_card = s.maybe_card;
            tmp = 1;
            _DAT_0074303c = -1;
          }
          else
          {
            if ((unk_00926804 & 2) == 0)
            {
              s.player = unk_008b35ec;
            }
            else
            {
              s.player = reason_for_trigger_controller;
            }

            if (unk_008b35ec == s.player)
            {
              unk_00716244 = -1;
              unk_00716248 = -1;
            }
            else
            {
              _DAT_00743034 = -1;
              unk_007161d4 = -1;
            }

            s.selected_card = FUN_004e9c50(s.player, -1, s.player, 0xff, 0, unk_00748770, 2);
            s.selected_player = unk_00742fcc;
            if ((int)s.selected_card >= 0)
            {
              DAT_007abc90[human_player * 0x26 + current_phase] |= 2;
            }
          }

          if (_DAT_0074303c == -3)
          {
            tmp = 0;
          }
          else if (_DAT_0074303c == -2)
          {
            tmp = 1;
            s.selected_card = 0xffffffffU;

            DAT_007ab2bc &= ~2;
            if (trigger_condition != -1)
            {
              _DAT_0074303c = 0;
            }

            if (s.num_candidates != 0)
            {
              DAT_00742f64 = DAT_007161d0;
              _DAT_00743088 = 1;
              if (unk_008a9000 != 1)
              {
                DAT_00742fc8 = 1;
              }
            }
          }
          else if (_DAT_0074303c == 0)
          {
            if (s.selected_player != -1 && s.selected_card != 0xffffffffU)
            {
              tmp = 1;
            }
            else if (s.selected_player != -1 && s.selected_card == 0xffffffffU)
            {
              tmp = 0;
            }
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
        _DAT_00743088 = 1;
      }
    }

    /* original uses an empty string at 0x00573048 */
    unk_00748770[0] = '\0';

    if ((unk_00926804 & 2) == 0)
    {
      s.player = unk_008b35ec;
    }
    else
    {
      s.player = reason_for_trigger_controller;
    }

    if (s.selected_card != 0xffffffffU &&
        (s.selected_player == s.player ||
         (((PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card).state & 2) != 0 &&
           FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player) != 0) ||
          DAT_007aadec == 4)))
    {
      instance = &PLAYER_CARD_INSTANCE(s.selected_player, (int)s.selected_card);
      s.internal_card_id = instance->internal_card_id;

      if (FUN_004af0ec(s.selected_player, (int)s.selected_card) == 0)
      {
        if (FUN_0044b6af(s.selected_player, (int)s.selected_card) != 0 && s.selected_player == reason_for_trigger_controller)
        {
          FUN_0044b7d4(s.selected_player, (int)s.selected_card);
        }
      }
      else
      {
        if (!is_in_play(s.selected_player, (int)s.selected_card))
        {
          FUN_0043ece1(s.selected_player, (int)s.selected_card);
          if (global_cards_data[s.internal_card_id].type == 0x20)
          {
            DAT_00742f68 = _DAT_00743020 & 0x20;
          }

          if (internal_rand(3) == 0)
          {
            strcpy(unk_00748770, "I knew that was coming.");
            FUN_004b5fc9(unk_00748770);
          }
        }
        else
        {
          tmp = FUN_004b082f(s.selected_player, (int)s.selected_card, 0x7d, s.player);
          if (!tmp)
          {
            if (instance->unknown0x14 == (unsigned int)trigger_condition && DAT_007ab2cc != NULL && trigger_condition != -1)
            {
              DAT_007ab2cc(s.selected_player, (int)s.selected_card);
            }
            else
            {
              if (activate(s.player, s.selected_player, (int)s.selected_card))
              {
                FUN_0044096f(s.selected_player, (int)s.selected_card);
              }

              if (spell_fizzled != 1 && unk_008a9000 != 1)
              {
                play_sound_effect(0x26);
              }

              spell_fizzled = 0;
            }
          }
          else
          {
            FUN_00440ad2(s.selected_player, (int)s.selected_card, s.player);
          }
        }

        TENTATIVE_reassess_all_cards();
        s.result_flags |= 2;
      }

      s.result_flags |= 2;
    }

    DAT_00789714 = 1;
    _DAT_00743088 = 0;
  }

  if (s.result_flags == 0)
  {
    DAT_00742f68 = _DAT_00743020 & 0x30;
  }

  _DAT_00742fbc = 0;
  unk_00742f60 = s.saved_unk_00742f60;
  unk_008b4278 = s.saved_unk_008b4278;
  --DAT_007161d0;

  return (int)s.result_flags;
}

// FUNCTION: MAGIC 0x004d91c9
void FUN_004d91c9(char *out, int csvid, int field, const char *csv_name)
{
  struct
  {
    int current_id;
    char in_quotes;
    char field_idx;
    char delim[11];
    char token[513];
    FILE *fp;
    int fscanf_ret;
  } s;

  s.fp = fopen(csv_name, "rb");
  s.in_quotes = 0;
  s.field_idx = '\0';
  out[0] = '\0';

  if (master_csv_offsets[csvid] != -1 && strcmp(csv_name, "master.csv") == 0)
  {
    fseek(s.fp, (long)master_csv_offsets[csvid], 0);
  }

  while ((s.fscanf_ret = fscanf(s.fp, "%[^,\n] %[,\n]", s.token, s.delim)) != 0)
  {
    if (s.token[0] == '0')
    {
      s.current_id = atoi(s.token);
    }

    if (csvid == s.current_id)
    {
      ++s.field_idx;

      if (s.field_idx == field && s.in_quotes)
      {
        strcat(out, ",");
      }

      if (s.token[0] == '"')
      {
        s.in_quotes = 1;
      }

      if (s.field_idx == field)
      {
        strcat(out, s.token);
      }

      if (s.token[strlen(s.token) - 1] == '"')
      {
        s.in_quotes = 0;
      }

      if (s.in_quotes)
      {
        --s.field_idx;
      }
    }

    if (s.fscanf_ret == -1 || (s.field_idx != '\0' && csvid != s.current_id))
    {
      break;
    }
  }

  fclose(s.fp);
}

// FUNCTION: MAGIC 0x004e1c8c
void FUN_004e1c8c(unsigned int internal_card_id, int unk1, char *prompt, int unk2)
{
  (void)internal_card_id;
  (void)unk1;
  (void)prompt;
  (void)unk2;
}

// FUNCTION: MAGIC 0x00435c1f
int FUN_00435c1f(void)
{
  int color;

  if (unk_005607e0 < 10)
  {
    for (color = 0; color < 7; ++color)
    {
      unk_00637558[unk_005607e0][color] = 0;
    }
    ++unk_005607e0;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435cdd
int FUN_00435cdd(void)
{
  if (unk_005607e0 > 0)
  {
    --unk_005607e0;
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435d11
int FUN_00435d11(int player)
{
  int color;

  if (unk_005607e0 > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      raw_mana_available[player][color] += unk_00637558[unk_005607e0 - 1][color];
      raw_mana_available[player][7] += unk_00637558[unk_005607e0 - 1][color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x004438cb
int FUN_004438cb(int param_1)
{
  int stack_index;
  int player;
  int card;
  int saved_parent_card;
  int saved_parent_controller;
  int saved_timestamp;

  stack_index = unk_008b2934 - 1;
  player = global_stack_cards[stack_index].player;
  card = global_stack_cards[stack_index].card;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == unk_0091a80c)
  {
    saved_parent_card = PLAYER_CARD_INSTANCE(player, card).parent_card;
    saved_parent_controller = PLAYER_CARD_INSTANCE(player, card).parent_controller;
    saved_timestamp = PLAYER_CARD_INSTANCE(player, card).timestamp;

    memcpy(&PLAYER_CARD_INSTANCE(player, card), &PLAYER_CARD_INSTANCE(saved_parent_controller, saved_parent_card), 300);
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = unk_0091a80c;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
    PLAYER_CARD_INSTANCE(player, card).state |= 2;

    PLAYER_CARD_INSTANCE(player, card).parent_controller = saved_parent_controller;
    PLAYER_CARD_INSTANCE(player, card).parent_card = saved_parent_card;
    PLAYER_CARD_INSTANCE(player, card).timestamp = saved_timestamp;

    if (PLAYER_CARD_INSTANCE(saved_parent_controller, saved_parent_card).internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(saved_parent_controller, saved_parent_card).internal_card_id;
    }

    if ((int)PLAYER_CARD_INSTANCE(player, card).original_internal_card_id < unk_009266a4 ||
        (unk_009266a4 + 0x2d) <= (int)PLAYER_CARD_INSTANCE(player, card).original_internal_card_id)
    {
      unsigned int packed_id;

      packed_id =
          (unsigned int)global_cards_data[PLAYER_CARD_INSTANCE(saved_parent_controller, saved_parent_card).original_internal_card_id].id;
      PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id = (unsigned short)packed_id;
      PLAYER_CARD_INSTANCE(player, card).display_pic_num = (unsigned short)(packed_id >> 16);
    }
  }

  if (unk_008a9000 != 1)
  {
    unk_007a7750[stack_index] = param_1;
  }

  global_stack_damage_targets[stack_index].player =
      (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
  global_stack_damage_targets[stack_index].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
  return 0;
}

// FUNCTION: MAGIC 0x00444443
int set_stack_damage_targets(void)
{
  struct
  {
    int card;
    int stack_index;
    int player;
  } s;

  for (s.stack_index = 0; s.stack_index < unk_008b2934; ++s.stack_index)
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

// FUNCTION: MAGIC 0x004427d9
int FUN_004427d9(int param_1)
{
  struct
  {
    int count;
    int card;
    int internal_card_id;
  } s;

  s.count = 0;
  for (s.card = 0; s.card < active_cards_count[param_1]; ++s.card)
  {
    s.internal_card_id = PLAYER_CARD_INSTANCE(param_1, s.card).internal_card_id;
    if (s.internal_card_id != -1)
    {
      if ((PLAYER_CARD_INSTANCE(param_1, s.card).state & 2) == 0)
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

int FUN_00496128(int windows, int count);
int FUN_004961ab(HWND param_1, int param_2, int param_3);

// FUNCTION: MAGIC 0x00495fa1
void FUN_00495fa1(void)
{
  struct
  {
    HWND *next;       // ebp-0x3c
    int count;        // ebp-0x38
    int best_index;   // ebp-0x34
    int i;            // ebp-0x30
    HWND wnd;         // ebp-0x2c
    int found_index;  // ebp-0x28
    int changed;      // ebp-0x24
    HWND windows[8];  // ebp-0x20
  } s;

  extern int DAT_007a7d74;
  extern int DAT_00895204;
  extern int DAT_00896714;
  extern int DAT_008cff0c;
  extern int DAT_0091c0f0;
  extern int DAT_0091c4f8;

  s.windows[7] = (HWND)DAT_007a7d74;
  s.windows[6] = DAT_0093a7b0;
  s.windows[5] = (HWND)DAT_0091c0f0;
  s.windows[4] = (HWND)DAT_008cff0c;
  s.windows[3] = DAT_0094ca30;
  s.windows[2] = (HWND)DAT_00895204;
  s.windows[1] = (HWND)DAT_0091c4f8;
  s.windows[0] = (HWND)DAT_00896714;

  s.count = 8;
  s.changed = 0;
  s.wnd = (HWND)unk_008cf1b4;
  s.best_index = -1;

  while (s.wnd != (HWND)0)
  {
    s.wnd = GetWindow(s.wnd, 3);
    s.i = 0;
    s.found_index = -1;

    for (;;)
    {
      if (s.i >= s.count)
      {
        break;
      }
      if (s.found_index != -1)
      {
        break;
      }
      if (s.windows[s.i] == s.wnd)
      {
        s.found_index = s.i;
      }
      ++s.i;
    }

    if (s.found_index != -1)
    {
      if (IsWindowVisible(s.wnd))
      {
        if (s.found_index < s.best_index)
        {
          s.changed = 1;
        }
        s.best_index = s.found_index;
      }
    }
  }

  if (s.changed != 0)
  {
    s.found_index = FUN_00496128((int)&s.windows[0], s.count);
    if (s.found_index != -1)
    {
      SetWindowPos(s.windows[s.found_index], (HWND)1, 0, 0, 0, 0, 3);

      if (s.found_index + 1 >= s.count)
      {
        s.next = (HWND *)0;
      }
      else
      {
        s.next = &s.windows[s.found_index + 1];
      }

      FUN_004961ab(s.windows[s.found_index], (int)s.next, s.count - (s.found_index + 1));
    }
  }
}

// FUNCTION: MAGIC 0x00496128
int FUN_00496128(int windows, int count)
{
  int result;
  int i;
  HWND *windows_arr;

  result = -1;
  if (windows == 0 || count == 0)
  {
    return -1;
  }

  windows_arr = (HWND *)windows;
  for (i = 0; i < count && result == -1; ++i)
  {
    if (IsWindowVisible(windows_arr[i]))
    {
      result = i;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x004961ab
int FUN_004961ab(HWND param_1, int param_2, int param_3)
{
  struct
  {
    HWND *next;  // ebp-0x8
    int i;       // ebp-0x4
  } s;
  HWND *windows_arr;

  if (param_2 == 0 || param_3 <= 0)
  {
    return 0;
  }

  windows_arr = (HWND *)param_2;
  s.i = FUN_00496128(param_2, param_3);
  if (s.i != -1)
  {
    SetWindowPos(windows_arr[s.i], param_1, 0, 0, 0, 0, 3);

    if (s.i + 1 >= param_3)
    {
      s.next = (HWND *)0;
    }
    else
    {
      s.next = &windows_arr[s.i + 1];
    }

    FUN_004961ab(windows_arr[s.i], (int)s.next, param_3 - (s.i + 1));
    return 1;
  }
  else
  {
    return 0;
  }
}
