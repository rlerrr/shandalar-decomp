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
  unk_009266a4 = g_card_count - 0x2d;
  
  LEGACY_EFFECT_PUMP = unk_009266a4 + 1;
  unk_007abc7c = unk_009266a4 + 2;
  unk_00896534 = unk_009266a4 + 3;
  _DAT_008cdab8 = unk_009266a4 + 4;
  _DAT_008ce7d0 = unk_009266a4 + 5;
  unk_008b3104 = unk_009266a4 + 6;
  DAT_008a9198 = unk_009266a4 + 7;
  DAT_008ce7cc = unk_009266a4 + 8;
  unk_008b28f8 = unk_009266a4 + 9;
  DAT_00925bf4 = unk_009266a4 + 10;
  DAT_007a7878 = unk_009266a4 + 0xb;
  DAT_00926734 = unk_009266a4 + 0xc;
  unk_007894a0 = unk_009266a4 + 0xd;
  unk_008a8c34 = unk_009266a4 + 0xe;
  unk_008b49c4 = unk_009266a4 + 0xf;
  DAT_008a918c = unk_009266a4 + 0x10;
  unk_008a9194 = unk_009266a4 + 0x11;
  unk_00939348 = unk_009266a4 + 0x12;
  unk_0091a80c = unk_009266a4 + 0x13;
  _DAT_00777d28 = unk_009266a4 + 0x14;
  DAT_007ab2c8 = unk_009266a4 + 0x15;
  DAT_007ab150 = unk_009266a4 + 0x16;
  unk_008b3d10 = unk_009266a4 + 0x17;
  unk_00789314 = unk_009266a4 + 0x18;
  DAT_0091b27c = unk_009266a4 + 0x19;
  unk_0093d848 = unk_009266a4 + 0x1a;
  g_duel_generated_iid_1b = unk_009266a4 + 0x1b;
  DAT_007aaee4 = unk_009266a4 + 0x1c;
  DAT_008b28f4 = unk_009266a4 + 0x1d;
  unk_0079140c = unk_009266a4 + 0x1e;
  DAT_00896530 = unk_009266a4 + 0x1f;
  DAT_008b3278 = unk_009266a4 + 0x20;
  g_duel_generated_iid_21 = unk_009266a4 + 0x21;
  DAT_008b60e4 = unk_009266a4 + 0x22;
  unk_00789108 = unk_009266a4 + 0x23;
  unk_008cf1bc = unk_009266a4 + 0x24;
  DAT_007ab01c = unk_009266a4 + 0x25;
  DAT_008b40cc = unk_009266a4 + 0x26;
  unk_008d0340 = unk_009266a4 + 0x27;
  unk_0093933c = unk_009266a4 + 0x28;
  unk_008a8df0 = unk_009266a4 + 0x29;
  unk_008b3bd4 = unk_009266a4 + 0x2a;
  unk_008cefb8 = unk_009266a4 + 0x2b;
  DAT_0091c48c = unk_009266a4 + 0x2c;

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
  active_player = 1;
  unk_00712938 = -1;
  DAT_00742f68 = 0x30;
  DAT_00742fc0 = 1;
  DAT_0057d9e4 = 2;
}
