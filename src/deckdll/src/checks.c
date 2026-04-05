#include "mystdbool.h"
#include "filtermenus.h"

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
// GLOBAL: MAGIC 0x0057a4f8
const Restriction restrictions[] =
    {
        {CARD_ID_ANCESTRAL_RECALL, RST_RESTRICTED},
        {CARD_ID_BALANCE, RST_RESTRICTED},
        {CARD_ID_BERSERK, RST_RESTRICTED},
        {CARD_ID_BLACK_LOTUS, RST_RESTRICTED},
        {CARD_ID_BLACK_VISE, RST_RESTRICTED},
        {CARD_ID_BRAINGEYSER, RST_RESTRICTED},
        {CARD_ID_CHANNEL, RST_BANNED},
        {CARD_ID_CONTRACT_FROM_BELOW, RST_ANTE},
        {CARD_ID_DARKPACT, RST_ANTE},
        {CARD_ID_DEMONIC_ATTORNEY, RST_ANTE},
        {CARD_ID_DEMONIC_TUTOR, RST_RESTRICTED},
        {CARD_ID_FASTBOND, RST_RESTRICTED},
        {CARD_ID_FORK, RST_RESTRICTED},
        {CARD_ID_MIND_TWIST, RST_BANNED},
        {CARD_ID_MOX_EMERALD, RST_RESTRICTED},
        {CARD_ID_MOX_JET, RST_RESTRICTED},
        {CARD_ID_MOX_PEARL, RST_RESTRICTED},
        {CARD_ID_MOX_RUBY, RST_RESTRICTED},
        {CARD_ID_MOX_SAPPHIRE, RST_RESTRICTED},
        {CARD_ID_REGROWTH, RST_RESTRICTED},
        {CARD_ID_SOL_RING, RST_RESTRICTED},
        {CARD_ID_TIME_VAULT, RST_BANNED},
        {CARD_ID_TIME_WALK, RST_RESTRICTED},
        {CARD_ID_TIMETWISTER, RST_RESTRICTED},
        {CARD_ID_WHEEL_OF_FORTUNE, RST_RESTRICTED},
        {CARD_ID_JEWELED_BIRD, RST_ANTE},
        {CARD_ID_LIBRARY_OF_ALEXANDRIA, RST_RESTRICTED},
        {CARD_ID_BRONZE_TABLET, RST_ANTE},
        {CARD_ID_IVORY_TOWER, RST_RESTRICTED},
        {CARD_ID_STRIP_MINE, RST_RESTRICTED},
        {CARD_ID_REBIRTH, RST_ANTE},
        {CARD_ID_TEMPEST_EFREET, RST_ANTE},
        {CARD_ID_AIR_ELEMENTAL, RST_0},
        {CARD_ID_AIR_ELEMENTAL, RST_0},
        {CARD_ID_AIR_ELEMENTAL, RST_0},
    };

// FUNCTION: DECKDLL 0x1001013d
// FUNCTION: MAGIC 0x004e0291
bool check_restricted(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_RESTRICTED))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x1001019e
// FUNCTION: MAGIC 0x004e02f6
bool check_banned(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_BANNED))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x100101ff
// FUNCTION: MAGIC 0x004e035b
bool check_ante(csvid_t csvid)
{
  int i;

  for (i = 0; i < 35; ++i)
    if (restrictions[i].csvid == csvid && (restrictions[i].restriction & RST_ANTE))
      return true;
  return false;
}

// FUNCTION: DECKDLL 0x100100e6
// FUNCTION: MAGIC 0x004e0225
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
