#include "defs.h"
#include "mystdbool.h"
#include "deckdll.h"

extern HWND global_main_hwnd;
extern bool global_cfg_view_all;
extern char *card_coded;

extern int global_deck_num_entries;
extern GlobalDeckEntry global_deck[];
extern int global_available_slots;
extern card_ptr_t *global_raw_cards_ptr;
extern const char *const read_db_artist_names[];
extern DBFlags global_db_flags_1;

extern OrigRarities global_origrarities[];

extern FilterAbilities global_filter_abilities;
extern FilterSets global_filter_cardsets;
extern FilterTypes global_filter_cardtypes;
extern FilterNum global_filter_casting_cost;
extern int global_filter_casting_cost_value;
extern FilterNum global_filter_power;
extern int global_filter_power_value;
extern FilterNum global_filter_toughness;
extern int global_filter_toughness_value;
extern FilterRarities global_filter_rarity;
extern FilterColors global_filter_colors;
extern FilterExpansions global_filter_expansions;

extern int global_num_expansions;
extern int global_expansion_size;
extern uint32_t global_filter_expansion_list[];
extern uint32_t global_filter_creature_list[];

extern char *global_raw_rarities;

extern int (*global_is_valid_card_fn)(int);

enum Restriction
{
  RST_0 = 0x0,
  RST_ANTE = 0x1,
  RST_RESTRICTED = 0x2,
  RST_BANNED = 0x4,
};

typedef struct Restriction_t
{
  csvid_t csvid;
  int restriction;
} Restriction;

// GLOBAL: DECKDLL 0x10033278
const Restriction restrictions[] =
    {
        {CARD_ID_AMULET_OF_QUOZ, RST_ANTE | RST_BANNED},
        {CARD_ID_BRONZE_TABLET, RST_ANTE | RST_BANNED},
        {CARD_ID_CONTRACT_FROM_BELOW, RST_ANTE | RST_BANNED},
        {CARD_ID_DARKPACT, RST_ANTE | RST_BANNED},
        {CARD_ID_DEMONIC_ATTORNEY, RST_ANTE | RST_BANNED},
        {CARD_ID_JEWELED_BIRD, RST_ANTE | RST_BANNED},
        {CARD_ID_REBIRTH, RST_ANTE | RST_BANNED},
        {CARD_ID_TEMPEST_EFREET, RST_ANTE | RST_BANNED},
        {CARD_ID_TIMMERIAN_FIENDS, RST_ANTE | RST_BANNED},

        {CARD_ID_ANCESTRAL_RECALL, RST_RESTRICTED},
        {CARD_ID_BALANCE, RST_RESTRICTED},
        {CARD_ID_BLACK_LOTUS, RST_RESTRICTED},
        {CARD_ID_BRAINSTORM, RST_RESTRICTED},
        {CARD_ID_CHALICE_OF_THE_VOID, RST_RESTRICTED},
        {CARD_ID_CHANNEL, RST_RESTRICTED},
        {CARD_ID_DEMONIC_CONSULTATION, RST_RESTRICTED},
        {CARD_ID_DEMONIC_TUTOR, RST_RESTRICTED},
        {CARD_ID_DIG_THROUGH_TIME, RST_RESTRICTED},
        {CARD_ID_FASTBOND, RST_RESTRICTED},
        {CARD_ID_FLASH, RST_RESTRICTED},
        {CARD_ID_IMPERIAL_SEAL, RST_RESTRICTED},
        {CARD_ID_LIBRARY_OF_ALEXANDRIA, RST_RESTRICTED},
        {CARD_ID_LIONS_EYE_DIAMOND, RST_RESTRICTED},
        {CARD_ID_LODESTONE_GOLEM, RST_RESTRICTED},
        {CARD_ID_LOTUS_PETAL, RST_RESTRICTED},
        {CARD_ID_MANA_CRYPT, RST_RESTRICTED},
        {CARD_ID_MANA_VAULT, RST_RESTRICTED},
        {CARD_ID_MEMORY_JAR, RST_RESTRICTED},
        {CARD_ID_MERCHANT_SCROLL, RST_RESTRICTED},
        {CARD_ID_MINDS_DESIRE, RST_RESTRICTED},
        {CARD_ID_MOX_EMERALD, RST_RESTRICTED},
        {CARD_ID_MOX_JET, RST_RESTRICTED},
        {CARD_ID_MOX_PEARL, RST_RESTRICTED},
        {CARD_ID_MOX_RUBY, RST_RESTRICTED},
        {CARD_ID_MOX_SAPPHIRE, RST_RESTRICTED},
        {CARD_ID_MYSTICAL_TUTOR, RST_RESTRICTED},
        {CARD_ID_NECROPOTENCE, RST_RESTRICTED},
        {CARD_ID_PONDER, RST_RESTRICTED},
        {CARD_ID_SOL_RING, RST_RESTRICTED},
        {CARD_ID_STRIP_MINE, RST_RESTRICTED},
        {CARD_ID_TIME_VAULT, RST_RESTRICTED},
        {CARD_ID_TIME_WALK, RST_RESTRICTED},
        {CARD_ID_TIMETWISTER, RST_RESTRICTED},
        {CARD_ID_TINKER, RST_RESTRICTED},
        {CARD_ID_TOLARIAN_ACADEMY, RST_RESTRICTED},
        {CARD_ID_TREASURE_CRUISE, RST_RESTRICTED},
        {CARD_ID_TRINISPHERE, RST_RESTRICTED},
        {CARD_ID_VAMPIRIC_TUTOR, RST_RESTRICTED},
        {CARD_ID_WHEEL_OF_FORTUNE, RST_RESTRICTED},
        {CARD_ID_WINDFALL, RST_RESTRICTED},
        {CARD_ID_YAWGMOTHS_BARGAIN, RST_RESTRICTED},
        {CARD_ID_YAWGMOTHS_WILL, RST_RESTRICTED},

        {CARD_ID_CHAOS_ORB, RST_BANNED},
        {CARD_ID_FALLING_STAR, RST_BANNED},
        {CARD_ID_SHAHRAZAD, RST_BANNED},

        {CARD_ID_CONSPIRACY_ADVANTAGEOUS_PROCLAMATION, RST_BANNED},
        {CARD_ID_CONSPIRACY_BACKUP_PLAN, RST_BANNED},
        {CARD_ID_CONSPIRACY_BRAGOS_FAVOR, RST_BANNED},
        {CARD_ID_CONSPIRACY_DOUBLE_STROKE, RST_BANNED},
        {CARD_ID_CONSPIRACY_IMMEDIATE_ACTION, RST_BANNED},
        {CARD_ID_CONSPIRACY_ITERATIVE_ANALYSIS, RST_BANNED},
        {CARD_ID_CONSPIRACY_MUZZIOS_PREPARATIONS, RST_BANNED},
        {CARD_ID_CONSPIRACY_POWER_PLAY, RST_BANNED},
        {CARD_ID_CONSPIRACY_SECRETS_OF_PARADISE, RST_BANNED},
        {CARD_ID_CONSPIRACY_SECRET_SUMMONING, RST_BANNED},
        {CARD_ID_CONSPIRACY_SENTINEL_DISPATCH, RST_BANNED},
        {CARD_ID_CONSPIRACY_UNEXPECTED_POTENTIAL, RST_BANNED},
        {CARD_ID_CONSPIRACY_WORLDKNIT, RST_BANNED},

        {CARD_ID_AIR_ELEMENTAL, RST_0},
};

static bool __inline
check_restriction_impl(csvid_t csvid, int rst)
{
  int i;

  for (i = 0; restrictions[i].restriction != RST_0; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & rst))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x1001013d
bool check_restricted(csvid_t csvid)
{
  return check_restriction_impl(csvid, RST_RESTRICTED);
}

// FUNCTION: DECKDLL 0x1001019e
bool check_banned(csvid_t csvid)
{
  return check_restriction_impl(csvid, RST_BANNED);
}

// FUNCTION: DECKDLL 0x100101ff
bool check_ante(csvid_t csvid)
{
  return check_restriction_impl(csvid, RST_ANTE);
}

// FUNCTION: DECKDLL 0x100100e6
bool check_basic(csvid_t csvid)
{
  switch (csvid)
  {
  case CARD_ID_SWAMP:
  case CARD_ID_ISLAND:
  case CARD_ID_FOREST:
  case CARD_ID_MOUNTAIN:
  case CARD_ID_PLAINS:
  case CARD_ID_SNOW_COVERED_SWAMP:
  case CARD_ID_SNOW_COVERED_ISLAND:
  case CARD_ID_SNOW_COVERED_FOREST:
  case CARD_ID_SNOW_COVERED_MOUNTAIN:
  case CARD_ID_SNOW_COVERED_PLAINS:
    // case CARD_ID_WASTES:

    // not basic lands, but still basic
  case CARD_ID_RELENTLESS_RATS:
  case CARD_ID_SHADOWBORN_APOSTLE:
    return true;

  default:
    return false;
  }
}


// FUNCTION: DECKDLL 0x1000f719
static bool
check_card_global_deck_availability(csvid_t csvid)
{
  int i;
  for (i = 0; i < global_deck_num_entries; ++i)
    if (global_deck[i].GDE_csvid == csvid && global_deck[i].GDE_Available == 1)
      return true;

  return false;
}

static bool
check_expansion_list_filter(csvid_t csvid)
{
  int expid;
  for (expid = 0; expid < global_num_expansions; ++expid)
    if ((1 << (expid & 0x1F)) & global_filter_expansion_list[expid / 32])
    {
      int bit_pos = expid * 3;
      int bit_mask = 7 << (bit_pos & 7);
      int offset = (csvid * global_expansion_size) + (bit_pos / 8);
      if ((MAKEWORD(global_raw_rarities[offset], global_raw_rarities[offset + 1]) & bit_mask) >> (bit_pos & 0x7))
        return true;
    }

  return false;
}

static bool
is_valid_card(csvid_t csvid)
{
  if (global_is_valid_card_fn)
    return global_is_valid_card_fn(csvid);
  else
    return (csvid < global_available_slots && (global_cfg_view_all || (card_coded[csvid / 8] & (1 << (csvid % 8)))));
}

static int
FUN_1000e20a(unsigned int *param_1)
{
  *param_1 = 0xFFFFFFFF;
  return 0;
}

// FUNCTION: DECKDLL 0x100271e3
unsigned int
FUN_100271e3(unsigned char param_1)
{
  unsigned int local_8;
  unsigned int local_4;

  local_4 = 0;
  if (param_1 & 1)
    return 1;

  local_8 = 0;
  if (FUN_1000e20a(&local_8) != 0)
    return 0;

  if ((param_1 & 1) && (local_8 & 0x400000))
    local_4 = 1;
  else if ((param_1 & 2) && (local_8 & 0x100))
    local_4 = 1;
  else if ((param_1 & 4) && (local_8 & 0x800))
    local_4 = 1;
  else if ((param_1 & 8) && (local_8 & 0x20000))
    local_4 = 1;
  else if ((param_1 & 0x10) && (local_8 & 0x10000000))
    local_4 = 1;
  else
    local_4 = 0;

  return local_4;
}

// FUNCTION: DECKDLL 0x1002709f
static int
FUN_1002709f(unsigned int param_1, int param_2)
{
  int local_4;

  local_4 = 1;
  if (param_2 == 1)
  {
    if (FUN_100271e3(2) == 0)
      local_4 = 0;
  }
  if (param_2 == 2)
  {
    if (FUN_100271e3(4) == 0)
      local_4 = 0;
  }
  if (((((int)param_1 < 0) || (0x3FF < (int)param_1)) || (param_2 < 0)) || (2 < param_2))
    local_4 = 0;

  if ((*(unsigned int *)(card_coded + param_2 * 0x80 + ((int)(param_1 & 0xFFFFFFE0) >> 3)) &
       (1 << ((unsigned char)param_1 & 0x1F))) == 0)
    local_4 = 0;

  return local_4;
}

// FUNCTION: DECKDLL 0x10010c57
static bool
check_set_availability(csvid_t csvid, unsigned char allowed_sets)
{
  int set;
  bool bit20a;
  int bit80;
  int bit40;
  int bit20b;
  int bit10;
  int bit08;
  int bit02;
  int bit01;
  int bit04;

  bit80 = 0;
  bit40 = 0;
  bit20b = 0;
  bit20a = false;
  bit10 = 0;
  bit08 = 0;
  bit02 = 0;
  bit01 = 0;
  bit04 = 0;

  if (FUN_1002709f(csvid, 0) == 0 && FUN_1002709f(csvid, 1) == 0 && FUN_1002709f(csvid, 2) == 0)
    return false;

  set = global_origrarities[csvid].set;
  if (set == -1)
    return false;

  if ((allowed_sets & 1) != 0)
    bit01 = (global_origrarities[csvid].exp_rarities[1] != '-') ? 1 : 0;

  if ((allowed_sets & 4) != 0)
    bit04 = (global_origrarities[csvid].exp_rarities[0] != '-') ? 1 : 0;

  if ((allowed_sets & 2) != 0)
    bit02 = (set == SET_UNLIMITED) ? 1 : 0;

  if ((allowed_sets & 8) != 0)
    bit08 = (set == SET_ARABIAN_NIGHTS) ? 1 : 0;

  if ((allowed_sets & 0x10) != 0)
    bit10 = (set == SET_ANTIQUITIES) ? 1 : 0;

  if ((allowed_sets & 0x20) != 0)
  {
    bit20a = (set == SET_ASTRAL);
    bit20b = (set == SET_PROMO) ? 1 : 0;
  }

  if ((allowed_sets & 0x40) != 0)
    bit40 = (set == SET_LEGENDS) ? 1 : 0;

  if ((allowed_sets & 0x80) != 0)
    bit80 = (set == SET_THE_DARK) ? 1 : 0;

  return (bit80 | bit40 | bit20b | bit20a | bit10 | bit08 | bit02 | bit01 | bit04) != 0;
}

// FUNCTION: DECKDLL 0x1000fa34
static bool
check_lands(int cardtype, int db_card_type_2)
{
  int rval;

  rval = 0;
  if (((unsigned char)global_filter_cardtypes & FT_LAND) == 0)
    return rval;

  if (((unsigned char)global_filter_cardtypes & FT_LAND_LAND_AND_MANA) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 == 10)
    rval = 1;
  if (((unsigned char)global_filter_cardtypes & FT_LAND_LAND_ONLY) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 != 10)
    rval = 1;
  if (((unsigned char)global_filter_cardtypes & FT_LAND_MANA_ONLY) != 0 && cardtype != CP_TYPE_LAND && db_card_type_2 == 10)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000fade
static bool
check_artifacts(int cardtype, int subtype1)
{
  int rval;

  rval = 0;
  if (cardtype == CP_TYPE_ARTIFACT && ((unsigned char)global_filter_cardtypes & FT_ARTIFACT) != 0)
  {
    if (((unsigned char)global_filter_cardtypes & FT_ARTIFACT_CREATURE) != 0 &&
        subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
      rval = 1;

    if (((unsigned char)global_filter_cardtypes & FT_ARTIFACT_NON_CREATURE) != 0 &&
        subtype1 != HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
      rval = 1;
  }
  else
    rval = 0;

  return rval;
}

// FUNCTION: DECKDLL 0x10010bcf
static bool
check_creature_list_filter(int subtype1)
{
  int local_c;
  int local_8;

  local_c = 0;
  do
  {
    if (6 < local_c)
      return false;

    for (local_8 = 0; local_8 < 0x20; ++local_8)
      if ((global_filter_creature_list[local_c] & (1 << ((unsigned char)local_8 & 0x1f))) != 0 &&
          ((local_c * 0x20) + local_8 + 1) == subtype1)
        return true;

    local_c = local_c + 1;
  } while (1);

  return false;
}

// FUNCTION: DECKDLL 0x1000fb56
static bool
check_creatures(int cardtype, int subtype1)
{
  int rval;
  unsigned char *f;

  rval = 0;
  f = (unsigned char *)&global_filter_cardtypes;

  if (((unsigned char)global_filter_cardtypes & FT_CREATURE) == 0)
    return rval;

  if ((f[1] & 1) != 0 && cardtype == CP_TYPE_CREATURE)
    rval = 1;

  if ((f[1] & 2) != 0 && cardtype == CP_TYPE_TOKEN)
    rval = 1;

  if ((f[1] & 4) != 0 &&
      cardtype == CP_TYPE_ARTIFACT &&
      subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    rval = 1;

  if ((f[1] & 8) != 0 &&
      check_creature_list_filter(subtype1))
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000fc14
static bool
check_enchantments(int cardtype, int subtype1)
{
  int rval = 0;
  unsigned char *f = (unsigned char *)&global_filter_cardtypes;

  if ((f[1] & 0x10) == 0 || cardtype != CP_TYPE_ENCHANTMENT)
    return rval;

  if ((f[1] & 0x20) && subtype1 == 0xd3)
    rval = 1;
  if ((f[1] & 0x40) && subtype1 == 0xcd)
    rval = 1;
  if ((f[1] & 0x80) && subtype1 == 0x6c)
    rval = 1;
  if ((f[2] & 0x01) && subtype1 == 0x2c)
    rval = 1;
  if ((f[2] & 0x02) && subtype1 == 0x0b)
    rval = 1;
  if ((f[2] & 0x04) && subtype1 == 0x44)
    rval = 1;
  if (subtype1 == 0xc5)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000fd1e
static bool
check_casting_cost(csvid_t csvid, char *entry)
{
  int cmc;
  char txt[84];
  int rval = 0;

  if (entry == NULL)
  {
    sprintf(txt, "Card Number %d does not have a valid cast cost!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONSTOP);
    return 0;
  }

  if (!(global_filter_casting_cost & FN_ENABLE))
    return 1;

  if (*entry == '(')
    return (global_filter_casting_cost & FN_CC_X) ? 1 : 0;

  cmc = (int)entry[2] + (int)entry[5] + (int)entry[7] + (int)entry[8] + (int)entry[1] + (int)entry[0];

  if ((global_filter_casting_cost & FN_GT) && global_filter_casting_cost_value <= cmc)
    rval = 1;
  if ((global_filter_casting_cost & FN_LT) && cmc <= global_filter_casting_cost_value)
    rval = 1;
  if ((global_filter_casting_cost & FN_EQ) && global_filter_casting_cost_value == cmc)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000fe6e
static bool
check_power(int cp_power)
{
  int rval = 0;

  if (((char)global_filter_power & FN_ENABLE) == 0)
    return 1;

  if (((char)global_filter_power & FN_GT) && (short)global_filter_power_value <= cp_power)
    rval = 1;
  if (((char)global_filter_power & FN_LT) && cp_power <= (short)global_filter_power_value)
    rval = 1;
  if (((char)global_filter_power & FN_EQ) && (short)global_filter_power_value == cp_power)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ff16
static bool
check_toughness(int cp_toughness)
{
  int rval = 0;

  if (((char)global_filter_toughness & FN_ENABLE) == 0)
    return 1;

  if (((char)global_filter_toughness & FN_GT) && (short)global_filter_toughness_value <= cp_toughness)
    rval = 1;
  if (((char)global_filter_toughness & FN_LT) && cp_toughness <= (short)global_filter_toughness_value)
    rval = 1;
  if (((char)global_filter_toughness & FN_EQ) && (short)global_filter_toughness_value == cp_toughness)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ffbe
static bool
check_rarity(csvid_t csvid, int cp_rarity)
{
  if (cp_rarity >= 5)
  {
    char txt[80];
    sprintf(txt, "Card Number %d does not have a valid rarity value!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONERROR);
    return false;
  }

  return (!(global_filter_rarity & FR_ENABLE) || ((global_filter_rarity & FR_COMMON) && cp_rarity <= 1) || ((global_filter_rarity & FR_UNCOMMON) && cp_rarity == 4) || ((global_filter_rarity & FR_RARE) && cp_rarity == 2) || ((global_filter_rarity & FR_RESTRICTED) && check_restricted(csvid)) || ((global_filter_rarity & FR_BANNED) && check_banned(csvid)));
}

// FUNCTION: DECKDLL 0x10010260
static bool
check_abilities(csvid_t csvid, int num_abils, char *abils)
{
  int i;
  int native;
  int grants;
  char txt[80];

  native = 0;
  grants = 0;

  if (!abils)
  {
    sprintf(txt, "Card Number %d does not have a valid ability value!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONERROR);
    return false;
  }

  if (!(global_filter_abilities & FA_ENABLE))
    return true;

  if (global_filter_abilities & FA_NATIVE)
    native = 1;
  if (global_filter_abilities & FA_GRANTS)
    grants = 1;

  if (!native && !grants)
    return false;

  for (i = 0; i < num_abils; ++i)
  {
    if ((global_filter_abilities & FA_FLYING) != 0)
    {
      if (abils[i] == ABIL_NATIVE_FLYING && native)
        return true;
      if (abils[i] == ABIL_GRANTS_FLYING && grants)
        return true;
    }

    if ((global_filter_abilities & FA_FIRSTSTRIKE) != 0)
    {
      if (abils[i] == ABIL_NATIVE_FIRSTSTRIKE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_FIRSTSTRIKE && grants)
        return true;
    }

    if ((global_filter_abilities & FA_TRAMPLE) != 0)
    {
      if (abils[i] == ABIL_NATIVE_TRAMPLE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_TRAMPLE && grants)
        return true;
    }

    if ((global_filter_abilities & FA_REGENERATION) != 0)
    {
      if (abils[i] == ABIL_NATIVE_REGENERATION && native)
        return true;
      if (abils[i] == ABIL_GRANTS_REGENERATION && grants)
        return true;
    }

    if ((global_filter_abilities & FA_BANDING) != 0)
    {
      if (abils[i] == ABIL_NATIVE_BANDING && native)
        return true;
      if (abils[i] == ABIL_GRANTS_BANDING && grants)
        return true;
    }

    if ((global_filter_abilities & FA_PROTECTION) != 0)
    {
      if (abils[i] == ABIL_NATIVE_PROTECTION_FROM_BLACK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_PROTECTION_FROM_RED && native)
        return true;
      if (abils[i] == ABIL_NATIVE_PROTECTION_FROM_WHITE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_BLACK && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_RED && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_WHITE && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_BLUE && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_GREEN && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PROTECTION_FROM_ARTIFACTS && grants)
        return true;
    }

    if ((global_filter_abilities & FA_LANDWALK) != 0)
    {
      if (abils[i] == ABIL_NATIVE_DESERTWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_FORESTWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_ISLANDWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_LEGENDARY_LANDWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_MOUNTAINWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_PLAINSWALK && native)
        return true;
      if (abils[i] == ABIL_NATIVE_SWAMPWALK && native)
        return true;
      if (abils[i] == ABIL_GRANTS_FORESTWALK && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_ISLANDWALK && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_MOUNTAINWALK && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_PLAINSWALK && grants)
        return true;
      if (abils[i] == ABIL_GRANTS_SWAMPWALK && grants)
        return true;
    }

    if ((global_filter_abilities & FA_INFECT) != 0 && abils[i] == ABIL_NATIVE_INFECT && native)
      return true;

    if ((global_filter_abilities & FA_RAMPAGE) != 0)
    {
      if (abils[i] == ABIL_NATIVE_RAMPAGE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_RAMPAGE && grants)
        return true;
    }

    if ((global_filter_abilities & FA_REACH) != 0)
    {
      if (abils[i] == ABIL_NATIVE_REACH && native)
        return true;
      if (abils[i] == ABIL_GRANTS_REACH && grants)
        return true;
    }

    if ((global_filter_abilities & FA_DEATHTOUCH) != 0)
    {
      if (abils[i] == ABIL_NATIVE_DEATHTOUCH && native)
        return true;
      if (abils[i] == ABIL_GRANTS_DEATHTOUCH && grants)
        return true;
    }

    if ((global_filter_abilities & FA_VIGILANCE) != 0)
    {
      if (abils[i] == ABIL_NATIVE_VIGILANCE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_VIGILANCE && grants)
        return true;
    }

    if (((int)(short)global_filter_abilities & FA_HASTE) != 0)
    {
      if (abils[i] == ABIL_NATIVE_HASTE && native)
        return true;
      if (abils[i] == ABIL_GRANTS_HASTE && grants)
        return true;
    }
  }

  return false;
}

static uint32_t global_filter_artist_bitmask_low = 0xFFFFFFFF;
static uint32_t global_filter_artist_bitmask_high = 0xFFFFFFFF;

// FUNCTION: DECKDLL 0x10010b1e
static int
find_artist_name_idx(char *artist, int num_artists)
{
  int m;
  int cmp;
  int l;
  int r;

  l = 0;
  r = num_artists - 1;
  while (1)
  {
    while (1)
    {
      if (r < l)
        return -1;

      m = (r + l) / 2;
      cmp = strcmp(read_db_artist_names[m], artist);
      if (cmp >= 0)
        break;

      l = m + 1;
    }

    cmp = strcmp(read_db_artist_names[m], artist);
    if (cmp <= 0)
      break;

    r = m - 1;
  }

  return m;
}

// FUNCTION: DECKDLL 0x10010a72
static bool
check_artist_filter(csvid_t csvid, char *artist)
{
  char txt[80];
  int idx;
  bool rval;

  rval = false;
  if (artist == NULL)
  {
    sprintf(txt, "Card Number %d does not have a valid artist value!", csvid);
    rval = false;
  }
  else if ((((unsigned char *)&global_filter_rarity)[1] & 1) == 0)
    rval = true;
  else
  {
    idx = find_artist_name_idx(artist, 53);
    if (idx >= 0)
    {
      if (idx < 32)
      {
        if ((global_filter_artist_bitmask_low & (1 << ((unsigned char)idx & 0x1f))) != 0)
          rval = true;
      }
      else if ((global_filter_artist_bitmask_high & (1 << ((unsigned char)idx & 0x1f))) != 0)
        rval = true;
    }
  }

  return rval;
}

// FUNCTION: DECKDLL 0x1000f1a5
bool check_filters(csvid_t csvid)
{
  unsigned int power;
  unsigned int toughness;
  unsigned int rarity;
  bool color_ok;
  bool type_ok;
  int subtype1;
  int local_c;
  int i;
  int artist_ok;
  unsigned char setmask;
  char *artist;
  casting_cost_t *req;
  unsigned int db_card_type_2;
  unsigned int cardtype;
  unsigned int color;
  unsigned int toughness_ok;
  unsigned int power_ok;
  unsigned int rarity_ok;
  char *abils;

  i = strcmp(global_raw_cards_ptr[csvid].full_name, "Blank");
  if (i == 0)
  {
    local_c = 0;
  }
  else if (((global_db_flags_1 & 0x61) == 0) || ((i = check_card_global_deck_availability(csvid)) != 0))
  {
    color = global_raw_cards_ptr[csvid].color;
    cardtype = global_raw_cards_ptr[csvid].card_type;
    subtype1 = global_raw_cards_ptr[csvid].subtype1;
    db_card_type_2 = global_raw_cards_ptr[csvid].db_card_type_2;
    req = &global_raw_cards_ptr[csvid].req;
    power = global_raw_cards_ptr[csvid].power;
    toughness = global_raw_cards_ptr[csvid].toughness;
    artist = (char *)global_raw_cards_ptr[csvid].artist;
    rarity = global_raw_cards_ptr[csvid].rarity;

    if (color == CP_COLOR_LESS)
      color_ok = true;
    else if ((((color == CP_COLOR_WHITE) && ((global_filter_colors & FC_WHITE) != 0)) ||
              (((color == CP_COLOR_GREEN) && ((global_filter_colors & FC_GREEN) != 0)) ||
               (((color == CP_COLOR_RED) && ((global_filter_colors & FC_RED) != 0)) ||
                ((color == CP_COLOR_BLACK) && ((global_filter_colors & FC_BLACK) != 0))))) ||
             (((color == CP_COLOR_BLUE) && ((global_filter_colors & FC_BLUE) != 0)) ||
              color == CP_COLOR_LAND || color == CP_COLOR_ARTIFACT))
      color_ok = true;
    else
      color_ok = false;

    setmask = 0;
    if ((global_filter_cardsets & FS_4TH_EDITION) != 0)
    {
      setmask = (global_filter_expansions & FE_4TH_EDITION) != 0;
      if ((global_filter_expansions & FE_UNLIMITED) != 0)
        setmask |= 2;
      if ((global_filter_expansions & FE_REVISED) != 0)
        setmask |= 4;
    }
    if ((global_filter_cardsets & FS_ASTRAL) != 0)
      setmask |= 0x20;
    if ((global_filter_cardsets & FS_ARABIAN_NIGHTS) != 0)
      setmask |= 8;
    if ((global_filter_cardsets & FS_ANTIQUITIES) != 0)
      setmask |= 0x10;
    if ((global_filter_cardsets & FS_LEGENDS) != 0)
      setmask |= 0x40;
    if ((global_filter_cardsets & FS_THE_DARK) != 0)
      setmask |= 0x80;

    i = check_lands(cardtype, db_card_type_2);
    if (((((i == 0) &&
           ((i = check_artifacts(cardtype, subtype1)), i == 0)) &&
          (((i = check_creatures(cardtype, subtype1)), i == 0) &&
           ((i = check_enchantments(cardtype, subtype1)), i == 0))) &&
         (((cardtype != CP_TYPE_INSTANT || ((((unsigned char *)&global_filter_cardtypes)[2] & 8) == 0)) &&
           (cardtype != CP_TYPE_INTERRUPT || ((((unsigned char *)&global_filter_cardtypes)[2] & 0x10) == 0)))) &&
        (cardtype != CP_TYPE_SORCERY || ((((unsigned char *)&global_filter_cardtypes)[2] & 0x20) == 0))))
      type_ok = false;
    else
      type_ok = true;

    i = check_casting_cost(csvid, (char *)req);
    power_ok = check_power(power & 0xfff) != 0;
    toughness_ok = check_toughness(toughness & 0xfff) != 0;
    rarity_ok = check_rarity(csvid, rarity) != 0;
    abils = ((char *)&global_raw_cards_ptr[csvid]) + 0x88;
    artist_ok = check_artist_filter(csvid, artist);

    if (color_ok &&
        check_set_availability(csvid, setmask) &&
        type_ok &&
        i != 0 &&
        power_ok != 0 &&
        toughness_ok != 0 &&
        rarity_ok != 0 &&
        check_abilities(csvid, 4, abils) != 0 &&
        artist_ok != 0)
      local_c = 1;
    else
      local_c = 0;
  }
  else
    local_c = 0;

  return local_c;
}
