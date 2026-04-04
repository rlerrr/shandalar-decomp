#ifndef FULL_DECK_H
#define FULL_DECK_H

#include "deckdll.h"

typedef struct DeckBucket_t
{
  DeckEntry entries[10];
  int total;
} DeckBucket;

typedef struct FullDeck_t
{
  DeckEntry entries[300];
  int cards;
  int total;
  DeckBucket sideboard[6];
  DeckBucket trade[7];
} FullDeck;

#endif
