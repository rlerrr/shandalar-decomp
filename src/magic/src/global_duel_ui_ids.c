#define GLOBAL_DUEL_UI_IDS_IMPL
#include "global_duel_ui_ids.h"
#include "global_other.h"
#include "global_state.h"

// FUNCTION: MAGIC 0x004a7813
// FUNCTION: SHANDALAR 0x00559999
void InitializeDuelUiGlobalIds(void)
{
  g_card_count = g_card_data_bytes / 0x48;
  g_card_count -= 0x10;
  damage_card_internal_card_id = g_card_count - 0x2d;

  LEGACY_EFFECT_PUMP = damage_card_internal_card_id + 1;
  unk_007abc7c = damage_card_internal_card_id + 2;
  unk_00896534 = damage_card_internal_card_id + 3;
  _DAT_008cdab8 = damage_card_internal_card_id + 4;
  _DAT_008ce7d0 = damage_card_internal_card_id + 5;
  unk_008b3104 = damage_card_internal_card_id + 6;
  DAT_008a9198 = damage_card_internal_card_id + 7;
  DAT_008ce7cc = damage_card_internal_card_id + 8;
  draw_card_placeholder_internal_card_id = damage_card_internal_card_id + 9;
  DAT_00925bf4 = damage_card_internal_card_id + 10;
  DAT_007a7878 = damage_card_internal_card_id + 0xb;
  DAT_00926734 = damage_card_internal_card_id + 0xc;
  unk_007894a0 = damage_card_internal_card_id + 0xd;
  unk_008a8c34 = damage_card_internal_card_id + 0xe;
  unk_008b49c4 = damage_card_internal_card_id + 0xf;
  damage_effect_internal_card_id = damage_card_internal_card_id + 0x10;
  unk_008a9194 = damage_card_internal_card_id + 0x11;
  unk_00939348 = damage_card_internal_card_id + 0x12;
  stack_proxy_internal_card_id = damage_card_internal_card_id + 0x13;
  _DAT_00777d28 = damage_card_internal_card_id + 0x14;
  DAT_007ab2c8 = damage_card_internal_card_id + 0x15;
  DAT_007ab150 = damage_card_internal_card_id + 0x16;
  unk_008b3d10 = damage_card_internal_card_id + 0x17;
  unk_00789314 = damage_card_internal_card_id + 0x18;
  DAT_0091b27c = damage_card_internal_card_id + 0x19;
  unk_0093d848 = damage_card_internal_card_id + 0x1a;
  g_duel_generated_iid_1b = damage_card_internal_card_id + 0x1b;
  DAT_007aaee4 = damage_card_internal_card_id + 0x1c;
  DAT_008b28f4 = damage_card_internal_card_id + 0x1d;
  unk_0079140c = damage_card_internal_card_id + 0x1e;
  DAT_00896530 = damage_card_internal_card_id + 0x1f;
  DAT_008b3278 = damage_card_internal_card_id + 0x20;
  g_duel_generated_iid_21 = damage_card_internal_card_id + 0x21;
  DAT_008b60e4 = damage_card_internal_card_id + 0x22;
  unk_00789108 = damage_card_internal_card_id + 0x23;
  unk_008cf1bc = damage_card_internal_card_id + 0x24;
  DAT_007ab01c = damage_card_internal_card_id + 0x25;
  DAT_008b40cc = damage_card_internal_card_id + 0x26;
  control_aura_special_internal_card_id = damage_card_internal_card_id + 0x27;
  unk_0093933c = damage_card_internal_card_id + 0x28;
  unk_008a8df0 = damage_card_internal_card_id + 0x29;
  unk_008b3bd4 = damage_card_internal_card_id + 0x2a;
  control_aura_legacy_internal_card_id = damage_card_internal_card_id + 0x2b;
  DAT_0091c48c = damage_card_internal_card_id + 0x2c;

  unk_007a7d64 = 0x385;
  unk_008a8de8 = 0x386;
  unk_00789734 = 0x387;
  unk_009266ac = 0x388;
  unk_00789b80 = 0x389;
  unk_0092666c = 0x38a;
  unk_008cf1ac = 0x38b;

  DAT_007a7d10[0] = 8;
  DAT_007a7d10[1] = 8;
#ifdef SHANDALAR
  DAT_007a7d10[2] = 0xc;
  DAT_007a7d10[3] = 0xc;
#else
  unk_007a7d18[0] = 0xc;
  unk_007a7d18[1] = 0xc;
#endif

  DAT_008a8d6c = -1;
  DAT_007aadf0 = -1;
  produced_mana_color = -1;
  other_player = 1;
  unk_00712938 = -1;
  DAT_00742f68 = 0x30;
  DAT_00742fc0 = 1;
  DAT_0057d9e4 = 2;
}
