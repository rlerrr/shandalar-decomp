#include <process.h>
#include <string.h>
#include "magic_shell_dialogs.h"
#include "magic_sealed_duel.h"
#include "magic_sealed_build.h"
#include "global_duel_ui_ids.h"
#include "global_other.h"
#include "global_state.h"

extern char shell_gauntlet_opponent_deck_path[264];
int initialize_duel_engine_window(void);
void shutdown_duel_engine_window(void);

// GLOBAL: MAGIC 0x00707bac
int g_sealed_match_minimum_deck_size;
// GLOBAL: MAGIC 0x00707bb4
int g_sealed_match_maximum_deck_size;
// GLOBAL: MAGIC 0x00707bbc
static sealed_deck_player_t *g_sealed_match_player;
// GLOBAL: MAGIC 0x00707be0
sealed_deck_player_t *g_sealed_match_opponent;

static void shell_update_sealed_ante_cards(void);

// FUNCTION: MAGIC 0x0050c743
int shell_run_sealed_player_match(int resume, HWND owner, sealed_deck_player_t *player,
    sealed_deck_player_t *opponent, int ante, int minimum_deck_size)
{
  struct
  {
    unsigned int thread_id;
    int resume_match;
    HANDLE build_thread;
    int card_index;
    int result;
  } s;

  s.build_thread = NULL;
  g_duel_run_mode = 3;
  if (resume != 0)
  {
    s.resume_match = 1;
    resume = 0;
    g_sealed_match_minimum_deck_size = minimum_deck_size;
    g_sealed_match_maximum_deck_size = g_sealed_deck_extra_card_limit + minimum_deck_size;
  }
  else
  {
    s.resume_match = 0;
    g_sealed_match_minimum_deck_size = minimum_deck_size;
    g_sealed_match_maximum_deck_size = g_sealed_deck_extra_card_limit + minimum_deck_size;
    s.build_thread = (HANDLE)_beginthreadex(NULL, 0,
        (unsigned int (__stdcall *)(void *))shell_build_sealed_match_opponent, opponent, 0, &s.thread_id);
    g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1 = -1;
    InitializeDuelUiGlobalIds();
    strcpy(shell_gauntlet_opponent_deck_path, "");
    _OpponFace = 0;
    strcpy((char *)g_duel_state_block_008ce570, opponent->face_path);
    strcpy(g_saved_player_name, opponent->name);
    g_opponent_initial_library_index = 1;
    for (s.card_index = 0; s.card_index < 200; ++s.card_index)
    {
      if (s.card_index < opponent->deck_count)
      {
        ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid = opponent->deck[s.card_index];
        g_initial_library[g_opponent_initial_library_index][s.card_index].csvid =
            ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid;
        ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards = 1;
        g_initial_library[g_opponent_initial_library_index][s.card_index].numcards =
            ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards;
      }
      else
      {
        ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid = -1;
        g_initial_library[g_opponent_initial_library_index][s.card_index].csvid =
            ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid;
        ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards = 0;
        g_initial_library[g_opponent_initial_library_index][s.card_index].numcards =
            ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards;
      }
    }
    g_shandalar_difficulty = opponent->difficulty;
    g_duel_state_00789104 = ante;
    strcpy(shell_gauntlet_player_deck_path, "");
    _PlayerFace = 0;
    strcpy(g_player_name, player->name);
    g_selected_wizard_color = 0;
    for (s.card_index = 0; s.card_index < 200; ++s.card_index)
    {
      if (s.card_index < player->deck_count)
      {
        ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid = player->deck[s.card_index];
        g_initial_library[g_selected_wizard_color][s.card_index].csvid =
            ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid;
        ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards = 1;
        g_initial_library[g_selected_wizard_color][s.card_index].numcards =
            ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
      }
      else
      {
        ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid = -1;
        g_initial_library[g_selected_wizard_color][s.card_index].csvid =
            ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid;
        ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards = 0;
        g_initial_library[g_selected_wizard_color][s.card_index].numcards =
            ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
      }
    }
  }
  if (s.resume_match == 0)
    shell_show_gauntlet_matchup(owner, "", g_player_name, NULL,
        (char *)g_duel_state_block_008b3fc0, g_saved_player_name, NULL,
        (char *)g_duel_state_block_008ce570, s.build_thread);
  PostMessageA(owner, 0x496, 0, 0);
  if (initialize_duel_engine_window() == 0)
  {
    return -2;
  }
  if (s.build_thread != NULL)
  {
    WaitForSingleObject(s.build_thread, INFINITE);
    CloseHandle(s.build_thread);
    s.build_thread = NULL;
  }
  g_sealed_match_player = player;
  g_sealed_match_opponent = opponent;
  s.result = shell_execute_gauntlet_match(s.resume_match, g_sealed_deck_options.best_of, 1,
      ante, 0, minimum_deck_size, shell_update_sealed_ante_cards, 0);
  SendMessageA(owner, 0x496, 1, 0);
  shutdown_duel_engine_window();
  return s.result;
}


// FUNCTION: MAGIC 0x0050cb6c
unsigned int __stdcall shell_build_sealed_match_opponent(sealed_deck_player_t *opponent)
{
  shell_build_sealed_deck(opponent->card_pool, opponent->card_pool_count, 0,
      opponent->difficulty, opponent->reserved_0304, g_sealed_match_minimum_deck_size,
      g_sealed_match_maximum_deck_size, opponent->deck, &opponent->deck_count);
  return 1;
}

// FUNCTION: MAGIC 0x0050cbd2
static void shell_update_sealed_ante_cards(void)
{
  struct
  {
    sealed_deck_player_t *player;
    int pool_index;
    int old_deck[300];
    int card_index;
    int current_count;
    sealed_deck_player_t *opponent;
    int old_count;
    int current_deck[300];
    int found;
  } s;

  s.player = g_sealed_match_player;
  s.opponent = g_sealed_match_opponent;
  for (s.card_index = 0; s.card_index < s.player->deck_count; ++s.card_index)
    s.old_deck[s.card_index] = s.player->deck[s.card_index];
  s.old_count = s.player->deck_count;
  s.card_index = 0;
  for (s.current_count = 0; s.card_index < 200; ++s.card_index)
  {
    if (((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid != -1)
    {
      for (s.pool_index = 0; s.pool_index < ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
          ++s.pool_index)
      {
        s.current_deck[s.current_count] = ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid;
        ++s.current_count;
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.old_count; ++s.card_index)
  {
    s.pool_index = 0;
    s.found = 0;
    for (; s.pool_index < s.current_count && s.found == 0; ++s.pool_index)
    {
      if (s.current_deck[s.pool_index] == s.old_deck[s.card_index])
      {
        s.found = 1;
        s.old_deck[s.card_index] = -1;
        s.current_deck[s.pool_index] = -1;
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.old_count; ++s.card_index)
  {
    if (s.old_deck[s.card_index] != -1)
    {
      s.pool_index = 0;
      s.found = 0;
      for (; s.pool_index < s.player->card_pool_count && s.found == 0; ++s.pool_index)
      {
        if (s.player->card_pool[s.pool_index] == s.old_deck[s.card_index])
        {
          s.found = 1;
          s.player->card_pool[s.pool_index] = s.player->card_pool[s.player->card_pool_count - 1];
          --s.player->card_pool_count;
        }
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.current_count; ++s.card_index)
  {
    if (s.current_deck[s.card_index] != -1)
    {
      s.player->card_pool[s.player->card_pool_count] = s.current_deck[s.card_index];
      ++s.player->card_pool_count;
    }
  }
  for (s.card_index = 0; s.card_index < s.opponent->deck_count; ++s.card_index)
    s.old_deck[s.card_index] = s.opponent->deck[s.card_index];
  s.old_count = s.opponent->deck_count;
  s.card_index = 0;
  for (s.current_count = 0; s.card_index < 200; ++s.card_index)
  {
    if (((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid != -1)
    {
      for (s.pool_index = 0; s.pool_index < ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards;
          ++s.pool_index)
      {
        s.current_deck[s.current_count] = ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid;
        ++s.current_count;
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.old_count; ++s.card_index)
  {
    s.pool_index = 0;
    s.found = 0;
    for (; s.pool_index < s.current_count && s.found == 0; ++s.pool_index)
    {
      if (s.current_deck[s.pool_index] == s.old_deck[s.card_index])
      {
        s.found = 1;
        s.old_deck[s.card_index] = -1;
        s.current_deck[s.pool_index] = -1;
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.old_count; ++s.card_index)
  {
    if (s.old_deck[s.card_index] != -1)
    {
      s.pool_index = 0;
      s.found = 0;
      for (; s.pool_index < s.opponent->card_pool_count && s.found == 0; ++s.pool_index)
      {
        if (s.opponent->card_pool[s.pool_index] == s.old_deck[s.card_index])
        {
          s.found = 1;
          s.opponent->card_pool[s.pool_index] = s.opponent->card_pool[s.opponent->card_pool_count - 1];
          --s.opponent->card_pool_count;
        }
      }
    }
  }
  for (s.card_index = 0; s.card_index < s.current_count; ++s.card_index)
  {
    if (s.current_deck[s.card_index] != -1)
    {
      s.opponent->card_pool[s.opponent->card_pool_count] = s.current_deck[s.card_index];
      ++s.opponent->card_pool_count;
    }
  }
}
