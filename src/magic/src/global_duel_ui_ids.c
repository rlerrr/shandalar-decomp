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
  g_duel_generated_internal_card_id_02 = g_damage_card_internal_card_id + 2;
  g_duel_generated_internal_card_id_03 = g_damage_card_internal_card_id + 3;
  g_duel_generated_internal_card_id_04 = g_damage_card_internal_card_id + 4;
  g_duel_generated_internal_card_id_05 = g_damage_card_internal_card_id + 5;
  g_duel_generated_internal_card_id_06 = g_damage_card_internal_card_id + 6;
  g_duel_generated_internal_card_id_07 = g_damage_card_internal_card_id + 7;
  g_duel_generated_internal_card_id_08 = g_damage_card_internal_card_id + 8;
  g_draw_card_placeholder_internal_card_id = g_damage_card_internal_card_id + 9;
  g_duel_generated_internal_card_id_0a = g_damage_card_internal_card_id + 10;
  g_duel_generated_internal_card_id_0b = g_damage_card_internal_card_id + 0xb;
  g_duel_generated_internal_card_id_0c = g_damage_card_internal_card_id + 0xc;
  g_duel_generated_internal_card_id_0d = g_damage_card_internal_card_id + 0xd;
  g_duel_generated_internal_card_id_0e = g_damage_card_internal_card_id + 0xe;
  g_duel_generated_internal_card_id_0f = g_damage_card_internal_card_id + 0xf;
  g_damage_effect_internal_card_id = g_damage_card_internal_card_id + 0x10;
  g_duel_generated_internal_card_id_11 = g_damage_card_internal_card_id + 0x11;
  g_duel_generated_internal_card_id_12 = g_damage_card_internal_card_id + 0x12;
  g_stack_proxy_internal_card_id = g_damage_card_internal_card_id + 0x13;
  g_duel_generated_internal_card_id_14 = g_damage_card_internal_card_id + 0x14;
  g_duel_generated_internal_card_id_15 = g_damage_card_internal_card_id + 0x15;
  g_duel_generated_internal_card_id_16 = g_damage_card_internal_card_id + 0x16;
  g_duel_generated_internal_card_id_17 = g_damage_card_internal_card_id + 0x17;
  g_duel_generated_internal_card_id_18 = g_damage_card_internal_card_id + 0x18;
  g_duel_generated_internal_card_id_19 = g_damage_card_internal_card_id + 0x19;
  g_duel_generated_internal_card_id_1a = g_damage_card_internal_card_id + 0x1a;
  g_duel_generated_iid_1b = g_damage_card_internal_card_id + 0x1b;
  g_duel_generated_internal_card_id_1c = g_damage_card_internal_card_id + 0x1c;
  g_duel_generated_internal_card_id_1d = g_damage_card_internal_card_id + 0x1d;
  g_duel_generated_internal_card_id_1e = g_damage_card_internal_card_id + 0x1e;
  g_duel_generated_internal_card_id_1f = g_damage_card_internal_card_id + 0x1f;
  g_duel_generated_internal_card_id_20 = g_damage_card_internal_card_id + 0x20;
  g_duel_generated_iid_21 = g_damage_card_internal_card_id + 0x21;
  g_duel_generated_internal_card_id_22 = g_damage_card_internal_card_id + 0x22;
  g_duel_generated_internal_card_id_23 = g_damage_card_internal_card_id + 0x23;
  g_duel_generated_internal_card_id_24 = g_damage_card_internal_card_id + 0x24;
  g_duel_generated_internal_card_id_25 = g_damage_card_internal_card_id + 0x25;
  g_duel_generated_internal_card_id_26 = g_damage_card_internal_card_id + 0x26;
  g_control_aura_special_internal_card_id = g_damage_card_internal_card_id + 0x27;
  g_duel_generated_internal_card_id_28 = g_damage_card_internal_card_id + 0x28;
  g_duel_generated_internal_card_id_29 = g_damage_card_internal_card_id + 0x29;
  g_duel_generated_internal_card_id_2a = g_damage_card_internal_card_id + 0x2a;
  g_control_aura_legacy_internal_card_id = g_damage_card_internal_card_id + 0x2b;
  g_duel_generated_internal_card_id_2c = g_damage_card_internal_card_id + 0x2c;

  g_damage_display_internal_card_id = 0x385;
  g_legacy_display_internal_card_id = 0x386;
  g_effect_display_internal_card_id = 0x387;
  g_card_back_display_internal_card_id = 0x388;
  g_hunting_display_internal_card_id = 0x389;
  g_activation_display_internal_card_id = 0x38a;
  g_multiblock_display_internal_card_id = 0x38b;

  g_ai_combat_value_weights[0] = 8;
  g_ai_combat_value_weights[1] = 8;
  g_ai_combat_value_weights[2] = 0xc;
  g_ai_combat_value_weights[3] = 0xc;

  g_produced_mana_card = -1;
  g_produced_mana_color_valid = -1;
  g_produced_mana_color = -1;
  g_other_player = 1;
  g_adventure_duel_internal_card_id = -1;
  g_response_card_type_mask = 0x30;
  g_duel_state_00742fc0 = 1;
  g_game_time_scale = 2;
}
