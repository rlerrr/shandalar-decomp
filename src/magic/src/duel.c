#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
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
void FUN_004432ed(int player, int phase);
int reset_empty_card_original_ids(void);
void compact_timestamp_slots(void);
int reset_stack_tracking_state(void);
void rebuild_battlefield_summary(void);
extern int DAT_008ced00[16];
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

// GLOBAL: MAGIC 0x00746880
ShandalarMonsterDefinition g_shandalar_monster_definitions[56];

// GLOBAL: MAGIC 0x009251d4
int DAT_009251d4;

// GLOBAL: MAGIC 0x00896690
char DAT_00896690;

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
    append_to_trace_txt(&unk_0056e49c[4]);
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
      unk_00716244 = human_player;
      unk_00716248 = current_phase;
      if (current_phase == 0)
      {
        unk_00716248 = -1;
      }
      if (current_phase == 0x22)
      {
        unk_00716248 = 0x20;
      }
      if (current_phase == 0)
      {
        next_state = 2;
      }
      else if (current_phase == 1)
      {
        next_state = 3;
      }
      else if (current_phase == 4)
      {
        next_state = 4;
      }
      else if (current_phase == 10)
      {
        next_state = 5;
      }
      else if (current_phase == 0x14)
      {
        next_state = 6;
      }
      else if (current_phase == 0x1f)
      {
        next_state = 7;
      }
      else if (current_phase == 0x22)
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
    s.trace_counter = DAT_00777aa0;
    DAT_00777aa0++;
    sprintf(s.trace, s__d__Entering_Init_Turn__0056e560, s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  s.other_player = 1 - player;
  human_player = player;
  DAT_00777a9c = -1;
  if (unk_008b35ec == player)
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
    DAT_007abc90[s.card] &= 5;
    DAT_007abc90[s.card + 0x26] &= 5;
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
    s.trace_counter = DAT_00777aa0;
    DAT_00777aa0++;
    sprintf(s.trace, s__d__Entering_Start_Turn__0056e57c, s.trace_counter);
    append_to_trace_txt(s.trace);
  }
  if ((g_duel_ai_mode_state != 1) && ((g_duel_network_flags & 2) == 0))
  {
    TENTATIVE_savegame(0);
  }
  if ((unk_008b4278 & 0x8000) != 0)
  {
    unk_008b4278 &= -32769;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  FUN_004e4e75();
  current_phase = 0;
  FUN_004432ed(player, current_phase);
  C_dispatch_event_raw(0x6a);
  if ((unk_008b4278 & 0x8000) != 0)
  {
    unk_008b4278 &= -32769;
    C_dispatch_event_raw(0x22);
    return 1;
  }
  DAT_00896690 = 0;
  unk_008b60e0 = 0;
  unk_008b4278 &= -512;
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
}

// FUNCTION: MAGIC 0x004432ed
void FUN_004432ed(int player, int phase)
{
  (void)player;
  (void)phase;
}

// FUNCTION: MAGIC 0x0044bfe9
int untap_phase_exe(unsigned int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x0044c88f
int upkeep_phase(unsigned int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x0044caa3
int draw_phase(unsigned int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x0044cda7
int main_phase(unsigned int player, int phase_mode, int *phase_value)
{
  (void)player;
  (void)phase_mode;
  (void)phase_value;
  return 0;
}

// FUNCTION: MAGIC 0x0044f682
int discard_phase(unsigned int player, int phase_mode)
{
  (void)player;
  (void)phase_mode;
  return 0;
}

// FUNCTION: MAGIC 0x0044fb96
void cleanup_phase(unsigned int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x0044fe1a
void end_turn_phase(unsigned int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x00450468
int ai_decision_phase(unsigned int player, int *next_state, int *phase_mode, int *phase_value)
{
  (void)player;
  (void)next_state;
  (void)phase_mode;
  (void)phase_value;
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
  unk_007abc78 = 0;
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
    DAT_008a8ffc = 1;
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
      DAT_008a8ffc = 0;
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
    DAT_008a8ffc = 1;
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
    DAT_008b484c = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 1;
  }
  else if (g_duel_ai_mode_state == -2)
  {
    DAT_008b484c = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 0;
  }
  else
  {
    DAT_008b484c = 1;
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
