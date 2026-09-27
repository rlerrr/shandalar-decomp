#ifndef MAGIC_SEALED_DUEL_H
#define MAGIC_SEALED_DUEL_H

#include "magic_sealed_tournament.h"

int shell_run_sealed_player_match(int resume, HWND owner, sealed_deck_player_t *player,
    sealed_deck_player_t *opponent, int ante, int minimum_deck_size);

#endif
