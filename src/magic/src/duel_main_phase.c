#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"

extern int life[2];
extern char DAT_00896690;
extern int _DAT_0091c0ec;
#ifdef SHANDALAR
char DAT_0091ce40[300];
#else
extern char DAT_0091ce40[300];
#endif
void TENTATIVE_reassess_all_cards();
void append_to_trace_txt(char *text);
void FUN_004432ed(int player, int phase);
void FUN_00444d1f(void);
int FUN_0044ac96(int player, int phase);
int allow_response(int param_1, int param_2, char *param_3, int param_4);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void FUN_00441cf2(int param_1, int param_2);
void __stdcall FUN_004e4e9a(void);
void FUN_0044b3d4(void);
int check_duel_finished(void);
void FUN_004b5fc9(char *text);
int FUN_0044118f(int player);
int FUN_0044ae9e(int phase);
int FUN_0044aa01(int player);
int FUN_004e9c50(int player, int a2, int a3, int a4, int a5, char *prompt, int a7);
int FUN_00441565(int player);
int FUN_004ad7e0(int player, char *prompt);
int FUN_004e3380(int player);
int put_card_on_stack(int player, int card, int pay_costs);
int FUN_0043f9ed(int player, int card);
int dispatch_event_to_single_card(int player, int card, int event, int a4, int a5);
int activate(int player, int a2, int card);
void FUN_0044096f(int player, int card);
void FUN_004a61a1(int player, int card, int a3, int a4);
int FUN_0044125c(int player, int card);
void push_affected_card_stack(void);
void pop_affected_card_stack(void);
unsigned int C_get_abilities(int player, int card, int event, int a4);
int C_real_select_target();
void FUN_004a61d6(char *text);
void FUN_004afa4b(int player);
unsigned int FUN_00510a3f(int player);
void FUN_00440d82(int player);
void FUN_0050e79b(int player);
void FUN_005129d1(int player);
void FUN_00445d1a(int player);
void FUN_005136ed(int player);
void FUN_00515f34(int player);
void FUN_00441d78(void);

// GLOBAL: MAGIC 0x00637804
int DAT_00637804;

// GLOBAL: MAGIC 0x00637808
char DAT_00637808[0x100];

// GLOBAL: MAGIC 0x00637908
int _DAT_00637908;

// GLOBAL: MAGIC 0x0063790c
int DAT_0063790c;

// GLOBAL: MAGIC 0x00637910
int DAT_00637910;

// GLOBAL: MAGIC 0x0093f4ac
int DAT_0093f4ac;

// GLOBAL: MAGIC 0x0056e60c
char s__d__Entering_Main_Phase__0056e60c[0x1a] = "%d: Entering Main Phase.\n";

// GLOBAL: MAGIC 0x0056e628
char DAT_0056e628[4] = "";

// GLOBAL: MAGIC 0x0056e62c
char s__d__Main_Phase_skipping_PickACar_0056e62c[0x61] = "%d: Main Phase skipping PickACard. SkipPhase:%d ThePhase:%d mustattackers:%d UserWantsToStop:%d\n";

// GLOBAL: MAGIC 0x0056e690
char s__d__Main_Phase_PickACard__ThePha_0056e690[0x39] = "%d: Main Phase PickACard: ThePhase:%d Player:%d card:%d\n";

// GLOBAL: MAGIC 0x0056e6cc
char s__d__Main_Phase_Non_Phasing_playe_0056e6cc[0x3f] = "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e70c
char s__d__Main_Phase__No_attackers__No_0056e70c[0x4e] = "%d: Main Phase (No attackers) Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e75c
char DAT_0056e75c[4] = "";

// GLOBAL: MAGIC 0x0056e760
char s__d__Main_Phase_Non_Phasing_playe_0056e760[0x3f] = "%d: Main Phase Non-Phasing player's Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e7a0
char s__d__Main_Phase_assigning_attacke_0056e7a0[0x42] = "%d: Main Phase assigning attacker: ThePhase:%d Player:%d card:%d\n";

// GLOBAL: MAGIC 0x0056e7e4
char DAT_0056e7e4[4] = "";

// GLOBAL: MAGIC 0x0056e7e8
char s__d__Main_Phase_PX_Choose_Attacke_0056e7e8[0x31] = "%d: Main Phase PX Choose Attackers: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e81c
char s__d__Main_Phase_Attackers_Fast_Ef_0056e81c[0x34] = "%d: Main Phase Attackers Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e850
char s__d__Main_Phase_PX_Choose_Blocker_0056e850[0x30] = "%d: Main Phase PX Choose Blockers: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e880
char s__d__Main_Phase_Blockers_Fast_Eff_0056e880[0x33] = "%d: Main Phase Blockers Fast Effects: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e8b4
char s__d__Main_Phase_Combat__ThePhase__0056e8b4[0x24] = "%d: Main Phase Combat: ThePhase:%d\n";

// GLOBAL: MAGIC 0x0056e8d8
char s__Ouch__that_hurt___0056e8d8[0x14] = "'Ouch, that hurt.'";

// GLOBAL: MAGIC 0x0056e8ec
char s__Give_up__you_re_doomed___0056e8ec[0x1a] = "'Give up, you're doomed.'";

// FUNCTION: MAGIC 0x0044118f
int FUN_0044118f(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x00441565
int FUN_00441565(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x00440d82
void FUN_00440d82(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x00445d1a
void FUN_00445d1a(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x0044ae9e
int FUN_0044ae9e(int phase)
{
  (void)phase;
  return 0;
}

// FUNCTION: MAGIC 0x004e3380
int FUN_004e3380(int player)
{
  (void)player;
  return -1;
}

// FUNCTION: MAGIC 0x0050e79b
void FUN_0050e79b(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x00510a3f
unsigned int FUN_00510a3f(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x005129d1
void FUN_005129d1(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x005136ed
void FUN_005136ed(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x00515f34
void FUN_00515f34(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x0044cda7
int main_phase(unsigned int player, int phase_mode, int *phase_value)
{
  struct
  {
    int trace_choose_attackers_counter;
    int previous_attacker_count;
    int trace_assign_attacker_counter;
    int trace_nonphasing_postcombat_counter;
    int trace_no_attackers_counter;
    int trace_nonphasing_precombat_counter;
    int trace_pick_card_counter;
    int trace_skip_pick_card_counter;
    int trace_enter_phase_counter;
    char trace_combat[100];
    char trace_blockers_fast_effects[100];
    char trace_choose_blockers[100];
    char trace_attackers_fast_effects[100];
    char trace_choose_attackers[100];
    int bandmate_count;
    int bandmate_index;
    int bandmates[150];
    char trace_assign_attacker[100];
    int activation_result;
    char trace_nonphasing_postcombat_fast_effects[100];
    char trace_no_attackers_nonphasing_fast_effects[100];
    char trace_nonphasing_precombat_fast_effects[100];
    char trace_pick_card[100];
    char trace_skip_pick_card[100];
    char trace_enter_phase[100];
    int player_can_stop_here;
    target_t selected_bandmate;
    int human_stop_enabled;
    int other_player;
    int allow_response_result;
    int retry_phase_prompt;
    int skip_phase;
  } s;
  unsigned int *puVar1;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int saved_response_state;

  s.other_player = 1 - player;
  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_enter_phase_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace_enter_phase, s__d__Entering_Main_Phase__0056e60c, s.trace_enter_phase_counter);
    append_to_trace_txt(s.trace_enter_phase);
  }
  switch (phase_mode)
  {
  case 1:
    break;
  case 2:
    goto LAB_0044f33e;
  case 3:
  case 5:
  default:
    break;
  case 4:
    goto LAB_0044e02d;
  case 6:
    goto LAB_0044f0b1;
  case 7:
    goto LAB_0044e3a2;
  case 8:
    goto LAB_0044ee2f;
  }

  unk_008b60e0 = 0;
  land_can_be_played &= -512;
  current_phase = 0x14;
  FUN_004432ed(player, current_phase);
  phase_was_skipped = 0;
  phase_response_window_open = 0;
  if (g_duel_ai_mode_state != 1)
  {
    phase_stop_suppressed = 0;
  }
LAB_0044ced8:
  if ((player == active_player) && ((g_duel_network_flags & 2) == 0))
  {
    TENTATIVE_reassess_all_cards(0, 0xff);
    FUN_00441cf2(1, ((current_phase < 0x1e) - 1 & 0xffffffd3) + 0x5a);
    DAT_008a8d7c = 0;
  }
LAB_0044cf2b:
  do
  {
    phase_response_window_open = 1;
    _DAT_0091c0ec = 0;
    if ((player == active_player) && ((g_duel_network_flags & 2) == 0))
    {
      FUN_004e4e9a();
      DAT_008cdab4 = 0;
      DAT_008cdab0 = DAT_008cdab4;
      DAT_008a8de4 = DAT_008cdab0;
      ai_modifier = DAT_008a8de4;
      if ((g_duel_ai_mode_state != 1) && (((land_can_be_played & 0x40U) == 0 && (DAT_00896690 != '\0'))))
      {
        FUN_004b5fc9(&DAT_00896690);
        land_can_be_played |= 0x40;
        DAT_00896690 = '\0';
      }
    }
  LAB_0044cfcc:
    *phase_value = 0;
    _DAT_00637908 = 6;
    do
    {
      DAT_0093d850 = 0;
      if ((((player == active_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1)) &&
          ((DAT_00925d30 & 2U) != 0))
      {
        DAT_00925d30 = 0;
        goto LAB_0044ced8;
      }
      unk_00712938 = 0;
      if ((g_duel_ai_mode_state != 1) && (DAT_00896690 != '\0'))
      {
        FUN_004b5fc9(&DAT_00896690);
      }
      if ((player == nonactive_player) || ((g_duel_network_flags & 2) != 0))
      {
        if (spell_fizzled == 1)
        {
          spell_fizzled = 0;
        }
        do
        {
          s.retry_phase_prompt = 1;
          land_can_be_played |= 0x80;
          strcpy(unk_00748770, DAT_0056e628);
          if (current_phase <= 0x14)
          {
            strcpy(unk_00748770, gs_prompt_main_phase_precombat_cast_spells_008cc710);
            if ((land_can_be_played & 1U) == 0)
            {
              strcpy(unk_00748770, gs_prompt_main_phase_precombat_cast_spells_play_land_00777970);
            }
          }
          else if (current_phase >= 0x1e)
          {
            strcpy(unk_00748770, gs_prompt_main_phase_postcombat_cast_spells_007ab2d0);
            if ((land_can_be_played & 1U) == 0)
            {
              strcpy(unk_00748770, gs_prompt_main_phase_postcombat_cast_spells_play_land_0091cf50);
            }
          }
          else
          {
            if (player == human_player)
            {
              strcpy(unk_00748770, gs_prompt_combat_choose_attackers_0093d990);
            }
            else
            {
              strcpy(unk_00748770, gs_prompt_combat_choose_blockers_008b4140);
            }
            if ((FUN_0044118f(player) == 0) && (FUN_0044ae9e(0x15) == 0))
            {
              current_phase = 0x1e;
              s.retry_phase_prompt = 0;
            }
          }
        } while (s.retry_phase_prompt == 0);
        do
        {
          s.retry_phase_prompt = 1;
          FUN_00444d1f();
          phase_response_window_open = 1;
          phase_stop_suppressed = 0;
          if ((g_duel_network_flags & 2) != 0)
          {
            s.skip_phase = FUN_0044aa01(player);
            s.player_can_stop_here = FUN_0044ac96(player, current_phase);
          }
          if (FUN_0044aa01(player) != 0)
          {
            if ((current_phase != 0x15) || (DAT_0093f4ac == 0))
            {
              goto LAB_skip_pick_card;
            }
          }
          if (FUN_0044aa01(player) != 0)
            goto LAB_pick_card;
          if (current_phase != 0x1e)
            goto LAB_pick_card;
          if (FUN_0044ac96(player, current_phase) != 0)
            goto LAB_pick_card;
        LAB_skip_pick_card:
          _DAT_0074303c = -2;
          DAT_00637910 = 0xffffffff;
          if ((g_duel_network_flags & 2) != 0)
          {
            s.trace_skip_pick_card_counter = duel_trace_counter;
            duel_trace_counter++;
            sprintf(s.trace_skip_pick_card, s__d__Main_Phase_skipping_PickACar_0056e62c, s.trace_skip_pick_card_counter, s.skip_phase,
                    current_phase, DAT_0093f4ac, s.player_can_stop_here);
            append_to_trace_txt(s.trace_skip_pick_card);
          }
          goto LAB_after_pick_card;
        LAB_pick_card:
          DAT_00637910 = FUN_004e9c50(player, player, player, 0, 0, unk_00748770, 2);
          if ((g_duel_network_flags & 2) != 0)
          {
            s.trace_pick_card_counter = duel_trace_counter;
            duel_trace_counter++;
            sprintf(s.trace_pick_card, s__d__Main_Phase_PickACard__ThePha_0056e690, s.trace_pick_card_counter, current_phase,
                    player, DAT_00637910);
            append_to_trace_txt(s.trace_pick_card);
          }
        LAB_after_pick_card:
          if (DAT_00637910 == 0xfffffffe)
          {
            if (player == nonactive_player)
            {
              stop_phase_player = -1;
              stop_phase = stop_phase_player;
            }
            else
            {
              previous_stop_phase_player = -1;
              previous_stop_phase = previous_stop_phase_player;
            }
            DAT_00637910 = 0xffffffff;
          }
          if ((current_phase == 0x15) && ((int)DAT_00637910 < 0))
          {
            FUN_00441565(player);
            if (DAT_0093f4ac != 0)
            {
              s.retry_phase_prompt = 0;
            }
          }
        } while (s.retry_phase_prompt == 0);
        if ((_DAT_0074303c == -2) && (g_duel_ai_mode_state != 1))
        {
          if (current_phase < 0x18)
          {
            if ((stop_phase > 0x18) || (previous_stop_phase > 0x18))
            {
              phase_stop_suppressed = 1;
            }
            else
            {
              phase_stop_suppressed = 0;
            }
          }
          if (current_phase < 0x16)
          {
            if (stop_phase > 0x16)
            {
              phase_stop_suppressed = 1;
            }
            else
            {
              phase_stop_suppressed = 0;
            }
          }
        }
        land_can_be_played &= -129;
        if ((_DAT_0074303c == -2) &&
            ((((stop_phase_player == -1 || (stop_phase != -1)) || (previous_stop_phase_player == -1)) ||
              (previous_stop_phase != -1))))
        {
          if (current_phase <= 0x14)
          {
            if (battlefield_extra_ability_flags & 0x1000000)
            {
              C_dispatch_event_raw(0x89);
            }
            FUN_0044b3d4();
            if (check_duel_finished() != 0)
            {
              return 1;
            }
            FUN_004432ed(player, current_phase);
            if (!((unk_008b60e0 == 0) &&
                  ((FUN_0044118f(player) == 0) || (FUN_0044aa01(player) != 0)) &&
                  ((nonactive_player != stop_phase_player) || (stop_phase != 0x15)) &&
                  ((active_player != previous_stop_phase_player) || (previous_stop_phase != 0x15)) &&
                  (((char)DAT_007abc90[player * 0x26 + 0x15] & 5) == 0)))
            {
              if ((battlefield_extra_ability_flags & 0x2000000) != 0)
              {
                C_dispatch_event_raw(0x92);
              }
              if ((g_duel_network_flags & 2) != 0)
              {
                DAT_00789714 = 1;
                _DAT_00743020 = 0xffffffff;
                spell_fizzled = 0;
                unk_008b3270 = spell_fizzled;
                saved_response_state = DAT_00777aa8;
                FUN_00444d1f();
                phase_response_window_open = 1;
                phase_stop_suppressed = 0;
                s.human_stop_enabled = 0;
                if (((nonactive_player == human_player) &&
                     (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0)) ||
                    ((active_player == human_player &&
                      (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0))))
                {
                  s.human_stop_enabled = 1;
                }

                if ((FUN_0044aa01(player) == 0) &&
                    (((s.human_stop_enabled != 0 ||
                       ((human_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                      ((human_player == stop_phase_player && (stop_phase == current_phase))))))
                {
                  DAT_00777aa8 = 1;
                }
                else
                {
                  DAT_00777aa8 = 0;
                }
                if (DAT_00777aa8 != 0)
                {
                  s.trace_nonphasing_precombat_counter = duel_trace_counter;
                  duel_trace_counter++;
                  sprintf(s.trace_nonphasing_precombat_fast_effects, s__d__Main_Phase_Non_Phasing_playe_0056e6cc, s.trace_nonphasing_precombat_counter, current_phase);
                  append_to_trace_txt(s.trace_nonphasing_precombat_fast_effects);
                  if (FUN_004ad7e0(1 - human_player,
                                   gs_prompt_main_phase_precombat_cast_spells_008cc710) != 0)
                  {
                    DAT_00777aa8 = saved_response_state;
                    phase_response_window_open = 0;
                    phase_stop_suppressed = 0;
                    goto LAB_0044cf2b;
                  }
                }
                DAT_00777aa8 = saved_response_state;
                phase_response_window_open = 0;
                phase_stop_suppressed = 0;
              }
              current_phase = 0x15;
              if (((nonactive_player == human_player) && (nonactive_player == stop_phase_player)) &&
                  (stop_phase == 0x15))
              {
                DAT_007abc90[nonactive_player * 0x26 + 0x15] = (char)DAT_007abc90[nonactive_player * 0x26 + 0x15] | 2;
                DAT_007abc90[nonactive_player * 0x26 + 0x16] = (char)DAT_007abc90[nonactive_player * 0x26 + 0x16] | 2;
                DAT_007abc90[nonactive_player * 0x26 + 0x18] = (char)DAT_007abc90[nonactive_player * 0x26 + 0x18] | 2;
              }
              if ((((g_duel_network_flags & 2) != 0) && (active_player == human_player)) &&
                  ((active_player == previous_stop_phase_player && (previous_stop_phase == 0x15))))
              {
                DAT_007abc90[active_player * 0x26 + 0x15] = (char)DAT_007abc90[active_player * 0x26 + 0x15] | 2;
                DAT_007abc90[active_player * 0x26 + 0x16] = (char)DAT_007abc90[active_player * 0x26 + 0x16] | 2;
                DAT_007abc90[active_player * 0x26 + 0x18] = (char)DAT_007abc90[active_player * 0x26 + 0x18] | 2;
              }
              if ((FUN_0044aa01(player) != 0) &&
                  (unk_008b60e0 == 0) &&
                  (FUN_00441565(player) != 0))
                break;
              FUN_004432ed(player, current_phase);
              if (((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state != 1))
              {
                strcpy(unk_00748770, gs_prompt_combat_choose_attackers_0093d990);
              }
            }
            else
            {
              if ((g_duel_network_flags & 2) != 0)
              {
                DAT_00789714 = 1;
                _DAT_00743020 = 0xffffffff;
                spell_fizzled = 0;
                unk_008b3270 = spell_fizzled;
                saved_response_state = DAT_00777aa8;
                FUN_00444d1f();
                phase_response_window_open = 1;
                phase_stop_suppressed = 0;
                s.human_stop_enabled = 0;
                if (((nonactive_player == human_player) &&
                     (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0)) ||
                    ((active_player == human_player &&
                      (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0))))
                {
                  s.human_stop_enabled = 1;
                }
                if ((FUN_0044aa01(player) == 0) &&
                    (((s.human_stop_enabled != 0 ||
                       ((human_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                      ((human_player == stop_phase_player && (stop_phase == current_phase))))))
                {
                  DAT_00777aa8 = 1;
                }
                else
                {
                  DAT_00777aa8 = 0;
                }
                if (DAT_00777aa8 != 0)
                {
                  s.trace_no_attackers_counter = duel_trace_counter;
                  duel_trace_counter = duel_trace_counter + 1;
                  sprintf(s.trace_no_attackers_nonphasing_fast_effects, s__d__Main_Phase__No_attackers__No_0056e70c, s.trace_no_attackers_counter, current_phase);
                  append_to_trace_txt(s.trace_no_attackers_nonphasing_fast_effects);
                  if (FUN_004ad7e0(1 - human_player,
                                   gs_prompt_main_phase_precombat_cast_spells_008cc710) != 0)
                  {
                    DAT_00777aa8 = saved_response_state;
                    phase_response_window_open = 0;
                    phase_stop_suppressed = 0;
                    goto LAB_0044cf2b;
                  }
                }
                DAT_00777aa8 = saved_response_state;
                phase_response_window_open = 0;
                phase_stop_suppressed = 0;
              }
              break;
            }
          }
          else
          {
            if (current_phase < 0x1e)
            {
              strcpy(unk_00748770, DAT_0056e75c);
              if ((0 < unk_008b60e0) || (FUN_00441565(player) == 0))
              {
                *phase_value = 1;
              }
              break;
            }
            else
            {
              if ((g_duel_network_flags & 2) == 0)
                goto LAB_0044f62b;
              DAT_00789714 = 1;
              _DAT_00743020 = 0xffffffff;
              spell_fizzled = 0;
              unk_008b3270 = spell_fizzled;
              saved_response_state = DAT_00777aa8;
              FUN_00444d1f();
              phase_response_window_open = 1;
              phase_stop_suppressed = 0;
              s.human_stop_enabled = 0;
              if (((nonactive_player == human_player) &&
                   (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0)) ||
                  ((active_player == human_player &&
                    (((char)DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0))))
              {
                s.human_stop_enabled = 1;
              }
              if ((FUN_0044aa01(player) == 0) &&
                  (((s.human_stop_enabled != 0 ||
                     ((human_player == previous_stop_phase_player && (previous_stop_phase == current_phase)))) ||
                    ((human_player == stop_phase_player && (stop_phase == current_phase))))))
              {
                DAT_00777aa8 = 1;
              }
              else
              {
                DAT_00777aa8 = 0;
              }
              if ((g_duel_network_flags & 2) != 0)
              {
                s.trace_nonphasing_postcombat_counter = duel_trace_counter;
                duel_trace_counter++;
                sprintf(s.trace_nonphasing_postcombat_fast_effects, s__d__Main_Phase_Non_Phasing_playe_0056e760, s.trace_nonphasing_postcombat_counter, current_phase);
                append_to_trace_txt(s.trace_nonphasing_postcombat_fast_effects);
              }
              if (FUN_004ad7e0(1 - human_player,
                               gs_prompt_main_phase_postcombat_cast_spells_007ab2d0) != 0)
              {
                DAT_00777aa8 = saved_response_state;
                phase_response_window_open = 0;
                phase_stop_suppressed = 0;
                goto LAB_0044cf2b;
              }
              DAT_00777aa8 = saved_response_state;
              phase_response_window_open = 0;
              phase_stop_suppressed = 0;
              goto LAB_0044f62b;
            }
          }
        }
        else
        {
          FUN_00444d1f();
          unk_00712938 = 2;
          DAT_00637910 = FUN_004e3380(player);
          if (DAT_00637910 == 0xffffffff)
          {
            if (g_duel_ai_mode_state != 1)
            {
              allow_response(0, current_phase, gs_main_phase_008cee80, current_phase);
            }
            if (current_phase == 0x14)
            {
              current_phase = 0x15;
            }
            else
            {
              current_phase = 0x1e;
            }
            FUN_004432ed(player, current_phase);
            FUN_0044b3d4();
            if (check_duel_finished() != 0)
            {
              return 1;
            }
            *phase_value = 1;
          }
        }
        unk_00712938 = 3;
        _DAT_00637908 = 0;
        if (DAT_00637910 != 0xffffffff)
        {
          DAT_00637804 = global_card_instances[player][DAT_00637910].internal_card_id;
          if ((global_card_instances[player][DAT_00637910].state & 0x12) == 0)
          {
            if (((global_cards_data[DAT_00637804].type & 1) == 0) || ((land_can_be_played & 1U) == 0))
            {
              if (current_phase != 0x15)
              {
                x_value = 0;
                unk_008ce508 = -1;
                if (put_card_on_stack(player, DAT_00637910, 0) != 0)
                {
                  if (((player == nonactive_player) && ((g_duel_network_flags & 2) == 0)) &&
                      ((current_phase != 0x15 && ((global_cards_data[DAT_00637804].type & 1) == 0))))
                  {
                    FUN_00441cf2(4, 0x1e);
                  }
                LAB_0044e02d:
                  if (ai_decision_code == 4)
                  {
                    FUN_004e4e9a();
                    DAT_008cdab4 = 0;
                    DAT_008cdab0 = DAT_008cdab4;
                    DAT_008a8de4 = DAT_008cdab0;
                    ai_modifier = DAT_008a8de4;
                  }
                  if (put_card_on_stack(player, DAT_00637910, 1) != 0)
                  {
                    DAT_00637804 = global_card_instances[player][DAT_00637910].internal_card_id;
                    if (FUN_0043f9ed(player, DAT_00637910) != 0)
                    {
                      _DAT_00637908 = 6;
                      if ((global_cards_data[DAT_00637804].type & 1) != 0)
                      {
                        if (player == nonactive_player)
                        {
                          DAT_0091c500 = 0;
                        }
                        land_can_be_played |= 1;
                      }
                      if ((global_cards_data[DAT_00637804].type & 2) != 0)
                      {
                        global_card_instances[player][DAT_00637910].state |= 0x400;
                      }
                    }
                  }
                  if ((((player == active_player) && ((g_duel_network_flags & 2) == 0)) &&
                       (g_duel_ai_mode_state != 1)) &&
                      (DAT_0072c8e4 == 0))
                    goto LAB_0044ced8;
                  if ((g_duel_ai_mode_state == 1) && (ai_decision_code == 4))
                  {
                    *phase_value = 1;
                  }
                }
              }
            }
            else if (g_duel_ai_mode_state == 1)
            {
              *phase_value = 1;
            }
          }
          else if (((current_phase < 0x15) || (0x1d < current_phase)) ||
                   ((active_player == player && ((g_duel_network_flags & 2) == 0))))
          {
            max_x_value = -1;
            unk_00925bb8 = -1;
            if (((((global_cards_data[DAT_00637804].extra_ability & 3) != 0) &&
                  ((global_card_instances[player][DAT_00637910].state & 0x24) == 0)) ||
                 ((global_cards_data[DAT_00637804].extra_ability & 0x1000) != 0)) &&
                (dispatch_event_to_single_card(player, DAT_00637910, 0x73, s.other_player, -1) != 0))
            {
              if (((player == active_player) && ((g_duel_network_flags & 2) == 0)) &&
                  (g_duel_ai_mode_state != 1))
              {
                stop_phase_player = human_player;
                stop_phase = current_phase;
              }
              if (g_duel_ai_mode_state != 1)
              {
                spell_fizzled = -1;
              }
              s.activation_result = activate(player, player, DAT_00637910);
              max_x_value = -1;
              if (s.activation_result != 0)
              {
                if ((((g_duel_network_flags & 2) == 0) && (g_duel_ai_mode_state != 1)) &&
                    ((player == nonactive_player &&
                      ((current_phase != 0x15 &&
                        ((global_cards_data[global_card_instances[player][DAT_00637910].internal_card_id].extra_ability & 0x1000) == 0))))))
                {
                  FUN_00441cf2(7, 0xf);
                }
              LAB_0044e3a2:
                DAT_00637804 = global_card_instances[player][DAT_00637910].internal_card_id;
                if (ai_decision_code == 7)
                {
                  FUN_004e4e9a();
                  DAT_008cdab4 = 0;
                  DAT_008cdab0 = DAT_008cdab4;
                  DAT_008a8de4 = DAT_008cdab0;
                  ai_modifier = DAT_008a8de4;
                }
                if (spell_fizzled != 1)
                {
                  FUN_0044096f(player, DAT_00637910);
                }
                if ((g_duel_ai_mode_state == 1) && (ai_decision_code == 7))
                {
                  *phase_value = 1;
                }
              }
              if (spell_fizzled == 1)
              {
                if ((player == active_player) && ((g_duel_network_flags & 2) == 0))
                {
                  DAT_0093d850 = 1;
                  *phase_value = 1;
                }
                spell_fizzled = 0;
                goto LAB_0044e98b;
              }
              spell_fizzled = 0;
              if ((((player == active_player) && ((g_duel_network_flags & 2) == 0)) &&
                   (g_duel_ai_mode_state != 1)) &&
                  (DAT_0072c8e4 == 0))
                goto LAB_0044ced8;
              if (g_duel_ai_mode_state != 1)
              {
                FUN_004a61a1(player, DAT_00637910, 3, 2);
              }
            }
            x_value = 0;
          }
          else if ((((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) &&
                    (((global_cards_data[DAT_00637804].type & 2) != 0 ||
                      ((global_card_instances[player][DAT_00637910].state & 0x1000000) != 0)))) &&
                   (((global_card_instances[player][DAT_00637910].state & 0x10014) == 0 &&
                     (-1 < unk_008b60e0))) &&
                   (FUN_0044125c(player, DAT_00637910) != 0))
          {
            if ((g_duel_network_flags & 2) != 0)
            {
              s.trace_assign_attacker_counter = duel_trace_counter;
              duel_trace_counter++;
              sprintf(s.trace_assign_attacker, s__d__Main_Phase_assigning_attacke_0056e7a0, s.trace_assign_attacker_counter, current_phase,
                      player, DAT_00637910);
              append_to_trace_txt(s.trace_assign_attacker);
            }
            unk_0078944c = 0;
            if ((battlefield_extra_ability_flags & 0x400000) != 0)
            {
              push_affected_card_stack();
              strcpy(&DAT_00637808, unk_00748770);
              trigger_cause_controller = player;
              trigger_cause = DAT_00637910;
              dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
              strcpy(unk_00748770, &DAT_00637808);
              pop_affected_card_stack();
            }
            if (unk_0078944c == 0)
            {
              global_card_instances[player][DAT_00637910].blocking = 0xff;
              s.previous_attacker_count = unk_008b60e0;
              unk_008b60e0 = unk_008b60e0 + 1;
              if (s.previous_attacker_count == 0)
              {
                FUN_0044b3d4();
                if (check_duel_finished() != 0)
                {
                  return 1;
                }
                FUN_004432ed(player, current_phase);
              }
              if ((((C_get_abilities(player, DAT_00637910, 0x34, -1) & 0x200040) != 0) && (1 < unk_008b60e0)) && (unk_00715fb0 == 0))
              {
                strcpy(unk_00748770, gs_prompt_band_with_other_attacker_007a7af0);
                if (C_real_select_target(player, player, player, 0x200, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 2, 0,
                                         unk_00748770, 2, &s.selected_bandmate) != 0)
                {
                  DAT_0063790c = s.selected_bandmate.card;
                  if ((char)global_card_instances[player][s.selected_bandmate.card].blocking == -1)
                  {
                    ((unsigned char *)&DAT_0063790c)[0] = (unsigned char)s.selected_bandmate.card;
                    global_card_instances[player][s.selected_bandmate.card].blocking = (unsigned char)DAT_0063790c;
                    global_card_instances[player][DAT_00637910].blocking = (unsigned char)DAT_0063790c;
                  }
                  else
                  {
                    global_card_instances[player][DAT_00637910].blocking =
                        global_card_instances[player][s.selected_bandmate.card].blocking;
                  }
                }
              }
              global_card_instances[player][DAT_00637910].state |= 4;
              if ((battlefield_extra_ability_flags & 0x80000) != 0)
              {
                trigger_cause_controller = player;
                trigger_cause = DAT_00637910;
                dispatch_trigger_twice_once_with_each_player_as_reason(human_player, 0xde, gs_attacker_selected_008b32d0, 0);
              }
              _DAT_00637908 = 2;
            }
            else if (g_duel_ai_mode_state != 1)
            {
              FUN_004a61d6(gs_prompt_illegal_attacker_008b3d20);
              Sleep(2000);
              FUN_004a61d6(DAT_0056e7e4);
            }
          }
        }
      LAB_0044e98b:
        if (DAT_0091bfb0 != 0)
        {
          return 1;
        }
      }
    } while (*phase_value == 0);
    unk_00712938 = 4;
    DAT_00789714 = 1;
    FUN_004afa4b(player);
    if ((((player == active_player) && ((g_duel_network_flags & 2) == 0)) &&
         ((g_duel_ai_mode_state != 1 || (ai_decision_code != 1)))) &&
        (current_phase == 0x1e))
      goto LAB_0044f62b;
    if (((player == active_player) && ((g_duel_network_flags & 2) == 0)) && (current_phase < 0x1e))
    {
      if ((battlefield_extra_ability_flags & 0x1000000) != 0)
      {
        C_dispatch_event_raw(0x89);
      }
      if (FUN_00510a3f(player) != 0)
      {
        if (active_player == human_player)
        {
          stop_phase = -1;
          stop_phase_player = stop_phase;
        }
        trigger_cause = 0;
        trigger_cause_controller = player;
        for (; trigger_cause < active_cards_count[player];
             trigger_cause = trigger_cause + 1)
        {
          if ((global_card_instances[trigger_cause_controller][trigger_cause].internal_card_id != -1) &&
              ((global_card_instances[trigger_cause_controller][trigger_cause].state & 0x800006) == 6))
          {
            unk_0078944c = 0;
            if ((battlefield_extra_ability_flags & 0x400000) != 0)
            {
              push_affected_card_stack();
              strcpy(&DAT_00637808, unk_00748770);
              dispatch_trigger(player, 0xdc, gs_pay_for_attacker_007a7880, 1);
              strcpy(unk_00748770, &DAT_00637808);
              pop_affected_card_stack();
            }
            if (unk_0078944c != 0)
            {
              *(unsigned int *)&global_card_instances[trigger_cause_controller][trigger_cause].state &= 0xfffffffb;
              if ((char)global_card_instances[trigger_cause_controller][trigger_cause].blocking != -1)
              {
                s.bandmate_index = 0;
                s.bandmate_count = 0;
                for (; s.bandmate_index < active_cards_count[player]; s.bandmate_index = s.bandmate_index + 1)
                {
                  if ((s.bandmate_index != trigger_cause) &&
                      ((char)global_card_instances[player][s.bandmate_index].blocking ==
                       (char)global_card_instances[trigger_cause_controller][trigger_cause].blocking))
                  {
                    s.bandmates[s.bandmate_count] = s.bandmate_index;
                    s.bandmate_count = s.bandmate_count + 1;
                  }
                }
                if (s.bandmate_count == 1)
                {
                  global_card_instances[player][s.bandmates[0]].blocking = 0xff;
                }
                else
                {
                  for (s.bandmate_index = 0; s.bandmate_index < s.bandmate_count; s.bandmate_index = s.bandmate_index + 1)
                  {
                    global_card_instances[player][s.bandmates[s.bandmate_index]].blocking =
                        (unsigned char)s.bandmates[0];
                  }
                }
              }
            }
          }
        }
        FUN_0044b3d4();
        if (check_duel_finished() != 0)
        {
          return 1;
        }
        FUN_004432ed(player, current_phase);
        C_dispatch_event_raw(0x15);
        FUN_004432ed(player, current_phase);
        current_phase = 0x16;
        FUN_004432ed(player, current_phase);
        do
        {
          if (g_duel_ai_mode_state != 1)
          {
            TENTATIVE_reassess_all_cards(0, 0xff);
            FUN_00441cf2(8, 0x1e);
          }
        LAB_0044ee2f:
          if (ai_decision_code == 8)
          {
            FUN_004e4e9a();
            DAT_008cdab4 = 0;
            DAT_008cdab0 = DAT_008cdab4;
            DAT_008a8de4 = DAT_008cdab0;
            ai_modifier = DAT_008a8de4;
          }
          phase_response_window_open = 1;
          s.allow_response_result = allow_response(-2, current_phase, gs_assign_attackers_008b45f0, 0x16);
          phase_response_window_open = 0;
        } while (s.allow_response_result != 0);
        current_phase = 0x17;
        FUN_004432ed(player, current_phase);
        dispatch_trigger(1 - player, 0xda, gs_choose_defenders_00789a50, 0);
        FUN_00440d82(player);
        unk_008b60e0 = 1;
      }
    }
    if (((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) && (current_phase < 0x1e))
    {
      if ((FUN_0044aa01(player) != 0) &&
          (unk_008b60e0 == 0) &&
          (FUN_0044ae9e(0x15) == 0) &&
          ((g_duel_network_flags & 2) == 0))
        goto LAB_0044f4db;
      C_dispatch_event_raw(0x15);
      if ((g_duel_ai_mode_state == 1) && (current_phase < 0x15))
      {
        FUN_00510a3f(player);
      }
      if ((FUN_00441565(player) != 0) &&
          (((player == active_player) && ((g_duel_network_flags & 2) == 0)) ||
           (FUN_0044ae9e(0x15) == 0)))
        goto LAB_0044f4db;
      if (unk_008b60e0 == 0)
      {
        unk_008b60e0++;
        FUN_0044b3d4();
        if (check_duel_finished() != 0)
        {
          return 1;
        }
        FUN_004432ed(player, current_phase);
      }
      if ((g_duel_network_flags & 2) != 0)
      {
        s.trace_choose_attackers_counter = duel_trace_counter;
        duel_trace_counter = duel_trace_counter + 1;
        sprintf(s.trace_choose_attackers, s__d__Main_Phase_PX_Choose_Attacke_0056e7e8, s.trace_choose_attackers_counter, current_phase);
        append_to_trace_txt(s.trace_choose_attackers);
      }
      dispatch_trigger(player, 0xd9, gs_choose_attackers_00939570, 0);
      do
      {
        if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
        {
          TENTATIVE_reassess_all_cards(0, 0xff);
          FUN_00441cf2(6, 0x1e);
        }
      LAB_0044f0b1:
        if (ai_decision_code == 6)
        {
          FUN_004e4e9a();
          DAT_008cdab4 = 0;
          DAT_008cdab0 = DAT_008cdab4;
          DAT_008a8de4 = DAT_008cdab0;
          ai_modifier = DAT_008a8de4;
        }
        current_phase = 0x15;
        FUN_004432ed(player, current_phase);
        current_phase = 0x16;
        FUN_004432ed(player, current_phase);
        TENTATIVE_reassess_all_cards(0, 0xff);
        phase_response_window_open = 1;
        if ((g_duel_network_flags & 2) != 0)
        {
          iVar2 = duel_trace_counter;
          duel_trace_counter++;
          sprintf(s.trace_attackers_fast_effects, s__d__Main_Phase_Attackers_Fast_Ef_0056e81c, iVar2, current_phase);
          append_to_trace_txt(s.trace_attackers_fast_effects);
        }
        s.allow_response_result = allow_response(-2, current_phase, gs_assign_attackers_008b45f0, 0x16);
        phase_response_window_open = 0;
      } while (s.allow_response_result != 0);
      current_phase = 0x17;
      FUN_004432ed(player, current_phase);
      if ((g_duel_network_flags & 2) != 0)
      {
        iVar3 = duel_trace_counter;
        duel_trace_counter = duel_trace_counter + 1;
        sprintf(s.trace_choose_blockers, s__d__Main_Phase_PX_Choose_Blocker_0056e850, iVar3, current_phase);
        append_to_trace_txt(s.trace_choose_blockers);
      }
      dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xda, gs_choose_defenders_00789a50, 0);
      if ((g_duel_network_flags & 2) == 0)
      {
        FUN_0050e79b(player);
        FUN_005129d1(player);
      }
      else
      {
        FUN_00440d82(player);
      }
      FUN_004432ed(player, current_phase);
    }

  LAB_0044f284:
    C_dispatch_event_raw(0x1a);
      FUN_00445d1a(player);
      if (((((0 < unk_008b60e0) ||
             (((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) &&
              (FUN_0044ae9e(0x18) != 0))) &&
            ((land_can_be_played & 8U) == 0))) ||
          (((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state == 1)))
      {
        do
        {
          if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
          {
            TENTATIVE_reassess_all_cards(0, 0xff);
            FUN_00441cf2(2, 0x1e);
          }
        LAB_0044f33e:
          if (ai_decision_code == 2)
          {
            FUN_004e4e9a();
            DAT_008cdab4 = 0;
            DAT_008cdab0 = DAT_008cdab4;
            DAT_008a8de4 = DAT_008cdab0;
            ai_modifier = DAT_008a8de4;
          }
          current_phase = 0x18;
          FUN_004432ed(player, current_phase);
        if ((g_duel_network_flags & 2) != 0)
        {
          uVar4 = duel_trace_counter;
          duel_trace_counter = duel_trace_counter + 1;
          sprintf(s.trace_blockers_fast_effects, s__d__Main_Phase_Blockers_Fast_Eff_0056e880, uVar4, current_phase);
            append_to_trace_txt(s.trace_blockers_fast_effects);
          }
          phase_response_window_open = 1;
          s.allow_response_result = allow_response(-2, current_phase, gs_assign_blockers_008a8c40, 0x18);
          phase_response_window_open = 0;
        } while (s.allow_response_result != 0);
        if ((g_duel_network_flags & 2) != 0)
        {
          saved_response_state = duel_trace_counter;
          duel_trace_counter = duel_trace_counter + 1;
          sprintf(s.trace_combat, s__d__Main_Phase_Combat__ThePhase__0056e8b4, saved_response_state, current_phase);
          append_to_trace_txt(s.trace_combat);
        }
        FUN_005136ed(player);
        land_can_be_played |= 8;
        DAT_00789714 = 1;
        dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcc, gs_end_of_combat_008b43a0, 0);
        FUN_00515f34(player);
        FUN_00441d78();
        TENTATIVE_reassess_all_cards(0, 0xff);
        FUN_0044b3d4();
        if (check_duel_finished() != 0)
        {
          return 1;
        }
      }
    
  LAB_0044f4db:
    if (((player == nonactive_player) || ((g_duel_network_flags & 2) != 0)) && (g_duel_ai_mode_state != 1))
    {
      if (current_phase < 0x1e)
      {
        if (life[1] <= (int)*(int *)&DAT_008cfd70[4] / 2)
        {
          FUN_004b5fc9(s__Ouch__that_hurt___0056e8d8);
        }
        unk_008b60e0 = 0;
        current_phase = 0x1e;
        phase_stop_suppressed = 0;
        FUN_004432ed(player, current_phase);
        if (FUN_0044aa01(player) == 0)
          goto LAB_0044cf2b;
      }
      else if (life[0] <= life[1] / 2)
      {
        FUN_004b5fc9(s__Give_up__you_re_doomed___0056e8ec);
      }
    }
    if (((player == active_player) && ((g_duel_network_flags & 2) == 0)) && (current_phase != 0x1e))
    {
      current_phase = 0x1e;
      FUN_004432ed(player, current_phase);
      if (g_duel_ai_mode_state == 1)
      {
        if ((ai_decision_code == 1) || (ai_decision_code == 2))
        {
          goto LAB_0044cfcc;
        }
        goto LAB_0044f62b;
      }
      land_can_be_played |= 0x100;
      goto LAB_0044ced8;
    }
  LAB_0044f62b:
    unk_00712938 = 5;
    if (battlefield_extra_ability_flags & 2)
    {
      dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe3, gs_end_main_008b26c0, 1);
    }
    FUN_0044b3d4();
    if (check_duel_finished() != 0)
      return 1;
    return 0;
  } while (1);
}

