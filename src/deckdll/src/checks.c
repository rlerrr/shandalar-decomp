#include "defs.h"
#include "mystdbool.h"
#include "deckdll.h"

extern HWND global_main_hwnd;
extern bool global_cfg_view_all;

extern int global_deck_num_entries;
extern GlobalDeckEntry global_deck[];
extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[1000];
extern card_ptr_t *global_raw_cards_ptr;
extern const char read_db_artist_names[][100];
extern DBFlags global_db_flags_1;

extern OrigRarities global_origrarities[];

extern short global_filter_abilities;
extern short global_filter_cardsets;
extern FilterTypes global_filter_cardtypes;
extern char global_filter_casting_cost;
extern short global_filter_casting_cost_value;
extern char global_filter_power;
extern short global_filter_power_value;
extern char global_filter_toughness;
extern short global_filter_toughness_value;
extern char global_filter_rarity;
extern short global_filter_colors;
extern short global_filter_expansions;
extern char global_filter_artist;

extern int global_num_expansions;
extern int global_expansion_size;
extern uint32_t global_filter_expansion_list[];
extern uint32_t global_filter_creature_list[];

extern char *global_raw_rarities;

extern int (*global_is_valid_card_fn)(int);

int IsCardAvailable(csvid_t csvid, int param_2);

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

// FUNCTION: DECKDLL 0x1001013d
bool check_restricted(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_RESTRICTED))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x1001019e
bool check_banned(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_BANNED))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x100101ff
bool check_ante(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_ANTE))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x100100e6
bool check_basic(csvid_t csvid)
{
  if (csvid == CARD_ID_SWAMP ||
      csvid == CARD_ID_ISLAND ||
      csvid == CARD_ID_FOREST ||
      csvid == CARD_ID_MOUNTAIN ||
      csvid == CARD_ID_PLAINS)
    return true;

  return false;
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

// FUNCTION: DECKDLL 0x10010c57
static bool
check_set_availability(csvid_t csvid, unsigned char allowed_sets)
{
  int bits[9];

  bits[5] = 0;
  bits[8] = bits[5];
  bits[6] = bits[8];
  bits[2] = bits[6];
  bits[0] = bits[2];
  bits[7] = bits[0];
  bits[1] = bits[7];
  bits[4] = bits[1];
  bits[3] = bits[4];

  if (IsCardAvailable(csvid, 0) == 0 && IsCardAvailable(csvid, 1) == 0 && IsCardAvailable(csvid, 2) == 0)
    return false;

  if (global_origrarities[csvid].set == -1)
    return false;

  if (allowed_sets & 0x1)
    bits[3] = (global_origrarities[csvid].exp_rarities[1] != '-') ? 1 : 0;

  if (allowed_sets & 0x4)
    bits[4] = (global_origrarities[csvid].exp_rarities[0] != '-') ? 1 : 0;

  if (allowed_sets & 0x2)
    bits[1] = (global_origrarities[csvid].set == SET_UNLIMITED) ? 1 : 0;

  if (allowed_sets & 0x8)
    bits[7] = (global_origrarities[csvid].set == SET_ARABIAN_NIGHTS) ? 1 : 0;

  if (allowed_sets & 0x10)
    bits[0] = (global_origrarities[csvid].set == SET_ANTIQUITIES) ? 1 : 0;

  if (allowed_sets & 0x20)
  {
    bits[2] = (global_origrarities[csvid].set == SET_ASTRAL);
    bits[6] = (global_origrarities[csvid].set == SET_PROMO) ? 1 : 0;
  }

  if (allowed_sets & 0x40)
    bits[8] = (global_origrarities[csvid].set == SET_LEGENDS) ? 1 : 0;

  if (allowed_sets & 0x80)
    bits[5] = (global_origrarities[csvid].set == SET_THE_DARK) ? 1 : 0;

  return bits[5] | bits[8] | bits[6] | bits[2] | bits[0] | bits[7] | bits[1] | bits[3] | bits[4];
}

// FUNCTION: DECKDLL 0x1000fa34
static bool
check_lands(int cardtype, int db_card_type_2)
{
  struct
  {
    int rval;
  } s;

  s.rval = 0;
  if ((global_filter_cardtypes & FT_LAND) == 0)
    return s.rval;

  if ((global_filter_cardtypes & FT_LAND_LAND_AND_MANA) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 == 10)
    s.rval = 1;
  if ((global_filter_cardtypes & FT_LAND_LAND_ONLY) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 != 10)
    s.rval = 1;
  if ((global_filter_cardtypes & FT_LAND_MANA_ONLY) != 0 && cardtype != CP_TYPE_LAND && db_card_type_2 == 10)
    s.rval = 1;

  return s.rval;
}

// FUNCTION: DECKDLL 0x1000fade
static bool
check_artifacts(int cardtype, int subtype1)
{
  int result = 0;
  if (cardtype != CP_TYPE_ARTIFACT || (global_filter_cardtypes & FT_ARTIFACT) == 0) {
    return result;
  }

  if ((global_filter_cardtypes & FT_ARTIFACT_CREATURE) != 0 &&
      subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  if ((global_filter_cardtypes & FT_ARTIFACT_NON_CREATURE) != 0 &&
      subtype1 != HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x10010bcf
static bool
check_creature_list_filter(int subtype1)
{
  struct {
    int i;
    int j;
  } s;

  for (s.i = 0; s.i < 7; s.i++)
  {
    for (s.j = 0; s.j < 0x20; s.j++)
    {
      if ((global_filter_creature_list[s.i] & (1 << (byte)s.j)) != 0)
        if (s.i * 0x20 + s.j + 1 == subtype1)
          return true;
    }
  }

  return false;
}

// FUNCTION: DECKDLL 0x1000fb56
static bool
check_creatures(int cardtype, int subtype1)
{

  int result = 0;
  if ((global_filter_cardtypes & FT_CREATURE) == 0)
  {
    return result;
  }

  if ((global_filter_cardtypes & 0x100) != 0 && cardtype == CP_TYPE_CREATURE)
    result = 1;

  if ((global_filter_cardtypes & 0x200) != 0 && cardtype == CP_TYPE_TOKEN)
    result = 1;

  if ((global_filter_cardtypes & 0x400) != 0 &&
      cardtype == CP_TYPE_ARTIFACT &&
      subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  if ((global_filter_cardtypes & 0x800) != 0)
  {
    if (check_creature_list_filter(subtype1) != 0)
      result = 1;
  }

  return result;
}

// FUNCTION: DECKDLL 0x1000fc14
static bool
check_enchantments(int cardtype, int subtype1)
{
  int result = 0;
  if ((global_filter_cardtypes & 0x1000) == 0 || cardtype != CP_TYPE_ENCHANTMENT)
  {
    return result;
  }

  if ((global_filter_cardtypes & 0x2000) != 0 && subtype1 == 0xd3)
    result = 1;
  if ((global_filter_cardtypes & 0x4000) != 0 && subtype1 == 0xcd)
    result = 1;
  if ((global_filter_cardtypes & 0x8000) != 0 && subtype1 == 0x6c)
    result = 1;
  if ((global_filter_cardtypes & 0x10000) != 0 && subtype1 == 0x2c)
    result = 1;
  if ((global_filter_cardtypes & 0x20000) != 0 && subtype1 == 0x0b)
    result = 1;
  if ((global_filter_cardtypes & 0x40000) != 0 && subtype1 == 0x44)
    result = 1;
  if (subtype1 == 0xc5)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x1000fd1e
static bool
check_casting_cost(csvid_t csvid, char *entry)
{
  int cmc;
  int result = 0;

  if (entry == NULL)
  {
    char txt[80];
    sprintf(txt, "Card Number %d does not have a valid cast cost!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONSTOP);
    return 0;
  }
  
  if ((global_filter_casting_cost & FN_ENABLE) == 0)
  {
    return 1;
  }

  if (*entry == '(')
  {
    if ((global_filter_casting_cost & FN_CC_X) != 0)
      return 1;
    else
      return 0;
  }

  cmc = (int)entry[2] + (int)entry[5] + (int)entry[7] + (int)entry[8] + (int)entry[1] + (int)entry[0];

  if ((global_filter_casting_cost & FN_GT) && global_filter_casting_cost_value <= cmc)
    result = 1;
  if ((global_filter_casting_cost & FN_LT) && cmc <= global_filter_casting_cost_value)
    result = 1;
  if ((global_filter_casting_cost & FN_EQ) && global_filter_casting_cost_value == cmc)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x1000fe6e
static bool
check_power(int cp_power)
{
  int rval = 0;

  if ((global_filter_power & FN_ENABLE) == 0)
    return 1;

  if ((global_filter_power & FN_GT) && global_filter_power_value <= cp_power)
    rval = 1;
  if ((global_filter_power & FN_LT) && cp_power <= global_filter_power_value)
    rval = 1;
  if ((global_filter_power & FN_EQ) && global_filter_power_value == cp_power)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ff16
static bool
check_toughness(int cp_toughness)
{
  int rval = 0;

  if ((global_filter_toughness & FN_ENABLE) == 0)
    return 1;

  if ((global_filter_toughness & FN_GT) && global_filter_toughness_value <= cp_toughness)
    rval = 1;
  if ((global_filter_toughness & FN_LT) && cp_toughness <= global_filter_toughness_value)
    rval = 1;
  if ((global_filter_toughness & FN_EQ) && global_filter_toughness_value == cp_toughness)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ffbe
static bool
check_rarity(csvid_t csvid, int cp_rarity)
{
  int result = 0;
  if (cp_rarity >= 5)
  {
    char txt[80];
    sprintf(txt, "Card Number %d does not have a valid rarity value!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONERROR);
    return 0;
  }

  if ((global_filter_rarity & FR_ENABLE) == 0)
    return 1;

  if ((global_filter_rarity & FR_COMMON) && cp_rarity <= 1)
    result = 1;
  if ((global_filter_rarity & FR_UNCOMMON) && cp_rarity == 4)
    result = 1;
  if ((global_filter_rarity & FR_RARE) && cp_rarity == 2)
    result = 1;
  if ((global_filter_rarity & FR_RESTRICTED) && check_restricted(csvid))
    result = 1;
  if ((global_filter_rarity & FR_BANNED) && check_banned(csvid))
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x10010260
static bool
check_abilities(csvid_t csvid, int num_abils, char *abils)
{
  struct
  {
    char txt[80];
    int native; // ebp - 0xc
    int grants; // ebp - 0x8
    int i; // ebp - 0x4
  } s;

  s.native = 0;
  s.grants = 0;

  if (!abils)
  {
    sprintf(s.txt, "Card Number %d does not have a valid abilities value!", csvid);
    MessageBox(global_main_hwnd, s.txt, "Card Error", MB_ICONERROR);
    return false;
  }

  if (!(global_filter_abilities & FA_ENABLE))
    return true;

  if (global_filter_abilities & FA_NATIVE)
    s.native = 1;
  if (global_filter_abilities & FA_GRANTS)
    s.grants = 1;

  if (!s.native && !s.grants)
    return false;

  for (s.i = 0; s.i < num_abils; ++s.i)
  {
    if ((global_filter_abilities & FA_FLYING) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_FLYING && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FLYING && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_FIRSTSTRIKE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_FIRSTSTRIKE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FIRSTSTRIKE && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_TRAMPLE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_TRAMPLE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_TRAMPLE && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_REGENERATION) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_REGENERATION && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_REGENERATION && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_BANDING) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_BANDING && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_BANDING && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_PROTECTION) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_BLACK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_RED && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_WHITE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_BLACK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_RED && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_WHITE && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_BLUE && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_GREEN && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_ARTIFACTS && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_LANDWALK) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_DESERTWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_FORESTWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_ISLANDWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_LEGENDARY_LANDWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_MOUNTAINWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PLAINSWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_SWAMPWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FORESTWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_ISLANDWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_MOUNTAINWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PLAINSWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_SWAMPWALK && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_INFECT) != 0 && abils[s.i] == ABIL_NATIVE_INFECT && s.native)
      return true;

    if ((global_filter_abilities & FA_RAMPAGE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_RAMPAGE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_RAMPAGE && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_REACH) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_REACH && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_REACH && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_DEATHTOUCH) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_DEATHTOUCH && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_DEATHTOUCH && s.grants)
        return true;
    }

    if ((global_filter_abilities & FA_VIGILANCE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_VIGILANCE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_VIGILANCE && s.grants)
        return true;
    }

    if (((int)global_filter_abilities & FA_HASTE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_HASTE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_HASTE && s.grants)
        return true;
    }
  }

  return false;
}

// GLOBAL: DECKDLL 0x1012e75c
static int num_artists = 53;

// GLOBAL: DECKDLL 0x101bc600
static int check_filters_debug_log_enabled;
// GLOBAL: DECKDLL 0x10142640
static FILE *check_filters_debug_log_file;

// FUNCTION: DECKDLL 0x10010b1e
static int
find_artist_name_idx(char *artist, int num_artists)
{
  struct {
    int l; // ebp - 0xc
    int m; // ebp - 0x8
    int r; // ebp - 0x4
  } s;

  s.l = 0;
  s.r = num_artists - 1;
  while (s.r >= s.l)
  {
    s.m = (s.r + s.l) / 2;
    if (strcmp(read_db_artist_names[s.m], artist) < 0)
    {
      s.l = s.m + 1;
    }
    else if (strcmp(read_db_artist_names[s.m], artist) > 0)
    {
      s.r = s.m - 1;
    }
    else
    {
      return s.m;
    }
  }
  return -1;
}

// FUNCTION: DECKDLL 0x10010a72
static bool
check_artist_filter(csvid_t csvid, char *artist)
{
  struct
  {
    int idx_hi;
    uint32_t low;
    unsigned __int64 enabled;
    char txt[80];
    unsigned __int64 mask;
    int rval;
  } s;

  s.rval = 0;
  if (artist == NULL)
  {
    sprintf(s.txt, "Card Number %d does not have a valid artist name!", csvid);
    return false;
  }

  if ((global_filter_artist & 0x1) == 0)
    return true;

  //TODO: What in the fuck is going on here?
  if ((global_filter_expansion_list[14]) & ((unsigned __int64)1 << (uint8_t)find_artist_name_idx(artist, num_artists)))
      return 1;

  return s.rval;
}

// FUNCTION: DECKDLL 0x1000f1a5
bool check_filters(csvid_t csvid)
{
  struct
  {
    unsigned int setmask; // ebp - 0x88
    int set_ok;
    unsigned int power; // ebp - 0x80
    int i; // ebp - 0x7c
    int subtypes; // ebp - 0x78
    bool type_ok; // ebp - 0x74
    bool color_ok; // ebp - 0x70
    int abils_ok; // ebp - 0x6c
    int artist_ok; // ebp - 0x68
    unsigned int color; // ebp - 0x64
    unsigned int toughness; // ebp - 0x60
    unsigned int rarity; // ebp - 0x5c
    char *abils;
    char txt[52]; // ebp - 0x54
    unsigned int rarity_ok; // ebp - 0x20
    unsigned int db_card_type_2; // ebp - 0x1c
    char *artist; // ebp - 0x18
    casting_cost_t *req; // ebp - 0x14
    unsigned int toughness_ok; // ebp - 0x10
    unsigned int cardtype; // ebp - 0xc
    int local_c; // ebp - 0x8
    unsigned int power_ok; // ebp - 0x4
  } s;

  if (strcmp(global_raw_cards_storage[csvid].full_name, "Blank") == 0)
    return 0;
  if ((global_db_flags_1 & 0x61) != 0 && check_card_global_deck_availability(csvid) == 0)
    return 0;

  s.color = global_raw_cards_storage[csvid].color;
  s.cardtype = global_raw_cards_storage[csvid].card_type;
  s.subtypes = *(int *)&global_raw_cards_storage[csvid].subtype1;
  s.db_card_type_2 = global_raw_cards_storage[csvid].db_card_type_2;
  s.req = &global_raw_cards_storage[csvid].req;
  s.power = global_raw_cards_storage[csvid].power;
  s.toughness = global_raw_cards_storage[csvid].toughness;
  s.abils = global_raw_cards_storage[csvid].abilities;
  s.artist = global_raw_cards_storage[csvid].artist;
  s.rarity = global_raw_cards_storage[csvid].rarity;

  if (s.color == CP_COLOR_LESS)
    s.color_ok = true;
  else if (((((s.color == CP_COLOR_WHITE) && ((global_filter_colors & FC_WHITE) != 0)) ||
             ((s.color == CP_COLOR_GREEN) && ((global_filter_colors & FC_GREEN) != 0))) ||
            ((s.color == CP_COLOR_RED) && ((global_filter_colors & FC_RED) != 0))) ||
           (((s.color == CP_COLOR_BLACK) && ((global_filter_colors & FC_BLACK) != 0)) ||
            (((s.color == CP_COLOR_BLUE) && ((global_filter_colors & FC_BLUE) != 0)) ||
             s.color == CP_COLOR_LAND || s.color == CP_COLOR_ARTIFACT)))
    s.color_ok = true;
  else
    s.color_ok = false;

  s.setmask = 0;
  if ((global_filter_cardsets & FS_4TH_EDITION) != 0)
  {
    if ((global_filter_expansions & FE_4TH_EDITION) != 0)
      s.setmask |= 1;
    if ((global_filter_expansions & FE_UNLIMITED) != 0)
      s.setmask |= 2;
    if ((global_filter_expansions & FE_REVISED) != 0)
      s.setmask |= 4;
  }
  if ((global_filter_cardsets & FS_ASTRAL) != 0)
    s.setmask |= 0x20;
  if ((global_filter_cardsets & FS_ARABIAN_NIGHTS) != 0)
    s.setmask |= 8;
  if ((global_filter_cardsets & FS_ANTIQUITIES) != 0)
    s.setmask |= 0x10;
  if ((global_filter_cardsets & FS_LEGENDS) != 0)
    s.setmask |= 0x40;
  if ((global_filter_cardsets & FS_THE_DARK) != 0)
    s.setmask |= 0x80;
  s.set_ok = check_set_availability(csvid, s.setmask) ? 1 : 0;

  if (check_lands(s.cardtype, s.db_card_type_2) || 
      check_artifacts(s.cardtype, s.subtypes) ||
      check_creatures(s.cardtype, s.subtypes) ||
      check_enchantments(s.cardtype, s.subtypes) ||
      (s.cardtype == CP_TYPE_INSTANT && (global_filter_cardtypes & 0x80000) != 0) ||
      (s.cardtype == CP_TYPE_INTERRUPT && (global_filter_cardtypes & 0x100000) != 0) ||
      (s.cardtype == CP_TYPE_SORCERY && (global_filter_cardtypes & 0x200000) != 0))
    s.type_ok = true;
  else
    s.type_ok = false;

  s.i = check_casting_cost(csvid, (char *)s.req) ? 1 : 0;
  s.power_ok = check_power(s.power & 0xfff) ? 1 : 0;
  s.toughness_ok = check_toughness(s.toughness & 0xfff) ? 1 : 0;
  s.rarity_ok = check_rarity(csvid, s.rarity) ? 1 : 0;
  s.abils_ok = check_abilities(csvid, 4, s.abils) ? 1 : 0;
  s.artist_ok = check_artist_filter(csvid, s.artist) ? 1 : 0;

  if (s.color_ok && s.set_ok != 0 && s.type_ok &&
      s.i != 0 && s.power_ok != 0 && s.toughness_ok != 0 && s.rarity_ok != 0 &&
      s.abils_ok != 0 && s.artist_ok != 0)
    s.local_c = 1;
  else
    s.local_c = 0;

  if (check_filters_debug_log_enabled != 0 && s.local_c != 0)
  {
    sprintf(s.txt, "%d\n", csvid);
    fprintf(check_filters_debug_log_file, s.txt);
  }

  return s.local_c;
}
