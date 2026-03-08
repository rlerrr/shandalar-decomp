#ifndef DECKDLL_H	// -*- tab-width:8; c-basic-offset:2; -*-
#define DECKDLL_H 1
// DeckDll: standalone and integrated deckbuilder.

#include <string.h>
#include <math.h>
#include <stdio.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "../defs.h"
#include "resources.h"

typedef enum
{
  DBFLAGS_0 = 0,
  DBFLAGS_SHANDALAR = 1,
  DBFLAGS_STANDALONE = 2,
  DBFLAGS_NOCARDCOUNTCHECK = 4,
  DBFLAGS_GAUNTLET = 8,
  DBFLAGS_EDITDECK = 0x40,
} DBFlags;

typedef enum 
{
  FE_0 = 0,
  FE_4TH_EDITION = 0x2,
  FE_REVISED = 0x20,
  FE_UNLIMITED = 0x1000,
  FE_EXPANSIONLIST = 0x8000,
} FilterExpansions;

typedef enum
{ // Using color_test_t would make too much sense.
  FC_0 = 0,
  FC_WHITE = 0x0002,
  FC_GREEN = 0x0004,
  FC_RED = 0x0008,
  FC_BLACK = 0x0010,
  FC_BLUE = 0x0020,
  FC_GOLD = 0x0040,
  FC_GOLD_ALL = 0x0100,
  FC_GOLD_ALLSELECTED = 0x0200,
  FC_GOLD_ANYSELECTED = 0x0400,
} FilterColors;

typedef enum
{
  FN_0 = 0,
  FN_ENABLE = 0x1,
  FN_GT = 0x2,
  FN_LT = 0x4,
  FN_EQ = 0x8,
  FN_CC_X = 0x10,
} FilterNum;

typedef enum
{
  FR_0 = 0,
  FR_ENABLE = 0x1,
  FR_COMMON = 0x2,
  FR_UNCOMMON = 0x4,
  FR_RARE = 0x8,
  FR_RESTRICTED = 0x10,
  FR_BANNED = 0x20,
} FilterRarities;

typedef enum
{
  FS_0 = 0,
  FS_4TH_EDITION = 0x0002,
  FS_ASTRAL = 0x0004,
  FS_ARABIAN_NIGHTS = 0x0008,
  FS_ANTIQUITIES = 0x0010,
  FS_THE_DARK = 0x0040,
  FS_LEGENDS = 0x0100,
  FS_OTHER = 0x1000,
} FilterSets;

typedef enum
{ // Seems to be FilterSets rightshifted by one.  How inconvenient.  FSCF_Q_* values are tentative.
  FCSF_0 = 0,
  FCSF_Q_ENABLE = 0x0001,
  FCSF_Q_ASTRAL = 0x0002,
  FCSF_ARABIAN_NIGHTS = 0x0004,
  FCSF_ANTIQUITIES = 0x0008,
  FCSF_THE_DARK = 0x0010,
  FCSF_LEGENDS = 0x0040,
  FCSF_Q_OTHER = 0x0400,
} FilterCardSetsFlags;

typedef enum
{
  FT_0 = 0,
  FT_LAND = 0x00000001,
  FT_LAND_LAND_AND_MANA = 0x00000002,
  FT_LAND_LAND_ONLY = 0x00000004,
  FT_LAND_MANA_ONLY = 0x00000008,
  FT_ARTIFACT = 0x00000010,
  FT_ARTIFACT_CREATURE = 0x00000020,
  FT_ARTIFACT_NON_CREATURE = 0x00000040,
  FT_CREATURE = 0x00000080,
  FT_CREATURE_CREATURE = 0x00000100,
  FT_CREATURE_TOKEN = 0x00000200,
  FT_CREATURE_ARTIFACT = 0x00000400,
  FT_CREATURE_LIST = 0x00000800,
  FT_ENCHANTMENT = 0x00001000,
  FT_ENCHANTMENT_ENCHANTMENTS = 0x00002000,
  FT_ENCHANTMENT_WORLD = 0x00004000,
  FT_ENCHANTMENT_LAND = 0x00008000,
  FT_ENCHANTMENT_CREATURE = 0x00010000,
  FT_ENCHANTMENT_ARTIFACT = 0x00020000,
  FT_ENCHANTMENT_ENCHANT = 0x00040000,
  FT_ENCHANTMENT_PERMANENT = 0x00080000,
  FT_ENCHANTMENT_PLAYER = 0x00100000,
  FT_ENCHANTMENT_INSTANT = 0x00200000,
  FT_INSTANT = 0x00400000,
  FT_INTERRUPT = 0x00800000,
  FT_SORCERY = 0x01000000,
} FilterTypes;

typedef enum
{
  FA_0 = 0,
  FA_ENABLE = 0x1,
  FA_NATIVE = 0x2,
  FA_GRANTS = 0x4,

  FA_FLYING = 0x8,
  FA_FIRSTSTRIKE = 0x10, // and double strike
  FA_TRAMPLE = 0x20,
  FA_REGENERATION = 0x40,
  FA_BANDING = 0x80,
  FA_PROTECTION = 0x100,
  FA_LANDWALK = 0x200,
  FA_INFECT = 0x400,
  FA_RAMPAGE = 0x800,
  FA_REACH = 0x1000,
  FA_DEATHTOUCH = 0x2000,
  FA_VIGILANCE = 0x4000,
  FA_HASTE = 0x8000,
} FilterAbilities;

enum Abilities
{
  ABIL_NATIVE_BANDING = 1,
  ABIL_NATIVE_DESERTWALK = 2,
  ABIL_NATIVE_FIRSTSTRIKE = 3, // or double strike
  ABIL_NATIVE_FLYING = 4,
  ABIL_NATIVE_FORESTWALK = 5,
  ABIL_NATIVE_VIGILANCE = 6,
  ABIL_NATIVE_ISLANDWALK = 7,
  ABIL_NATIVE_LEGENDARY_LANDWALK = 8,
  ABIL_NATIVE_MOUNTAINWALK = 9,
  ABIL_NATIVE_PLAINSWALK = 10,
  ABIL_NATIVE_INFECT = 11,
  ABIL_NATIVE_PROTECTION_FROM_BLACK = 12,
  ABIL_NATIVE_PROTECTION_FROM_RED = 13,
  ABIL_NATIVE_PROTECTION_FROM_WHITE = 14,
  ABIL_NATIVE_HASTE = 15,
  ABIL_NATIVE_RAMPAGE = 16,
  ABIL_NATIVE_REGENERATION = 17,
  ABIL_NATIVE_DEATHTOUCH = 18,
  ABIL_NATIVE_SWAMPWALK = 19,
  ABIL_NATIVE_TRAMPLE = 20,
  ABIL_NATIVE_REACH = 21,
  ABIL_GRANTS_BANDING = 22,
  ABIL_GRANTS_FIRSTSTRIKE = 23, // or double strike
  ABIL_GRANTS_FLYING = 24,
  ABIL_GRANTS_FORESTWALK = 25,
  ABIL_GRANTS_VIGILANCE = 26,
  ABIL_GRANTS_ISLANDWALK = 27,
  ABIL_GRANTS_MOUNTAINWALK = 28,
  ABIL_GRANTS_PLAINSWALK = 29,
  ABIL_GRANTS_PROTECTION_FROM_ARTIFACTS = 30,
  ABIL_GRANTS_PROTECTION_FROM_BLACK = 31,
  ABIL_GRANTS_PROTECTION_FROM_BLUE = 32,
  ABIL_GRANTS_PROTECTION_FROM_GREEN = 33,
  ABIL_GRANTS_PROTECTION_FROM_RED = 34,
  ABIL_GRANTS_PROTECTION_FROM_WHITE = 35,
  ABIL_GRANTS_HASTE = 36,
  ABIL_GRANTS_RAMPAGE = 37,
  ABIL_GRANTS_REGENERATION = 38,
  ABIL_GRANTS_DEATHTOUCH = 39,
  ABIL_GRANTS_SWAMPWALK = 40,
  ABIL_GRANTS_TRAMPLE = 41,
  ABIL_GRANTS_REACH = 42,
};

enum OrigRaritySet
{
  SET_INVALID = 0xFF,
  SET_ANTIQUITIES = 0,
  SET_ARABIAN_NIGHTS = 1,
  SET_ASTRAL = 2,
  SET_THE_DARK = 3,
  SET_LEGENDS = 4,
  SET_PROMO = 5,
  SET_UNLIMITED = 6,
  SET_8TH_EDITION = 7,
  SET_FALLEN_EMPIRES = 8,
  SET_TEMPEST = 9,
  SET_RANDOM = 10
};

typedef struct OrigRarities_t
{
  uint8_t set;
  char rarity;
  char exp_rarities[4];
} OrigRarities;

enum Pack1
{ // Using color_t would make too much sense.
  PACK1_WHITE = 0,
  PACK1_BLUE = 1,
  PACK1_BLACK = 2,
  PACK1_RED = 3,
  PACK1_GREEN = 4,
  PACK1_OTHER = 5,
  PACK1_MAX = PACK1_OTHER,
};

enum Pack2
{
  PACK2_LAND = 0,
  PACK2_CREATURE = 1,
  PACK2_ENCHANTMENT = 2,
  PACK2_SORCERY = 3,
  PACK2_INTERRUPT = 4,
  PACK2_INSTANT = 5,
  PACK2_MAX = PACK2_INSTANT,
};

typedef struct Table_t
{
  unsigned short csvid;
  unsigned short amt;
} Table;

typedef struct Packs_t
{
  int num;
  Table table[300];
} Packs;

typedef struct GlobalDeckEntry_t
{
  csvid_t GDE_csvid;
  iid_t GDE_iid;
  int GDE_Available;
  int GDE_DecksBits;
} GlobalDeckEntry;

typedef struct DeckEntry_t
{
  csvid_t DeckEntry_csvid;
  int DeckEntry_Amount;
  const char* DeckEntry_FullName;
} DeckEntry;

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#define FREEZ(memory) \
  do                  \
  {                   \
    if (memory)       \
    {                 \
      free(memory);   \
      memory = NULL;  \
    }                 \
  } while (0)

#define DELETE_IMPL(fn, obj) \
  do                         \
  {                          \
    if (obj)                 \
    {                        \
      fn(obj);               \
      obj = NULL;            \
    }                        \
  } while (0)

#define DELETE_DC(obj) DELETE_IMPL(DeleteDC, obj)
#define DELETE_OBJ(obj) DELETE_IMPL(DeleteObject, obj)
#define DESTROY_MENU(obj) DELETE_IMPL(DestroyMenu, obj)

#define CHECKMENU_IF(menu, cmd, val) CheckMenuItem((menu), (cmd), MF_BYCOMMAND | ((val) ? MF_CHECKED : MF_UNCHECKED))

// only used to choose which background music to play, so that's ok.  Evaluates lo twice.
#define RANDRANGE(lo, hi) ((rand() / (RAND_MAX + 1.0)) * ((hi) - (lo) + 1) + (lo))

#define RECT_WIDTH(which) ((which).right - (which).left)
#define RECT_HEIGHT(which) ((which).bottom - (which).top)

#ifdef __cplusplus
extern "C" {
#endif
  // Our exports
  BOOL WINAPI DllEntryPoint(HINSTANCE dll, DWORD reason, LPVOID reserved);
  BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved);
  WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
  WPARAM deckbuilder_main(HWND parent_hwnd, int db_flags_1, int db_flags_2);
  void Deckdll_initialize_for_shandalar(const card_ptr_t* i_raw_cards_ptr,
					int i_available_slots,
					char* i_card_coded,
					int (*check_card_count_fn)(const DeckEntry*, int, int),
					int (*is_valid_card_fn)(int),
					BOOL (*colors_match_fn)(iid_t, color_test_t),
					int (*check_colors_inout_edited_deck_fn)(const GlobalDeckEntry*, int, BOOL));

#ifdef __cplusplus
};
#endif

#endif
