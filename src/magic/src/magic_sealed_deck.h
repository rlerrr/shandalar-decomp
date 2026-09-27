#ifndef MAGIC_SEALED_DECK_H
#define MAGIC_SEALED_DECK_H

#include "defs.h"

typedef struct sealed_deck_pack_definition_t
{
  char name[0x34];
  int starter_count;
  int booster_count;
  int free_lands;
  int rounds;
  int color_count;
} sealed_deck_pack_definition_t;

typedef struct sealed_deck_pack_t
{
  int is_starter;
  int pack_type;
  int variation;
  int opened;
  int cards[60];
  int card_count;
  int revealed_card_index;
} sealed_deck_pack_t;

STATIC_ASSERT(sizeof(sealed_deck_pack_definition_t) == 0x48,
              sealed_deck_pack_definition_wrong_size);
STATIC_ASSERT(sizeof(sealed_deck_pack_t) == 0x108,
              sealed_deck_pack_wrong_size);

extern sealed_deck_pack_definition_t g_sealed_pack_definitions[9];

void shell_generate_sealed_packs(sealed_deck_pack_t *packs, int *cards,
                                int *card_count, int starter_count,
                                int *starter_types, int booster_count,
                                int *booster_types);

#endif
