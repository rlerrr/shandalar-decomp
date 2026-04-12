#ifndef __GLOBAL_STATE_H__
#define __GLOBAL_STATE_H__

#include "defs.h"

// Single header library style
#ifdef GLOBAL_STATE_IMPL
#define GLOBAL_STATE_GLOBAL
#else
#define GLOBAL_STATE_GLOBAL extern
#endif

// See manalink.lds and ai.c for more some of these addresses and types
// A lot of these can potentially be correlated through save_or_load_ver1 and save_or_load_ver2

// GLOBAL: MAGIC 0x8b60f0
// GLOBAL: SHANDALAR 0x8ca270
GLOBAL_STATE_GLOBAL card_instance_t global_card_instances[2][151];

// GLOBAL: MAGIC 0x939810
// GLOBAL: SHANDALAR 0x94d930
GLOBAL_STATE_GLOBAL int global_graveyard_slots[2][500];

// TODO: is this actually int???
// GLOBAL: MAGIC 0x8cecc0
GLOBAL_STATE_GLOBAL char global_ante_cards[2][16];

typedef struct
{
  int csvid;
  int numcards;
} csvid_and_numcards;

// GLOBAL: MAGIC 0x0056fb20
GLOBAL_STATE_GLOBAL csvid_and_numcards initial_library[2][200];

// GLOBAL: MAGIC 0x008cc980
GLOBAL_STATE_GLOBAL int global_exile[2][500];

// GLOBAL: MAGIC 0x00895210
GLOBAL_STATE_GLOBAL int global_library[2][500];

// GLOBAL: MOK 0x507ec0
// GLOBAL: MAGIC 0x0093a990
GLOBAL_STATE_GLOBAL int TENTATIVE_timestamp_player[500];

// GLOBAL: MOK 0x5076f0
// GLOBAL: MAGIC 0x8b5910
GLOBAL_STATE_GLOBAL int TENTATIVE_timestamp_card[500];

// GLOBAL: MOK 0x5096b0
// GLOBAL: MAGIC 0x9251e0
GLOBAL_STATE_GLOBAL target_t global_stack_cards[32];

// GLOBAL: MOK 0x5097b0
// GLOBAL: MAGIC 0x926810
GLOBAL_STATE_GLOBAL target_t global_stack_damage_targets[32];

#undef GLOBAL_STATE_GLOBAL
#endif