#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/network.h"
#include "magic/src/shared_startup.h"

#define CARDCLASS_PLAYER_WINDOW_LONG_OFFSET g_cardclass_player_window_long_offset
#define CARDCLASS_CARD_WINDOW_LONG_OFFSET g_cardclass_card_window_long_offset
#define CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET g_cardclass_hidden_parent_window_long_offset
#define CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET g_cardclass_snapshot_window_long_offset
#define CARDCLASS_PREVIOUS_Z_ORDER_WINDOW_LONG_OFFSET g_cardclass_previous_z_order_window_long_offset

typedef BOOL (WINAPI *PtInRectByCoordsProc)(const RECT *rect, LONG x, LONG y);

#define PtInRectByCoords(rect_, x_, y_) \
  (((PtInRectByCoordsProc)PtInRect)((rect_), (LONG)(x_), (LONG)(y_)))

int load_text_with_tab_escapes(char *filename, char *section_name);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void draw_duel_face_window(HDC dc, RECT *rect, int player);
int FUN_004483be(int player, int card);
unsigned int FUN_00448c93(int *displayed_player_and_card, int player, int card);
int get_displayed_card_id(int player, int card);
int get_displayed_card_internal_id(int player, int card);
unsigned int get_displayed_card_special_counters(int player, int card);
unsigned int get_displayed_card_display_flags(int player, int card);
unsigned int get_displayed_card_ui_flags(int player, int card);
int FUN_004486de(int player, int card);
unsigned char FUN_00448df6(int player, int card);
int get_displayed_card_zone(int player, int card);
int FUN_00449057(int player, int card);
unsigned int FUN_00449ac3(int player, int card);
void FUN_00449a0f(int *displayed_player_and_card, int player, int card);
void get_current_duel_selection(int *selected_player, int *selected_card);
LONG get_card_window_hidden_flag(HWND hwnd);
void FUN_004d1263(HWND hwnd);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
void FUN_0055a986(HDC dc, int *rect, int card_id, unsigned int player, int card);
void FUN_0055af78(HDC dc, int *rect, int card_id, unsigned int player, int card, int attached_player, int attached_card);
void FUN_0055b1e4(HDC dc, int *rect, unsigned int player, int card, int ui_flags,
                  unsigned int target_ui_flags, int parent_card, int activation, int upkeep);
void FUN_0055d393(HDC dc, int *rect, int highlighted);
void FUN_0055d429(HDC dc, int *rect, unsigned char untap_status);
void FUN_0055d910(int bmi, int bits, int *rect);
void FUN_00494cf0(int bmi, int bits, RECT *rect);
unsigned int draw_target_canttarget(HDC dc, int *rect, int is_target, int can_target);
void draw_id_tag(HDC dc, int *rect, int player, int card, int enabled);
void FUN_004d2385(HDC dc, int rect, int player, int card);
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2);
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd);
void notify_duel_action(int player, unsigned int value);
unsigned int refresh_duel_display_cache(void);
void set_duel_prompt_context(HWND hwnd, char *prompt, unsigned int mode);
void set_duel_tooltip_text(char *text);
void FUN_004480a6(char *text);
int FUN_004e698f();
int FUN_004418c6();
int FUN_004d7953(int player, int card, int unused_player, int unused_card);
int FUN_0044184B(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
int FUN_00441b94(int blocker_player, int blocker_card, int attacker_player, int attacker_card);

extern char DAT_00708da8[];
extern char DAT_00709100[];
extern char global_base_directory[];
extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[2000];
extern BITMAPINFO g_duel_backbuffer_bmi;
extern int g_duel_modal_action_active;
extern char g_duel_action_request_copy[];
extern int DAT_007a7d74;

#define CARDCLASS_STATE(player, card) \
  (*(int *)((char *)&global_card_instances[0][(card)] + (player) * 0xb0f4 + 8))
#define CARDCLASS_DISPLAYED_STATE(player, card) \
  (*(int *)((char *)&global_displayed_card_instances[0][(card)] + (player) * 0xb0f4 + 8))
#define CARDCLASS_TOKEN_STATUS(player, card) \
  (*(int *)((char *)&global_card_instances[0][(card)] + (player) * 0xb0f4 + 0x18))
#define CARDCLASS_BLOCKING(player, card) \
  (*(unsigned char *)((char *)&global_card_instances[0][(card)] + (player) * 0xb0f4 + 0x24))
#define CARDCLASS_INTERNAL_CARD_ID(player, card) \
  (*(int *)((char *)&global_card_instances[0][(card)] + (player) * 0xb0f4 + 0x6c))

// GLOBAL: MAGIC 0x0055e0fc
int g_cardclass_player_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e100
int g_cardclass_card_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e104
int g_cardclass_hidden_parent_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e108
int g_cardclass_snapshot_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e10c
int g_cardclass_previous_z_order_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e110
int g_cardclass_window_extra_bytes = 0x14;

// GLOBAL: MAGIC 0x00575100
char s_Type_00575100[8] = "Type ";

// GLOBAL: MAGIC 0x00575108
char s_Summoned_00575108[0xc] = "Summoned ";

// GLOBAL: MAGIC 0x00575114
char s_Attacking_Bit_00575114[0x10] = "Attacking Bit ";

// GLOBAL: MAGIC 0x00575124
char s_Tapped_Bit_00575124[0xc] = "Tapped Bit ";

// GLOBAL: MAGIC 0x00575130
char s_Owner_00575130[8] = "Owner ";

// GLOBAL: MAGIC 0x00575138
char s_Targeted_00575138[0xc] = "Targeted ";

// GLOBAL: MAGIC 0x00575144
char s_MayNotTarget_00575144[0x10] = "MayNotTarget ";

// GLOBAL: MAGIC 0x00575154
char s_Damage_00575154[8] = "Damage ";

// GLOBAL: MAGIC 0x0057515c
char s_Power_0057515c[8] = "Power ";

// GLOBAL: MAGIC 0x00575164
char s_Tough_00575164[8] = "Tough ";

// GLOBAL: MAGIC 0x0057516c
char s_Mana_0057516c[8] = "Mana ";

// GLOBAL: MAGIC 0x00575174
char s_Color_00575174[8] = "Color ";

// GLOBAL: MAGIC 0x0057517c
char s_Blocking_0057517c[0xc] = "Blocking ";

// GLOBAL: MAGIC 0x00575188
char s_Hack_00575188[8] = "Hack ";

// GLOBAL: MAGIC 0x00575190
char s_Data_00575190[8] = "Data ";

// GLOBAL: MAGIC 0x00575198
char s_Counters_00575198[0xc] = "Counters ";

// GLOBAL: MAGIC 0x005751a4
char s_HighLight_005751a4[0xc] = "HighLight ";

// GLOBAL: MAGIC 0x005751b0
char s_LinkedID_005751b0[0xc] = "LinkedID ";

// GLOBAL: MAGIC 0x005751bc
char s_UntapStatus_005751bc[0x10] = "UntapStatus ";

// GLOBAL: MAGIC 0x005751cc
char s_Dying_005751cc[8] = "Dying ";

// GLOBAL: MAGIC 0x005751d4
char s_Empty_005751d4[4] = "";

// GLOBAL: MAGIC 0x005751d8
char s_MENU_SMALLCARD_005751d8[0x10] = "MENU_SMALLCARD";

// GLOBAL: MAGIC 0x005751e8
char s_PROMPT_BANDWITHWHOM_005751e8[0x14] = "PROMPT_BANDWITHWHOM";

// GLOBAL: MAGIC 0x005751fc
char s_PROMPT_DEFENDWHOM_005751fc[0x14] = "PROMPT_DEFENDWHOM";

// GLOBAL: MAGIC 0x00575210
char s_CUECARD_COUNTERS_ArmageddonClock_00575210[0x24] = "CUECARD_COUNTERS_ArmageddonClock";

// GLOBAL: MAGIC 0x00575234
char s_CUECARD_COUNTERS_ManaBattery_00575234[0x20] = "CUECARD_COUNTERS_ManaBattery";

// GLOBAL: MAGIC 0x00575254
char s_CUECARD_COUNTERS_ClockworkAvian_00575254[0x20] = "CUECARD_COUNTERS_ClockworkAvian";

// GLOBAL: MAGIC 0x00575274
char s_CUECARD_COUNTERS_ClockworkBeast_00575274[0x20] = "CUECARD_COUNTERS_ClockworkBeast";

// GLOBAL: MAGIC 0x00575294
char s_CUECARD_COUNTERS_LuckyCharms_00575294[0x20] = "CUECARD_COUNTERS_LuckyCharms";

// GLOBAL: MAGIC 0x005752b4
char s_CUECARD_COUNTERS_Fungusaur_005752b4[0x1c] = "CUECARD_COUNTERS_Fungusaur";

// GLOBAL: MAGIC 0x005752d0
char s_CUECARD_COUNTERS_WhirlingDervish_005752d0[0x24] = "CUECARD_COUNTERS_WhirlingDervish";

// GLOBAL: MAGIC 0x005752f4
char s_CUECARD_COUNTERS_LivingArtifact_005752f4[0x20] = "CUECARD_COUNTERS_LivingArtifact";

// GLOBAL: MAGIC 0x00575314
char s_CUECARD_COUNTERS_OsaiVultures_00575314[0x20] = "CUECARD_COUNTERS_OsaiVultures";

// GLOBAL: MAGIC 0x00575334
char s_CUECARD_COUNTERS_ScavengingGhoul_00575334[0x24] = "CUECARD_COUNTERS_ScavengingGhouls";

// GLOBAL: MAGIC 0x00575358
char s_CUECARD_COUNTERS_SengirVampire_00575358[0x20] = "CUECARD_COUNTERS_SengirVampire";

// GLOBAL: MAGIC 0x00575378
char s_CUECARD_COUNTERS_NecropolisOfAza_00575378[0x24] = "CUECARD_COUNTERS_NecropolisOfAzar";

// GLOBAL: MAGIC 0x0057539c
char s_CUECARD_COUNTERS_Triskelion_0057539c[0x1c] = "CUECARD_COUNTERS_Triskelion";

// GLOBAL: MAGIC 0x005753b8
char s_CUECARD_COUNTERS_Tetravus_005753b8[0x1c] = "CUECARD_COUNTERS_Tetravus";

// GLOBAL: MAGIC 0x005753d4
char s_CUECARD_COUNTERS_TimeVault_005753d4[0x1c] = "CUECARD_COUNTERS_TimeVault";

// GLOBAL: MAGIC 0x005753f0
char s_CUECARD_COUNTERS_Cyclone_005753f0[0x1c] = "CUECARD_COUNTERS_Cyclone";

// GLOBAL: MAGIC 0x0057540c
char s_CUECARD_COUNTERS_CitanulDruid_0057540c[0x20] = "CUECARD_COUNTERS_CitanulDruid";

// GLOBAL: MAGIC 0x0057542c
char s_CUECARD_COUNTERS_RockHydra_0057542c[0x1c] = "CUECARD_COUNTERS_RockHydra";

// GLOBAL: MAGIC 0x00575448
char s_CUECARD_COUNTERS_KhabalGhoul_00575448[0x20] = "CUECARD_COUNTERS_KhabalGhoul";

// GLOBAL: MAGIC 0x00575468
char s_CUECARD_COUNTERS_OrcishCatapult_00575468[0x20] = "CUECARD_COUNTERS_OrcishCatapult";

// GLOBAL: MAGIC 0x00575488
char s_CUECARD_COUNTERS_UnstableMutatio_00575488[0x24] = "CUECARD_COUNTERS_UnstableMutation";

// GLOBAL: MAGIC 0x005754ac
char s_CUECARD_COUNTERS_SpiritShackle_005754ac[0x20] = "CUECARD_COUNTERS_SpiritShackle";

// GLOBAL: MAGIC 0x005754cc
char s_CUECARD_COUNTERS_DwarvenWeaponsm_005754cc[0x24] = "CUECARD_COUNTERS_DwarvenWeaponsmith";

// GLOBAL: MAGIC 0x005754f0
char s_CUECARD_COUNTERS_AshnodsTransmor_005754f0[0x28] = "CUECARD_COUNTERS_AshnodsTransmorgrant";

// GLOBAL: MAGIC 0x00575518
char s_Comma_00575518[4] = ",";

// GLOBAL: MAGIC 0x0057551c
char s__duel_hlp_0057551c[0xc] = "\\duel.hlp";

// GLOBAL: MAGIC 0x00575528
char s__M__Add_mana_for_this_card_00575528[0x1c] = "&M: Add mana for this card";

// GLOBAL: MAGIC 0x00575544
char s__B__Bury_this_card_00575544[0x14] = "&B: Bury this card";

// GLOBAL: MAGIC 0x00575558
char s__M__Add_mana_for_this_card_00575558[0x1c] = "&M: Add mana for this card";

// GLOBAL: MAGIC 0x00575574
char s__T__Tap_untap_this_card_00575574[0x18] = "&T: Tap/untap this card";

// GLOBAL: MAGIC 0x0057558c
char s__B__Bury_this_card_0057558c[0x14] = "&B: Bury this card";

// GLOBAL: MAGIC 0x005755a0
char s__X__Increment_counters_for_this_c_005755a0[0x28] = "&X: Increment counters for this card";

// GLOBAL: MAGIC 0x005755c8
char s__s__s_005755c8[0xc] = "\n\n%s %s\n";

// GLOBAL: MAGIC 0x005755d4
char s__s__d__d_005755d4[0xc] = "%s %d %d ";

// GLOBAL: MAGIC 0x005755e0
char s__s_005755e0[8] = "%s";

// GLOBAL: MAGIC 0x005755e8
char s__d__d_005755e8[8] = "%d,%d";

// GLOBAL: MAGIC 0x005755f0
char s_Times_New_Roman_005755f0[0x10] = "Times New Roman";

// GLOBAL: MAGIC 0x0069b3b8
char g_cardclass_menu_view_stats_text[0x68];

// GLOBAL: MAGIC 0x0069b420
char g_cardclass_menu_view_card_text[0x68];

// GLOBAL: MAGIC 0x0069b488
char g_cardclass_cuecard_counter_tetravus_text[0x68];

// GLOBAL: MAGIC 0x0069b4f0
char g_cardclass_prompt_defend_attacker_text[0x68];

// GLOBAL: MAGIC 0x0069b558
char g_cardclass_cuecard_counter_unstable_mutation_text[0x68];

// GLOBAL: MAGIC 0x0069b5c0
char g_cardclass_cuecard_counter_ashnods_transmogrant_text[0x68];

// GLOBAL: MAGIC 0x0069b628
char g_cardclass_cuecard_counter_rock_hydra_text[0x68];

// GLOBAL: MAGIC 0x0069b690
char g_cardclass_cuecard_counter_cyclone_text[0x68];

// GLOBAL: MAGIC 0x0069b6f8
char g_cardclass_menu_autotarget_text[0x68];

// GLOBAL: MAGIC 0x0069b760
char g_cardclass_cuecard_counter_orcish_catapult_text[0x64];

// GLOBAL: MAGIC 0x0069b7c4
HMENU g_cardclass_alternate_card_submenu;

// GLOBAL: MAGIC 0x0069b7c8
char g_cardclass_menu_protect_text[0x68];

// GLOBAL: MAGIC 0x0069b830
char g_cardclass_cuecard_counter_lucky_charms_text[0x68];

// GLOBAL: MAGIC 0x0069b898
char g_cardclass_prompt_band_whom_text[0x68];

// GLOBAL: MAGIC 0x0069b900
char g_cardclass_cuecard_counter_citanul_druid_text[0x68];

// GLOBAL: MAGIC 0x0069b968
int g_cardclass_ability_masks[0x11];

// GLOBAL: MAGIC 0x0069b9b0
char g_cardclass_cuecard_counter_osai_vultures_text[0x68];

// GLOBAL: MAGIC 0x0069ba18
char g_cardclass_prompt_defend_error_text[0x68];

// GLOBAL: MAGIC 0x0069ba80
char g_cardclass_prompt_band_error_text[0x68];

// GLOBAL: MAGIC 0x0069bae8
char g_cardclass_cuecard_counter_clockwork_avian_text[0x68];

// GLOBAL: MAGIC 0x0069bb50
char g_cardclass_cuecard_counter_armageddon_clock_text[0x68];

// GLOBAL: MAGIC 0x0069bbb8
char g_cardclass_cuecard_counter_clockwork_beast_text[0x68];

// GLOBAL: MAGIC 0x0069bc20
char g_cardclass_cuecard_counter_spirit_shackle_text[0x68];

// GLOBAL: MAGIC 0x0069bc88
char g_cardclass_cuecard_counter_dwarven_weaponsmith_text[0x68];

// GLOBAL: MAGIC 0x0069bcf0
char g_cardclass_menu_help_text[0x64];

// GLOBAL: MAGIC 0x0069bd54
HMENU g_cardclass_popup_menu;

// GLOBAL: MAGIC 0x0069bd58
char g_cardclass_prompt_defend_prompt_text[0x68];

// GLOBAL: MAGIC 0x0069bdc0
char g_cardclass_cuecard_counter_triskelion_text[0x70];

// GLOBAL: MAGIC 0x0069be30
char g_cardclass_cuecard_counter_sengir_vampire_text[0x68];

// GLOBAL: MAGIC 0x0069be98
char g_cardclass_cuecard_counter_khabal_ghoul_text[0x68];

// GLOBAL: MAGIC 0x0069bf00
char g_cardclass_menu_show_damage_text[0x68];

// GLOBAL: MAGIC 0x0069bf68
char g_cardclass_cuecard_counter_living_artifact_text[0x68];

// GLOBAL: MAGIC 0x0069bfd0
char g_cardclass_cuecard_counter_time_vault_text[0x68];

// GLOBAL: MAGIC 0x0069c038
char g_cardclass_prompt_band_cannot_text[0x68];

// GLOBAL: MAGIC 0x0069c0a0
char g_cardclass_cuecard_counter_fungusaur_text[0x68];

// GLOBAL: MAGIC 0x0069c108
char g_cardclass_cuecard_counter_mana_battery_text[0x68];

// GLOBAL: MAGIC 0x0069c170
char g_cardclass_cuecard_counter_whirling_dervish_text[0x68];

// GLOBAL: MAGIC 0x0069c1d8
char g_cardclass_menu_id_tags_text[0x78];

// GLOBAL: MAGIC 0x0069c250
char g_cardclass_cuecard_counter_scavenging_ghouls_text[0x68];

// GLOBAL: MAGIC 0x0069c2b8
char g_cardclass_cuecard_counter_necropolis_of_azar_text[0x68];

// GLOBAL: MAGIC 0x0069c320
char g_cardclass_menu_alternate_card_text[0x68];

// GLOBAL: MAGIC 0x005750b0
char *PTR_s_Type_005750b0[20] =
{
  s_Type_00575100,
  s_Summoned_00575108,
  s_Attacking_Bit_00575114,
  s_Tapped_Bit_00575124,
  s_Owner_00575130,
  s_Targeted_00575138,
  s_MayNotTarget_00575144,
  s_Damage_00575154,
  s_Power_0057515c,
  s_Tough_00575164,
  s_Mana_0057516c,
  s_Color_00575174,
  s_Blocking_0057517c,
  s_Hack_00575188,
  s_Data_00575190,
  s_Counters_00575198,
  s_HighLight_005751a4,
  s_LinkedID_005751b0,
  s_UntapStatus_005751bc,
  s_Dying_005751cc
};

// FUNCTION: MAGIC 0x00449e0a
int FUN_00449e0a(void *snapshot, int player, int card)
{
  if (snapshot == NULL)
  {
    return 0;
  }

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  memcpy(snapshot, &global_displayed_card_instances[player][card], sizeof(card_instance_t));
  LeaveCriticalSection(&g_duel_render_lock);
  return 1;
}

// FUNCTION: MAGIC 0x004d1889
unsigned int FUN_004d1889(int *old_snapshot, int *new_snapshot)
{
  unsigned int changed_flags;
  int changed_index;
  char debug_text[256];

  changed_flags = 0;
  if (new_snapshot[0x1b] == old_snapshot[0x1b])
  {
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x30000U)) & 0x30000U) != 0)
    {
      changed_flags = 2;
    }
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x10U)) & 0x10U) != 0)
    {
      changed_flags |= 8;
    }
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x1000U)) & 0x1000U) != 0)
    {
      changed_flags |= 0x10;
    }
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x200000U)) & 0x200000U) != 0)
    {
      changed_flags |= 0x20;
    }
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x100000U)) & 0x100000U) != 0)
    {
      changed_flags |= 0x40;
    }
    if (((old_snapshot[2] ^ (new_snapshot[2] & 0x800000U)) & 0x800000U) != 0)
    {
      changed_flags |= 0x40;
    }
    if ((short)new_snapshot[4] != (short)old_snapshot[4])
    {
      changed_flags |= 0x80;
    }
    if ((short)new_snapshot[0xd] != (short)old_snapshot[0xd])
    {
      changed_flags |= 0x100;
    }
    if (*(short *)((char *)new_snapshot + 0xe) != *(short *)((char *)old_snapshot + 0xe))
    {
      changed_flags |= 0x200;
    }
    if ((char)new_snapshot[0x16] != (char)old_snapshot[0x16])
    {
      changed_flags |= 0x400;
    }
    if (*(char *)((char *)new_snapshot + 0x1e) != *(char *)((char *)old_snapshot + 0x1e))
    {
      changed_flags |= 0x800;
    }
    if ((char)new_snapshot[9] != (char)old_snapshot[9])
    {
      changed_flags |= 0x1000;
    }
    if (new_snapshot[0xe] != old_snapshot[0xe])
    {
      changed_flags |= 0x4000;
    }
    if (*new_snapshot != *old_snapshot)
    {
      changed_flags |= 0x8000;
    }
    if (new_snapshot[0x48] != old_snapshot[0x48])
    {
      changed_flags |= 0x8000;
    }
    if (((new_snapshot[6] ^ (old_snapshot[6] & 0x20000U)) & 0x20000U) != 0)
    {
      changed_flags |= 0x10000;
    }
    if (new_snapshot[0x19] != old_snapshot[0x19])
    {
      changed_flags |= 0x20000;
    }
    if (new_snapshot[0x47] != old_snapshot[0x47])
    {
      changed_flags |= 0x40000;
    }
    if ((char)new_snapshot[0x1a] != (char)old_snapshot[0x1a])
    {
      changed_flags |= 0x80000;
    }
  }
  else
  {
    changed_flags = 1;
  }

  sprintf(debug_text, s__s__s_005755c8,
          global_cards_data[old_snapshot[0x1b]].name,
          global_cards_data[new_snapshot[0x1b]].name);
  OutputDebugStringA(debug_text);
  for (changed_index = 0; changed_index < 0x20; changed_index++)
  {
    if ((changed_flags & (1 << (unsigned char)changed_index)) != 0)
    {
      if (changed_index == 0)
      {
        sprintf(debug_text, s__s__d__d_005755d4, PTR_s_Type_005750b0[0],
                old_snapshot[0x1b], new_snapshot[0x1b]);
      }
      else
      {
        sprintf(debug_text, s__s_005755e0, PTR_s_Type_005750b0[changed_index]);
      }
      OutputDebugStringA(debug_text);
    }
  }
  return changed_flags;
}

// FUNCTION: MAGIC 0x004d15b3
int FUN_004d15b3(int *old_snapshot, int *new_snapshot)
{
  int unchanged;

  unchanged = 1;
  if (new_snapshot[0x1b] != old_snapshot[0x1b])
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x30000U)) & 0x30000U) != 0)
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x10U)) & 0x10U) != 0)
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x1000U)) & 0x1000U) != 0)
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x200000U)) & 0x200000U) != 0)
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x100000U)) & 0x100000U) != 0)
  {
    unchanged = 0;
  }
  if (((new_snapshot[2] ^ (old_snapshot[2] & 0x800000U)) & 0x800000U) != 0)
  {
    unchanged = 0;
  }
  if ((short)new_snapshot[4] != (short)old_snapshot[4])
  {
    unchanged = 0;
  }
  if ((short)new_snapshot[0xd] != (short)old_snapshot[0xd])
  {
    unchanged = 0;
  }
  if (*(short *)((char *)new_snapshot + 0xe) != *(short *)((char *)old_snapshot + 0xe))
  {
    unchanged = 0;
  }
  if ((char)new_snapshot[0x16] != (char)old_snapshot[0x16])
  {
    unchanged = 0;
  }
  if (*(char *)((char *)new_snapshot + 0x1e) != *(char *)((char *)old_snapshot + 0x1e))
  {
    unchanged = 0;
  }
  if ((char)new_snapshot[9] != (char)old_snapshot[9])
  {
    unchanged = 0;
  }
  if (new_snapshot[0xe] != old_snapshot[0xe])
  {
    unchanged = 0;
  }
  if (*new_snapshot != *old_snapshot)
  {
    unchanged = 0;
  }
  if (new_snapshot[0x48] != old_snapshot[0x48])
  {
    unchanged = 0;
  }
  if (((new_snapshot[6] ^ (old_snapshot[6] & 0x20000U)) & 0x20000U) != 0)
  {
    unchanged = 0;
  }
  if (new_snapshot[0x19] != old_snapshot[0x19])
  {
    unchanged = 0;
  }
  if (new_snapshot[0x47] != old_snapshot[0x47])
  {
    unchanged = 0;
  }
  if ((char)new_snapshot[0x1a] != (char)old_snapshot[0x1a])
  {
    unchanged = 0;
  }
  if (unchanged == 0 && g_duel_palette_refresh_pending != 0)
  {
    FUN_004d1889(old_snapshot, new_snapshot);
  }
  return unchanged;
}

// FUNCTION: MAGIC 0x004d1c68
int FUN_004d1c68(int old_snapshot, int new_snapshot)
{
  int unchanged;

  unchanged = 1;
  if (((int *)new_snapshot)[0xa] != ((int *)old_snapshot)[0xa])
  {
    unchanged = 0;
  }
  return unchanged;
}

// FUNCTION: MAGIC 0x004d1314
int FUN_004d1314(HWND hwnd)
{
  int player;
  int card;
  int type_flags;
  unsigned int color_flags;
  HWND parent;
  int owner_selector;
  int zone_flags;
  int result;
  int *action_request;

  player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
  card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
  type_flags = FUN_00448df6(player, card);
  color_flags = FUN_00449057(player, card);
  owner_selector = -1;
  zone_flags = -1;
  parent = GetParent(hwnd);
  if (g_duel_full_card_window_hwnd == parent)
  {
    owner_selector = 0;
    zone_flags = 2;
  }
  else if (g_duel_life_window_hwnd == parent)
  {
    owner_selector = 1;
    zone_flags = 2;
  }
  else if (g_duel_player_battlefield_window_hwnd == parent)
  {
    owner_selector = 0;
    zone_flags = 1;
  }
  else if (g_duel_help_owner_hwnd == parent)
  {
    owner_selector = 1;
    zone_flags = 1;
  }
  else if (parent == g_duel_player_graveyard_window_hwnd)
  {
    owner_selector = 0;
    zone_flags = 0x10;
  }
  else if (g_phasebar_your_untap_window_hwnd == parent)
  {
    owner_selector = 1;
    zone_flags = 0x10;
  }

  action_request = (int *)g_duel_action_request_copy;
  if ((action_request[0] == -1 || action_request[0] == player) &&
      (action_request[1] == -1 ||
       action_request[1] == 0 ||
       (type_flags & action_request[1]) != 0) &&
      (action_request[2] == -1 ||
       action_request[2] == 0 ||
       (color_flags & action_request[2]) != 0 ||
       ((action_request[2] & 0x100) != 0 && (type_flags & 0x40) != 0) ||
       ((action_request[2] & 0x200) != 0 && (type_flags & 1) != 0)) &&
      (action_request[3] == -1 || action_request[3] == owner_selector) &&
      (action_request[4] == -1 || (zone_flags & action_request[4]) != 0))
  {
    result = 1;
  }
  else
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x004d1263
void FUN_004d1263(HWND hwnd)
{
  int attached_player_and_card[2];
  int card_id;
  int player;
  int card;
  int *action_packet;

  player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
  card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
  card_id = get_displayed_card_id(player, card);
  action_packet = (int *)(g_cardclass_menu_id_tags_text + 0x68);
  if (unk_0092666c == card_id)
  {
    FUN_00449a0f(attached_player_and_card, player, card);
    action_packet[1] = attached_player_and_card[0];
    action_packet[2] = attached_player_and_card[1];
  }
  else
  {
    action_packet[1] = player;
    action_packet[2] = card;
  }
  action_packet[0] = 0;
  PostMessageA(g_duel_prompt_context_hwnd, 0x464, 0, (LPARAM)action_packet);
}

// FUNCTION: MAGIC 0x0044850c
unsigned int FUN_0044850c(int player, int card)
{
  unsigned int counters;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  counters = global_displayed_card_instances[player][card].special_counters & 0xff;
  LeaveCriticalSection(&g_duel_render_lock);
  return counters;
}

// FUNCTION: MAGIC 0x0044858d
void FUN_0044858d(int player,
                  int card,
                  unsigned int *counter_power,
                  unsigned int *counter_m1m1,
                  unsigned int *counter_p1p1,
                  unsigned int *counter_p0p1,
                  unsigned int *counter_p1p0)
{
  if (FUN_004483be(player, card) == 0)
  {
    EnterCriticalSection(&g_duel_render_lock);
    *counter_power = (unsigned char)global_displayed_card_instances[player][card].counter_power;
    *counter_m1m1 = (global_displayed_card_instances[player][card].special_counters & 0xff0000) >> 0x10;
    *counter_p1p1 = global_displayed_card_instances[player][card].special_counters >> 0x18;
    *counter_p0p1 = global_displayed_card_instances[player][card].counters & 0xff;
    *counter_p1p0 = (unsigned char)global_displayed_card_instances[player][card].counters5;
    LeaveCriticalSection(&g_duel_render_lock);
  }
}

// FUNCTION: MAGIC 0x0044875b
int FUN_0044875b(int player, int card)
{
  int damage_on_card;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  damage_on_card = global_displayed_card_instances[player][card].damage_on_card;
  LeaveCriticalSection(&g_duel_render_lock);
  return damage_on_card;
}

// FUNCTION: MAGIC 0x00448df6
unsigned char FUN_00448df6(int player, int card)
{
  int internal_card_id;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  internal_card_id = get_displayed_card_internal_id(player, card);
  if (internal_card_id == -1)
  {
    return 0;
  }

  return global_cards_data[internal_card_id].type;
}

// FUNCTION: MAGIC 0x00448f5c
unsigned int FUN_00448f5c(int player, int card)
{
  unsigned int regen_status;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  regen_status = global_displayed_card_instances[player][card].regen_status;
  if ((global_displayed_card_instances[player][card].regen_status & 0x200000) != 0 &&
      (global_displayed_card_instances[player][card].state & 4) != 0)
  {
    regen_status |= 0x40;
  }
  if (regen_status == 0xffffffff)
  {
    regen_status = 0;
  }
  LeaveCriticalSection(&g_duel_render_lock);
  return regen_status;
}

// FUNCTION: MAGIC 0x00449151
unsigned int FUN_00449151(int player, int card)
{
  unsigned int state;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  state = global_displayed_card_instances[player][card].state;
  LeaveCriticalSection(&g_duel_render_lock);
  return state;
}

// FUNCTION: MAGIC 0x0044931a
unsigned int FUN_0044931a(int player, int card)
{
  unsigned int state;

  if (FUN_004483be(player, card) != 0)
  {
    return 0xffffffff;
  }

  state = FUN_00449151(player, card);
  return (state & 0x1000) != 0;
}

// FUNCTION: MAGIC 0x004494a3
unsigned int FUN_004494a3(int player, int card)
{
  unsigned int info_slot;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  info_slot = global_displayed_card_instances[player][card].info_slot;
  LeaveCriticalSection(&g_duel_render_lock);
  return info_slot;
}

// FUNCTION: MAGIC 0x0044951f
int FUN_0044951f(int player, int card)
{
  int result;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  result = (global_displayed_card_instances[player][card].state & 0x200000) != 0;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x004495b2
int FUN_004495b2(int player, int card)
{
  int result;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  result = (global_displayed_card_instances[player][card].state & 0x100000) == 0;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x00449914
unsigned int FUN_00449914(int player, int card)
{
  unsigned int untap_status;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  untap_status = global_displayed_card_instances[player][card].untap_status;
  LeaveCriticalSection(&g_duel_render_lock);
  return untap_status;
}

// FUNCTION: MAGIC 0x00449b42
int FUN_00449b42(int player, int card)
{
  int kill_code;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  kill_code = (int)(char)global_displayed_card_instances[player][card].kill_code;
  LeaveCriticalSection(&g_duel_render_lock);
  return kill_code;
}

// FUNCTION: MAGIC 0x004d1d5d
void FUN_004d1d5d(char *text, int card_id, unsigned int counters)
{
  if (text != NULL)
  {
    if (card_id == 0x1d7)
    {
      sprintf(text, g_cardclass_cuecard_counter_armageddon_clock_text, counters);
    }
    else if (card_id == 0x244 || card_id == 0x248 || card_id == 0x296 ||
             card_id == 0x2fd || card_id == 0x354)
    {
      sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
    }
    else if (card_id == 0x1e4)
    {
      sprintf(text, g_cardclass_cuecard_counter_clockwork_avian_text, counters);
    }
    else if (card_id == 0x26)
    {
      sprintf(text, g_cardclass_cuecard_counter_clockwork_beast_text, counters);
    }
    else if (card_id == 0x34 || card_id == 0x7b || card_id == 0x80 ||
             card_id == 0xf6 || card_id == 0x120)
    {
      sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
    }
    else if (card_id == 0x5e)
    {
      sprintf(text, g_cardclass_cuecard_counter_fungusaur_text, counters);
    }
    else if (card_id == 0x353)
    {
      sprintf(text, g_cardclass_cuecard_counter_whirling_dervish_text, counters);
    }
    else if (card_id == 0x92)
    {
      sprintf(text, g_cardclass_cuecard_counter_living_artifact_text, counters);
    }
    else if (card_id == 0x2e0)
    {
      sprintf(text, g_cardclass_cuecard_counter_osai_vultures_text, counters);
    }
    else if (card_id == 0xd7)
    {
      sprintf(text, g_cardclass_cuecard_counter_scavenging_ghouls_text, counters);
    }
    else if (card_id == 0xdc)
    {
      sprintf(text, g_cardclass_cuecard_counter_sengir_vampire_text, counters);
    }
    else if (card_id == 0x367)
    {
      sprintf(text, g_cardclass_cuecard_counter_necropolis_of_azar_text, counters);
    }
    else if (card_id == 0x21a)
    {
      sprintf(text, g_cardclass_cuecard_counter_triskelion_text, counters);
    }
    else if (card_id == 0x216)
    {
      sprintf(text, g_cardclass_cuecard_counter_tetravus_text, counters);
    }
    else if (card_id == 0xf8)
    {
      sprintf(text, g_cardclass_cuecard_counter_time_vault_text, counters);
    }
    else if (card_id == 0x195)
    {
      sprintf(text, g_cardclass_cuecard_counter_cyclone_text, counters);
    }
    else if (card_id == 0x1e2)
    {
      sprintf(text, g_cardclass_cuecard_counter_citanul_druid_text, counters);
    }
    else if (card_id == 0xcf)
    {
      sprintf(text, g_cardclass_cuecard_counter_rock_hydra_text, counters);
    }
    else if (card_id == 0x1b3)
    {
      sprintf(text, g_cardclass_cuecard_counter_khabal_ghoul_text, counters);
    }
    else
    {
      text[0] = '\0';
    }
  }
}

// FUNCTION: MAGIC 0x004d21a9
void FUN_004d21a9(char *text, int counter_type, int counters)
{
  if (text != NULL)
  {
    strcpy(text, "");
    if (counters != 0)
    {
      if (counter_type == 1)
      {
        sprintf(text, g_cardclass_cuecard_counter_orcish_catapult_text, counters);
      }
      else if (counter_type == 2)
      {
        sprintf(text, g_cardclass_cuecard_counter_unstable_mutation_text, counters);
      }
      else if (counter_type == 3)
      {
        sprintf(text, g_cardclass_cuecard_counter_spirit_shackle_text, counters);
      }
      else if (counter_type == 4)
      {
        sprintf(text, g_cardclass_cuecard_counter_dwarven_weaponsmith_text, counters);
      }
      else if (counter_type == 5)
      {
        sprintf(text, g_cardclass_cuecard_counter_ashnods_transmogrant_text, counters);
      }
    }
  }
}

// FUNCTION: MAGIC 0x0055bcb1
void FUN_0055bcb1(LPRECT rect, int *client_rect)
{
  int width;
  int height;

  if (rect != NULL)
  {
    if (client_rect == NULL)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else
    {
      width = client_rect[2] - client_rect[0];
      height = client_rect[3] - client_rect[1];
      rect->left = client_rect[0] + (width * 0x41) / 100;
      rect->right = client_rect[2] - (width * 3) / 100;
      rect->top = client_rect[1] + (height * 8) / 100;
      rect->bottom = client_rect[1] + (height * 0x1c) / 100;
    }
  }
}

// FUNCTION: MAGIC 0x0055c12a
void FUN_0055c12a(LPRECT rect, int *client_rect, int counters)
{
  struct
  {
    BITMAP bitmap;
    int rect_width;
    int text_height;
    int icon_height;
    int icon_width;
    RECT result_rect;
    int right_limit;
    int top;
    int left;
    int bottom;
    int step_width;
  } s;

  if (rect != NULL)
  {
    if (client_rect == NULL)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else if (counters == 0)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else
    {
      SetRect(&s.result_rect, 0, 0, 0, 0);
      if (*(int *)(DAT_00708da8 + 400) != 0)
      {
        s.left = client_rect[0] + ((client_rect[2] - client_rect[0]) * 7) / 100;
        s.right_limit = client_rect[2] - ((client_rect[2] - client_rect[0]) * 10) / 100;
        s.top = client_rect[1] + ((client_rect[3] - client_rect[1]) * 8) / 100;
        s.bottom = client_rect[1] + ((client_rect[3] - client_rect[1]) * 0x23) / 100;
        GetObjectA((HANDLE)*(int *)(DAT_00708da8 + 400), sizeof(BITMAP), &s.bitmap);
        s.text_height = s.bitmap.bmHeight / 0x19;
        s.icon_height = s.bottom - s.top;
        s.icon_width = (s.bitmap.bmWidth * s.icon_height) / s.text_height;
        for (s.step_width = s.icon_width;
             s.right_limit < (counters - 1) * s.step_width + s.icon_width + s.left && 1 < s.step_width;
             s.step_width--)
        {
        }
        SetRect(&s.result_rect, s.left, s.top,
                (counters - 1) * s.step_width + s.icon_width + s.left,
                s.icon_height + s.top);
      }
      CopyRect(rect, &s.result_rect);
    }
  }
}

// FUNCTION: MAGIC 0x0055ca6f
void FUN_0055ca6f(LPRECT rect, unsigned int ability, int *client_rect, unsigned int ability_flags)
{
  struct
  {
    BITMAP bitmap;
    int icon_height;
    int icon_width;
    int row;
    int y;
    int index;
    int x;
    unsigned int ability_masks[18];
    RECT result_rect;
  } s;

  s.ability_masks[0] = 0x20;
  s.ability_masks[1] = 0x400;
  s.ability_masks[2] = 0x40;
  s.ability_masks[3] = 0x80;
  s.ability_masks[4] = 0x100;
  s.ability_masks[5] = 0x200;
  s.ability_masks[6] = 1;
  s.ability_masks[7] = 2;
  s.ability_masks[8] = 4;
  s.ability_masks[9] = 8;
  s.ability_masks[10] = 0x10;
  s.ability_masks[11] = 0x800;
  s.ability_masks[12] = 0x1000;
  s.ability_masks[13] = 0x2000;
  s.ability_masks[14] = 0x4000;
  s.ability_masks[15] = 0x8000;
  s.ability_masks[16] = 0x10000;
  s.ability_masks[17] = 0x11;

  if (rect != NULL)
  {
    if (client_rect == NULL)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else if (ability_flags == 0)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else
    {
      s.icon_width = ((client_rect[2] - client_rect[0]) * 0x11) / 100;
      s.icon_height = s.icon_width;
      if (*(int *)(DAT_00709100 + 0x34) != 0)
      {
        GetObjectA((HANDLE)*(int *)(DAT_00709100 + 0x34), sizeof(BITMAP), &s.bitmap);
        s.icon_height = (s.icon_width * (s.bitmap.bmHeight / (int)(s.ability_masks[17] + 1))) / s.bitmap.bmWidth;
      }
      s.x = client_rect[0] + 1;
      s.y = (client_rect[3] - 1) - s.icon_height;
      SetRect(&s.result_rect, 0, 0, 0, 0);
      s.row = 1;
      for (s.index = 0; s.index < (int)s.ability_masks[17]; s.index++)
      {
        if ((ability_flags & s.ability_masks[s.index]) != 0)
        {
          if (s.ability_masks[s.index] == ability)
          {
            SetRect(&s.result_rect, s.x, s.y, s.icon_width + s.x, s.icon_height + s.y);
          }
          s.x += s.icon_width + 1;
          if ((s.row < 3 &&
               client_rect[2] - ((client_rect[2] - client_rect[0]) * 0x19) / 100 < s.icon_width + s.x) ||
              (2 < s.row && client_rect[2] < s.icon_width + s.x))
          {
            s.x = client_rect[0] + 1;
            s.y -= s.icon_height + 1;
            s.row++;
          }
        }
      }
      CopyRect(rect, &s.result_rect);
    }
  }
}

// FUNCTION: MAGIC 0x0055d471
void FUN_0055d471(LPRECT rect, int *client_rect)
{
  int width;
  int height;

  if (rect != NULL)
  {
    if (client_rect == NULL)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
    else
    {
      width = ((client_rect[2] - client_rect[0]) * 0x3c) / 100;
      height = ((client_rect[3] - client_rect[1]) * 0x3c) / 100;
      rect->left = client_rect[0] + ((client_rect[2] - client_rect[0]) - width) / 2;
      rect->right = rect->left + width;
      rect->top = client_rect[1] + ((client_rect[3] - client_rect[1]) - height) / 2;
      rect->bottom = rect->top + height;
    }
  }
}

// FUNCTION: MAGIC 0x0055a986
void FUN_0055a986(HDC dc, int *rect, int card_id, unsigned int player, int card)
{
  unsigned int owner_state;
  unsigned int ui_flags;

  if (dc != (HDC)0 && rect != NULL && card_id >= 0 && card_id < global_available_slots)
  {
    DrawSmallCard(dc, (RECT *)rect, global_raw_cards_storage + card_id, 0, 1);
    owner_state = FUN_0044931a(player, card);
    owner_state = owner_state == player;
    ui_flags = FUN_004494a3(player, card);
    DrawSmallCardTitle(dc, rect, global_raw_cards_storage[card_id].full_name, ui_flags,
                       owner_state);
  }
}

// FUNCTION: MAGIC 0x0055af78
void FUN_0055af78(HDC dc, int *rect, int card_id, unsigned int player, int card,
                  int attached_player, int attached_card)
{
  unsigned int owner_state;
  unsigned int ui_flags;

  if (dc != (HDC)0 && rect != NULL)
  {
    if (card_id == unk_0092666c && FUN_00449ac3(player, card) == (unsigned int)unk_008b28f8)
    {
      DrawCardBack(dc, (RECT *)rect);
      DrawSmallCardTitle(dc, rect, gs_cardtitle_activation_007aaeb0, 0, 1);
    }
    else if (card_id >= 0 && card_id < global_available_slots)
    {
      DrawSmallCard(dc, (RECT *)rect, global_raw_cards_storage + card_id, 0, 1);
      owner_state = FUN_0044931a(player, card);
      owner_state = owner_state == player;
      ui_flags = FUN_004494a3(player, card);
      DrawSmallCardTitle(dc, rect, gs_cardtitle_activation_007aaeb0, ui_flags, owner_state);
    }
  }
}

// FUNCTION: MAGIC 0x0055b1e4
void FUN_0055b1e4(HDC dc, int *rect, unsigned int player, int card, int ui_flags,
                  unsigned int target_ui_flags, int parent_card, int activation, int upkeep)
{
  int card_id;
  unsigned int owner_state;

  (void)target_ui_flags;
  (void)parent_card;
  (void)activation;
  (void)upkeep;

  if (dc != (HDC)0 && rect != NULL)
  {
    card_id = get_displayed_card_id(player, card);
    if (card_id != -1 && card_id < global_available_slots)
    {
      DrawSmallCard(dc, (RECT *)rect, global_raw_cards_storage + card_id, 0, 1);
      owner_state = FUN_0044931a(player, card);
      owner_state = owner_state == player;
      DrawSmallCardTitle(dc, rect, global_raw_cards_storage[card_id].full_name, ui_flags,
                         owner_state);
    }
  }
}

// FUNCTION: MAGIC 0x0055bd85
void draw_id_tag(HDC dc, int *rect, int player, int card, int enabled)
{
  int saved_dc;
  char text[8];
  RECT tag_rect;

  if (dc != (HDC)0 && rect != NULL && (player == 0 || player == 1) &&
      card != -1 && enabled != 0 && get_displayed_card_zone(player, card) == 1)
  {
    saved_dc = SaveDC(dc);
    tag_rect.left = rect[0] + ((rect[2] - rect[0]) * 0x50) / 100;
    tag_rect.top = rect[1];
    tag_rect.right = rect[0] + ((rect[2] - rect[0]) * 0x5f) / 100;
    tag_rect.bottom = rect[1] + ((rect[3] - rect[1]) * 0xe) / 100;
    SetMapMode(dc, MM_ANISOTROPIC);
    SetWindowExtEx(dc, tag_rect.right - tag_rect.left, 0x14, NULL);
    SetViewportExtEx(dc, tag_rect.right - tag_rect.left, tag_rect.bottom - tag_rect.top,
                     NULL);
    DPtoLP(dc, (LPPOINT)&tag_rect, 2);
    sprintf(text, "%d", card);
    SelectObject(dc, global_idtag_font);
    if (player == 0)
    {
      SetBkColor(dc, global_palette_col_7c);
    }
    else
    {
      SetBkColor(dc, global_palette_col_2f);
    }
    OffsetRect(&tag_rect, 1, 1);
    SetTextColor(dc, global_palette_col_c9);
    SetBkMode(dc, OPAQUE);
    DrawTextA(dc, text, -1, &tag_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    OffsetRect(&tag_rect, -1, -1);
    SetTextColor(dc, global_palette_col_bf);
    SetBkMode(dc, TRANSPARENT);
    DrawTextA(dc, text, -1, &tag_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RestoreDC(dc, saved_dc);
  }
}

// FUNCTION: MAGIC 0x00494cf0
void FUN_00494cf0(int bmi, int bits, RECT *rect)
{
  (void)bmi;
  (void)bits;
  (void)rect;
}

// FUNCTION: MAGIC 0x0055d26d
unsigned int draw_target_canttarget(HDC dc, int *rect, int is_target, int can_target)
{
  int saved_dc;
  unsigned int result;
  RECT target_rect;

  if (dc == (HDC)0 || rect == NULL)
  {
    return 0;
  }

  saved_dc = SaveDC(dc);
  target_rect.left = rect[0] + ((rect[2] - rect[0]) * 10) / 100;
  target_rect.right = rect[2] - ((rect[2] - rect[0]) * 10) / 100;
  target_rect.top = rect[1] + ((rect[3] - rect[1]) * 10) / 100;
  target_rect.bottom = rect[3] - ((rect[3] - rect[1]) * 10) / 100;
  result = 1;
  if (is_target != 0)
  {
    result &= draw_masked_bitmap_left_half_to_rect(dc, &target_rect, global_target_pic);
  }
  if (can_target == 0)
  {
    result &= draw_masked_bitmap_left_half_to_rect(dc, &target_rect, global_cant_target_pic);
  }
  RestoreDC(dc, saved_dc);
  return result;
}

// FUNCTION: MAGIC 0x0055d393
void FUN_0055d393(HDC dc, int *rect, int highlighted)
{
  HPEN pen;
  int saved_dc;

  pen = CreatePen(PS_INSIDEFRAME, 3, global_palette_col_1f);
  if (highlighted != 0)
  {
    saved_dc = SaveDC(dc);
    SelectObject(dc, pen);
    SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, rect[0], rect[1], rect[2], rect[3]);
    RestoreDC(dc, saved_dc);
  }
  DeleteObject(pen);
}

// FUNCTION: MAGIC 0x0055d429
void FUN_0055d429(HDC dc, int *rect, unsigned char untap_status)
{
  RECT untap_rect;

  if ((untap_status & 1) != 0 && (untap_status & 2) != 0)
  {
    FUN_0055d471(&untap_rect, rect);
    draw_masked_bitmap_left_half_to_rect(dc, &untap_rect, global_will_untap_pic);
  }
}

// FUNCTION: MAGIC 0x0055d910
void FUN_0055d910(int bmi, int bits, int *rect)
{
  (void)bmi;
  (void)bits;
  (void)rect;
}

// FUNCTION: MAGIC 0x004d2385
void FUN_004d2385(HDC dc, int rect, int player, int card)
{
  size_t text_length;
  char text[12];
  int saved_dc;
  HFONT font;

  saved_dc = SaveDC(dc);
  sprintf(text, s__d__d_005755e8, player, card);
  font = CreateFontA(0x12, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0x12,
                     s_Times_New_Roman_005755f0);
  SelectObject(dc, font);
  SetBkMode(dc, TRANSPARENT);
  SetTextAlign(dc, TA_RIGHT);
  SetTextColor(dc, 0xffffff);
  text_length = strlen(text);
  TextOutA(dc, ((RECT *)rect)->right, ((RECT *)rect)->top + 1, text, (int)text_length);
  SetTextColor(dc, 0x808080);
  text_length = strlen(text);
  TextOutA(dc, ((RECT *)rect)->right - 1, ((RECT *)rect)->top, text, (int)text_length);
  RestoreDC(dc, saved_dc);
  DeleteObject(font);
}

// FUNCTION: MAGIC 0x004d7953
int FUN_004d7953(int player, int card, int unused_player, int unused_card)
{
  int result;

  result = 1;
  if (FUN_0044125c(player, card) == 0)
  {
    result = 0;
  }
  if ((FUN_00448f5c(player, card) & 0x40) == 0)
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0044184b
int FUN_0044184B(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  struct
  {
    int result;
    int attacker_abilities;
    unsigned int selected;
    unsigned int tmp2;
    unsigned int tmp1;
  } s;

  s.attacker_abilities = C_get_abilities(attacker_player, attacker_card, 0x34, -1);
  FUN_004e698f(&s.tmp1, &s.tmp2);

  if (blocker_player == 1)
  {
    s.selected = s.tmp1;
  }
  else
  {
    s.selected = s.tmp2;
  }

  s.result = FUN_004418c6(blocker_player,
                          blocker_card,
                          attacker_player,
                          attacker_card,
                          (unsigned int)s.attacker_abilities,
                          s.selected);
  return s.result;
}

// FUNCTION: MAGIC 0x00441b94
int FUN_00441b94(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  int saved_state;
  int saved_blocking;
  int result;

  saved_state = global_card_instances[blocker_player][blocker_card].state;
  saved_blocking = (int)(char)global_card_instances[blocker_player][blocker_card].blocking;
  global_card_instances[blocker_player][blocker_card].state &= ~8;
  global_card_instances[blocker_player][blocker_card].blocking = 0xff;

  result = FUN_0044184B(blocker_player, blocker_card, attacker_player, attacker_card);
  if (result == 0)
  {
    global_card_instances[blocker_player][blocker_card].state = saved_state;
    global_card_instances[blocker_player][blocker_card].blocking = (unsigned char)saved_blocking;
  }

  return result;
}

// FUNCTION: MAGIC 0x004cd940
// FUNCTION: SHANDALAR 0x00573f30
int register_MAGICGAME_CardClass(LPCSTR class_name)
{
  struct
  {
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0xb, wndproc_MAGICGAME_CardClass, g_cardclass_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  g_cardclass_popup_menu = CreatePopupMenu();
  g_cardclass_alternate_card_submenu = CreatePopupMenu();
  AppendMenuA(g_cardclass_alternate_card_submenu, 0, 0x72, s_Empty_005751d4);

  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_SMALLCARD_005751d8);
  strcpy(g_cardclass_menu_alternate_card_text, text_lines[0]);
  strcpy(g_cardclass_menu_view_stats_text, text_lines[1]);
  strcpy(g_cardclass_menu_view_card_text, text_lines[2]);
  strcpy(g_cardclass_menu_protect_text, text_lines[3]);
  strcpy(g_cardclass_menu_id_tags_text, text_lines[4]);
  strcpy(g_cardclass_menu_autotarget_text, text_lines[5]);
  strcpy(g_cardclass_menu_show_damage_text, text_lines[6]);
  strcpy(g_cardclass_menu_help_text, text_lines[7]);

  g_cardclass_ability_masks[0] = 0x20;
  g_cardclass_ability_masks[1] = 0x400;
  g_cardclass_ability_masks[2] = 0x40;
  g_cardclass_ability_masks[3] = 0x80;
  g_cardclass_ability_masks[4] = 0x100;
  g_cardclass_ability_masks[5] = 0x200;
  g_cardclass_ability_masks[6] = 1;
  g_cardclass_ability_masks[7] = 2;
  g_cardclass_ability_masks[8] = 4;
  g_cardclass_ability_masks[9] = 8;
  g_cardclass_ability_masks[10] = 0x10;
  g_cardclass_ability_masks[11] = 0x800;
  g_cardclass_ability_masks[12] = 0x1000;
  g_cardclass_ability_masks[13] = 0x2000;
  g_cardclass_ability_masks[14] = 0x4000;
  g_cardclass_ability_masks[15] = 0x8000;
  g_cardclass_ability_masks[16] = 0x10000;

  load_text_with_tab_escapes(global_ui_strings_filename, s_PROMPT_BANDWITHWHOM_005751e8);
  strcpy(g_cardclass_prompt_band_whom_text, text_lines[0]);
  strcpy(g_cardclass_prompt_band_cannot_text, text_lines[1]);
  strcpy(g_cardclass_prompt_band_error_text, text_lines[2]);

  load_text_with_tab_escapes(global_ui_strings_filename, s_PROMPT_DEFENDWHOM_005751fc);
  strcpy(g_cardclass_prompt_defend_attacker_text, text_lines[0]);
  strcpy(g_cardclass_prompt_defend_error_text, text_lines[1]);
  strcpy(g_cardclass_prompt_defend_prompt_text, text_lines[2]);

  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_ArmageddonClock_00575210);
  strcpy(g_cardclass_cuecard_counter_armageddon_clock_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_ManaBattery_00575234);
  strcpy(g_cardclass_cuecard_counter_mana_battery_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_ClockworkAvian_00575254);
  strcpy(g_cardclass_cuecard_counter_clockwork_avian_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_ClockworkBeast_00575274);
  strcpy(g_cardclass_cuecard_counter_clockwork_beast_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_LuckyCharms_00575294);
  strcpy(g_cardclass_cuecard_counter_lucky_charms_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_Fungusaur_005752b4);
  strcpy(g_cardclass_cuecard_counter_fungusaur_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_WhirlingDervish_005752d0);
  strcpy(g_cardclass_cuecard_counter_whirling_dervish_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_LivingArtifact_005752f4);
  strcpy(g_cardclass_cuecard_counter_living_artifact_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_OsaiVultures_00575314);
  strcpy(g_cardclass_cuecard_counter_osai_vultures_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_ScavengingGhoul_00575334);
  strcpy(g_cardclass_cuecard_counter_scavenging_ghouls_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_SengirVampire_00575358);
  strcpy(g_cardclass_cuecard_counter_sengir_vampire_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_NecropolisOfAza_00575378);
  strcpy(g_cardclass_cuecard_counter_necropolis_of_azar_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_Triskelion_0057539c);
  strcpy(g_cardclass_cuecard_counter_triskelion_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_Tetravus_005753b8);
  strcpy(g_cardclass_cuecard_counter_tetravus_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_TimeVault_005753d4);
  strcpy(g_cardclass_cuecard_counter_time_vault_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_Cyclone_005753f0);
  strcpy(g_cardclass_cuecard_counter_cyclone_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_CitanulDruid_0057540c);
  strcpy(g_cardclass_cuecard_counter_citanul_druid_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_RockHydra_0057542c);
  strcpy(g_cardclass_cuecard_counter_rock_hydra_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_KhabalGhoul_00575448);
  strcpy(g_cardclass_cuecard_counter_khabal_ghoul_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_OrcishCatapult_00575468);
  strcpy(g_cardclass_cuecard_counter_orcish_catapult_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_UnstableMutatio_00575488);
  strcpy(g_cardclass_cuecard_counter_unstable_mutation_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_SpiritShackle_005754ac);
  strcpy(g_cardclass_cuecard_counter_spirit_shackle_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_DwarvenWeaponsm_005754cc);
  strcpy(g_cardclass_cuecard_counter_dwarven_weaponsmith_text, text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, s_CUECARD_COUNTERS_AshnodsTransmor_005754f0);
  strcpy(g_cardclass_cuecard_counter_ashnods_transmogrant_text, text_lines[0]);

  return s.registered;
}

// FUNCTION: MAGIC 0x004cdf7b
// FUNCTION: SHANDALAR 0x0057456b
void destroy_MAGICGAME_CardClass(void)
{
  if (g_cardclass_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_cardclass_popup_menu);
  }
  if (g_cardclass_alternate_card_submenu != (HMENU)0)
  {
    DestroyMenu(g_cardclass_alternate_card_submenu);
  }

  g_cardclass_popup_menu = (HMENU)0;
  g_cardclass_alternate_card_submenu = (HMENU)0;
}

// FUNCTION: MAGIC 0x004cdfcc
// FUNCTION: SHANDALAR 0x005745bc
LRESULT CALLBACK wndproc_MAGICGAME_CardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int menu_item_count;
  unsigned int menu_select_flags;
  struct
  {
    HWND previous_z_order;
    HDC dc;
    PAINTSTRUCT paint_struct;
    POINT test_point;
    unsigned char type_flags;
    unsigned int owner_state;
      unsigned int ui_flags;
      union
      {
        struct
        {
          card_instance_t current_snapshot;
          int player_and_card[2];
          union
          {
            char pad_0190_to_07d7[0x648];
            struct
            {
              char pad_0190_to_0557[0x3c8];
              int compare_result;
              unsigned char snapshot[0x12c];
            } msg_404;
            struct
            {
              char pad_0190_to_0683[0x4f4];
              HWND hidden_parent;
              char pad_0688_to_068f[8];
              HDC title_dc;
              RECT title_rect;
              int card_id;
              unsigned char snapshot[0x12c];
              int pad_07d4;
            } msg_432;
          } u;
        } common;
        struct
        {
          char pad_0000_to_001b[0x1c];
          unsigned int unused_878;
          int parent_card;
          unsigned int target_ui_flags;
          unsigned int paint_ui_flags;
          int attached_player_and_card[2];
          int displayed_player_and_card[2];
          HDC dc;
          PAINTSTRUCT paint_struct;
          RECT rect;
          DWORD start_tick;
          int card_id;
        } paint;
        struct
        {
          char pad_0000_to_00f3[0xf4];
          unsigned char saved_phase_flags;
          char pad_00f5_to_00f7[3];
          char defend_tooltip_text[100];
          int defend_selected_parent_card;
          int defend_select_ok;
          target_t defend_target;
          int defend_saved_modal_active;
          char defend_action_request_copy[0xe8];
          int defend_attach_card;
          int linked_scan_card;
          int linked_unique_card;
          char band_tooltip_text[100];
          int band_attach_card;
          int band_select_ok;
          target_t band_target;
          int band_saved_modal_active;
          char band_action_request_copy[0xe8];
          int selected_card;
          int attach_card;
          char help_path[100];
          char pad_0430_to_04d3[0xa4];
          int help_card_id;
          int protect_on;
          int preview_player_and_card[2];
          int preview_card_id;
          int command_player_and_card[2];
          HWND child_hwnd;
          int unused_3a0;
          HWND previous_z_order;
        } wm_command;
      } shared;
      unsigned int temp_mouse;
      unsigned int can_target_again;
    unsigned int counter_p0p1;
    unsigned int ability_flags;
    RECT ability_rect;
    char cuecard_text[100];
    unsigned int untap_status;
    unsigned int counter_p1p1;
    unsigned int mouse_x;
    unsigned int mouse_y;
    int cuecard_found;
    unsigned int counter_p1p0;
    int ability_loop_index;
    unsigned int counter_m1m1;
    RECT special_counter_rect;
    int counter_column_width;
    int kill_code;
    unsigned int counter_power;
    unsigned int special_counters;
    RECT client_rect;
    int ability_index;
    int damage_on_card;
    int card_id;
    RECT damage_rect;
    RECT untap_rect;
    int is_target;
    card_instance_t *snapshot;
    int card;
    int scratch;
    int player;
    HWND hidden_parent;
  } s;

#define current_snapshot shared.common.current_snapshot
#define player_and_card shared.common.player_and_card
#define u shared.common.u

  if (msg == WM_MOUSEMOVE || msg == WM_NCMOUSEMOVE)
  {
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  switch (msg)
  {
  case 0x437:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.card_id = get_displayed_card_id(s.player, s.card);
    s.mouse_x = (unsigned int)lparam & 0xffff;
    s.mouse_y = (unsigned short)((((unsigned int)lparam >> 0x10) & 0xffff));
    if (s.player != -1 && s.card == -1)
    {
      s.cuecard_found = 1;
      strcpy(s.cuecard_text, gs_cuecard_damage_to_player_007a77d0);
    }
    else
    {
      s.cuecard_found = 0;
      GetClientRect(hwnd, &s.client_rect);
      if ((get_displayed_card_ui_flags(s.player, s.card) & 2) != 0)
      {
        s.temp_mouse = s.mouse_x;
        s.mouse_x = s.mouse_y;
        s.mouse_y = s.client_rect.bottom - s.temp_mouse;
      }

      s.ability_flags = FUN_00448f5c(s.player, s.card);
      s.ability_index = -1;
      if (s.ability_flags != 0)
      {
        for (s.ability_loop_index = 0; s.ability_loop_index < 0x11 && s.ability_index == -1;
             s.ability_loop_index++)
        {
          FUN_0055ca6f(&s.ability_rect, g_cardclass_ability_masks[s.ability_loop_index],
                       (int *)&s.client_rect, s.ability_flags);
          if (PtInRectByCoords(&s.ability_rect, s.mouse_x, s.mouse_y) != 0)
          {
            s.ability_index = s.ability_loop_index;
          }
        }
      }

      s.special_counters = FUN_0044850c(s.player, s.card);
      FUN_0044858d(s.player, s.card, &s.counter_power, &s.counter_m1m1, &s.counter_p1p1,
                   &s.counter_p0p1, &s.counter_p1p0);
      FUN_0055c12a(&s.special_counter_rect, (int *)&s.client_rect, s.special_counters);
      s.is_target = FUN_0044951f(s.player, s.card);
      s.can_target_again = FUN_004495b2(s.player, s.card) == 0;
      s.damage_on_card = FUN_0044875b(s.player, s.card);
      FUN_0055bcb1(&s.damage_rect, (int *)&s.client_rect);
      s.untap_status = FUN_00449914(s.player, s.card);
      FUN_0055d471(&s.untap_rect, (int *)&s.client_rect);
      s.kill_code = FUN_00449b42(s.player, s.card);

      if ((s.untap_status & 1) != 0 &&
          (s.untap_status & 2) != 0 &&
          PtInRectByCoords(&s.untap_rect, s.mouse_x, s.mouse_y) != 0)
      {
        strcpy(s.cuecard_text, gs_cuecard_this_card_will_untap_00925040);
        s.cuecard_found = 1;
      }
      else if (s.ability_index != -1)
      {
        strcpy(s.cuecard_text, gs_ability_word_008b4de0[s.ability_index]);
        s.cuecard_found = 1;
      }
      else if (s.damage_on_card > 0 &&
               PtInRectByCoords(&s.damage_rect, s.mouse_x, s.mouse_y) != 0)
      {
        sprintf(s.cuecard_text, gs_cuecard_damage_n_0091b290, s.damage_on_card);
        s.cuecard_found = 1;
      }
      else if ((int)s.special_counters > 0 &&
               PtInRectByCoords(&s.special_counter_rect, s.mouse_x, s.mouse_y) != 0)
      {
        FUN_004d1d5d(s.cuecard_text, s.card_id, s.special_counters);
        s.cuecard_found = 1;
      }
      else if ((int)(s.counter_p1p1 + s.counter_m1m1 + s.counter_power) > 0 &&
               (int)s.mouse_y > (s.client_rect.bottom * 0x23) / 100 &&
               (s.client_rect.bottom * 0x3e) / 100 > (int)s.mouse_y)
      {
        s.counter_column_width = (s.client_rect.right - s.client_rect.left) / 3;
        if ((int)s.mouse_x < s.client_rect.left + s.counter_column_width)
        {
          FUN_004d21a9(s.cuecard_text, 1, s.counter_p1p1);
        }
        else if ((int)s.mouse_x < s.counter_column_width * 2 + s.client_rect.left)
        {
          FUN_004d21a9(s.cuecard_text, 2, s.counter_m1m1);
        }
        else
        {
          FUN_004d21a9(s.cuecard_text, 3, s.counter_power);
        }
        s.cuecard_found = 1;
      }
      else if ((int)(s.counter_p1p0 + s.counter_p0p1) > 0 &&
               (int)s.mouse_y > (s.client_rect.bottom * 0x35) / 100 &&
               (s.client_rect.bottom * 0x59) / 100 > (int)s.mouse_y)
      {
        s.counter_column_width = (s.client_rect.right - s.client_rect.left) / 2;
        if ((int)s.mouse_x < s.client_rect.left + (s.client_rect.right - s.client_rect.left) / 2)
        {
          FUN_004d21a9(s.cuecard_text, 4, s.counter_p0p1);
        }
        else
        {
          FUN_004d21a9(s.cuecard_text, 5, s.counter_p1p0);
        }
        s.cuecard_found = 1;
      }
      else
      {
        s.owner_state = FUN_0044931a(s.player, s.card);
        if ((int)s.owner_state != s.player && (s.client_rect.bottom * 0xc) / 100 > (int)s.mouse_y)
        {
          strcpy(s.cuecard_text, gs_cuecard_not_controlled_by_owner_008a8e40);
          s.cuecard_found = 1;
        }
        else if (((s.is_target != 0 || s.can_target_again != 0) &&
                  (int)s.mouse_x > (s.client_rect.right * 5) / 100 &&
                  (s.client_rect.right * 0x5f) / 100 > (int)s.mouse_x &&
                  (int)s.mouse_y > (s.client_rect.bottom * 0xf) / 100 &&
                  (s.client_rect.bottom * 0x5f) / 100 > (int)s.mouse_y))
        {
          if (s.is_target == 0 || s.can_target_again == 0)
          {
            if (s.is_target == 0)
            {
              if (s.can_target_again != 0)
              {
                strcpy(s.cuecard_text, gs_cuecard_cant_target_this_008b4990);
              }
            }
            else
            {
              strcpy(s.cuecard_text, gs_cuecard_is_a_target_00789c40);
            }
          }
          else
          {
            strcpy(s.cuecard_text, gs_cuecard_is_a_target_cant_target_again_008cf230);
          }
          s.cuecard_found = 1;
        }
        else if (s.kill_code == 2 &&
                 (s.client_rect.right * 5) / 100 < (int)s.mouse_x &&
                 (int)s.mouse_x < (s.client_rect.right * 0x5f) / 100 &&
                 (s.client_rect.bottom * 0xf) / 100 < (int)s.mouse_y &&
                 (int)s.mouse_y < (s.client_rect.bottom * 0x5f) / 100)
        {
          strcpy(s.cuecard_text, gs_cuecard_dying_0091c640);
          s.cuecard_found = 1;
        }
        else if ((s.client_rect.right * 5) / 100 < (int)s.mouse_x &&
                 (int)s.mouse_x < (s.client_rect.right * 0x5f) / 100 &&
                 (s.client_rect.bottom * 0xf) / 100 < (int)s.mouse_y &&
                 (int)s.mouse_y < (s.client_rect.bottom * 0x5f) / 100)
        {
          s.type_flags = FUN_00448df6(s.player, s.card);
          if ((s.type_flags & 2) != 0 &&
              (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0 &&
              (get_displayed_card_ui_flags(s.player, s.card) & 0x20) != 0)
          {
            strcpy(s.cuecard_text, gs_cuecard_summoning_sickness_008a8e00);
            strcat(s.cuecard_text, s_Comma_00575518);
            strcat(s.cuecard_text, gs_cuecard_phased_007aae00);
            s.cuecard_found = 1;
          }
          else
          {
            s.type_flags = FUN_00448df6(s.player, s.card);
            if ((s.type_flags & 2) != 0 && (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0)
            {
              strcpy(s.cuecard_text, gs_cuecard_summoning_sickness_008a8e00);
              s.cuecard_found = 1;
            }
            else if ((get_displayed_card_ui_flags(s.player, s.card) & 0x20) != 0)
            {
              strcpy(s.cuecard_text, gs_cuecard_phased_007aae00);
              s.cuecard_found = 1;
            }
          }
        }
      }
        }

    if (s.cuecard_found != 0)
    {
      strcpy((char *)wparam, s.cuecard_text);
    }
    if (g_duel_interface_options.layout == 2)
    {
      return s.cuecard_found;
    }
    SendMessageA(hwnd, WM_COMMAND, 0x6e, 0);
    return s.cuecard_found;

  case 0x432:
    if (IsWindowVisible(hwnd) == 0)
    {
      return 0;
    }
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.snapshot = (card_instance_t *)GetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
    FUN_00449e0a(s.u.msg_432.snapshot, s.player, s.card);
    if (s.player != -1 && s.card == -1)
    {
      return 0;
    }
    if (FUN_004d15b3((int *)s.snapshot, (int *)s.u.msg_432.snapshot) == 0)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    else if (g_duel_interface_options.show_abilities_on_cards == 0 ||
             FUN_004d1c68((int)s.snapshot, (int)s.u.msg_432.snapshot) != 0)
    {
      if (((int *)s.snapshot)[0x1c] != ((int *)s.u.msg_432.snapshot)[0x1c])
      {
        s.u.msg_432.card_id = get_displayed_card_id(s.player, s.card);
        if ((get_displayed_card_ui_flags(s.player, s.card) & 2) == 0)
        {
          if (s.u.msg_432.card_id == unk_0092666c ||
              s.u.msg_432.card_id == unk_009266ac ||
              s.u.msg_432.card_id == unk_00789734 ||
              s.u.msg_432.card_id == unk_007a7d64 ||
              s.u.msg_432.card_id == unk_008a8de8 ||
              s.u.msg_432.card_id == unk_00789b80 ||
              s.u.msg_432.card_id == unk_008cf1ac)
          {
            InvalidateRect(hwnd, (RECT *)0, 0);
          }
          else if (s.u.msg_432.card_id != -1)
          {
            s.u.msg_432.title_dc = GetDC(hwnd);
            ApplyCardArtPaletteToDc(s.u.msg_432.title_dc);
            GetClientRect(hwnd, &s.u.msg_432.title_rect);
            s.owner_state = FUN_0044931a(s.player, s.card);
            s.owner_state = s.owner_state == (unsigned int)s.player;
            s.ui_flags = FUN_004494a3(s.player, s.card);
            DrawSmallCardTitle(s.u.msg_432.title_dc, (int *)&s.u.msg_432.title_rect,
                               global_raw_cards_storage[s.u.msg_432.card_id].full_name,
                               s.ui_flags, s.owner_state);
            ReleaseDC(hwnd, s.u.msg_432.title_dc);
            ((int *)s.snapshot)[0x1c] = ((int *)s.u.msg_432.snapshot)[0x1c];
          }
        }
        else
        {
          InvalidateRect(hwnd, (RECT *)0, 0);
        }
      }
    }
    else
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    return 0;

  case 0x400:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    return get_displayed_card_id(s.player, s.card);

  case 0x401:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    if (wparam != 0)
    {
      ((int *)wparam)[0] = s.player;
      ((int *)wparam)[1] = s.card;
    }
    return 0;

  case 0x402:
    s.hidden_parent = (HWND)GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET);
    if (s.hidden_parent != (HWND)wparam)
    {
      if (wparam == 0)
      {
        BringWindowToTop(hwnd);
      }
      s.hidden_parent = (HWND)wparam;
      SetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET, (LONG)s.hidden_parent);
    }
    return 0;

  case 0x403:
    return GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET);

  case 0x404:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.snapshot = (card_instance_t *)GetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
    if (s.player != -1 && s.card == -1)
    {
      s.u.msg_404.compare_result = 0;
    }
    else
    {
      FUN_00449e0a(s.u.msg_404.snapshot, s.player, s.card);
      s.u.msg_404.compare_result = memcmp(s.snapshot, s.u.msg_404.snapshot, sizeof(card_instance_t));
    }
    return s.u.msg_404.compare_result;

  case WM_COMMAND:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.hidden_parent = (HWND)GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET);
    s.shared.wm_command.command_player_and_card[0] = s.player;
    s.shared.wm_command.command_player_and_card[1] = s.card;
    if (GetParent(hwnd) == g_duel_attack_phase_window_hwnd)
    {
      s.shared.wm_command.child_hwnd = hwnd;
      find_battlefield_card_window(g_duel_player_battlefield_window_hwnd,
                                   s.shared.wm_command.command_player_and_card, (int *)0,
                                   &s.shared.wm_command.previous_z_order);
    }
    else
    {
      s.shared.wm_command.previous_z_order = hwnd;
      find_attack_phase_card_window(g_duel_attack_phase_window_hwnd,
                                    s.shared.wm_command.command_player_and_card, (int *)0,
                                    &s.shared.wm_command.child_hwnd, (int *)0);
    }
    switch ((unsigned int)wparam & 0xffff)
    {
    case 0x6e:
      s.shared.wm_command.preview_card_id = get_displayed_card_id(s.player, s.card);
      s.shared.wm_command.preview_player_and_card[0] = s.player;
      s.shared.wm_command.preview_player_and_card[1] = s.card;
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                   s.shared.wm_command.preview_card_id,
                   (LPARAM)s.shared.wm_command.preview_player_and_card);
      break;

    case 0x70:
      s.shared.wm_command.protect_on = (FUN_00449151(s.player, s.card) & 0x40000) != 0;
      s.shared.wm_command.protect_on = s.shared.wm_command.protect_on == 0;
      if (s.shared.wm_command.protect_on != 0)
      {
        CARDCLASS_STATE(s.player, s.card) |= 0x40000;
      }
      else
      {
        CARDCLASS_STATE(s.player, s.card) &= 0xfffbffff;
      }
      EnterCriticalSection(&g_duel_render_lock);
      CARDCLASS_DISPLAYED_STATE(s.player, s.card) =
          CARDCLASS_STATE(s.player, s.card);
      LeaveCriticalSection(&g_duel_render_lock);
      InvalidateRect(hwnd, (RECT *)0, 1);
      break;

    case 0x71:
      s.shared.wm_command.help_card_id = get_displayed_card_id(s.player, s.card);
      if (s.shared.wm_command.help_card_id == unk_009266ac)
      {
        s.shared.wm_command.help_card_id = 0xc1b;
      }
      if (s.shared.wm_command.help_card_id != -1)
      {
        strcpy(s.shared.wm_command.help_path, global_base_directory);
        strcat(s.shared.wm_command.help_path, s__duel_hlp_0057551c);
        WinHelpA(g_duel_window_hwnd, s.shared.wm_command.help_path, HELP_CONTEXT,
                 s.shared.wm_command.help_card_id);
      }
      break;

    case 0x6f:
      if (GetParent(hwnd) == g_duel_full_card_window_hwnd ||
          GetParent(hwnd) == g_duel_life_window_hwnd)
      {
        SendMessageA(GetParent(hwnd), 0x400, (WPARAM)hwnd, 0);
      }
      break;

    case 0x73:
      SendMessageA(g_duel_window_hwnd, msg, 0x279, 0);
      break;

    case 0x74:
      SendMessageA(g_duel_window_hwnd, msg, 0x27a, 0);
      break;

    case 0x75:
      SendMessageA(g_duel_window_hwnd, msg, 0x27c, 0);
      break;

    case 0x65:
      get_current_duel_selection((int *)0, &s.shared.wm_command.selected_card);
      if (FUN_0044125c(s.player, s.card) != 0)
      {
        s.shared.wm_command.attach_card = -1;
        CARDCLASS_STATE(s.player, s.card) |= 4;
        CARDCLASS_BLOCKING(s.player, s.card) =
            (unsigned char)s.shared.wm_command.attach_card;
        refresh_duel_display_cache();
        if (s.shared.wm_command.selected_card > 0x14 &&
            s.shared.wm_command.selected_card < 0x1e)
        {
          SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
        }
        else
        {
          SendMessageA(GetParent(hwnd), 0x436,
                       (WPARAM)s.shared.wm_command.command_player_and_card, 0);
        }
      }
      break;

    case 0x66:
      if (FUN_004486de(s.player, s.card) != -1)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x69, 0);
      }
      CARDCLASS_STATE(s.player, s.card) &= 0xfffffffb;
      CARDCLASS_BLOCKING(s.player, s.card) = 0xff;
      refresh_duel_display_cache();
      if (s.shared.wm_command.child_hwnd == hwnd)
      {
        SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
      }
      else
      {
        SendMessageA(GetParent(hwnd), 0x436,
                     (WPARAM)s.shared.wm_command.command_player_and_card, 0);
      }
      break;

    case 0x68:
      SendMessageA(hwnd, WM_COMMAND, 0x69, 0);
    case 0x67:
      if (FUN_0044125c(s.player, s.card) != 0)
      {
        memcpy(s.shared.wm_command.band_action_request_copy, g_duel_action_request_copy, 0xe8);
        GetWindowTextA((HWND)DAT_007a7d74, s.shared.wm_command.band_tooltip_text, 100);
        s.shared.wm_command.band_saved_modal_active = g_duel_modal_action_active;
        s.shared.wm_command.band_select_ok =
            C_real_select_target(0, 0, 1, 0x200, 2, 0, 0, 0, 0, 0,
                                 -1, -1, -1, -1, 0, 2, 0,
                                 g_cardclass_prompt_band_whom_text, 1,
                                 &s.shared.wm_command.band_target);
        g_duel_modal_action_active = s.shared.wm_command.band_saved_modal_active;
        if (s.shared.wm_command.band_select_ok != 0)
        {
          if ((get_displayed_card_ui_flags(s.shared.wm_command.band_target.player,
                                           s.shared.wm_command.band_target.card) & 4) != 0)
          {
            if (FUN_004d7953(s.player, s.card,
                             s.shared.wm_command.band_target.player,
                             s.shared.wm_command.band_target.card) != 0)
            {
              s.shared.wm_command.band_attach_card =
                  FUN_004486de(s.shared.wm_command.band_target.player,
                               s.shared.wm_command.band_target.card);
              if (s.shared.wm_command.band_attach_card != -1)
              {
                CARDCLASS_STATE(s.player, s.card) |= 4;
                CARDCLASS_BLOCKING(s.player, s.card) =
                    (unsigned char)s.shared.wm_command.band_attach_card;
                refresh_duel_display_cache();
                SendMessageA(hwnd, 0x432, 0, 0);
                if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
                {
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
                }
              }
              else
              {
                s.shared.wm_command.band_attach_card = s.shared.wm_command.band_target.card;
                CARDCLASS_STATE(s.player, s.card) |= 4;
                CARDCLASS_BLOCKING(s.player, s.card) =
                    (unsigned char)s.shared.wm_command.band_attach_card;
                CARDCLASS_STATE(s.shared.wm_command.band_target.player,
                                s.shared.wm_command.band_target.card) |= 4;
                CARDCLASS_BLOCKING(s.shared.wm_command.band_target.player,
                                   s.shared.wm_command.band_target.card) =
                    (unsigned char)s.shared.wm_command.band_attach_card;
                refresh_duel_display_cache();
                SendMessageA(hwnd, 0x432, 0, 0);
                if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
                {
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x436,
                               (WPARAM)&s.shared.wm_command.band_target, 0);
                }
                else
                {
                  SendMessageA(GetParent(s.shared.wm_command.previous_z_order), 0x436,
                               (WPARAM)&s.shared.wm_command.band_target, 0);
                }
              }
            }
            else
            {
              UpdateWindow(g_duel_window_hwnd);
              FUN_004480a6(g_cardclass_prompt_band_cannot_text);
              UpdateWindow(g_duel_prompt_context_hwnd);
              Sleep(2000);
            }
          }
          else
          {
            UpdateWindow(g_duel_window_hwnd);
            FUN_004480a6(g_cardclass_prompt_band_error_text);
            UpdateWindow(g_duel_prompt_context_hwnd);
            Sleep(2000);
          }
        }
        memcpy(g_duel_action_request_copy, s.shared.wm_command.band_action_request_copy, 0xe8);
        set_duel_prompt_context(g_duel_prompt_context_hwnd,
                                s.shared.wm_command.band_action_request_copy + 0x18,
                                *(unsigned int *)(s.shared.wm_command.band_action_request_copy + 0x14));
        set_duel_tooltip_text(s.shared.wm_command.band_tooltip_text);
      }
      break;

    case 0x69:
      s.shared.wm_command.linked_unique_card = FUN_004486de(s.player, s.card);
      for (s.shared.wm_command.linked_scan_card = 0;
           s.shared.wm_command.linked_scan_card < active_cards_count[s.player];
           s.shared.wm_command.linked_scan_card++)
      {
        if (get_displayed_card_internal_id(s.player, s.shared.wm_command.linked_scan_card) != -1 &&
            (get_displayed_card_ui_flags(s.player, s.shared.wm_command.linked_scan_card) & 4) != 0 &&
            FUN_004486de(s.player, s.shared.wm_command.linked_scan_card) ==
                s.shared.wm_command.linked_unique_card)
        {
          CARDCLASS_BLOCKING(s.player, s.shared.wm_command.linked_scan_card) = 0xff;
          refresh_duel_display_cache();
          s.shared.wm_command.command_player_and_card[0] = s.player;
          s.shared.wm_command.command_player_and_card[1] = s.shared.wm_command.linked_scan_card;
          if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
          {
            SendMessageA(g_duel_attack_phase_window_hwnd, 0x436,
                         (WPARAM)s.shared.wm_command.command_player_and_card, 0);
          }
          else
          {
            SendMessageA(GetParent(s.shared.wm_command.previous_z_order), 0x436,
                         (WPARAM)s.shared.wm_command.command_player_and_card, 0);
          }
        }
      }
      if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
      {
        SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
      }
      break;

    case 0x6a:
    case 0x6b:
      memcpy(s.shared.wm_command.defend_action_request_copy, g_duel_action_request_copy, 0xe8);
      GetWindowTextA((HWND)DAT_007a7d74, s.shared.wm_command.defend_tooltip_text, 100);
      s.shared.wm_command.defend_saved_modal_active = g_duel_modal_action_active;
      s.shared.wm_command.defend_select_ok =
          C_real_select_target(0, 1, 0, 0x200, 2, 0, 0, 0, 0, 0,
                               -1, -1, -1, -1, 0, 2, 0,
                               g_cardclass_prompt_defend_attacker_text, 1,
                               &s.shared.wm_command.defend_target);
      g_duel_modal_action_active = s.shared.wm_command.defend_saved_modal_active;
      if (s.shared.wm_command.defend_select_ok != 0)
      {
        if ((get_displayed_card_ui_flags(s.shared.wm_command.defend_target.player,
                                         s.shared.wm_command.defend_target.card) & 4) != 0)
        {
          if (FUN_00441b94(s.player, s.card,
                           s.shared.wm_command.defend_target.player,
                           s.shared.wm_command.defend_target.card) != 0)
          {
            s.shared.wm_command.defend_selected_parent_card =
                FUN_004486de(s.shared.wm_command.defend_target.player,
                             s.shared.wm_command.defend_target.card);
            if (s.shared.wm_command.defend_selected_parent_card == -1)
            {
              s.shared.wm_command.defend_attach_card = s.shared.wm_command.defend_target.card;
            }
            else
            {
              s.shared.wm_command.defend_attach_card =
                  s.shared.wm_command.defend_selected_parent_card;
            }
            CARDCLASS_STATE(s.player, s.card) |= 8;
            CARDCLASS_BLOCKING(s.player, s.card) =
                (unsigned char)s.shared.wm_command.defend_attach_card;
            refresh_duel_display_cache();
            if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0 &&
                GetParent(hwnd) != g_duel_attack_phase_window_hwnd)
            {
              SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
            }
            else
            {
              SendMessageA(GetParent(hwnd), 0x436,
                           (WPARAM)s.shared.wm_command.command_player_and_card, 0);
            }
          }
          else
          {
            UpdateWindow(g_duel_window_hwnd);
            FUN_004480a6(g_cardclass_prompt_defend_error_text);
            UpdateWindow(g_duel_prompt_context_hwnd);
            Sleep(2000);
          }
        }
        else
        {
          UpdateWindow(g_duel_window_hwnd);
          FUN_004480a6(g_cardclass_prompt_defend_prompt_text);
          UpdateWindow(g_duel_prompt_context_hwnd);
          Sleep(2000);
        }
      }
      memcpy(g_duel_action_request_copy, s.shared.wm_command.defend_action_request_copy, 0xe8);
      set_duel_prompt_context(g_duel_prompt_context_hwnd,
                              s.shared.wm_command.defend_action_request_copy + 0x18,
                              *(unsigned int *)(s.shared.wm_command.defend_action_request_copy + 0x14));
      set_duel_tooltip_text(s.shared.wm_command.defend_tooltip_text);
      break;

    case 0x6c:
      CARDCLASS_STATE(s.player, s.card) &= 0xfffffff7;
      CARDCLASS_BLOCKING(s.player, s.card) = 0xff;
      refresh_duel_display_cache();
      if (s.shared.wm_command.child_hwnd == hwnd)
      {
        SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
      }
      else
      {
        SendMessageA(GetParent(hwnd), 0x436,
                     (WPARAM)s.shared.wm_command.command_player_and_card, 0);
      }
      break;

    case 100:
    case 0x6d:
      unk_00715fb0 = 0;
      FUN_004d1263(hwnd);
      break;

    case 0x262:
      if (g_duel_startup_state != 0)
      {
        s.shared.wm_command.unused_3a0 = CARDCLASS_INTERNAL_CARD_ID(s.player, s.card);
        produce_mana(s.player,
                     single_color_test_bit_to_color_t(
                         (int)global_cards_data[
                                  CARDCLASS_INTERNAL_CARD_ID(s.player, s.card)].color),
                     (int)(char)global_cards_data[s.shared.wm_command.unused_3a0].cc[0]);
        produce_mana(s.player, 0,
                     abs((int)(char)global_cards_data[s.shared.wm_command.unused_3a0].cc[1]));
        if ((g_duel_network_flags & 2) != 0 && s.player == nonactive_player)
        {
          for (s.scratch = 0; s.scratch < 8; s.scratch++)
          {
            ((int *)((char *)&unk_009251b0 + 4))[s.scratch] =
                raw_mana_available[nonactive_player][s.scratch];
          }
          unk_009251b0 = 0x11;
          TENTATIVE_send_network_result(nonactive_player, 0x11);
        }
        copy_mana_pool_to_display();
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x263:
      if (g_duel_startup_state != 0)
      {
        CARDCLASS_STATE(s.player, s.card) ^= 0x10;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x264:
      if (g_duel_startup_state != 0)
      {
        s.shared.wm_command.saved_phase_flags = DAT_007abc90[human_player * 0x26 + current_phase];
        DAT_007abc90[human_player * 0x26 + current_phase] =
            (unsigned char)((int)(char)DAT_007abc90[human_player * 0x26 + current_phase] & 0xfe);
        CARDCLASS_TOKEN_STATUS(s.player, s.card) |= 8;
        kill_card(s.player, s.card, KILL_DESTROY);
        DAT_007abc90[human_player * 0x26 + current_phase] =
            s.shared.wm_command.saved_phase_flags;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x266:
      if (g_duel_startup_state != 0)
      {
        add_special_counter(s.player, s.card);
        notify_duel_action(0, 0xff);
      }
      break;
    }
    return 0;

  case WM_CREATE:
    if (((CREATESTRUCTA *)lparam)->lpCreateParams == NULL)
    {
      return -1;
    }

    s.player = ((int *)((CREATESTRUCTA *)lparam)->lpCreateParams)[0];
    s.card = ((int *)((CREATESTRUCTA *)lparam)->lpCreateParams)[1];
    SetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET, s.player);
    SetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET, s.card);
    SetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET, 0);
    s.snapshot = (card_instance_t *)malloc(sizeof(card_instance_t));
    SetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET, (LONG)s.snapshot);
    SetWindowLongA(hwnd, CARDCLASS_PREVIOUS_Z_ORDER_WINDOW_LONG_OFFSET, 0);
    if (s.snapshot == NULL)
    {
      return -1;
    }
    memset(s.snapshot, 0, sizeof(card_instance_t));
    return 0;

  case WM_DESTROY:
    s.snapshot = (card_instance_t *)GetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
    free(s.snapshot);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.hidden_parent = (HWND)GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET);
    s.counter_power = GetDoubleClickTime();
    s.scratch = GetMessageTime();
    s.damage_on_card = GetTickCount();
    Sleep(s.counter_power - (s.scratch - s.damage_on_card));
    s.cuecard_found = PeekMessageA((MSG *)&s.paint_struct, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
    if (GetParent(hwnd) == g_duel_player_battlefield_window_hwnd ||
        GetParent(hwnd) == g_duel_help_owner_hwnd)
    {
      if (s.hidden_parent == (HWND)0)
      {
        s.previous_z_order = hwnd;
      }
      else
      {
        s.previous_z_order = s.hidden_parent;
        do
        {
          s.previous_z_order = s.hidden_parent;
          s.hidden_parent = (HWND)get_card_window_hidden_flag(s.previous_z_order);
        } while (s.hidden_parent != (HWND)0);
        s.hidden_parent = (HWND)0;
      }
      GetWindowRect(s.previous_z_order, &s.client_rect);
      s.hidden_parent = GetWindow(s.previous_z_order, GW_HWNDPREV);
      SendMessageA(s.previous_z_order, WM_SYSCOMMAND, 0xf012, 0);
      GetWindowRect(s.previous_z_order, &s.damage_rect);
      if (abs(s.client_rect.top - s.damage_rect.top) +
          abs(s.client_rect.left - s.damage_rect.left) < 5)
      {
        s.is_target = 0;
        SetWindowPos(s.previous_z_order, s.hidden_parent, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE);
      }
      else
      {
        s.is_target = 1;
      }
    }
    else
    {
      s.is_target = 0;
    }
    get_current_duel_selection(&s.counter_m1m1, &s.counter_p1p0);
    if (s.is_target == 0 && g_duel_modal_action_active != 0 && FUN_004d1314(hwnd) != 0)
    {
      unk_00715fb0 = s.cuecard_found;
      FUN_004d1263(hwnd);
    }
    return 0;

  case WM_MOVE:
    SendMessageA(GetParent(hwnd), 0x410, (WPARAM)hwnd, 0);
    return 0;

  case WM_PAINT:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    if (SendMessageA(hwnd, 0x404, 0, 0) != 0)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    EnterCriticalSection(&g_card_render_lock);
    s.shared.paint.start_tick = GetTickCount();
    GetClientRect(hwnd, &s.shared.paint.rect);
    s.shared.paint.dc = BeginPaint(hwnd, &s.shared.paint.paint_struct);
    if (s.shared.paint.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.shared.paint.dc);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.shared.paint.dc, &s.shared.paint.rect, GetStockObject(0));
        Sleep(200);
      }
      if (s.player == -1 && s.card == -1)
      {
        DrawCardBack(s.shared.paint.dc, &s.shared.paint.rect);
      }
      else if (s.card == -1)
      {
        draw_duel_face_window(s.shared.paint.dc, &s.shared.paint.rect, s.player);
        FrameRect(s.shared.paint.dc, &s.shared.paint.rect, GetStockObject(4));
      }
      else
      {
        s.shared.paint.card_id = get_displayed_card_id(s.player, s.card);
        if (s.shared.paint.card_id != -1 && global_available_slots > s.shared.paint.card_id)
        {
          if (s.shared.paint.card_id == unk_009266ac)
          {
            DrawCardBack(s.shared.paint.dc, &s.shared.paint.rect);
            DrawSmallCardTitle(s.shared.paint.dc, (int *)&s.shared.paint.rect,
                               gs_cardtitle_draw_a_card_008b4330,
                               FUN_004494a3(s.player, s.card), 1);
          }
          else if (s.shared.paint.card_id == unk_007a7d64 ||
                   unk_00789b80 == s.shared.paint.card_id ||
                   unk_008cf1ac == s.shared.paint.card_id ||
                   s.shared.paint.card_id == unk_00789734 ||
                   s.shared.paint.card_id == unk_008a8de8)
          {
            FUN_00448c93(s.shared.paint.displayed_player_and_card, s.player, s.card);
            FUN_0055a986(s.shared.paint.dc, (int *)&s.shared.paint.rect,
                         s.shared.paint.card_id, s.player, s.card);
            FUN_0055d393(s.shared.paint.dc, (int *)&s.shared.paint.rect,
                         get_displayed_card_display_flags(s.player, s.card) & 0x20000);
            draw_target_canttarget(s.shared.paint.dc, (int *)&s.shared.paint.rect,
                                   FUN_0044951f(s.player, s.card),
                                   FUN_004495b2(s.player, s.card));
            draw_id_tag(s.shared.paint.dc, (int *)&s.shared.paint.rect,
                        s.shared.paint.displayed_player_and_card[0],
                        s.shared.paint.displayed_player_and_card[1],
                        g_duel_interface_options.show_id_tags_on_cards);
          }
          else if (unk_0092666c == s.shared.paint.card_id)
          {
            FUN_00449a0f(s.shared.paint.attached_player_and_card, s.player, s.card);
            FUN_0055af78(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                         s.shared.paint.card_id, s.player, s.card,
                         s.shared.paint.attached_player_and_card[0],
                         s.shared.paint.attached_player_and_card[1]);
            draw_target_canttarget(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                                   FUN_0044951f(s.shared.paint.attached_player_and_card[0],
                                                s.shared.paint.attached_player_and_card[1]),
                                   FUN_004495b2(s.shared.paint.attached_player_and_card[0],
                                                s.shared.paint.attached_player_and_card[1]));
            draw_id_tag(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                        s.shared.paint.attached_player_and_card[0],
                        s.shared.paint.attached_player_and_card[1],
                        g_duel_interface_options.show_id_tags_on_cards);
            BitBlt(s.shared.paint.dc, 0, 0, s.shared.paint.rect.right,
                   s.shared.paint.rect.bottom,
                   g_shared_offscreen_dc, 0, 0, SRCCOPY);
          }
          else
          {
            s.shared.paint.paint_ui_flags = get_displayed_card_ui_flags(s.player, s.card);
            if (GetParent(hwnd) == g_duel_attack_phase_window_hwnd)
            {
              s.shared.paint.target_ui_flags = 0;
            }
            else
            {
              s.shared.paint.target_ui_flags = s.shared.paint.paint_ui_flags & 4;
              s.shared.paint.parent_card = FUN_004486de(s.player, s.card);
            }
            FUN_0055b1e4(g_shared_offscreen_dc, (int *)&s.shared.paint.rect, s.player, s.card,
                         FUN_004494a3(s.player, s.card),
                         s.shared.paint.target_ui_flags,
                         s.shared.paint.parent_card, 0, 0);
            draw_target_canttarget(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                                   FUN_0044951f(s.player, s.card),
                                   FUN_004495b2(s.player, s.card));
            FUN_0055d393(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                         get_displayed_card_display_flags(s.player, s.card) & 0x20000);
            FUN_0055d429(g_shared_offscreen_dc, (int *)&s.shared.paint.rect,
                         FUN_00449914(s.player, s.card));
            draw_id_tag(g_shared_offscreen_dc, (int *)&s.shared.paint.rect, s.player, s.card,
                        g_duel_interface_options.show_id_tags_on_cards);
            if (((FUN_00448df6(s.player, s.card) & 2) != 0 ||
                 g_duel_interface_options.show_all_cards_summon_sickness != 0) &&
                (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0)
            {
              FUN_0055d910((int)&g_duel_backbuffer_bmi, (int)g_shared_offscreen_bits,
                           (int *)&s.shared.paint.rect);
            }
            if ((s.shared.paint.paint_ui_flags & 2) != 0)
            {
              FUN_00494cf0((int)&g_duel_backbuffer_bmi, (int)g_shared_offscreen_bits,
                           &s.shared.paint.rect);
            }
            BitBlt(s.shared.paint.dc, 0, 0, s.shared.paint.rect.right,
                   s.shared.paint.rect.bottom,
                   g_shared_offscreen_dc, 0, 0, SRCCOPY);
          }
          s.snapshot = (card_instance_t *)GetWindowLongA(hwnd,
                                                         CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
          FUN_00449e0a(s.snapshot, s.player, s.card);
          if (g_duel_dialog_refresh_state != 0)
          {
            FUN_004d2385(s.shared.paint.dc, (int)&s.shared.paint.rect, s.player, s.card);
          }
        }
      }
      EndPaint(hwnd, &s.shared.paint.paint_struct);
    }
    *(int *)&g_duel_tick_adjustment += GetTickCount() - s.shared.paint.start_tick;
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.is_target = 1;
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    if (GetParent(hwnd) != g_duel_full_card_window_hwnd &&
        GetParent(hwnd) != g_duel_life_window_hwnd)
    {
      s.previous_z_order = GetWindow(hwnd, GW_HWNDPREV);
      SetWindowLongA(hwnd, CARDCLASS_PREVIOUS_Z_ORDER_WINDOW_LONG_OFFSET, (LONG)s.previous_z_order);
      BringWindowToTop(hwnd);
    }
    if (g_duel_interface_options.layout == 2)
    {
      s.counter_power = GetDoubleClickTime();
      Sleep(s.counter_power);
      if (PeekMessageA((MSG *)&s.paint_struct, hwnd, WM_RBUTTONDBLCLK, WM_RBUTTONDBLCLK, 0) != 0)
      {
        s.is_target = 0;
      }
    }
    if (s.player != -1 && s.card == -1)
    {
      s.is_target = 0;
    }
    if (s.is_target != 0)
    {
      s.test_point.x = (unsigned int)lparam & 0xffff;
      s.test_point.y = (unsigned int)lparam >> 0x10;
      ClientToScreen(hwnd, &s.test_point);
      s.test_point.x += GetSystemMetrics(SM_CYMENU);
      s.test_point.y += 4;
      SetRect(&s.ability_rect, s.test_point.x, s.test_point.y,
              s.test_point.x + 1, s.test_point.y + 1);
      TrackPopupMenu(g_cardclass_popup_menu, TPM_RIGHTBUTTON, s.test_point.x, s.test_point.y, 0,
                     hwnd, (RECT *)0);
    }
    return 0;

  case WM_RBUTTONUP:
    if (GetParent(hwnd) != g_duel_full_card_window_hwnd &&
        GetParent(hwnd) != g_duel_life_window_hwnd)
    {
      s.previous_z_order = (HWND)GetWindowLongA(hwnd, CARDCLASS_PREVIOUS_Z_ORDER_WINDOW_LONG_OFFSET);
      SetWindowPos(hwnd, s.previous_z_order, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    return 0;

  case WM_INITMENU:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.ui_flags = get_displayed_card_ui_flags(s.player, s.card);
    s.can_target_again = s.ui_flags & 4;
    if ((s.ui_flags & 4) != 0)
    {
      FUN_004486de(s.player, s.card);
    }
    s.damage_on_card = FUN_004486de(s.player, s.card);
    s.is_target = FUN_004d1314(hwnd);
    s.ability_flags = FUN_00448f5c(s.player, s.card) & 0x40;
    s.untap_status = get_displayed_card_zone(s.player, s.card);
    s.special_counters = get_displayed_card_internal_id(s.player, s.card);
    s.card_id = get_displayed_card_id(s.player, s.card);
    s.counter_p1p1 = FUN_00449151(s.player, s.card) & 0x40000;
    s.counter_p0p1 = global_cards_data[s.special_counters].extra_ability & 0x1000;
    s.type_flags = FUN_00448df6(s.player, s.card);
    s.previous_z_order = GetParent(hwnd);
    s.counter_power = FUN_00449ac3(s.player, s.card);
    get_current_duel_selection(&s.counter_m1m1, &s.counter_p1p0);

    if (s.special_counters != s.counter_power)
    {
      AppendMenuA(g_cardclass_popup_menu, MF_POPUP, (UINT)g_cardclass_alternate_card_submenu,
                  g_cardclass_menu_alternate_card_text);
      s.scratch = CardIDFromType(s.counter_power);
      ModifyMenuA(g_cardclass_alternate_card_submenu, 0x72, 0, 0x72,
                  global_raw_cards_storage[s.scratch].full_name);
    }
    if (g_duel_interface_options.layout == 2)
    {
      AppendMenuA(g_cardclass_popup_menu, 0, 0x6e, g_cardclass_menu_view_stats_text);
    }
    else
    {
      AppendMenuA(g_cardclass_popup_menu, 0, 0x6e, g_cardclass_menu_view_card_text);
    }
    if ((s.type_flags & 1) != 0 &&
        s.counter_p0p1 != 0 &&
        GetParent(hwnd) == g_duel_player_battlefield_window_hwnd)
    {
      AppendMenuA(g_cardclass_popup_menu, 0, 0x70, g_cardclass_menu_protect_text);
      if (s.counter_p1p1 != 0)
      {
        CheckMenuItem(g_cardclass_popup_menu, 0x70, MF_CHECKED);
      }
    }
    AppendMenuA(g_cardclass_popup_menu, 0, 0x73, g_cardclass_menu_id_tags_text);
    if (g_duel_interface_options.show_id_tags_on_cards != 0)
    {
      CheckMenuItem(g_cardclass_popup_menu, 0x73, MF_CHECKED);
    }
    AppendMenuA(g_cardclass_popup_menu, 0, 0x74, g_cardclass_menu_autotarget_text);
    if (g_duel_interface_options.show_invisible_effect_cards != 0)
    {
      CheckMenuItem(g_cardclass_popup_menu, 0x74, MF_CHECKED);
    }
    AppendMenuA(g_cardclass_popup_menu, 0, 0x75, g_cardclass_menu_show_damage_text);
    if (g_duel_interface_options.show_all_cards_summon_sickness != 0)
    {
      CheckMenuItem(g_cardclass_popup_menu, 0x75, MF_CHECKED);
    }
    AppendMenuA(g_cardclass_popup_menu, 0, 0x71, g_cardclass_menu_help_text);
    if (g_duel_startup_state != 0 && g_duel_modal_action_active != 0)
    {
      if (s.untap_status == 0)
      {
        AppendMenuA(g_cardclass_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x262, s__M__Add_mana_for_this_card_00575528);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x264, s__B__Bury_this_card_00575544);
      }
      else
      {
        AppendMenuA(g_cardclass_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x262, s__M__Add_mana_for_this_card_00575558);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x263, s__T__Tap_untap_this_card_00575574);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x264, s__B__Bury_this_card_0057558c);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x266,
                    s__X__Increment_counters_for_this_c_005755a0);
      }
    }
    return 0;

  case WM_MENUSELECT:
    menu_select_flags = (unsigned int)wparam >> 0x10;
    if (menu_select_flags == 0xffff && lparam == 0)
    {
      menu_item_count = GetMenuItemCount(g_cardclass_popup_menu);
      while (menu_item_count-- != 0)
      {
        RemoveMenu(g_cardclass_popup_menu, 0, MF_BYPOSITION);
      }
      if (GetParent(hwnd) != g_duel_full_card_window_hwnd &&
          GetParent(hwnd) != g_duel_life_window_hwnd)
      {
        s.previous_z_order = (HWND)GetWindowLongA(hwnd, CARDCLASS_PREVIOUS_Z_ORDER_WINDOW_LONG_OFFSET);
        SetWindowPos(hwnd, s.previous_z_order, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      }
    }
    return 0;

  case WM_RBUTTONDBLCLK:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    if (s.player == -1 || s.card != -1)
    {
      SendMessageA(hwnd, WM_COMMAND, 0x6e, 0);
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case WM_QUERYNEWPALETTE:
  case WM_PALETTECHANGED:
  case WM_PALETTEISCHANGING:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  return 0;
}

#undef current_snapshot
#undef player_and_card
#undef u
