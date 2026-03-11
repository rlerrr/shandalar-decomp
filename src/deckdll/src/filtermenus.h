#include "defs.h"
#include "deckdll.h"

#define CREATURE_LIST_SIZE 7
// 1 bit per creature type, so 10 means a maximum of 320.  The highest used is currently 0xEA, for SUBTYPE_MOLE.
//STATIC_ASSERT((CREATURE_LIST_SIZE * 32) > SUBTYPE_MAX_USED_CREATURE_SUBTYPE, Too_Many_Creature_Types);

#define EXPANSION_LIST_SIZE 8

// And for both, they're limited by constants in dlgproc_FilterSubtype() to 320 entries.
#define MAX_FILTER_SUBTYPE_SIZE 320
#if ((CREATURE_LIST_SIZE * 32) > MAX_FILTER_SUBTYPE_SIZE)
#error "sizeof global_filters.creature_list > MAX_FILTER_SUBTYPE_SIZE"
#endif
#if ((EXPANSION_LIST_SIZE * 32) > MAX_FILTER_SUBTYPE_SIZE)
#error "sizeof global_filters.expansion_list > MAX_FILTER_SUBTYPE_SIZE"
#endif




struct global_filters_t {
  // : DECKDLL 0x101a9190
  short expansions;
  // : DECKDLL 0x101a9192
  short colors;
  // : DECKDLL 0x101a9194
  short cardsets;
  // : DECKDLL 0x101a9198
  FilterTypes cardtypes;
  // : DECKDLL 0x101a919c
  uint32_t creature_list[CREATURE_LIST_SIZE];
  // : DECKDLL 0x101a91b8
  char casting_cost;
  // : DECKDLL 0x101a91ba
  short casting_cost_value;
  // : DECKDLL 0x101a91bc
  char power;
  // : DECKDLL 0x101a91be
  short power_value;
  // : DECKDLL 0x101a91c0
  char toughness;
  // : DECKDLL 0x101a91c2
  short toughness_value;
  // : DECKDLL 0x101a91c4
  short abilities;
  // : DECKDLL 0x101a91c6
  char rarity;
  // : DECKDLL 0x101a91c7
  char artist;
  // : DECKDLL 0x101a91c8
  // 1 bit per expansion, so 8 means a maximum of 256.  We currently have 159, including 8 "Format" expansions at the start and 8 "Future Expansion" at the end.
  uint32_t expansion_list[EXPANSION_LIST_SIZE];
};
//STATIC_ASSERT(sizeof(global_filters_t) == 56, global_filters_t_bad_size);