#ifndef MAGIC_SEALED_TOURNAMENT_H
#define MAGIC_SEALED_TOURNAMENT_H

#include <windows.h>
#include <stddef.h>
#include "magic_sealed_deck.h"

typedef struct sealed_deck_player_t
{
  char name[200];
  char description[300];
  char face_path[264];
  HGDIOBJ art;
  int difficulty;
  int reserved_0304;
  sealed_deck_pack_t packs[6];
  int pack_count;
  int card_pool[300];
  int card_pool_count;
  int deck[300];
  int deck_count;
  int deck_built;
} sealed_deck_player_t;

typedef struct sealed_deck_tournament_t
{
  sealed_deck_player_t players[64];
  int player_count;
  int rounds;
  int ante;
  int best_of;
  int starter_types[2];
  int starter_count;
  int booster_types[4];
  int booster_count;
  int free_lands;
  int minimum_deck_size;
  int current_round;
  int bracket[255];
  int decks_changed;
  CRITICAL_SECTION critical_section;
  HANDLE opponent_match_thread;
  HANDLE deck_builder_thread;
  int duel_in_progress;
  int opponents_in_progress;
} sealed_deck_tournament_t;

STATIC_ASSERT(sizeof(sealed_deck_player_t) == 0x12a8,
              sealed_deck_player_wrong_size);
STATIC_ASSERT(sizeof(sealed_deck_tournament_t) == 0x4ae64,
              sealed_deck_tournament_wrong_size);
STATIC_ASSERT(offsetof(sealed_deck_player_t, art) == 0x2fc,
              sealed_deck_player_art_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_player_t, packs) == 0x308,
              sealed_deck_player_packs_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_player_t, card_pool) == 0x93c,
              sealed_deck_player_card_pool_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_player_t, deck) == 0xdf0,
              sealed_deck_player_deck_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_tournament_t, player_count) == 0x4aa00,
              sealed_deck_tournament_player_count_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_tournament_t, bracket) == 0x4aa3c,
              sealed_deck_tournament_bracket_wrong_offset);
STATIC_ASSERT(offsetof(sealed_deck_tournament_t, critical_section) == 0x4ae3c,
              sealed_deck_tournament_critical_section_wrong_offset);

extern sealed_deck_tournament_t g_sealed_deck_tournament;
extern int g_sealed_deck_extra_card_limit;
int shell_run_sealed_deck_duel(int resume_saved_game);
void shell_edit_sealed_deck(HWND owner, sealed_deck_player_t *player);

#endif
