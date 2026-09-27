#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "magic_shell_dialogs.h"
#include "magic_sealed_tournament.h"
#include "magic_sealed_build.h"
#include "magic_sealed_open_packs.h"
#include "magic_sealed_ladder.h"
#include "magic_shell.h"
#include "global_other.h"
#include "global_state.h"
#include "game_support.h"

extern HWND global_main_hwnd;
extern char global_base_directory[];
extern char g_exp1_art_path[];

// GLOBAL: MAGIC 0x0064fbc0
sealed_deck_tournament_t g_sealed_deck_tournament;
// GLOBAL: MAGIC 0x008b2944
int g_sealed_deck_extra_card_limit;
// FUNCTION: MAGIC 0x004c5659
static unsigned int __stdcall shell_build_sealed_opponent_decks(void *context)
{
  struct
  {
    int land_id;
    sealed_deck_player_t *player;
    int pending_players[255];
    int card_index;
    int stopped;
    int player_index;
  } s;

  EnterCriticalSection(
      &((sealed_deck_tournament_t *)context)->critical_section);
  for (s.player_index = 0;
       s.player_index < ((sealed_deck_tournament_t *)context)->player_count;
       ++s.player_index)
  {
    s.pending_players[s.player_index] = -1;
    if (((sealed_deck_tournament_t *)context)->bracket[s.player_index] != 0 &&
        ((sealed_deck_tournament_t *)context)->players[
            ((sealed_deck_tournament_t *)context)->bracket[s.player_index]].deck_built == 0)
      s.pending_players[s.player_index] =
          ((sealed_deck_tournament_t *)context)->bracket[s.player_index];
  }
  LeaveCriticalSection(
      &((sealed_deck_tournament_t *)context)->critical_section);
  s.stopped = 0;
  for (s.player_index = 1;
       s.player_index < ((sealed_deck_tournament_t *)context)->player_count &&
       s.stopped == 0; ++s.player_index)
  {
    s.player = &((sealed_deck_tournament_t *)context)->players[s.player_index];
    if (s.player->deck_built == 0)
    {
      shell_build_sealed_deck(
          s.player->card_pool, s.player->card_pool_count,
          ((sealed_deck_tournament_t *)context)->free_lands,
          s.player->difficulty, s.player->reserved_0304,
          ((sealed_deck_tournament_t *)context)->minimum_deck_size,
          ((sealed_deck_tournament_t *)context)->minimum_deck_size +
              g_sealed_deck_extra_card_limit,
          s.player->deck, &s.player->deck_count);
      for (s.card_index = 0; s.card_index < s.player->deck_count; ++s.card_index)
      {
        s.land_id = -1;
        if (s.player->deck[s.card_index] == -1)
          s.land_id = 0xef;
        else if (s.player->deck[s.card_index] == -2)
          s.land_id = 0x7e;
        else if (s.player->deck[s.card_index] == -3)
          s.land_id = 0x5b;
        else if (s.player->deck[s.card_index] == -4)
          s.land_id = 0xa4;
        else if (s.player->deck[s.card_index] == -5)
          s.land_id = 0xbc;
        if (s.land_id != -1)
        {
          s.player->deck[s.card_index] = s.land_id;
          s.player->card_pool[s.player->card_pool_count] = s.land_id;
          ++s.player->card_pool_count;
        }
      }
      if (((sealed_deck_tournament_t *)context)->deck_builder_thread != NULL)
      {
        s.player->deck_built = 1;
        ((sealed_deck_tournament_t *)context)->decks_changed = 1;
        s.pending_players[s.card_index] = -1;
      }
    }
    if (((sealed_deck_tournament_t *)context)->deck_builder_thread == NULL)
      s.stopped = 1;
  }
  MessageBeep(0);
  return 1;
}

// FUNCTION: MAGIC 0x004c5b52
static void shell_show_sealed_open_packs(HWND owner, sealed_deck_pack_t *packs,
                                        int pack_count)
{
  g_sealed_deck_open_packs = packs;
  g_sealed_deck_open_pack_count = pack_count;
  DialogBoxParamA(g_app_instance, "FoilPackScreen", owner,
                  shell_sealed_open_packs_dialog_proc, 0);
}

// FUNCTION: MAGIC 0x004c81d6
static void shell_show_sealed_ladder(HWND owner,
                                    sealed_deck_tournament_t *tournament)
{
  DialogBoxParamA(g_app_instance, "LadderScreen", owner,
                  shell_sealed_ladder_dialog_proc, (LPARAM)tournament);
}

// FUNCTION: MAGIC 0x004c4e68
int shell_run_sealed_deck_duel(int resume_saved_game)
{
  struct
  {
    int bracket_slot;
    int rogue_index;
    int result;
    sealed_deck_player_t *player;
    int player_index;
    unsigned int thread_id;
  } s;

  g_sealed_deck_extra_card_limit = 10;
  if (resume_saved_game != 0)
  {
    memcpy(&g_sealed_deck_tournament, g_savegame_data_pointer,
            sizeof(g_sealed_deck_tournament));
    g_sealed_deck_tournament.deck_builder_thread = NULL;
    g_sealed_deck_tournament.opponent_match_thread = 0;
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_tournament.player_count;
         ++s.player_index)
      g_sealed_deck_tournament.players[s.player_index].art = NULL;
    InitializeCriticalSection(&g_sealed_deck_tournament.critical_section);
    g_savegame_data_pointer = &g_sealed_deck_tournament;
    if (g_sealed_deck_tournament.duel_in_progress == 0 &&
        g_sealed_deck_tournament.opponents_in_progress == 0)
    {
      g_sealed_deck_tournament.deck_builder_thread = (HANDLE)_beginthreadex(
          NULL, 0, shell_build_sealed_opponent_decks,
          &g_sealed_deck_tournament, 0, &s.thread_id);
      SetThreadPriority(g_sealed_deck_tournament.deck_builder_thread, -1);
    }
  }
  else
  {
    g_savegame_data_pointer = &g_sealed_deck_tournament;
    g_sealed_deck_tournament.player_count =
        1 << (unsigned char)g_sealed_deck_options.rounds;
    g_sealed_deck_tournament.rounds = g_sealed_deck_options.rounds;
    g_sealed_deck_tournament.ante = g_sealed_deck_options.ante;
    g_sealed_deck_tournament.best_of = g_sealed_deck_options.best_of;
    g_sealed_deck_tournament.starter_count = g_sealed_deck_options.starter_count;
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_options.starter_count; ++s.player_index)
      g_sealed_deck_tournament.starter_types[s.player_index] =
          g_sealed_deck_options.starter_packs[s.player_index];
    g_sealed_deck_tournament.booster_count = g_sealed_deck_options.booster_count;
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_options.booster_count; ++s.player_index)
      g_sealed_deck_tournament.booster_types[s.player_index] =
          g_sealed_deck_options.booster_packs[s.player_index];
    g_sealed_deck_tournament.free_lands = g_sealed_deck_options.free_lands;
    g_sealed_deck_tournament.minimum_deck_size =
        g_sealed_deck_options.minimum_deck_size;
    shell_load_rogue_profiles();
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_tournament.player_count;
         ++s.player_index)
    {
      s.player = &g_sealed_deck_tournament.players[s.player_index];
      if (s.player_index == 0)
      {
        load_active_screen_name_profile();
        strcpy(s.player->name, g_player_name);
        strcpy(s.player->description, g_screen_name_profile.personal_quote);
        sprintf(s.player->face_path, "%s\\PlayFace\\%s.PIC",
                global_base_directory, g_screen_name_profile.playface_name);
        s.player->art = NULL;
      }
      else
      {
        s.rogue_index = rand() % shell_rogue_count;
        while (shell_rogue_profiles[s.rogue_index].name[0] == '\0')
          s.rogue_index = (s.rogue_index + 1) % shell_rogue_count;
        strcpy(s.player->name, shell_rogue_profiles[s.rogue_index].name);
        strcpy(s.player->description,
                shell_rogue_profiles[s.rogue_index].deck_name);
        sprintf(s.player->face_path, "%s\\Rogues\\%s", g_exp1_art_path,
                shell_rogue_profiles[s.rogue_index].face_name);
        s.player->art = NULL;
        s.player->difficulty = shell_rogue_profiles[s.rogue_index].difficulty;
        s.player->reserved_0304 = 0;
        strcpy(shell_rogue_profiles[s.rogue_index].name, "");
      }
      s.player->pack_count = g_sealed_deck_tournament.starter_count +
                             g_sealed_deck_tournament.booster_count;
      shell_generate_sealed_packs(
          s.player->packs, s.player->card_pool, &s.player->card_pool_count,
          g_sealed_deck_tournament.starter_count,
          g_sealed_deck_tournament.starter_types,
          g_sealed_deck_tournament.booster_count,
          g_sealed_deck_tournament.booster_types);
    }
    g_sealed_deck_tournament.decks_changed = 0;
    InitializeCriticalSection(&g_sealed_deck_tournament.critical_section);
    EnterCriticalSection(&g_sealed_deck_tournament.critical_section);
    for (s.player_index = 0; s.player_index < 255; ++s.player_index)
      g_sealed_deck_tournament.bracket[s.player_index] = -1;
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_tournament.player_count;
         ++s.player_index)
    {
      s.bracket_slot = rand() % g_sealed_deck_tournament.player_count;
      while (g_sealed_deck_tournament.bracket[s.bracket_slot] != -1)
        s.bracket_slot = (s.bracket_slot + 1) %
                          g_sealed_deck_tournament.player_count;
      g_sealed_deck_tournament.bracket[s.bracket_slot] = s.player_index;
    }
    LeaveCriticalSection(&g_sealed_deck_tournament.critical_section);
    g_sealed_deck_tournament.current_round = 1;
    g_sealed_deck_tournament.duel_in_progress = 0;
    g_sealed_deck_tournament.opponents_in_progress = 0;
    for (s.player_index = 0;
         s.player_index < g_sealed_deck_tournament.player_count;
         ++s.player_index)
      g_sealed_deck_tournament.players[s.player_index].deck_built = 0;
    g_sealed_deck_tournament.deck_builder_thread = (HANDLE)_beginthreadex(
        NULL, 0, shell_build_sealed_opponent_decks,
        &g_sealed_deck_tournament, 0, &s.thread_id);
    SetThreadPriority(g_sealed_deck_tournament.deck_builder_thread, -1);
    shell_show_sealed_open_packs(global_main_hwnd,
                                 g_sealed_deck_tournament.players[0].packs,
                                 g_sealed_deck_tournament.players[0].pack_count);
  }
  shell_show_sealed_ladder(global_main_hwnd, &g_sealed_deck_tournament);
  for (s.player_index = 0;
       s.player_index < g_sealed_deck_tournament.player_count; ++s.player_index)
  {
    s.player = &g_sealed_deck_tournament.players[s.player_index];
    if (s.player->art != NULL)
    {
      delete_and_close_object(s.player->art);
      s.player->art = NULL;
    }
  }
  DeleteCriticalSection(&g_sealed_deck_tournament.critical_section);
  /* The original returns this stack slot without initializing it. */
  return s.result;
}
