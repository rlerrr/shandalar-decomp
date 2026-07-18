#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#ifdef SHANDALAR
#include "deckdll/src/magsnd.h"
#include "shandalar/src/shandalar_global_strings.h"
#endif

extern int life[2];
unsigned int load_gametype0(char *path);
int DrawRandomCardFromInitialLibrary(int library_index);
int AddCardToDeckSorted(int card_id);
int GetCardRarity(int card_id);
void AddJournalEntry(int entry_type, int entry_arg);
void TENTATIVE_reassess_all_cards();
void append_to_trace_txt(char *text);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
void TENTATIVE_savegame(int autosave_slot);
void FUN_004e4e75(void);
void FUN_004432ed(int player, phase_t phase);
int FUN_0044ac96(int player, phase_t phase);
void FUN_00444d1f(void);
int FUN_0044b646(int *card_pairs, int card_pair_count, int player, int card);
int FUN_0044b6af(int player, int card);
int FUN_0044b7d4(int player, int card);
void FUN_0044af2d(int param_1);
void FUN_0044b3d4(void);
int FUN_004e6754(int param_1, int param_2);
int FUN_004a7ccf(void);
void FUN_00446036(void);
int allow_response(int param_1, int param_2, char *param_3, int param_4);
int dispatch_trigger(int player, trigger_t trig, const char *prompt, int TENTATIVE_allow_response);
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
void FUN_00441cf2(int param_1, int param_2);
void __stdcall FUN_004e4e9a(void);
int reset_empty_card_original_ids(void);
void compact_timestamp_slots(void);
int reset_stack_tracking_state(void);
void rebuild_battlefield_summary(void);
extern int DAT_008ced00[16];
extern int DAT_007aa928;
extern int DAT_007aaeec;
#ifdef SHANDALAR
int DAT_007a79b8;
#else
extern int DAT_007a79b8;
#endif

int init_turn(int player);
int TENTATIVE_start_turn(int player);
int untap_phase_exe(unsigned int player);
int draw_phase(unsigned int player);
int main_phase(unsigned int player, int phase_mode, int *phase_value);
int discard_phase(unsigned int player, int phase_mode);
void cleanup_phase(unsigned int player);
int restore_duel_turn_resume_state(void);
int upkeep_phase(unsigned int player);
void end_turn_phase(unsigned int player);
int ai_decision_phase(unsigned int player, int *next_state, int *phase_mode, int *phase_value);
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif
#ifdef SHANDALAR
extern int g_world_location_music_active;
extern int DAT_007483f0;
extern int _DAT_007483f4;
int single_color_test_bit_to_color_t(int color_mask);
#endif

typedef struct
{
  unsigned char pad_00[0x1f]; // +0x00
  char life;                  // +0x1f
  unsigned char pad_20;       // +0x20
  char kind;                  // +0x21
  unsigned char pad_22[0x0e]; // +0x22
  int flags;                  // +0x30
  unsigned char pad_31[0x58]; // +0x34
} ShandalarMonsterDefinition;
STATIC_ASSERT(sizeof(ShandalarMonsterDefinition) == 0x8c, shandalar_monster_definition_wrong_size);

// GLOBAL: MAGIC 0x005732b4
char s_Starting_the_duel_005732b4[0x14] = "Starting the duel.\n";

// GLOBAL: MAGIC 0x0056e4f0
char s_Turn___d__We_are_the_non_phasing_0056e4f0[0x2f] = "Turn #%d. We are the non-phasing player(%d).\n\n";

// GLOBAL: MAGIC 0x0056e520
char s_Turn___d__We_are_the_phasing_pla_0056e520[0x2b] = "Turn #%d. We are the phasing player(%d).\n\n";

// GLOBAL: MAGIC 0x0056e560
char s__d__Entering_Init_Turn__0056e560[0x19] = "%d: Entering Init Turn.\n";

// GLOBAL: MAGIC 0x0056e57c
char s__d__Entering_Start_Turn__0056e57c[0x1a] = "%d: Entering Start Turn.\n";

// GLOBAL: MAGIC 0x0056e598
char s__d__Entering_Untap_Phase__0056e598[0x1c] = "%d: Entering Untap Phase.\n";

// GLOBAL: MAGIC 0x0056e5b4
char s_PROMPT_UNTAP_0056e5b4[0x10] = "PROMPT_UNTAP";

// GLOBAL: MAGIC 0x0056e5c4
char s_PROMPT_UNTAP_0056e5c4[0x10] = "PROMPT_UNTAP";

// GLOBAL: MAGIC 0x0056e5d4
char s__d__Entering_UpKeep_Phase__0056e5d4[0x1c] = "%d: Entering UpKeep Phase.\n";

// GLOBAL: MAGIC 0x0056e5f0
char s__d__Entering_Draw_Phase__0056e5f0[0x1a] = "%d: Entering Draw Phase.\n";

// GLOBAL: MAGIC 0x0056e908
char s__d__Entering_Discard_Phase__0056e908[0x1d] = "%d: Entering Discard Phase.\n";

// GLOBAL: MAGIC 0x0056e928
char s__d__Entering_Cleanup_Phase__0056e928[0x1d] = "%d: Entering Cleanup Phase.\n";

// GLOBAL: MAGIC 0x0056e948
char s__d__Entering_End_Turn_Phase__0056e948[0x1f] = "%d: Entering End Turn Phase.\n";

// GLOBAL: MAGIC 0x0056e968
char s__d__Entering_AI_Decision_Phase__0056e968[0x24] = "%d: Entering AI Decision Phase.\n";

// GLOBAL: MAGIC 0x0056e98c
char s_phase___3d_num_tries___4d_mtime___0056e98c[0x32] = "phase = %3d num tries = %4d mtime = %4d, %d\n";

// GLOBAL: MAGIC 0x00746880
ShandalarMonsterDefinition g_shandalar_monster_definitions[56];

// GLOBAL: MAGIC 0x009251d4
int DAT_009251d4;

// GLOBAL: MAGIC 0x00896690
char DAT_00896690;

// GLOBAL: MAGIC 0x00712544
int DAT_00712544;

// GLOBAL: MAGIC 0x00743098
int DAT_00743098;

// GLOBAL: MAGIC 0x0091c0ec
int _DAT_0091c0ec;

// FUNCTION: MAGIC 0x004d2a09
void copy_autosave_to_save_file(LPCSTR save_path)
{
  (void)save_path;
}

// FUNCTION: MAGIC 0x00464b6f
// FUNCTION: SHANDALAR 0x00562893
void StopWorldLocationMusic(void)
{
#ifdef SHANDALAR
  if (g_world_location_music_active != 0)
  {
    sound_stop(0x10);
  }
  g_world_location_music_active = 0;
#endif
}

// FUNCTION: MAGIC 0x0043de00
void reset_duel_globals(void)
{
  int card;
  int player;

  nonactive_player = 0;
  active_player = 1;
  for (card = g_card_count; card < g_card_count + 16; ++card)
  {
    global_cards_data[card].id = -1;
  }

  for (player = 0; player < 2; ++player)
  {
    (&DAT_007abce0)[player] = 0;
    for (card = 0; card <= 7; ++card)
    {
      raw_mana_available[player][card] = 0;
      basiclandtypes_controlled[player][card] = raw_mana_available[player][card];
      unk_00742f70[player][card] = basiclandtypes_controlled[player][card];
      raw_mana_available_hex[player][0] = -1;
      unk_007161e0[player][0] = -1;
      unk_00743080[player] = 0;
    }
  }

  global_stack_cards[0].player = -1;
  for (card = 0; card < 16; ++card)
  {
    if ((char)global_cards_data[g_card_count + card].secret == -1)
    {
      global_cards_data[g_card_count + card].id = -1;
    }
  }

  land_can_be_played = 0;
  max_x_value = -1;
  DAT_0077784c = max_x_value;
  stop_phase = DAT_0077784c;
  previous_stop_phase_player = -1;
  stop_phase_player = previous_stop_phase_player;
  previous_stop_phase = stop_phase_player;
  unk_00743038 = 0;
  DAT_00791418 = 0;
  DAT_007aa928 = -1;
  DAT_007abc74 = DAT_007aa928;
  DAT_007a7d78 = 0;
  DAT_007abc80 = 0;
  DAT_007aaeec = 0;
  DAT_007aadec = -1;
  if (g_shandalar_difficulty == 0)
  {
    DAT_00925ac4 |= 2;
  }
  else
  {
    DAT_00925ac4 &= -3;
  }

  ai_search_flags = 0;
  DAT_00742f68 = 0x30;
  _DAT_00743020 = -1;
  DAT_0091b2c4 = -1;
  DAT_008a8d6c = -1;
  DAT_007aadf0 = -1;
  produced_mana_color = -1;
  g_duel_network_state = 0;
  DAT_008b33fc = 1;
  _DAT_0091c0ec = 0;
  phase_stop_suppressed = 0;
  phase_was_skipped = 0;
  unk_008b60e0 = 0;
  g_duel_extra_turn_player = -1;
  DAT_0093a848 = 0;
  DAT_007161d0 = 1;
  trigger_dispatch_depth = 0;
  max_trigger_dispatch_depth = 0;
  unk_0091a6d0 = 0;
  for (card = 0; card <= 7; ++card)
  {
    unk_008ce510[card] = 0;
  }

  max_x_value = -1;
  unk_008ce508 = -1;
  unk_008ce4f4 = -1;
  DAT_009251d4 = 0;
  unk_007a79b0[active_player] = 0;
  unk_007a79b0[nonactive_player] = unk_007a79b0[active_player];
  regenerate_or_graveyard_triggers_in_progress = 0;
}

// FUNCTION: MAGIC 0x004458d4
void reset_timestamp_players(void)
{
}

// FUNCTION: MAGIC 0x004ecdc0
void load_selected_duel_save_slot(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x004ef17b
void load_duel_run_mode_1_save(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x004ef35a
void load_duel_run_mode_2_save(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x004ef54b
void load_duel_run_mode_3_save(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x0044b56a
int check_duel_finished(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x0044a7e0
// FUNCTION: SHANDALAR 0x00403160
void run_duel_turn(unsigned int player)
{
  struct
  {
    char trace[100];
    int next_state;
    int phase_value;
    int phase_mode;
  } s;

  s.phase_mode = 0;
  s.next_state = 0;
  if ((g_duel_network_flags & 2) != 0)
  {
    append_to_trace_txt(unk_0056e49c);
    if (player != 0)
    {
      sprintf(s.trace, s_Turn___d__We_are_the_non_phasing_0056e4f0, DAT_009251d4, player);
    }
    else
    {
      sprintf(s.trace, s_Turn___d__We_are_the_phasing_pla_0056e520, DAT_009251d4, player);
    }
    append_to_trace_txt(s.trace);
  }

  while (s.next_state != -1)
  {
    switch (s.next_state)
    {
    case 0:
      init_turn(player);
      s.next_state = restore_duel_turn_resume_state();
      if (s.next_state != 10)
      {
        break;
      }
    case 2:
      if (TENTATIVE_start_turn(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 3:
      if (untap_phase_exe(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 4:
      if (upkeep_phase(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 5:
      if (draw_phase(player) != 0)
      {
        s.next_state = -1;
        break;
      }
    case 1:
    case 6:
      if (main_phase(player, s.phase_mode, &s.phase_value) != 0)
      {
        s.next_state = -1;
        break;
      }
      s.phase_mode = 0;
    case 7:
      if (discard_phase(player, s.phase_mode) != 0)
      {
        s.next_state = -1;
        break;
      }
      s.phase_mode = 0;
    case 8:
      cleanup_phase(player);
      end_turn_phase(player);
      if (ai_decision_phase(player, &s.next_state, &s.phase_mode, &s.phase_value) != 0)
      {
        s.next_state = -1;
        break;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044bcf7
int restore_duel_turn_resume_state(void)
{
  int next_state = 10;
  if ((g_duel_ai_mode_state < 0) || (g_duel_use_previous_backdrop_colors != 0))
  {
    next_state = g_duel_ai_mode_state;
    g_duel_ai_mode_state = 0;
    g_duel_use_previous_backdrop_colors = 0;
    if ((next_state == -1) || (next_state == -2))
    {
      next_state = 1;
    }
    else if (next_state == -10)
    {
      TENTATIVE_reassess_all_cards(0, 0xff);
      stop_phase_player = human_player;
      stop_phase = current_phase;
      if (current_phase == PHASE_START)
      {
        stop_phase = -1;
      }
      if (current_phase == PHASE_CLEANUP)
      {
        stop_phase = PHASE_CLEANUP2;
      }
      if (current_phase == PHASE_START)
      {
        next_state = 2;
      }
      else if (current_phase == PHASE_UNTAP)
      {
        next_state = 3;
      }
      else if (current_phase == PHASE_UPKEEP)
      {
        next_state = 4;
      }
      else if (current_phase == PHASE_DRAW)
      {
        next_state = 5;
      }
      else if (current_phase == PHASE_MAIN1)
      {
        next_state = 6;
      }
      else if (current_phase == PHASE_DISCARD)
      {
        next_state = 7;
      }
      else if (current_phase == PHASE_CLEANUP)
      {
        next_state = 8;
      }
    }
  }
  return next_state;
}

// FUNCTION: MAGIC 0x0044b89b
int init_turn(int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int other_player;
    int card;
    int loop_player;
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Init_Turn__0056e560, s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  s.other_player = 1 - player;
  human_player = player;
  ai_decision_code = -1;
  if (nonactive_player == player)
  {
    DAT_0077784c++;
  }
  DAT_009251d4++;
  for (s.card = 0; s.card < 0x97; s.card = s.card + 1)
  {
    unk_0093b280[s.other_player][s.card][1][0] = 0;
    unk_0093b280[s.other_player][s.card][0][0] = unk_0093b280[s.other_player][s.card][1][0];
    unk_0093b280[player][s.card][1][0] = unk_0093b280[s.other_player][s.card][0][0];
    unk_0093b280[player][s.card][0][0] = unk_0093b280[player][s.card][1][0];
    unk_0093b280[s.other_player][s.card][1][3] = -1;
    unk_0093b280[s.other_player][s.card][0][3] = unk_0093b280[s.other_player][s.card][1][3];
    unk_0093b280[s.other_player][s.card][1][2] = (char)unk_0093b280[s.other_player][s.card][0][3];
    unk_0093b280[s.other_player][s.card][0][2] = (char)unk_0093b280[s.other_player][s.card][1][2];
    unk_0093b280[player][s.card][1][3] = (char)unk_0093b280[s.other_player][s.card][0][2];
    unk_0093b280[player][s.card][0][3] = unk_0093b280[player][s.card][1][3];
    unk_0093b280[player][s.card][1][2] = (char)unk_0093b280[player][s.card][0][3];
    unk_0093b280[player][s.card][0][2] = (char)unk_0093b280[player][s.card][1][2];
  }
  reset_empty_card_original_ids();
  compact_timestamp_slots();
  for (s.card = 0; s.card < 0x26; s.card = s.card + 1)
  {
    g_duel_phase_stop_settings[0].phase_flags[s.card] &= (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT);
    g_duel_phase_stop_settings[1].phase_flags[s.card] &= (PHASE_STOP_ENABLED | PHASE_STOP_OPPONENT);
  }
  StopWorldLocationMusic();
  FUN_00441d78();
  reset_stack_tracking_state();
  rebuild_battlefield_summary();
  for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
  {
    for (s.card = 0; s.card < 0x96; s.card = s.card + 1)
    {
      if (global_card_instances[s.loop_player][s.card].internal_card_id != -1)
      {
        active_cards_count[s.loop_player] = s.card + 1;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044be6f
int TENTATIVE_start_turn(int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int card;
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Start_Turn__0056e57c, s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
  {
    TENTATIVE_savegame(0);
  }
  if ((land_can_be_played & 0x8000) != 0)
  {
    land_can_be_played &= -32769;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  FUN_004e4e75();
  current_phase = PHASE_START;
  FUN_004432ed(player, current_phase);
  C_dispatch_event_raw(0x6a);
  if ((land_can_be_played & 0x8000) != 0)
  {
    land_can_be_played &= -32769;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  DAT_00896690 = 0;
  unk_008b60e0 = 0;
  land_can_be_played &= -512;
  for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
  {
    global_card_instances[player][s.card].state &= -230413;
  }
  TENTATIVE_reassess_all_cards(0, 0xff);
  return 0;
}

// FUNCTION: MAGIC 0x004b51b9
int reset_empty_card_original_ids(void)
{
  int player;
  int card;

  for (player = 0; player < 2; player = player + 1)
  {
    for (card = 0; card < 0x96; card = card + 1)
    {
      if (global_card_instances[player][card].internal_card_id == -1)
      {
        global_card_instances[player][card].original_internal_card_id = -1;
      }
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x004459b5
void compact_timestamp_slots(void)
{
  int next_slot;
  int slot;

  for (slot = 0; slot < 500; slot = slot + 1)
  {
    while ((TENTATIVE_timestamp_player[slot] != -1) &&
           ((global_card_instances[TENTATIVE_timestamp_player[slot]][TENTATIVE_timestamp_card[slot]].internal_card_id == -1) ||
            ((int)global_card_instances[TENTATIVE_timestamp_player[slot]][TENTATIVE_timestamp_card[slot]].timestamp != slot)))
    {
      next_slot = slot;
      while (next_slot + 1 < 500)
      {
        next_slot++;
        TENTATIVE_timestamp_player[next_slot - 1] = TENTATIVE_timestamp_player[next_slot];
        TENTATIVE_timestamp_card[next_slot - 1] = TENTATIVE_timestamp_card[next_slot];
        if ((int)global_card_instances[TENTATIVE_timestamp_player[next_slot]][TENTATIVE_timestamp_card[next_slot]].timestamp == next_slot)
        {
          global_card_instances[TENTATIVE_timestamp_player[next_slot]][TENTATIVE_timestamp_card[next_slot]].timestamp--;
        }
      }
      TENTATIVE_timestamp_player[499] = -1;
    }
  }
}

// FUNCTION: MAGIC 0x0044386c
int reset_stack_tracking_state(void)
{
  unk_008b2934 = 0;
  global_stack_cards[unk_008b2934].player = -1;
  return 0;
}

// FUNCTION: MAGIC 0x00442f0c
void rebuild_battlefield_summary(void)
{
  int card;
  int color;
  int internal_card_id;
  int player;
  int toughness;
  int power;

  for (color = 0; color < 8; color = color + 1)
  {
    unk_00939520[1][color] = 0;
    unk_00939520[0][color] = unk_00939520[1][color];
    ((int *)DAT_008cf690)[color + 8] = unk_00939520[0][color];
    ((int *)DAT_008cf690)[color] = ((int *)DAT_008cf690)[color + 8];
    unk_008cf1c0[1][color] = ((int *)DAT_008cf690)[color];
    unk_008cf1c0[0][color] = unk_008cf1c0[1][color];
  }
  card_types_in_play[1] = 0;
  card_types_in_play[0] = 0;
  unk_007a7c58[1] = 0;
  unk_007a7c58[0] = 0;
  DAT_008b42e8[1] = 0;
  DAT_008b42e8[0] = 0;
  for (color = 0; color < 0x18; color = color + 1)
  {
    ((int *)DAT_008cfd70)[color] = 0;
  }
  ((int *)DAT_008cfd70)[0] = life[0];
  ((int *)DAT_008cfd70)[1] = life[1];
  for (player = 0; player < 2; player = player + 1)
  {
    hand_count[player] = 0;
    for (card = 0; card < active_cards_count[player]; card = card + 1)
    {
      if (is_in_play(player, card) == 0)
      {
        if (((global_card_instances[player][card].state & 0x800000) == 0) &&
            (global_card_instances[player][card].internal_card_id != -1))
        {
          hand_count[player]++;
        }
      }
      else
      {
        internal_card_id = global_card_instances[player][card].internal_card_id;
        if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
        {
          power = C_get_abilities(player, card, EVENT_POWER, -1);
          toughness = C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
          color = single_color_test_bit_to_color_t(global_cards_data[internal_card_id].color);
          unk_008cf1c0[player][color] += power;
          unk_008cf1c0[player][7] += power;
          color = single_color_test_bit_to_color_t(global_cards_data[internal_card_id].color);
          ((int *)DAT_008cf690)[player * 8 + color] += toughness;
          ((int *)DAT_008cf690)[player * 8 + 7] += toughness;
          DAT_008b42e8[player]++;
        }
        card_types_in_play[player] |= global_cards_data[internal_card_id].type;
        if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
        {
          ((int *)DAT_008cfd70)[player + 4]++;
        }
        if ((global_cards_data[internal_card_id].type & TYPE_ARTIFACT) != 0)
        {
          ((int *)DAT_008cfd70)[player + 6]++;
        }
        if ((global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0)
        {
          ((int *)DAT_008cfd70)[player + 8]++;
        }
      }
    }
    for (card = 0; card < 500; card = card + 1)
    {
      if (global_graveyard_slots[player][card] != -1)
      {
        unk_007a7c58[player] |= global_cards_data[global_graveyard_slots[player][card]].type;
      }
    }
  }
}

// FUNCTION: MAGIC 0x004d2843
void TENTATIVE_savegame(int autosave_slot)
{
  (void)autosave_slot;
}

// FUNCTION: MAGIC 0x004e4e75
void FUN_004e4e75(void)
{
  recorded_action_count = 0;
  recorded_action_codes[recorded_action_count] = 99;
}

// FUNCTION: MAGIC 0x004432ed
void FUN_004432ed(int player, phase_t phase)
{
  (void)player;
  (void)phase;
}

// FUNCTION: MAGIC 0x00446036
void FUN_00446036(void)
{
  int card;
  int player;

  for (player = 0; player < 2; player = player + 1)
  {
    for (card = 0; card < active_cards_count[player]; card = card + 1)
    {
      if (is_in_play(player, card) != 0)
      {
        global_card_instances[player][card].upkeep_flags = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044bfe9
int untap_phase_exe(unsigned int player)
{
  int trace_counter;
  struct
  {
    char trace[100];
    int selected_card;
    int winter_orb_card_type;
    int optional_untap_count;
    int event_result_copy;
    int must_untap_cards[100];
    int optional_untap_cards[100];
    int card;
    int must_untap_count;
    int did_skip_untap;
    int loop_player;
    int done;
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Untap_Phase__0056e598, trace_counter);
    append_to_trace_txt(s.trace);
  }

  s.winter_orb_card_type = CardTypeFromID(0xe9);
  if ((s.winter_orb_card_type == -1) ||
      (real_target_available(NULL, 0, player, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, s.winter_orb_card_type, -1, -1, -1, 0, 0, 0) == 0))
  {
    current_phase = PHASE_UNTAP;
    FUN_004432ed(player, current_phase);
    phase_was_skipped = 0;
    phase_stop_suppressed = 0;
    phase_response_window_open = 1;
    unk_00925d38 = 0;

    for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
    {
      if ((is_in_play(player, s.card) != 0) &&
          ((global_card_instances[player][s.card].state & 0x10) != 0))
      {
        global_card_instances[player][s.card].untap_status = 3;
        dispatch_event(player, s.card, 0x82);
      }
    }

    s.done = 0;
    while (s.done == 0)
    {
      s.optional_untap_count = 0;
      s.must_untap_count = 0;
      for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
      {
        for (s.card = 0; s.card < active_cards_count[s.loop_player]; s.card = s.card + 1)
        {
          if (is_in_play(s.loop_player, s.card) != 0)
          {
            affected_card_controller = s.loop_player;
            affected_card = s.card;
            event_result = 0;
            C_dispatch_event_raw(0x7d);
            s.event_result_copy = event_result;
            if (s.event_result_copy == 1)
            {
              s.optional_untap_cards[s.optional_untap_count * 2] = s.loop_player;
              s.optional_untap_cards[s.optional_untap_count * 2 + 1] = s.card;
              s.optional_untap_count++;
            }
            else if (s.event_result_copy == 2)
            {
              s.must_untap_cards[s.must_untap_count * 2] = s.loop_player;
              s.must_untap_cards[s.must_untap_count * 2 + 1] = s.card;
              s.must_untap_count++;
            }
          }
        }
      }

      if (((active_player == player) && ((g_duel_network_flags & 2) == 0)) ||
          (g_duel_ai_mode_state == 1) || (g_duel_network_state != 0))
      {
        s.done = 1;
        if ((active_player == player) &&
            ((g_duel_network_flags & 2) == 0) &&
            (g_duel_ai_mode_state != 1) &&
            (FUN_0044ac96(player, PHASE_UNTAP) != 0))
        {
          s.done = 0;
        }
      }
      else if (((s.must_untap_count == 0) || (s.must_untap_count == 1)) &&
               (s.optional_untap_count == 0) &&
               (FUN_0044ac96(player, PHASE_UNTAP) == 0))
      {
        s.done = 1;
      }

      if (s.done == 0)
      {
        FUN_00444d1f();
        if (((nonactive_player == player) || ((g_duel_network_flags & 2) != 0)) &&
            (g_duel_ai_mode_state != 1) &&
            (g_duel_network_state == 0) &&
            ((s.must_untap_count > 0) || (s.optional_untap_count > 0)))
        {
          load_text(global_ui_strings_filename, s_PROMPT_UNTAP_0056e5b4);
          s.selected_card = FUN_004e9c50(player, -1, player, 0xff, 0, text_lines[0], 2);
        }
        else
        {
          if (nonactive_player == player)
          {
            stop_phase_player = -1;
            stop_phase = -1;
          }
          else
          {
            previous_stop_phase_player = -1;
            previous_stop_phase = -1;
          }
          load_text(global_ui_strings_filename, s_PROMPT_UNTAP_0056e5c4);
          s.selected_card = FUN_004e9c50(player, player, player, 0xff, 0, text_lines[1], 2);
          phase_was_skipped = 1;
        }

        if (_DAT_0074303c != -3)
        {
          if (_DAT_0074303c == -2)
          {
            s.done = 1;
          }
          else if ((_DAT_0074303c == 0) && (s.selected_card != -1))
          {
            if ((FUN_0044b646(s.must_untap_cards, s.must_untap_count, unk_00742fcc, s.selected_card) == 0) &&
                (FUN_0044b646(s.optional_untap_cards, s.optional_untap_count, unk_00742fcc, s.selected_card) == 0))
            {
              if ((FUN_0044b6af(unk_00742fcc, s.selected_card) != 0) &&
                  (unk_00742fcc == player))
              {
                FUN_0044b7d4(unk_00742fcc, s.selected_card);
              }
            }
            else
            {
              affected_card_controller = unk_00742fcc;
              affected_card = s.selected_card;
              C_dispatch_event_raw(0x7e);
            }
          }
        }
      }
    }

    if (s.must_untap_count != 0)
    {
      s.done = 0;
      while (s.done == 0)
      {
        s.must_untap_count = 0;
        s.loop_player = 0;
        while ((s.loop_player < 2) && (s.must_untap_count == 0))
        {
          s.card = 0;
          while ((s.card < active_cards_count[s.loop_player]) && (s.must_untap_count == 0))
          {
            if (is_in_play(s.loop_player, s.card) != 0)
            {
              affected_card_controller = s.loop_player;
              affected_card = s.card;
              event_result = 0;
              C_dispatch_event_raw(0x7d);
              s.event_result_copy = event_result;
              if (s.event_result_copy == 2)
              {
                s.must_untap_cards[s.must_untap_count * 2] = s.loop_player;
                s.must_untap_cards[s.must_untap_count * 2 + 1] = s.card;
                s.must_untap_count++;
              }
            }
            s.card++;
          }
          s.loop_player++;
        }

        if (s.must_untap_count == 0)
        {
          s.done = 1;
        }
        else
        {
          affected_card_controller = s.must_untap_cards[0];
          affected_card = s.must_untap_cards[1];
          C_dispatch_event_raw(0x7e);
        }
      }
    }

    for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
    {
      if (((global_card_instances[player][s.card].untap_status & 1) != 0) &&
          ((global_card_instances[player][s.card].untap_status & 2) != 0))
      {
        global_card_instances[player][s.card].state &= -17;
        dispatch_event_to_single_card(player, s.card, 0x83, 1 - player, -1);
      }
    }

    for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
    {
      if (is_in_play(player, s.card) != 0)
      {
        global_card_instances[player][s.card].untap_status = 0;
      }
    }

    FUN_00441d78();
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.did_skip_untap = phase_was_skipped;
    if (s.did_skip_untap == 0)
    {
      FUN_0044af2d(1);
    }
    FUN_0044b3d4();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044c88f
int upkeep_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_UpKeep_Phase__0056e5d4, s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  unk_00939330 = 0;
  phase_was_skipped = 0;
  phase_response_window_open = 0;
  if ((g_duel_network_flags & 2) != 0)
  {
    if (stop_phase == -1)
    {
      phase_stop_suppressed = 0;
    }
    else if (((stop_phase == PHASE_UPKEEP) && (player == stop_phase_player)) ||
             ((previous_stop_phase == PHASE_UPKEEP) && (player == previous_stop_phase_player)))
    {
      phase_stop_suppressed = 0;
    }
    else
    {
      phase_stop_suppressed = 1;
    }
  }
  else
  {
    if (g_duel_ai_mode_state != 1)
    {
      if ((stop_phase == -1) ||
          ((stop_phase == PHASE_UPKEEP) && (player == stop_phase_player)))
      {
        phase_stop_suppressed = 0;
      }
      else
      {
        phase_stop_suppressed = 1;
      }
    }
  }
  current_phase = PHASE_BEGIN_UPKEEP;
  FUN_004432ed(player, current_phase);
  dispatch_trigger(player, 0xc9, gs_begin_upkeep_008cf080, 0);
  phase_response_window_open = 1;
  current_phase = PHASE_UPKEEP;
  FUN_00446036();
  allow_response(-1, current_phase, gs_upkeep_phase_00777d30, PHASE_UPKEEP);
  phase_response_window_open = 0;
  dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcb, gs_end_upkeep_00925c00, 0);
  phase_stop_suppressed = 0;
  DAT_00789714 = 1;
  FUN_004afa4b(player);
  DAT_00789714 = 0;
  unk_00939330 = 0;
  FUN_0044b3d4();
  if (check_duel_finished() != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044caa3
int draw_phase(unsigned int player)
{
  int trace_counter;
  struct
  {
    char trace[100];
    int card;
    int draw_index;
    int draw_count;
  } s;

  if ((g_duel_network_flags & 2) != 0)
  {
    trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Draw_Phase__0056e5f0, trace_counter);
    append_to_trace_txt(s.trace);
  }

  if (first_turn_draw_pending != 0)
  {
    first_turn_draw_pending = 0;
    if (starting_player_was_random != 0)
    {
      return 0;
    }
  }

  {
    current_phase = PHASE_DRAW;
    FUN_004432ed(player, current_phase);
    phase_was_skipped = 0;
    phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (stop_phase == -1)
      {
        phase_stop_suppressed = 0;
      }
      else if (((stop_phase == PHASE_DRAW) && (player == stop_phase_player)) ||
               ((previous_stop_phase == PHASE_DRAW) && (player == previous_stop_phase_player)))
      {
        phase_stop_suppressed = 0;
      }
      else
      {
        phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((stop_phase == -1) ||
            ((stop_phase == PHASE_DRAW) && (player == stop_phase_player)))
        {
          phase_stop_suppressed = 0;
        }
        else
        {
          phase_stop_suppressed = 1;
        }
      }
    }
    dispatch_trigger(player, 0xce, gs_draw_phase_008cf560, 1);
    event_result = 1;
    C_dispatch_event_raw(10);
    s.draw_count = event_result;
    if (s.draw_count > 0)
    {
      if ((player == active_player) && ((g_duel_network_flags & 2) == 0))
      {
        for (s.draw_index = 0; s.draw_count > s.draw_index; s.draw_index = s.draw_index + 1)
        {
          draw_card_for_player(player);
        }
      }
      else
      {
        for (s.draw_index = 0; s.draw_count > s.draw_index; s.draw_index = s.draw_index + 1)
        {
          s.card = add_card_to_hand(player, unk_008b28f8);
          if (s.card != -1)
          {
            global_card_instances[player][s.card].state |= 2;
          }
        }
        TENTATIVE_reassess_all_cards(0, 0xff);
      }
    }

    phase_response_window_open = 1;
    phase_was_skipped = 0;
    allow_response(-1, current_phase, gs_draw_phase_0091c100, current_phase);
    phase_response_window_open = 0;
    if ((battlefield_extra_ability_flags & 2) != 0)
    {
      dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe2, gs_end_draw_0091c510, 1);
    }
    phase_stop_suppressed = 0;
    FUN_0044b3d4();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044f682
int discard_phase(unsigned int player, int phase_mode)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int hand_count_for_discard;
    int card;
    int did_discard;
    int allow_response_result;
  } s;

  s.trace_counter = duel_trace_counter;
  s.did_discard = 0;
  if ((g_duel_network_flags & 2) != 0)
  {
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Discard_Phase__0056e908, s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
  {
    if ((global_card_instances[player][s.card].internal_card_id != -1) &&
        ((global_card_instances[player][s.card].state & 2) != 0) &&
        (global_cards_data[global_card_instances[player][s.card].internal_card_id].id == 0x8c))
    {
      return 0;
    }
  }

  if (phase_mode == 0)
  {
    phase_was_skipped = 0;
    phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (stop_phase == -1)
      {
        phase_stop_suppressed = 0;
      }
      else if (((stop_phase == PHASE_DISCARD) && (player == stop_phase_player)) ||
               ((previous_stop_phase == PHASE_DISCARD) && (player == previous_stop_phase_player)))
      {
        phase_stop_suppressed = 0;
      }
      else
      {
        phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((stop_phase == -1) ||
            ((stop_phase == PHASE_DISCARD) && (player == stop_phase_player)))
        {
          phase_stop_suppressed = 0;
        }
        else
        {
          phase_stop_suppressed = 1;
        }
      }
    }
  }

  do
  {
    if ((phase_mode == 0) &&
        (g_duel_ai_mode_state != 1) &&
        (nonactive_player == player) &&
        ((g_duel_network_flags & 2) == 0))
    {
      TENTATIVE_reassess_all_cards(0, 0xff);
      FUN_00441cf2(3, 0x1e);
    }
    if (ai_decision_code == 3)
    {
      FUN_004e4e9a();
    DAT_008cdab4 = 0;
    DAT_008cdab0 = DAT_008cdab4;
    DAT_008a8de4 = DAT_008cdab0;
    ai_modifier = DAT_008a8de4;
    }
    current_phase = PHASE_DISCARD;
    FUN_004432ed(player, current_phase);
    phase_response_window_open = 1;
    s.allow_response_result = allow_response(-1, current_phase, gs_discard_phase_0091d080, PHASE_DISCARD);
    phase_response_window_open = 0;
  } while (s.allow_response_result != 0);

  if ((g_duel_ai_mode_state != 1) ||
      ((active_player == player) && ((g_duel_network_flags & 2) == 0)))
  {
    if (g_duel_ai_mode_state != 1)
    {
      phase_stop_suppressed = 0;
    }
    if ((unk_007a79b0[player] & 2) == 0)
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        if (nonactive_player == player)
        {
          s.hand_count_for_discard = 0;
        }
        else
        {
          s.hand_count_for_discard = unk_007161d8;
        }
      }
      else
      {
        s.hand_count_for_discard = 0;
      }
      for (s.card = 0; s.card < active_cards_count[player]; s.card = s.card + 1)
      {
        if ((global_card_instances[player][s.card].internal_card_id != -1) &&
            ((global_card_instances[player][s.card].state & 2) == 0))
        {
          s.hand_count_for_discard++;
        }
      }
      event_result = 0;
      C_dispatch_event_raw(0x1f);
      s.did_discard = 0;
      while ((event_result == 0) && (s.hand_count_for_discard > 7))
      {
        discard(player, 0, 1);
        s.hand_count_for_discard--;
        if ((player == active_player) &&
            ((g_duel_network_flags & 2) == 0) &&
            (g_duel_ai_mode_state == 1))
        {
          ai_modifier -= 0x18;
        }
        if (((nonactive_player == player) || ((g_duel_network_flags & 2) != 0)) &&
            (g_duel_ai_mode_state == 0) &&
            (g_duel_network_state == 0))
        {
          s.did_discard = 1;
        }
      }
    }
  }

  if ((battlefield_extra_ability_flags & 2) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xe4, gs_end_discard_007ab020, 1);
  }
  FUN_0044b3d4();
  if (check_duel_finished() != 0)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044fb96
void cleanup_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[100];
    int other_player;
    int card;
  } s;

  s.other_player = 1 - player;
  if ((g_duel_network_flags & 2) != 0)
  {
    s.trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_Cleanup_Phase__0056e928, s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  if (g_duel_ai_mode_state != 1)
  {
    current_phase = PHASE_CLEANUP;
    FUN_004432ed(player, current_phase);
    phase_was_skipped = 0;
    phase_response_window_open = 0;
    if ((g_duel_network_flags & 2) != 0)
    {
      if (stop_phase == -1)
      {
        phase_stop_suppressed = 0;
      }
      else if (((stop_phase == PHASE_CLEANUP2) && (player == stop_phase_player)) ||
               ((previous_stop_phase == PHASE_CLEANUP2) && (player == previous_stop_phase_player)))
      {
        phase_stop_suppressed = 0;
      }
      else
      {
        phase_stop_suppressed = 1;
      }
    }
    else
    {
      if (g_duel_ai_mode_state != 1)
      {
        if ((stop_phase == -1) ||
            ((stop_phase == PHASE_CLEANUP2) && (player == stop_phase_player)))
        {
          phase_stop_suppressed = 0;
        }
        else
        {
          phase_stop_suppressed = 1;
        }
      }
    }
  }

  for (s.card = 0; s.card < MAX(active_cards_count[0], active_cards_count[1]); s.card++)
  {
    if (is_in_play(player, s.card) != 0)
    {
      global_card_instances[player][s.card].damage_on_card = 0;
      dispatch_event_to_single_card(player, s.card, 0x22, s.other_player, -1);
    }
    if (is_in_play(s.other_player, s.card) != 0)
    {
      global_card_instances[s.other_player][s.card].damage_on_card = 0;
      dispatch_event_to_single_card(s.other_player, s.card, 0x22, player, -1);
    }
  }

  if (unk_00789308 != -1)
  {
    (*global_cards_data[unk_00789308].code_pointer)(0, 0x94, 0x22);
  }
}

// FUNCTION: MAGIC 0x0044fe1a
void end_turn_phase(unsigned int player)
{
  struct
  {
    int trace_counter;
    char trace[104];
    short dynamic_card_count;
    int other_player;
    int dynamic_card_index;
    int card;
    short dynamic_card_ids[300];
  } s;
  int found;

  s.trace_counter = duel_trace_counter;
  s.other_player = 1 - player;
  if ((g_duel_network_flags & 2) != 0)
  {
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_End_Turn_Phase__0056e948, s.trace_counter);
    append_to_trace_txt(s.trace);
  }

  dispatch_trigger_twice_once_with_each_player_as_reason(player, 0xcd, gs_end_of_turn_008cd980, 0);
  regenerate_or_graveyard_triggers();
  if (g_duel_ai_mode_state != 1)
  {
    phase_stop_suppressed = 0;
  }
  FUN_004afa4b(player);
  TENTATIVE_reassess_all_cards();

  s.dynamic_card_count = 0;
  for (s.card = 0; s.card < 0x96; s.card = s.card + 1)
  {
    if (global_card_instances[player][s.card].internal_card_id != -1)
    {
      global_card_instances[player][s.card].state &= 0xffff7db2;
      global_card_instances[player][s.card].mana_color = -1;
      global_card_instances[player][s.card].damage_on_card = 0;
    }
    if (global_card_instances[s.other_player][s.card].internal_card_id != -1)
    {
      global_card_instances[s.other_player][s.card].state &= 0xffff7db2;
      global_card_instances[s.other_player][s.card].mana_color = -1;
      global_card_instances[s.other_player][s.card].damage_on_card = 0;
    }
    if (global_card_instances[player][s.card].internal_card_id >= g_card_count)
    {
      s.dynamic_card_ids[s.dynamic_card_count] = (short)global_card_instances[player][s.card].internal_card_id;
      s.dynamic_card_count++;
    }
    if (global_card_instances[s.other_player][s.card].internal_card_id >= g_card_count)
    {
      s.dynamic_card_ids[s.dynamic_card_count] = (short)global_card_instances[s.other_player][s.card].internal_card_id;
      s.dynamic_card_count++;
    }
  }

  for (s.card = g_card_count; s.card < g_card_count + 0x10; s.card = s.card + 1)
  {
    if (global_cards_data[s.card].id != -1)
    {
      s.dynamic_card_index = s.dynamic_card_count;
      found = 0;
      while (((s.dynamic_card_index = s.dynamic_card_index - 1) >= 0) && (found == 0))
      {
        if (s.dynamic_card_ids[s.dynamic_card_index] == s.card)
        {
          found = 1;
        }
      }

      s.dynamic_card_index = 0;
      while (s.dynamic_card_index < MAX(active_cards_count[active_player], active_cards_count[nonactive_player]))
      {
        if ((global_card_instances[nonactive_player][s.dynamic_card_index].internal_card_id != -1) &&
            ((global_card_instances[nonactive_player][s.dynamic_card_index].state & 2) != 0) &&
            (global_card_instances[nonactive_player][s.dynamic_card_index].dummy3 == s.card))
        {
          found = 1;
        }
        if ((global_card_instances[active_player][s.dynamic_card_index].internal_card_id != -1) &&
            ((global_card_instances[active_player][s.dynamic_card_index].state & 2) != 0) &&
            (global_card_instances[active_player][s.dynamic_card_index].dummy3 == s.card))
        {
          found = 1;
        }
        s.dynamic_card_index++;
      }

      if (found == 0)
      {
        global_cards_data[s.card].id = -1;
      }
    }
  }

  battlefield_extra_ability_flags = 0;
}

// FUNCTION: MAGIC 0x00450468
int ai_decision_phase(unsigned int player, int *next_state, int *phase_mode, int *phase_value)
{
  struct
  {
    char debug_text[100];
    char trace[100];
    int saved_ai_try_count;
    int opponent;
    int index;
    int loop_player;
    int local_50;
    int saved_special_mana[2][8];
    int saved_ai_flags;
    int ai_score;
  } s;
  int trace_counter;

  if ((g_duel_network_flags & 2) != 0)
  {
    trace_counter = duel_trace_counter;
    duel_trace_counter++;
    sprintf(s.trace, s__d__Entering_AI_Decision_Phase__0056e968, trace_counter);
    append_to_trace_txt(s.trace);
  }

  *next_state = -1;
  if (g_duel_ai_mode_state == 1)
  {
    s.opponent = 1 - player;
    for (s.index = 0; s.index <= 7; s.index = s.index + 1)
    {
      s.saved_special_mana[s.opponent][s.index] = unk_00742f70[s.opponent][s.index];
      unk_00742f70[s.opponent][s.index] = basiclandtypes_controlled[s.opponent][s.index];
    }

    C_dispatch_event_raw(199);
    for (s.index = 0; s.index < active_cards_count[active_player]; s.index = s.index + 1)
    {
      if (global_card_instances[active_player][s.index].internal_card_id != -1)
      {
        (*global_cards_data[global_card_instances[active_player][s.index].internal_card_id].code_pointer)(active_player, s.index, 0x38);
      }
    }

    FUN_004afa4b(player);
    regenerate_or_graveyard_triggers();
    for (s.loop_player = 0; s.loop_player < 2; s.loop_player = s.loop_player + 1)
    {
      for (s.index = 0; s.index <= 7; s.index = s.index + 1)
      {
        unk_00742f70[s.loop_player][s.index] = s.saved_special_mana[s.loop_player][s.index];
      }
    }

    FUN_00441d78();
    s.ai_score = ai_modifier + ai_opinion_of_gamestate(active_player);
    if (life[1] > 0)
    {
      ai_search_flags |= 4;
    }
    if (DAT_00712544 != 0)
    {
      FUN_004e6754(0, s.ai_score);
    }
    if ((DAT_0089652c < s.ai_score) && (DAT_0093d850 == 0))
    {
      DAT_0089652c = s.ai_score;
      FUN_004e50f5();
      s.saved_ai_flags = ai_search_flags;
      s.saved_ai_try_count = unk_008cc840;
    }
    if (DAT_008a8d7c == 999)
    {
      DAT_008a8d7c = -1;
    }
    DAT_008a8d74 = 0;
    *phase_value = DAT_008a8d74;
    DAT_0093d850 = *phase_value;
    if ((DAT_0091c500 / 2 < FUN_004a7ccf()) &&
        (((g_shandalar_difficulty * 5 + 5) * 5 <= unk_008cc840) ||
         (((ai_search_flags & 4) == 0 ? 0xc8 : 0x32) < FUN_004a7ccf())) &&
        ((DAT_00712544 == 0) || (unk_008cc840 > 0x32)))
    {
      sprintf(s.debug_text, s_phase___3d_num_tries___4d_mtime___0056e98c, DAT_00743098, unk_008cc840, DAT_0091c500 / 2, unk_0093f9c0);
      OutputDebugStringA(s.debug_text);
      if (DAT_00712544 != 0)
      {
        FUN_004e6754(1, DAT_0089652c);
      }
      unk_00712938 = -1;
      g_duel_ai_mode_state = 0;
      DAT_008a8d7c = -1;
      ai_search_flags = s.saved_ai_flags;
    }

    unk_008cc840++;
    DAT_007161cc = 0;
    if (ai_decision_code == 1)
    {
      if ((land_can_be_played & 0x100) != 0)
      {
        current_phase = PHASE_MAIN2;
      }
      else
      {
        current_phase = PHASE_MAIN1;
      }
      *phase_mode = 1;
      *next_state = 6;
    }
    if (ai_decision_code == 2)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 2;
      *next_state = 6;
    }
    if (ai_decision_code == 3)
    {
      *phase_mode = 1;
      *next_state = 7;
    }
    if (ai_decision_code == 4)
    {
      if ((land_can_be_played & 8) != 0)
      {
        current_phase = PHASE_MAIN2;
      }
      else
      {
        current_phase = PHASE_MAIN1;
      }
      *phase_value = 0;
      *phase_mode = 4;
      *next_state = 6;
    }
    if (ai_decision_code == 5)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 5;
      *next_state = 6;
    }
    if (ai_decision_code == 6)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 6;
      *next_state = 6;
    }
    if (ai_decision_code == 7)
    {
      if ((land_can_be_played & 8) != 0)
      {
        current_phase = PHASE_MAIN2;
      }
      else
      {
        current_phase = PHASE_MAIN1;
      }
      *phase_value = 0;
      *phase_mode = 7;
      *next_state = 6;
    }
    if (ai_decision_code == 8)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *phase_mode = 8;
      *next_state = 6;
    }
  }

  if (*next_state == -1)
  {
    current_phase = PHASE_CLEANUP2;
    s.local_50 = 0;
    if (s.local_50 == 0)
    {
      FUN_0044af2d(0x20);
    }
    FUN_0044b3d4();
    if (check_duel_finished() != 0)
    {
      return 1;
    }
    if ((human_player == stop_phase_player) && (stop_phase == PHASE_CLEANUP2))
    {
      stop_phase_player = -1;
      stop_phase = stop_phase_player;
    }
    FUN_0044aa01(player);
    play_sound_effect(5);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004b559a
// FUNCTION: SHANDALAR 0x0056bd9d
int AddCardToDeckSorted(int card_id)
{

  struct
  {
    int selected_state_sprite;
    int location_block_start_index;
    unsigned int inner_index;
    int entry_index;
    int local_4;
  } s;

  if (GetCardRarity(card_id) >= 3)
  {
    AddJournalEntry((int)card_id / 0x100 + JOURNAL_ENTRY_CARD_FOUND_SET0, card_id & 0xff);
  }
  s.entry_index = single_color_test_bit_to_color_t(global_cards_data[card_id].color) * 0x20 + (unsigned int)global_cards_data[card_id].type * 0x100 +
                  (int)global_cards_data[card_id].name[0];
  s.selected_state_sprite = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 500; s.location_block_start_index = s.location_block_start_index + 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      s.selected_state_sprite = 1;
    }
  }
  if (s.selected_state_sprite == 0)
  {
    return -1;
  }
  for (s.location_block_start_index = 0x1f2; s.location_block_start_index >= 0; s.location_block_start_index = s.location_block_start_index - 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      continue;
    }

    s.inner_index = (unsigned int)deck[s.location_block_start_index] & 0xfff;
    s.local_4 = single_color_test_bit_to_color_t(global_cards_data[s.inner_index].color) * 0x20 + (int)global_cards_data[s.inner_index].name[0] +
                (unsigned int)global_cards_data[s.inner_index].type * 0x100;
    if (s.local_4 >= s.entry_index)
    {
      deck[s.location_block_start_index + 1] = card_id;
      card_id = (unsigned int)-1;
      return s.location_block_start_index + 1;
    }
    deck[s.location_block_start_index + 1] = (unsigned int)deck[s.location_block_start_index];
  }
  deck[0] = card_id;
  return 0;
}

// FUNCTION: MAGIC 0x004b6091
// FUNCTION: SHANDALAR 0x0056c892
int CountDuelPoolEligibleTowns(void)
{
  int duel_pool_count;
  int town_index;

  duel_pool_count = 0;
  for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
  {
    if ((((g_town_slots[town_index].status_and_ruling_wizard & 0xff01U) == 1) &&
         (1 < g_town_slots[town_index].location_type)) &&
        (g_town_slots[town_index].location_type < 4))
    {
      duel_pool_count = duel_pool_count + 1;
    }
  }
  return duel_pool_count;
}

// FUNCTION: MAGIC 0x004e1c4f
// FUNCTION: SHANDALAR 0x00561441
char *GetCreatureName(int creature_type)
{
#ifdef SHANDALAR
  return gs_creature_names_00591a08[creature_type].name;
#else
  (void)creature_type;
  return DAT_007a7c60;
#endif
}

// FUNCTION: MAGIC 0x004c0a0a
// FUNCTION: SHANDALAR 0x004ece9a
int SeedRandomFromTickCount(void)
{
  // GLOBAL: SHANDALAR 0x007483f4
  // GLOBAL: MAGIC 0x0074b634
  static int _DAT_007483f4;

  _DAT_007483f4 = GetTickCount() & 0x7fff;
  DAT_007483f0 = 1;
  return 0;
}

// FUNCTION: MAGIC 0x004b5416
int draw_random_unmarked_deck_card(void)
{
  int candidate_count;
  int candidate_indexes[500];
  int deck_index;
  int random_index;

  candidate_count = 0;
  for (deck_index = 0; deck_index < 500; deck_index++)
  {
    if ((deck[deck_index] != -1) && ((deck[deck_index] & 0xc000) == 0))
    {
      candidate_indexes[candidate_count] = deck_index;
      candidate_count++;
    }
  }
  if (candidate_count == 0)
  {
    return -1;
  }
  random_index = internal_rand(candidate_count);
  deck[candidate_indexes[random_index]] |= 0x8000;
  return deck[candidate_indexes[random_index]] & 0xfff;
}

// FUNCTION: MAGIC 0x00464700
void copy_initial_library_to_player_zero(int wizard_color)
{
  int card_index;

  for (card_index = 0; card_index < 200; card_index++)
  {
    initial_library[0][card_index].csvid = initial_library[wizard_color][card_index].csvid;
    initial_library[0][card_index].numcards = initial_library[wizard_color][card_index].numcards;
  }
}

// FUNCTION: MAGIC 0x0046476f
void remove_iid_from_initial_library(int player, int internal_card_id)
{
  int card_index;

  for (card_index = 0; card_index < 200; card_index++)
  {
    if ((initial_library[player][card_index].numcards > 0) &&
        (CardTypeFromID(initial_library[player][card_index].csvid) == internal_card_id))
    {
      initial_library[player][card_index].numcards--;
      return;
    }
  }
}

// FUNCTION: MAGIC 0x004b3dee
void perform_player_mulligan(int player, int wizard_color)
{
  (void)player;
  (void)wizard_color;
}

// FUNCTION: MAGIC 0x004b3cc2
void evaluate_opening_hand_land_counts(int *redraw_result, int *ante_result, int *ante_info)
{
  int card_index;
  int opponent_land_count;
  int player_land_count;

  player_land_count = 0;
  opponent_land_count = 0;
  for (card_index = 0; card_index < 7; card_index++)
  {
    if ((global_cards_data[global_card_instances[1][card_index].internal_card_id].type & TYPE_LAND) != 0)
    {
      opponent_land_count++;
    }
    if ((global_cards_data[global_card_instances[0][card_index].internal_card_id].type & TYPE_LAND) != 0)
    {
      player_land_count++;
    }
  }
  if (player_land_count == 0)
  {
    *redraw_result = 1;
  }
  else if (player_land_count == 7)
  {
    *redraw_result = 2;
  }
  else
  {
    *redraw_result = 0;
  }
  if (opponent_land_count == 0)
  {
    *ante_result = 1;
  }
  else if (opponent_land_count == 7)
  {
    *ante_result = 2;
  }
  else
  {
    *ante_result = 0;
  }
  if ((opponent_land_count < 2) || (opponent_land_count > 5))
  {
    *ante_info = 1;
  }
  else
  {
    *ante_info = 0;
  }
}

// FUNCTION: MAGIC 0x004b23a9
// FUNCTION: SHANDALAR 0x0056862e
int play_duel(int player, int creature_type)
{
  struct
  {
    int unused_68;                  // ebp - 0x68
    int wizard_color;               // ebp - 0x64
    int library_count;              // ebp - 0x60
    int quantity;                   // ebp - 0x5c
    int loop_5c;                    // ebp - 0x58
    int packet_card;                // ebp - 0x54
    int loop_player;                // ebp - 0x50
    int shandalar_deck_minimums[6]; // ebp - 0x4c
    unsigned int mulligan_accepted; // ebp - 0x34
    int ante_result;                // ebp - 0x30
    int victory_count;              // ebp - 0x2c
    int opening_hand_count;         // ebp - 0x28
    int random_starting_player;     // ebp - 0x24
    int town_index;                 // ebp - 0x20
    int redraw_result;              // ebp - 0x1c
    int card_index;                 // ebp - 0x18
    int ante_info;                  // ebp - 0x14
    unsigned int next_player;       // ebp - 0x10
    unsigned int starting_player;   // ebp - 0xc
    int town_count;                 // ebp - 0x8
    int player_index;               // ebp - 0x4
  } s;

  (void)player;
  if ((g_duel_mode_flags & 1) == 0)
  {
    creature_type = -1;
  }

  StopWorldLocationMusic();
  unk_00742fc4 = 1;
  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
    {
      global_card_instances[s.player_index][s.card_index].original_internal_card_id = -1;
      global_card_instances[s.player_index][s.card_index].internal_card_id =
          global_card_instances[s.player_index][s.card_index].original_internal_card_id;
    }
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      global_exile[s.player_index][s.card_index] = -1;
      global_graveyard_slots[s.player_index][s.card_index] = global_exile[s.player_index][s.card_index];
    }
    DAT_00938e38[s.player_index] = 0;
    active_cards_count[s.player_index] = 0;
  }

  g_duel_special_land_card_ids[1] = 0xef;
  g_duel_special_land_card_ids[2] = 0x7e;
  g_duel_special_land_card_ids[3] = 0x5b;
  g_duel_special_land_card_ids[4] = 0xa4;
  g_duel_special_land_card_ids[5] = 0xbc;
  battlefield_extra_ability_flags = 0;
  unk_008b44d0[0] = 0;
  unk_008b44d0[1] = 0;
  reset_duel_globals();
  reset_timestamp_players();

  if (creature_type != -1)
  {
    s.shandalar_deck_minimums[4] = 0;
    s.shandalar_deck_minimums[0] = 0x1e;
    s.shandalar_deck_minimums[1] = 0x23;
    s.shandalar_deck_minimums[2] = 0x28;
    s.shandalar_deck_minimums[3] = 0x28;
    life[0] = 10;
    if ((g_world_magic_bitmap & 2) != 0)
    {
      life[0] += 2;
    }
    if ((g_world_magic_bitmap & 0x800) != 0)
    {
      life[0] += 3;
    }
    if ((g_world_magic_bitmap & 0x80) != 0)
    {
      life[0] += 5;
    }
    life[0] = CountDuelPoolEligibleTowns();
    life[0] += g_next_duel_life_delta;
    life[0] += DAT_0074636c;
    if (g_next_duel_card_id > 0 && g_next_duel_card_id <= 5)
    {
      life[0] += g_next_duel_card_id;
    }
    DAT_00716024 = life[0];
    g_next_duel_life_delta = 0;
    life[1] = g_shandalar_monster_definitions[creature_type].life;
    if (creature_type <= 0x24 && creature_type % 7 != 0)
    {
      life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type <= 0x24 && creature_type % 7 == 0)
    {
      life[1] += g_shandalar_difficulty * 5;
    }
    else if (creature_type < 0x37)
    {
      life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type >= 0x37)
    {
      life[1] += g_shandalar_difficulty * 50;
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xb)
    {
      for (s.card_index = 0; s.card_index < 10; s.card_index = s.card_index + 1)
      {
        if ((g_world_magic_bitmap & (1 << (byte)s.card_index)) != 0)
        {
          life[1]++;
        }
      }
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xc)
    {
      life[1] += 10;
      s.victory_count = 0;
      s.town_count = 0;
      for (s.card_index = 0; s.card_index < 1000; s.card_index = s.card_index + 1)
      {
        if (g_duel_victory_log[s.card_index] == 0)
        {
          break;
        }
        if ((g_duel_victory_log[s.card_index] >> 4) == DAT_008ce538)
        {
          s.victory_count++;
        }
      }
      life[1] -= s.victory_count;
      for (s.town_index = 0; s.town_index < 128; s.town_index = s.town_index + 1)
      {
        if ((g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00) != 0)
        {
          if (((g_town_slots[s.town_index].status_and_ruling_wizard >> 8) - 1) == s.card_index)
          {
            s.town_count++;
          }
        }
      }
      life[1] += s.town_count * g_shandalar_difficulty;
      life[1] = MAX(life[1], g_shandalar_difficulty * 5 + 20);
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xd)
    {
      life[1] = g_shandalar_difficulty * 100 + 100;
    }
    strcpy(DAT_007a7c60, GetCreatureName(creature_type));
    s.opening_hand_count = ClampIntToRange(DAT_007a7874 + g_shandalar_difficulty + 4, 0, 99);
    if (g_shandalar_difficulty == 3)
    {
      s.opening_hand_count = 7;
    }
    s.opening_hand_count += DAT_00742fd0;
    s.opening_hand_count = 7;
    if (s.opening_hand_count < 3)
    {
      s.opening_hand_count = 3;
    }
    unk_007161d8 = 7 - s.opening_hand_count;
    SeedRandomFromTickCount();
    if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
    {
      s.starting_player = 1;
    }
    else
    {
      s.starting_player = 0;
    }
    s.random_starting_player = 1;
    starting_player_was_random = 1;
    if (DAT_00776514 != 0 || g_next_duel_card_id == 0)
    {
      if (DAT_00776514 != 0)
      {
        s.starting_player = 1;
      }
      else
      {
        s.starting_player = 0;
      }
      s.random_starting_player = 0;
      starting_player_was_random = 0;
      DAT_00776514 = 0;
    }

    if (g_duel_ai_mode_state != 0)
    {
      goto idk;
    }
    DAT_00776510 = 0;
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (deck[s.card_index] != -1 && (deck[s.card_index] & 0x4000) == 0)
      {
        s.shandalar_deck_minimums[4]++;
      }
    }
    if (s.shandalar_deck_minimums[4] < s.shandalar_deck_minimums[g_shandalar_difficulty])
    {
      DAT_00776510 = 1;
      memcpy(DAT_00775d40, deck, 2000);
      for (s.card_index = 0;
           s.card_index < s.shandalar_deck_minimums[g_shandalar_difficulty] - s.shandalar_deck_minimums[4];
           s.card_index = s.card_index + 1)
      {
        AddCardToDeckSorted(internal_rand(5));
      }
    }
    for (s.card_index = 0; s.card_index < 0x3c; s.card_index = s.card_index + 1)
    {
      global_library[1][s.card_index] = 0;
      global_library[0][s.card_index] = global_library[1][s.card_index];
    }
    for (s.card_index = 0x3c; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      global_library[1][s.card_index] = -1;
      global_library[0][s.card_index] = global_library[1][s.card_index];
    }
    TENTATIVE_reassess_all_cards(0, 0x30);
    for (s.town_index = 0; s.town_index < 4; s.town_index = s.town_index + 1)
    {
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if ((deck[s.card_index] & 0x4000) == 0 &&
            (deck[s.card_index] & 0xfff) == global_ante_cards[0][s.town_index])
        {
          deck[s.card_index] |= 0x8000;
          break;
        }
      }
    }
    for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
    {
      if (g_selected_wizard_color == -1)
      {
        add_card_to_hand(0, draw_random_unmarked_deck_card());
      }
      else
      {
        add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
      }
    }
    if (DAT_0057a750 != -1)
    {
      if (g_selected_wizard_color == -1)
      {
        copy_initial_library_to_player_zero(DAT_0057a750);
        if (DAT_008ced00[0] != -1)
        {
          remove_iid_from_initial_library(0, DAT_008ced00[0]);
        }
      }
      for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
      {
        add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
      }
    }
    evaluate_opening_hand_land_counts(&s.ante_info, &s.ante_result, &s.redraw_result);
    run_duel_coin_flip_dialogs(&s.starting_player,
                               &s.mulligan_accepted,
                               s.starting_player,
                               s.random_starting_player,
                               DAT_008ced00[0],
                               global_ante_cards[0][0],
                               s.ante_info,
                               s.ante_result,
                               s.redraw_result,
                               &s.shandalar_deck_minimums[5]);
    if (s.mulligan_accepted != 0)
    {
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        global_card_instances[0][s.card_index].internal_card_id = -1;
        global_card_instances[0][s.card_index].original_internal_card_id = -1;
      }
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if (deck[s.card_index] != -1)
        {
          deck[s.card_index] &= -32769;
        }
      }
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if ((deck[s.card_index] & 0xfff) == global_ante_cards[0][0])
        {
          deck[s.card_index] |= 0x8000;
          break;
        }
      }
      for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
      {
        if (g_selected_wizard_color == -1)
        {
          add_card_to_hand(0, draw_random_unmarked_deck_card());
        }
        else
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
        }
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
    }
    if (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))
    {
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        global_card_instances[1][s.card_index].internal_card_id = -1;
        global_card_instances[1][s.card_index].original_internal_card_id = -1;
      }
      if (g_selected_wizard_color == -1)
      {
        copy_initial_library_to_player_zero(DAT_0057a750);
        if (DAT_008ced00[0] != -1)
        {
          remove_iid_from_initial_library(0, DAT_008ced00[0]);
        }
      }
      for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
      {
        add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
    }
    if (g_selected_wizard_color == -1)
    {
      DAT_0057a750 = 0;
    }
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (g_selected_wizard_color == -1)
      {
        global_library[0][s.card_index] = draw_random_unmarked_deck_card();
      }
      else
      {
        global_library[0][s.card_index] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
      }
      global_library[1][s.card_index] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
    }
    g_selected_wizard_color = -1;
    if ((g_shandalar_monster_definitions[creature_type].flags & 2) != 0 && g_monster_timer % 3 == 0)
    {
      memcpy(global_library[1], global_library[0], 1000);
      s.card_index = g_duel_ai_mode_state;
      g_duel_ai_mode_state = 1;
      shuffle_duel_library(0, 1);
      g_duel_ai_mode_state = s.card_index;
      memcpy(global_card_instances[1], global_card_instances[0], 0xb0f4);
      for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
      {
        if (global_card_instances[1][s.card_index].internal_card_id != -1)
        {
          global_card_instances[1][s.card_index].state |= 0x1000;
        }
      }
      unk_007161d8 = 0;
    }
    g_duel_ai_mode_state = 0;
    reset_duel_globals();
    if (DAT_008cf6d0 != -1)
    {
      s.card_index = add_card_to_hand(1, DAT_008cf6d0);
      process_card_enters_play(1, s.card_index);
      DAT_008cf6d0 = -1;
      if (g_next_duel_card_id == -1)
      {
        g_next_duel_card_id = 0;
      }
    }
    if (unk_00789308 != -1)
    {
      s.card_index = add_card_to_hand(1, unk_00789308);
      process_card_enters_play(1, s.card_index);
      unk_00789308 = -1;
      if (g_next_duel_card_id == -1)
      {
        g_next_duel_card_id = 0;
      }
    }
    if (g_next_duel_card_id > 5)
    {
      s.card_index = add_card_to_hand(0, g_next_duel_card_id);
      process_card_enters_play(0, s.card_index);
    }

    goto idk;
  }
  else
  {
    if ((g_duel_network_flags & 2) != 0)
    {
      append_to_trace_txt(s_Starting_the_duel_005732b4);
    }
    life[1] = 0x14;
    life[0] = life[1];
    copy_opponent_name_prefix(DAT_007a7c60);
    s.opening_hand_count = 7;
    unk_007161d8 = 0;
    DAT_008951c8 = 1;
    DAT_008ce538 = -1;
    SeedRandomFromTickCount();
    if ((g_duel_network_flags & 2) != 0)
    {
      send_battlefield_status_packet(0);
      receive_battlefield_status_packet(1);
    }
    if (DAT_00896718 == -1)
    {
      if ((g_duel_network_flags & 2) != 0)
      {
        s.starting_player = internal_rand(2);
      }
      else
      {
        if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
        {
          s.starting_player = 1;
        }
        else
        {
          s.starting_player = 0;
        }
      }
      s.random_starting_player = 1;
    }
    else
    {
      s.starting_player = DAT_00896718;
      s.random_starting_player = 0;
    }
    if ((g_duel_network_flags & 2) != 0)
    {
      if (DAT_007a79b8 != 0)
      {
        g_network_result_packet_type = 1;
        g_network_result_value = 1 - s.starting_player;
        TENTATIVE_send_network_result(0, 1);
      }
      else
      {
        TENTATIVE_wait_for_network_result(1, 1);
        s.starting_player = g_network_result_value;
      }
    }
    starting_player_was_random = 1;
    if (g_duel_use_previous_backdrop_colors == 0)
    {
      for (s.loop_5c = 0; s.loop_5c < 0x3c; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = 0;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      for (s.loop_5c = 0x3c; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = -1;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
      for (s.loop_5c = 0; s.loop_5c < 0x10; s.loop_5c = s.loop_5c + 1)
      {
        DAT_008ced00[s.loop_5c] = -1;
        global_ante_cards[0][s.loop_5c] = DAT_008ced00[s.loop_5c];
      }
      if (DAT_00789104 != 0)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          g_network_result_packet_type = 2;
          g_network_result_value = global_ante_cards[0][0];
          TENTATIVE_send_network_result(0, 2);
          TENTATIVE_wait_for_network_result(1, 2);
          DAT_008ced00[0] = g_network_result_value;
        }
        else
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          DAT_008ced00[0] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
        }
      }
      if ((g_duel_network_flags & 2) != 0)
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          s.packet_card = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          add_card_to_hand(0, s.packet_card);
          AddCardToCLPacket(s.packet_card);
        }
        AddCardToCLPacket(-1);
        TENTATIVE_send_network_result(s.player_index, 3);
        TENTATIVE_wait_for_network_result(1, 3);
        s.packet_card = 0;
        s.loop_5c = s.packet_card;
        while (s.packet_card != -1)
        {
          s.unused_68 = s.loop_5c;
          s.loop_5c++;
          s.packet_card = GetCardFromCLPacket(s.unused_68);
          if (s.packet_card != -1)
          {
            add_card_to_hand(1, s.packet_card);
          }
        }
      }
      else
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
          add_card_to_hand(1, DrawRandomCardFromInitialLibrary(DAT_0057a750));
        }
      }
      evaluate_opening_hand_land_counts(&s.ante_info, &s.ante_result, &s.redraw_result);
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = DAT_0057a750;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] = 0;
              s.library_count++;
            }
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      run_duel_coin_flip_dialogs(&s.starting_player,
                                 &s.mulligan_accepted,
                                 s.starting_player,
                                 s.random_starting_player,
                                 DAT_008ced00[0],
                                 global_ante_cards[0][0],
                                 s.ante_info,
                                 s.ante_result,
                                 s.redraw_result,
                                 &s.shandalar_deck_minimums[5]);
      if (s.mulligan_accepted != 0)
      {
        perform_player_mulligan(0, g_selected_wizard_color);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      if (((g_duel_network_flags & 2) == 0 && (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))) ||
          ((g_duel_network_flags & 2) != 0 && s.shandalar_deck_minimums[5] != 0))
      {
        perform_player_mulligan(1, DAT_0057a750);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if ((g_duel_network_flags & 2) && s.loop_player == 1)
        {
          for (s.loop_5c = 0; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
          {
            global_library[s.loop_player][s.loop_5c] = -1;
          }

          continue;
        }

        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = DAT_0057a750;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] =
                  CardTypeFromID(initial_library[s.wizard_color][s.loop_5c].csvid);
              s.library_count++;
            }
            initial_library[s.wizard_color][s.loop_5c].numcards = 0;
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      g_selected_wizard_color = -1;
      s.loop_5c = g_duel_ai_mode_state;
      g_duel_ai_mode_state = 1;
      shuffle_duel_library(0, 0);
      shuffle_duel_library(1, 1);
      g_duel_ai_mode_state = s.loop_5c;
    }
  }

  idk:
  reset_duel_globals();

  if (g_duel_ai_mode_state == -1)
  {
    load_selected_duel_save_slot(0);
  }
  if (g_duel_ai_mode_state == -2)
  {
    load_selected_duel_save_slot(1);
  }
  if (g_duel_ai_mode_state == -10 && 0 < g_duel_run_mode)
  {
    if (g_duel_run_mode == 1)
    {
      load_duel_run_mode_1_save(DAT_008a915c);
    }
    else if (g_duel_run_mode == 2)
    {
      load_duel_run_mode_2_save(DAT_008a915c);
    }
    else if (g_duel_run_mode == 3)
    {
      load_duel_run_mode_3_save(DAT_008a915c);
    }
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = human_player;
  }
  else if (g_duel_ai_mode_state == -10)
  {
    load_gametype0(DAT_008a915c);
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = human_player;
  }
  else if (g_duel_ai_mode_state == -1)
  {
    first_turn_draw_pending = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 1;
  }
  else if (g_duel_ai_mode_state == -2)
  {
    first_turn_draw_pending = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 0;
  }
  else
  {
    first_turn_draw_pending = 1;
    s.next_player = s.starting_player;
  }

#ifndef SHANDALAR
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
#endif
  while (DAT_0091bfb0 == 0 && check_duel_finished() == 0)
  {
    if (DAT_0093a848 != 0)
    {
      if ((DAT_0093a848 & 1U) != 0)
      {
        DAT_0093a848 = 0;
        if (s.next_player == 1)
        {
          g_duel_extra_turn_player = -1;
        }
        run_duel_turn(0);
        g_duel_extra_turn_player = -1;
      }
      else
      {
        DAT_0093a848 = 0;
        if (s.next_player == 0)
        {
          g_duel_extra_turn_player = -1;
        }
        run_duel_turn(1);
        g_duel_extra_turn_player = -1;
      }
    }
    else
    {
      run_duel_turn(s.next_player);
      s.next_player = 1 - s.next_player;
    }
  }
#ifndef SHANDALAR
  ReleaseMutex(global_mutex_GameInit);
#endif

  if (DAT_0074302c == 0)
  {
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (deck[s.card_index] != -1)
      {
        deck[s.card_index] &= -32769;
      }
    }
  }
  else
  {
    OutputDebugStringA("OneDeck ONEDECK ONE DECK\n");
  }

  g_next_duel_card_id = -1;
  DAT_008cf6d0 = g_next_duel_card_id;
  unk_00789308 = DAT_008cf6d0;
  g_duel_ai_mode_state = 0;
  for (s.card_index = 0; s.card_index < 4; s.card_index = s.card_index + 1)
  {
    DAT_007a7d10[s.card_index] = 8;
  }
  unk_00742fc4 = 0;
  DAT_00742fc0 = 1;
  if (life[0] > 0 && DAT_007abce0 < 10 && (life[1] <= 0 || DAT_007abce4 >= 10))
  {
    return 1;
  }
  if (life[1] > 0 && DAT_007abce4 < 10 && (life[0] <= 0 || DAT_007abce0 >= 10))
  {
    return 0;
  }
  return -1;
}

// FUNCTION: MAGIC 0x004dbe21
// FUNCTION: SHANDALAR 0x00542adc
DWORD WINAPI start_duel_thread(LPVOID unused)
{
  DWORD result;
  int counter;

  (void)unused;
  srand(GetTickCount());
  for (counter = 0; counter < rand() % 50000; counter = counter + 1)
  {
    rand();
  }

  InitializeDuelUiGlobalIds();
  result = play_duel(0, g_duel_creature_type);
  PostMessageA(g_duel_window_hwnd, 0x401, result, 0);
  return result;
}
