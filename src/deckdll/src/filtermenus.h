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
  short expansions;
  short colors;
  short cardsets;
  FilterTypes cardtypes;
  uint32_t creature_list[CREATURE_LIST_SIZE];
  char casting_cost;
  short casting_cost_value;
  char power;
  short power_value;
  char toughness;
  short toughness_value;
  short abilities;
  char rarity;
  char artist;
  uint64_t artists_list;
};
STATIC_ASSERT(sizeof(struct global_filters_t) == 64, global_filters_t_bad_size);