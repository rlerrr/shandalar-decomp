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
  g_damage_card_internal_card_id = g_card_count - 0x2d;

  LEGACY_EFFECT_PUMP = g_damage_card_internal_card_id + 1;
  unk_007abc7c = g_damage_card_internal_card_id + 2;
  unk_00896534 = g_damage_card_internal_card_id + 3;
  _DAT_008cdab8 = g_damage_card_internal_card_id + 4;
  _DAT_008ce7d0 = g_damage_card_internal_card_id + 5;
  unk_008b3104 = g_damage_card_internal_card_id + 6;
  DAT_008a9198 = g_damage_card_internal_card_id + 7;
  DAT_008ce7cc = g_damage_card_internal_card_id + 8;
  g_draw_card_placeholder_internal_card_id = g_damage_card_internal_card_id + 9;
  DAT_00925bf4 = g_damage_card_internal_card_id + 10;
  DAT_007a7878 = g_damage_card_internal_card_id + 0xb;
  DAT_00926734 = g_damage_card_internal_card_id + 0xc;
  unk_007894a0 = g_damage_card_internal_card_id + 0xd;
  unk_008a8c34 = g_damage_card_internal_card_id + 0xe;
  unk_008b49c4 = g_damage_card_internal_card_id + 0xf;
  g_damage_effect_internal_card_id = g_damage_card_internal_card_id + 0x10;
  unk_008a9194 = g_damage_card_internal_card_id + 0x11;
  unk_00939348 = g_damage_card_internal_card_id + 0x12;
  g_stack_proxy_internal_card_id = g_damage_card_internal_card_id + 0x13;
  _DAT_00777d28 = g_damage_card_internal_card_id + 0x14;
  DAT_007ab2c8 = g_damage_card_internal_card_id + 0x15;
  DAT_007ab150 = g_damage_card_internal_card_id + 0x16;
  unk_008b3d10 = g_damage_card_internal_card_id + 0x17;
  unk_00789314 = g_damage_card_internal_card_id + 0x18;
  DAT_0091b27c = g_damage_card_internal_card_id + 0x19;
  unk_0093d848 = g_damage_card_internal_card_id + 0x1a;
  g_duel_generated_iid_1b = g_damage_card_internal_card_id + 0x1b;
  DAT_007aaee4 = g_damage_card_internal_card_id + 0x1c;
  DAT_008b28f4 = g_damage_card_internal_card_id + 0x1d;
  unk_0079140c = g_damage_card_internal_card_id + 0x1e;
  DAT_00896530 = g_damage_card_internal_card_id + 0x1f;
  DAT_008b3278 = g_damage_card_internal_card_id + 0x20;
  g_duel_generated_iid_21 = g_damage_card_internal_card_id + 0x21;
  DAT_008b60e4 = g_damage_card_internal_card_id + 0x22;
  unk_00789108 = g_damage_card_internal_card_id + 0x23;
  unk_008cf1bc = g_damage_card_internal_card_id + 0x24;
  DAT_007ab01c = g_damage_card_internal_card_id + 0x25;
  DAT_008b40cc = g_damage_card_internal_card_id + 0x26;
  g_control_aura_special_internal_card_id = g_damage_card_internal_card_id + 0x27;
  unk_0093933c = g_damage_card_internal_card_id + 0x28;
  unk_008a8df0 = g_damage_card_internal_card_id + 0x29;
  unk_008b3bd4 = g_damage_card_internal_card_id + 0x2a;
  g_control_aura_legacy_internal_card_id = g_damage_card_internal_card_id + 0x2b;
  DAT_0091c48c = g_damage_card_internal_card_id + 0x2c;

  unk_007a7d64 = 0x385;
  unk_008a8de8 = 0x386;
  unk_00789734 = 0x387;
  unk_009266ac = 0x388;
  unk_00789b80 = 0x389;
  unk_0092666c = 0x38a;
  unk_008cf1ac = 0x38b;

  g_ai_combat_value_weights[0] = 8;
  g_ai_combat_value_weights[1] = 8;
#ifdef SHANDALAR
  g_ai_combat_value_weights[2] = 0xc;
  g_ai_combat_value_weights[3] = 0xc;
#else
  unk_007a7d18[0] = 0xc;
  unk_007a7d18[1] = 0xc;
#endif

  DAT_008a8d6c = -1;
  DAT_007aadf0 = -1;
  g_produced_mana_color = -1;
  g_other_player = 1;
  unk_00712938 = -1;
  DAT_00742f68 = 0x30;
  DAT_00742fc0 = 1;
  g_game_time_scale = 2;
}
