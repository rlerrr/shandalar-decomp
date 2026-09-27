#ifndef MAGIC_SEALED_DUEL_H
#define MAGIC_SEALED_DUEL_H

#include "magic_sealed_tournament.h"

extern int g_sealed_match_minimum_deck_size;
extern int g_sealed_match_maximum_deck_size;
extern sealed_deck_player_t *g_sealed_match_opponent;
unsigned int __stdcall shell_build_sealed_match_opponent(sealed_deck_player_t *opponent);

int shell_run_sealed_player_match(int resume, HWND owner, sealed_deck_player_t *player,
    sealed_deck_player_t *opponent, int ante, int minimum_deck_size);

#endif
