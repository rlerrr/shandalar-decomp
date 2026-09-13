#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
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

int load_text_with_tab_escapes(char *filename, char *section_name);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void draw_duel_face_window(HDC dc, RECT *rect, int player);
int displayed_card_indices_invalid(int player, int card);
unsigned int get_displayed_card_display_pic_info(int *displayed_player_and_card, int player, int card);
card_id_t get_displayed_card_id(int player, int card);
int get_displayed_card_internal_id(int player, int card);
unsigned int get_displayed_card_special_counters(int player, int card);
unsigned int get_displayed_card_display_flags(int player, int card);
unsigned int get_displayed_card_ui_flags(int player, int card);
int get_displayed_card_blocking(int player, int card);
int get_displayed_card_type(int player, int card);
int get_displayed_card_zone(int player, int card);
int get_displayed_card_color_flags(int player, int card);
int get_displayed_card_mana_color(int player, int card);
unsigned int get_displayed_card_original_internal_id(int player, int card);
void get_displayed_card_parent(int *displayed_player_and_card, int player, int card);
unsigned int get_displayed_card_mana_source_flag(int player, int card);
int get_displayed_player_card_counts(int *player_card_counts);
void get_displayed_card_attachment(int *player_and_card, int player, int card);
void get_current_duel_selection(int *selected_player, int *selected_card);
LONG get_card_window_hidden_flag(HWND hwnd);
void post_card_action_selection(HWND hwnd);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int width, int height,
                            int src_x, int src_y, int mask_x, int mask_y);
void draw_special_effect_small_card(HDC dc, RECT *rect, card_id_t card_id, unsigned int player, int card);
void draw_activation_small_card(HDC dc, RECT *rect, card_id_t card_id, int player, int card, int attached_player, int attached_card);
void draw_cardclass_small_card(HDC dc, RECT *rect, unsigned int player, int card, int ui_flags,
                               unsigned int target_ui_flags, int parent_card, int activation, int upkeep);
void draw_card_highlight_frame(HDC dc, RECT *rect, int highlighted);
void draw_will_untap_overlay(HDC dc, RECT *rect, unsigned int untap_status);
void draw_summoning_sickness_dib_overlay(BITMAPINFO *bmi, unsigned char *bits, RECT *rect);
unsigned int get_dib_pixel(unsigned char *bits, int bit_count, int width, int x, int y);
void set_dib_pixel(unsigned char *bits, int bit_count, int width, int x, int y, unsigned int color);
void rotate_dib_rect_clockwise(BITMAPINFO *bmi, unsigned char *bits, RECT *rect);
unsigned int draw_target_canttarget(HDC dc, RECT *rect, int is_target, int can_target);
void draw_id_tag(HDC dc, RECT *rect, int player, int card, int enabled);
void draw_card_debug_id_tag(HDC dc, RECT *rect, int player, int card);
int card_window_matches_action_request(HWND hwnd);
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2);
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd);
void notify_duel_action(int player, unsigned int value);
unsigned int refresh_duel_display_cache(void);
void set_duel_prompt_context(HWND hwnd, char *prompt, unsigned int mode);
void set_duel_tooltip_text(char *text);
void display_duel_prompt_text(char *text);
void get_landwalk_evasion_masks(unsigned int *out_landwalk_mask, unsigned int *out_basic_land_mask);
int can_block_attacker_with_abilities(int blocker_player, int blocker_card, int attacker_player, int attacker_card,
                                      unsigned int attacker_abilities, unsigned int land_bits);
int can_card_join_banding_group(int player, int card, int unused_player, int unused_card);
int can_block_attacker_with_landwalk_checks(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
int assign_blocker_to_attacker(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
int get_card_display_pic_num(card_id_t card_id, int player, int card);
int get_displayed_card_eot_toughness(int player, int card);
int get_displayed_card_toughness(int player, int card);
int get_displayed_card_power(int player, int card);
void draw_manastripes(HDC dc, RECT *rect, int player, int card);
void draw_powertoughness_on_smallcard(HDC dc, RECT *rect, unsigned int power, unsigned int toughness);
void draw_summoning_sickness_overlay(HDC dc, RECT *rect);
void draw_card_ui_flag_20_overlay(HDC dc, RECT *rect);
void draw_dying_overlay(HDC dc, RECT *rect);
void draw_ability_icons_on_smallcard(HDC dc, RECT *rect, unsigned int ability_flags);
int draw_damage(HDC dc, RECT *rect, int damage);
int get_card_special_counter_icon(card_id_t card_id);
int draw_special_counter_icons(HDC dc, RECT *rect, int icon, int count);
void get_special_counter_icons_rect(LPRECT rect, RECT *client_rect, int counters);
unsigned int get_displayed_card_special_counter_count(int player, int card);
void get_displayed_card_standard_counters(int player, int card, unsigned int *counter_power,
                                          unsigned int *counter_m1m1,
                                          unsigned int *counter_p1p1,
                                          unsigned int *counter_p0p1,
                                          unsigned int *counter_p1p0);
unsigned int get_displayed_card_title_flags(int player, int card);
void format_standard_counter_cuecard_text(char *text, int counter_type, int counters);
void get_damage_overlay_rect(LPRECT rect, RECT *client_rect);
void get_will_untap_overlay_rect(LPRECT rect, RECT *client_rect);
int draw_standard_counters(HDC dc, RECT *rect, int power, int m1m1,
                           int p1p1, int loyalty, int charge);

extern char g_card_rules_text_buffer_00708da8[];
extern char g_activation_rules_text_buffer_00708f48[];
extern char g_card_title_buffer_00709100[];
extern char g_activation_card_title_buffer_00709150[];
extern int g_duel_cached_active_cards_count_player_0;
extern int g_duel_cached_active_cards_count_player_1;
extern char global_base_directory[];
extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[2000];
extern BITMAPINFO g_duel_backbuffer_bmi;
extern int g_duel_modal_action_active;
extern target_selection_request_t g_duel_action_request_copy;
extern HWND g_duel_tooltip_window;

// GLOBAL: MAGIC 0x0055e0fc
// GLOBAL: SHANDALAR 0x0057f19c
int g_cardclass_player_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e100
// GLOBAL: SHANDALAR 0x0057f1a0
int g_cardclass_card_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e104
// GLOBAL: SHANDALAR 0x0057f1a4
int g_cardclass_hidden_parent_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e108
// GLOBAL: SHANDALAR 0x0057f1a8
int g_cardclass_snapshot_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e10c
// GLOBAL: SHANDALAR 0x0057f1ac
int g_cardclass_previous_z_order_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e110
// GLOBAL: SHANDALAR 0x0057f1b0
int g_cardclass_window_extra_bytes = 0x14;

// GLOBAL: MAGIC 0x0069b3b8
// GLOBAL: SHANDALAR 0x00732800
char g_cardclass_menu_view_stats_text[0x68];

// GLOBAL: MAGIC 0x0069b420
// GLOBAL: SHANDALAR 0x00732868
char g_cardclass_menu_view_card_text[0x68];

// GLOBAL: MAGIC 0x0069b488
// GLOBAL: SHANDALAR 0x007328d0
char g_cardclass_cuecard_counter_tetravus_text[0x68];

// GLOBAL: MAGIC 0x0069b4f0
// GLOBAL: SHANDALAR 0x00732938
char g_cardclass_prompt_defend_attacker_text[0x68];

// GLOBAL: MAGIC 0x0069b558
// GLOBAL: SHANDALAR 0x007329a0
char g_cardclass_cuecard_counter_unstable_mutation_text[0x68];

// GLOBAL: MAGIC 0x0069b5c0
// GLOBAL: SHANDALAR 0x00732a08
char g_cardclass_cuecard_counter_ashnods_transmogrant_text[0x68];

// GLOBAL: MAGIC 0x0069b628
// GLOBAL: SHANDALAR 0x00732a70
char g_cardclass_cuecard_counter_rock_hydra_text[0x68];

// GLOBAL: MAGIC 0x0069b690
// GLOBAL: SHANDALAR 0x00732ad8
char g_cardclass_cuecard_counter_cyclone_text[0x68];

// GLOBAL: MAGIC 0x0069b6f8
// GLOBAL: SHANDALAR 0x00732b40
char g_cardclass_menu_autotarget_text[0x68];

// GLOBAL: MAGIC 0x0069b760
// GLOBAL: SHANDALAR 0x00732ba8
char g_cardclass_cuecard_counter_orcish_catapult_text[0x64];

// GLOBAL: MAGIC 0x0069b7c4
// GLOBAL: SHANDALAR 0x00732c0c
HMENU g_cardclass_alternate_card_submenu;

// GLOBAL: MAGIC 0x0069b7c8
// GLOBAL: SHANDALAR 0x00732c10
char g_cardclass_menu_protect_text[0x68];

// GLOBAL: MAGIC 0x0069b830
// GLOBAL: SHANDALAR 0x00732c78
char g_cardclass_cuecard_counter_lucky_charms_text[0x68];

// GLOBAL: MAGIC 0x0069b898
// GLOBAL: SHANDALAR 0x00732ce0
char g_cardclass_prompt_band_whom_text[0x68];

// GLOBAL: MAGIC 0x0069b900
// GLOBAL: SHANDALAR 0x00732d48
char g_cardclass_cuecard_counter_citanul_druid_text[0x68];

// GLOBAL: MAGIC 0x0069b968
// GLOBAL: SHANDALAR 0x00732db0
int g_cardclass_ability_masks[0x11];

// GLOBAL: MAGIC 0x0069b9b0
// GLOBAL: SHANDALAR 0x00732df8
char g_cardclass_cuecard_counter_osai_vultures_text[0x68];

// GLOBAL: MAGIC 0x0069ba18
// GLOBAL: SHANDALAR 0x00732e60
char g_cardclass_prompt_defend_error_text[0x68];

// GLOBAL: MAGIC 0x0069ba80
// GLOBAL: SHANDALAR 0x00732ec8
char g_cardclass_prompt_band_error_text[0x68];

// GLOBAL: MAGIC 0x0069bae8
// GLOBAL: SHANDALAR 0x00732f30
char g_cardclass_cuecard_counter_clockwork_avian_text[0x68];

// GLOBAL: MAGIC 0x0069bb50
// GLOBAL: SHANDALAR 0x00732f98
char g_cardclass_cuecard_counter_armageddon_clock_text[0x68];

// GLOBAL: MAGIC 0x0069bbb8
// GLOBAL: SHANDALAR 0x00733000
char g_cardclass_cuecard_counter_clockwork_beast_text[0x68];

// GLOBAL: MAGIC 0x0069bc20
// GLOBAL: SHANDALAR 0x00733068
char g_cardclass_cuecard_counter_spirit_shackle_text[0x68];

// GLOBAL: MAGIC 0x0069bc88
// GLOBAL: SHANDALAR 0x007330d0
char g_cardclass_cuecard_counter_dwarven_weaponsmith_text[0x68];

// GLOBAL: MAGIC 0x0069bcf0
// GLOBAL: SHANDALAR 0x00733138
char g_cardclass_menu_help_text[0x64];

// GLOBAL: MAGIC 0x0069bd54
// GLOBAL: SHANDALAR 0x0073319c
HMENU g_cardclass_popup_menu;

// GLOBAL: MAGIC 0x0069bd58
// GLOBAL: SHANDALAR 0x007331a0
char g_cardclass_prompt_defend_prompt_text[0x68];

// GLOBAL: MAGIC 0x0069bdc0
// GLOBAL: SHANDALAR 0x00733208
char g_cardclass_cuecard_counter_triskelion_text[0x70];

// GLOBAL: MAGIC 0x0069be30
// GLOBAL: SHANDALAR 0x00733278
char g_cardclass_cuecard_counter_sengir_vampire_text[0x68];

// GLOBAL: MAGIC 0x0069be98
// GLOBAL: SHANDALAR 0x007332e0
char g_cardclass_cuecard_counter_khabal_ghoul_text[0x68];

// GLOBAL: MAGIC 0x0069bf00
// GLOBAL: SHANDALAR 0x00733348
char g_cardclass_menu_show_damage_text[0x68];

// GLOBAL: MAGIC 0x0069bf68
// GLOBAL: SHANDALAR 0x007333b0
char g_cardclass_cuecard_counter_living_artifact_text[0x68];

// GLOBAL: MAGIC 0x0069bfd0
// GLOBAL: SHANDALAR 0x00733418
char g_cardclass_cuecard_counter_time_vault_text[0x68];

// GLOBAL: MAGIC 0x0069c038
// GLOBAL: SHANDALAR 0x00733480
char g_cardclass_prompt_band_cannot_text[0x68];

// GLOBAL: MAGIC 0x0069c0a0
// GLOBAL: SHANDALAR 0x007334e8
char g_cardclass_cuecard_counter_fungusaur_text[0x68];

// GLOBAL: MAGIC 0x0069c108
// GLOBAL: SHANDALAR 0x00733550
char g_cardclass_cuecard_counter_mana_battery_text[0x68];

// GLOBAL: MAGIC 0x0069c170
// GLOBAL: SHANDALAR 0x007335b8
char g_cardclass_cuecard_counter_whirling_dervish_text[0x68];

// GLOBAL: MAGIC 0x0069c1d8
// GLOBAL: SHANDALAR 0x00733620
char g_cardclass_menu_id_tags_text[0x68];

// GLOBAL: MAGIC 0x0069c240
// GLOBAL: SHANDALAR 0x00733688
target_selection_result_t g_cardclass_click_action_result;

// GLOBAL: MAGIC 0x0069c250
// GLOBAL: SHANDALAR 0x00733698
char g_cardclass_cuecard_counter_scavenging_ghouls_text[0x68];

// GLOBAL: MAGIC 0x0069c2b8
// GLOBAL: SHANDALAR 0x00733700
char g_cardclass_cuecard_counter_necropolis_of_azar_text[0x68];

// GLOBAL: MAGIC 0x0069c320
// GLOBAL: SHANDALAR 0x00733768
char g_cardclass_menu_alternate_card_text[0x68];

// GLOBAL: MAGIC 0x005750b0
// GLOBAL: SHANDALAR 0x005a10b8
char *PTR_s_Type_005750b0[20] =
    {
        "Type ",
        "Summoned ",
        "Attacking Bit ",
        "Tapped Bit ",
        "Owner ",
        "Targeted ",
        "MayNotTarget ",
        "Damage ",
        "Power ",
        "Tough ",
        "Mana ",
        "Color ",
        "Blocking ",
        "Hack ",
        "Data ",
        "Counters ",
        "HighLight ",
        "LinkedID ",
        "UntapStatus ",
        "Dying "};

// FUNCTION: MAGIC 0x00449e0a
// FUNCTION: SHANDALAR 0x0045398b
int copy_displayed_card_snapshot(card_instance_t *snapshot, int player, int card)
{
  if (snapshot == NULL)
  {
    return 0;
  }

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  memcpy(snapshot, &global_displayed_card_instances[player][card], sizeof(card_instance_t));
  LeaveCriticalSection(&g_duel_render_lock);
  return 1;
}

// FUNCTION: MAGIC 0x004d1889
// FUNCTION: SHANDALAR 0x00577e65
unsigned int log_card_snapshot_changes(card_instance_t *old_snapshot, card_instance_t *new_snapshot)
{
  unsigned int changed_flags;
  int changed_index;
  char debug_text[256];

  changed_flags = 0;
  if (new_snapshot->internal_card_id != old_snapshot->internal_card_id)
  {
    changed_flags |= 1;
  }
  else
  {
    if ((new_snapshot->state & STATE_SUMMONSICK_BOTH) !=
        (old_snapshot->state & STATE_SUMMONSICK_BOTH))
    {
      changed_flags |= 2;
    }
    if ((new_snapshot->state & STATE_TAPPED) != (old_snapshot->state & STATE_TAPPED))
    {
      changed_flags |= 8;
    }
    if ((new_snapshot->state & STATE_OWNED_BY_OPPONENT) !=
        (old_snapshot->state & STATE_OWNED_BY_OPPONENT))
    {
      changed_flags |= 0x10;
    }
    if ((new_snapshot->state & STATE_TARGETTED) != (old_snapshot->state & STATE_TARGETTED))
    {
      changed_flags |= 0x20;
    }
    if ((new_snapshot->state & STATE_CANNOT_TARGET) !=
        (old_snapshot->state & STATE_CANNOT_TARGET))
    {
      changed_flags |= 0x40;
    }
    if ((new_snapshot->state & STATE_OUBLIETTED) != (old_snapshot->state & STATE_OUBLIETTED))
    {
      changed_flags |= 0x40;
    }
    if (new_snapshot->damage_on_card != old_snapshot->damage_on_card)
    {
      changed_flags |= 0x80;
    }
    if (new_snapshot->power != old_snapshot->power)
    {
      changed_flags |= 0x100;
    }
    if (new_snapshot->toughness != old_snapshot->toughness)
    {
      changed_flags |= 0x200;
    }
    if (new_snapshot->mana_color != old_snapshot->mana_color)
    {
      changed_flags |= 0x400;
    }
    if (new_snapshot->color != old_snapshot->color)
    {
      changed_flags |= 0x800;
    }
    if ((char)new_snapshot->blocking != (char)old_snapshot->blocking)
    {
      changed_flags |= 0x1000;
    }
    if (new_snapshot->info_slot != old_snapshot->info_slot)
    {
      changed_flags |= 0x4000;
    }
    if (new_snapshot->special_counters != old_snapshot->special_counters)
    {
      changed_flags |= 0x8000;
    }
    if (new_snapshot->counters != old_snapshot->counters)
    {
      changed_flags |= 0x8000;
    }
    if ((new_snapshot->token_status & STATUS_RED_BORDER) !=
        (old_snapshot->token_status & STATUS_RED_BORDER))
    {
      changed_flags |= 0x10000;
    }
    if (*(int *)&new_snapshot->display_pic_csv_id != *(int *)&old_snapshot->display_pic_csv_id)
    {
      changed_flags |= 0x20000;
    }
    if (new_snapshot->untap_status != old_snapshot->untap_status)
    {
      changed_flags |= 0x40000;
    }
    if ((char)new_snapshot->kill_code != (char)old_snapshot->kill_code)
    {
      changed_flags |= 0x80000;
    }
  }
  sprintf(debug_text, "\n\n%s %s\n",
          global_cards_data[old_snapshot->internal_card_id].name,
          global_cards_data[new_snapshot->internal_card_id].name);
  OutputDebugStringA(debug_text);
  for (changed_index = 0; changed_index < 0x20; changed_index++)
  {
    if ((changed_flags & (1 << (unsigned char)changed_index)) != 0)
    {
      if (changed_index == 0)
      {
        sprintf(debug_text, "%s %d %d ", PTR_s_Type_005750b0[changed_index],
                old_snapshot->internal_card_id, new_snapshot->internal_card_id);
      }
      else
      {
        sprintf(debug_text, "%s", PTR_s_Type_005750b0[changed_index]);
      }
      OutputDebugStringA(debug_text);
    }
  }
  return changed_flags;
}

// FUNCTION: MAGIC 0x004d15b3
// FUNCTION: SHANDALAR 0x00577b8f
int card_snapshots_match_for_redraw(card_instance_t *old_snapshot, card_instance_t *new_snapshot)
{
  int unchanged;

  unchanged = 1;
  if (new_snapshot->internal_card_id != old_snapshot->internal_card_id)
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_SUMMONSICK_BOTH) !=
      (old_snapshot->state & STATE_SUMMONSICK_BOTH))
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_TAPPED) != (old_snapshot->state & STATE_TAPPED))
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_OWNED_BY_OPPONENT) !=
      (old_snapshot->state & STATE_OWNED_BY_OPPONENT))
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_TARGETTED) != (old_snapshot->state & STATE_TARGETTED))
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_CANNOT_TARGET) !=
      (old_snapshot->state & STATE_CANNOT_TARGET))
  {
    unchanged = 0;
  }
  if ((new_snapshot->state & STATE_OUBLIETTED) != (old_snapshot->state & STATE_OUBLIETTED))
  {
    unchanged = 0;
  }
  if (new_snapshot->damage_on_card != old_snapshot->damage_on_card)
  {
    unchanged = 0;
  }
  if (new_snapshot->power != old_snapshot->power)
  {
    unchanged = 0;
  }
  if (new_snapshot->toughness != old_snapshot->toughness)
  {
    unchanged = 0;
  }
  if (new_snapshot->mana_color != old_snapshot->mana_color)
  {
    unchanged = 0;
  }
  if (new_snapshot->color != old_snapshot->color)
  {
    unchanged = 0;
  }
  if ((char)new_snapshot->blocking != (char)old_snapshot->blocking)
  {
    unchanged = 0;
  }
  if (new_snapshot->info_slot != old_snapshot->info_slot)
  {
    unchanged = 0;
  }
  if (new_snapshot->special_counters != old_snapshot->special_counters)
  {
    unchanged = 0;
  }
  if (new_snapshot->counters != old_snapshot->counters)
  {
    unchanged = 0;
  }
  if ((new_snapshot->token_status & STATUS_RED_BORDER) !=
      (old_snapshot->token_status & STATUS_RED_BORDER))
  {
    unchanged = 0;
  }
  if (*(int *)&new_snapshot->display_pic_csv_id != *(int *)&old_snapshot->display_pic_csv_id)
  {
    unchanged = 0;
  }
  if (new_snapshot->untap_status != old_snapshot->untap_status)
  {
    unchanged = 0;
  }
  if ((char)new_snapshot->kill_code != (char)old_snapshot->kill_code)
  {
    unchanged = 0;
  }
  if (unchanged == 0 && g_duel_palette_refresh_pending != 0)
  {
    log_card_snapshot_changes(old_snapshot, new_snapshot);
  }
  return unchanged;
}

// FUNCTION: MAGIC 0x004d1c68
// FUNCTION: SHANDALAR 0x00578244
int card_snapshots_match_ability_icons(card_instance_t *old_snapshot, card_instance_t *new_snapshot)
{
  int unchanged;

  unchanged = 1;
  if (new_snapshot->regen_status != old_snapshot->regen_status)
  {
    unchanged = 0;
  }
  return unchanged;
}

// FUNCTION: MAGIC 0x004d1314
// FUNCTION: SHANDALAR 0x005778f0
int card_window_matches_action_request(HWND hwnd)
{
  struct
  {
    int requested_owner;
    int card;
    HWND parent;
    int type_flags;
    int result;
    int requested_zone;
    unsigned int color_flags;
    int player;
  } s;

  s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
  s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
  s.type_flags = get_displayed_card_type(s.player, s.card);
  s.color_flags = get_displayed_card_color_flags(s.player, s.card);
  s.requested_owner = -1;
  s.requested_zone = -1;
  s.parent = GetParent(hwnd);
  if (s.parent == g_duel_full_card_window_hwnd)
  {
    s.requested_owner = 0;
    s.requested_zone = 2;
  }
  else if (s.parent == g_duel_life_window_hwnd)
  {
    s.requested_owner = 1;
    s.requested_zone = 2;
  }
  else if (s.parent == g_duel_player_battlefield_window_hwnd)
  {
    s.requested_owner = 0;
    s.requested_zone = 1;
  }
  else if (s.parent == g_duel_help_owner_hwnd)
  {
    s.requested_owner = 1;
    s.requested_zone = 1;
  }
  else if (s.parent == g_duel_player_graveyard_window_hwnd)
  {
    s.requested_owner = 0;
    s.requested_zone = 0x10;
  }
  else if (s.parent == g_phasebar_your_untap_window_hwnd)
  {
    s.requested_owner = 1;
    s.requested_zone = 0x10;
  }

  if ((g_duel_action_request_copy.player == -1 || s.player == g_duel_action_request_copy.player) &&
      (g_duel_action_request_copy.type_flags == -1 ||
       g_duel_action_request_copy.type_flags == 0 ||
       (s.type_flags & g_duel_action_request_copy.type_flags) != 0) &&
      (g_duel_action_request_copy.color_flags == -1 ||
       g_duel_action_request_copy.color_flags == 0 ||
       (g_duel_action_request_copy.color_flags & s.color_flags) != 0 ||
       ((g_duel_action_request_copy.color_flags & 0x100) != 0 && (s.type_flags & 0x40) != 0) ||
       ((g_duel_action_request_copy.color_flags & 0x200) != 0 && (s.type_flags & 1) != 0)) &&
      (g_duel_action_request_copy.owner == -1 || g_duel_action_request_copy.owner == s.requested_owner) &&
      (g_duel_action_request_copy.zone_flags == -1 || (g_duel_action_request_copy.zone_flags & s.requested_zone) != 0))
  {
    s.result = 1;
  }
  else
  {
    s.result = 0;
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004d1263
// FUNCTION: SHANDALAR 0x0057783f
void post_card_action_selection(HWND hwnd)
{
  struct
  {
    int attached_player_and_card[2];
    int card;
    int player;
    card_id_t card_id;
  } s;

  s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
  s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
  s.card_id = get_displayed_card_id(s.player, s.card);
  if (g_activation_display_internal_card_id == s.card_id)
  {
    get_displayed_card_parent(s.attached_player_and_card, s.player, s.card);
    g_cardclass_click_action_result.target_player = s.attached_player_and_card[0];
    g_cardclass_click_action_result.target_card = s.attached_player_and_card[1];
  }
  else
  {
    g_cardclass_click_action_result.target_player = s.player;
    g_cardclass_click_action_result.target_card = s.card;
  }
  g_cardclass_click_action_result.selection_code = 0;
  PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)&g_cardclass_click_action_result);
}

// FUNCTION: MAGIC 0x0044850c
// FUNCTION: SHANDALAR 0x00452089
unsigned int get_displayed_card_special_counter_count(int player, int card)
{
  unsigned int counters;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  counters = global_displayed_card_instances[player][card].special_counters & 0xff;
  LeaveCriticalSection(&g_duel_render_lock);
  return counters;
}

// FUNCTION: MAGIC 0x0044858d
// FUNCTION: SHANDALAR 0x0045210a
void get_displayed_card_standard_counters(int player,
                                          int card,
                                          unsigned int *counter_power,
                                          unsigned int *counter_m1m1,
                                          unsigned int *counter_p1p1,
                                          unsigned int *counter_p0p1,
                                          unsigned int *counter_p1p0)
{
  if (displayed_card_indices_invalid(player, card) != 0)
    return;

  EnterCriticalSection(&g_duel_render_lock);
  *counter_power = (global_displayed_card_instances[player][card].special_counters & 0xff00) >> 8;
  *counter_m1m1 = (global_displayed_card_instances[player][card].special_counters & 0xff0000) >> 0x10;
  *counter_p1p1 = global_displayed_card_instances[player][card].special_counters >> 0x18;
  *counter_p0p1 = global_displayed_card_instances[player][card].counters & 0xff;
  *counter_p1p0 = (global_displayed_card_instances[player][card].counters & 0xff00) >> 8;
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x0044875b
// FUNCTION: SHANDALAR 0x004522d8
int get_displayed_card_damage_on_card(int player, int card)
{
  int damage_on_card;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  damage_on_card = global_displayed_card_instances[player][card].damage_on_card;
  LeaveCriticalSection(&g_duel_render_lock);
  return damage_on_card;
}

// FUNCTION: MAGIC 0x00448df6
// FUNCTION: SHANDALAR 0x00452973
int get_displayed_card_type(int player, int card)
{
  int internal_card_id;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  internal_card_id = get_displayed_card_internal_id(player, card);
  if (internal_card_id == -1)
  {
    return 0;
  }
  else
  {
    return global_cards_data[internal_card_id].type;
  }
}

// FUNCTION: MAGIC 0x00448f5c
// FUNCTION: SHANDALAR 0x00452ad9
unsigned int get_displayed_card_regen_status(int player, int card)
{
  unsigned int regen_status;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  regen_status = global_displayed_card_instances[player][card].regen_status;
  if ((global_displayed_card_instances[player][card].regen_status & 0x200000) != 0 &&
      (global_displayed_card_instances[player][card].state & STATE_ATTACKING) != 0)
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

// FUNCTION: MAGIC 0x004490d4
// FUNCTION: SHANDALAR 0x00452c51
int get_displayed_card_mana_color(int player, int card)
{
  int mana_color;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  mana_color = global_displayed_card_instances[player][card].mana_color;
  LeaveCriticalSection(&g_duel_render_lock);
  return mana_color;
}

// FUNCTION: MAGIC 0x00449151
// FUNCTION: SHANDALAR 0x00452cce
state_t get_displayed_card_state(int player, int card)
{
  state_t state;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  state = global_displayed_card_instances[player][card].state;
  LeaveCriticalSection(&g_duel_render_lock);
  return state;
}

// FUNCTION: MAGIC 0x0044931a
// FUNCTION: SHANDALAR 0x00452e97
int is_displayed_card_owned_by_opponent(int player, int card)
{
  int state;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0xffffffff;
  }

  if (get_displayed_card_state(player, card) & STATE_OWNED_BY_OPPONENT)
  {
    state = 1;
  }
  else
  {
    state = 0;
  }

  return state;
}

// FUNCTION: MAGIC 0x0044937e
// FUNCTION: SHANDALAR 0x00452efb
unsigned int get_displayed_card_mana_source_flag(int player, int card)
{
  int internal_card_id;
  unsigned int result;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  internal_card_id = get_displayed_card_internal_id(player, card);
  if (internal_card_id == -1)
  {
    return 0;
  }

  result = global_cards_data[internal_card_id].extra_ability & EA_MANA_SOURCE;
  return result;
}

// FUNCTION: MAGIC 0x004494a3
// FUNCTION: SHANDALAR 0x00453020
unsigned int get_displayed_card_title_flags(int player, int card)
{
  unsigned int info_slot;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  info_slot = global_displayed_card_instances[player][card].unknown0x70;
  LeaveCriticalSection(&g_duel_render_lock);
  return info_slot;
}

// FUNCTION: MAGIC 0x0044951f
// FUNCTION: SHANDALAR 0x0045309c
int is_displayed_card_targetted(int player, int card)
{
  int result;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  result = (global_displayed_card_instances[player][card].state & STATE_TARGETTED) != 0;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x004495b2
// FUNCTION: SHANDALAR 0x0045312f
int is_displayed_card_untargetable(int player, int card)
{
  int result;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  if ((global_displayed_card_instances[player][card].state & STATE_CANNOT_TARGET) != 0)
  {
    result = 0;
  }
  else
  {
    result = 1;
  }
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x00449914
// FUNCTION: SHANDALAR 0x00453495
unsigned int get_displayed_card_untap_status(int player, int card)
{
  unsigned int untap_status;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  untap_status = global_displayed_card_instances[player][card].untap_status;
  LeaveCriticalSection(&g_duel_render_lock);
  return untap_status;
}

// FUNCTION: MAGIC 0x00449b42
// FUNCTION: SHANDALAR 0x004536c3
int get_displayed_card_kill_code(int player, int card)
{
  int kill_code;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  kill_code = (int)(char)global_displayed_card_instances[player][card].kill_code;
  LeaveCriticalSection(&g_duel_render_lock);
  return kill_code;
}

// FUNCTION: MAGIC 0x0044a2f1
// FUNCTION: SHANDALAR 0x00453e74
int get_displayed_player_card_counts(int *player_card_counts)
{
  if (player_card_counts == NULL)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  player_card_counts[0] = g_duel_cached_active_cards_count_player_0;
  player_card_counts[1] = g_duel_cached_active_cards_count_player_1;
  LeaveCriticalSection(&g_duel_render_lock);
  return 1;
}

// FUNCTION: MAGIC 0x004d1d5d
// FUNCTION: SHANDALAR 0x00578339
void format_special_counter_cuecard_text(char *text, card_id_t card_id, unsigned int counters)
{
  if (text == NULL)
  {
    return;
  }

  if (card_id == CARD_ID_ARMAGEDDON_CLOCK)
  {
    sprintf(text, g_cardclass_cuecard_counter_armageddon_clock_text, counters);
  }
  else if (card_id == CARD_ID_BLACK_MANA_BATTERY)
  {
    sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
  }
  else if (card_id == CARD_ID_BLUE_MANA_BATTERY)
  {
    sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
  }
  else if (card_id == CARD_ID_GREEN_MANA_BATTERY)
  {
    sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
  }
  else if (card_id == CARD_ID_RED_MANA_BATTERY)
  {
    sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
  }
  else if (card_id == CARD_ID_WHITE_MANA_BATTERY)
  {
    sprintf(text, g_cardclass_cuecard_counter_mana_battery_text, counters);
  }
  else if (card_id == CARD_ID_CLOCKWORK_AVIAN)
  {
    sprintf(text, g_cardclass_cuecard_counter_clockwork_avian_text, counters);
  }
  else if (card_id == CARD_ID_CLOCKWORK_BEAST)
  {
    sprintf(text, g_cardclass_cuecard_counter_clockwork_beast_text, counters);
  }
  else if (card_id == CARD_ID_CRYSTAL_ROD)
  {
    sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
  }
  else if (card_id == CARD_ID_IRON_STAR)
  {
    sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
  }
  else if (card_id == CARD_ID_IVORY_CUP)
  {
    sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
  }
  else if (card_id == CARD_ID_THRONE_OF_BONE)
  {
    sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
  }
  else if (card_id == CARD_ID_WOODEN_SPHERE)
  {
    sprintf(text, g_cardclass_cuecard_counter_lucky_charms_text, counters);
  }
  else if (card_id == CARD_ID_FUNGUSAUR)
  {
    sprintf(text, g_cardclass_cuecard_counter_fungusaur_text, counters);
  }
  else if (card_id == CARD_ID_WHIRLING_DERVISH)
  {
    sprintf(text, g_cardclass_cuecard_counter_whirling_dervish_text, counters);
  }
  else if (card_id == CARD_ID_LIVING_ARTIFACT)
  {
    sprintf(text, g_cardclass_cuecard_counter_living_artifact_text, counters);
  }
  else if (card_id == CARD_ID_OSAI_VULTURES)
  {
    sprintf(text, g_cardclass_cuecard_counter_osai_vultures_text, counters);
  }
  else if (card_id == CARD_ID_SCAVENGING_GHOUL)
  {
    sprintf(text, g_cardclass_cuecard_counter_scavenging_ghouls_text, counters);
  }
  else if (card_id == CARD_ID_SENGIR_VAMPIRE)
  {
    sprintf(text, g_cardclass_cuecard_counter_sengir_vampire_text, counters);
  }
  else if (card_id == CARD_ID_NECROPOLIS_OF_AZAR)
  {
    sprintf(text, g_cardclass_cuecard_counter_necropolis_of_azar_text, counters);
  }
  else if (card_id == CARD_ID_TRISKELION)
  {
    sprintf(text, g_cardclass_cuecard_counter_triskelion_text, counters);
  }
  else if (card_id == CARD_ID_TETRAVUS)
  {
    sprintf(text, g_cardclass_cuecard_counter_tetravus_text, counters);
  }
  else if (card_id == CARD_ID_TIME_VAULT)
  {
    sprintf(text, g_cardclass_cuecard_counter_time_vault_text, counters);
  }
  else if (card_id == CARD_ID_CYCLONE)
  {
    sprintf(text, g_cardclass_cuecard_counter_cyclone_text, counters);
  }
  else if (card_id == CARD_ID_CITANUL_DRUID)
  {
    sprintf(text, g_cardclass_cuecard_counter_citanul_druid_text, counters);
  }
  else if (card_id == CARD_ID_ROCK_HYDRA)
  {
    sprintf(text, g_cardclass_cuecard_counter_rock_hydra_text, counters);
  }
  else if (card_id == CARD_ID_KHABAL_GHOUL)
  {
    sprintf(text, g_cardclass_cuecard_counter_khabal_ghoul_text, counters);
  }
  else
  {
    text[0] = '\0';
  }
}

// FUNCTION: MAGIC 0x004d21a9
// FUNCTION: SHANDALAR 0x00578785
void format_standard_counter_cuecard_text(char *text, int counter_type, int counters)
{
  if (text == NULL)
    return;

  strcpy(text, "");
  if (counters == 0)
    return;

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

// FUNCTION: MAGIC 0x0055bcb1
// FUNCTION: SHANDALAR 0x00571fcf
void get_damage_overlay_rect(LPRECT rect, RECT *client_rect)
{
  int width;
  int height;

  if (rect == NULL)
    return;

  if (client_rect == NULL)
  {
    SetRect(rect, 0, 0, 0, 0);
  }
  else
  {
    width = client_rect->right - client_rect->left;
    height = client_rect->bottom - client_rect->top;
    rect->left = client_rect->left + (width * 0x41) / 100;
    rect->right = client_rect->right - (width * 3) / 100;
    rect->top = (height * 8) / 100 + client_rect->top;
    rect->bottom = (height * 0x1c) / 100 + client_rect->top;
  }
}

// FUNCTION: MAGIC 0x0055c12a
// FUNCTION: SHANDALAR 0x00572448
void get_special_counter_icons_rect(LPRECT rect, RECT *client_rect, int counters)
{
  struct
  {
    int step_width;
    int rect_top;
    int rect_left;
    BITMAP bitmap;
    int text_height;
    int bitmap_width;
    int icon_height;
    int icon_width;
    RECT result_rect;
    int left;
    int top;
    int right_limit;
    int bottom;
  } s;

  if (rect == NULL)
    return;

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
    if (global_card_counters_pic != NULL)
    {
      s.left = client_rect->left + ((client_rect->right - client_rect->left) * 7) / 100;
      s.right_limit = client_rect->right - ((client_rect->right - client_rect->left) * 10) / 100;
      s.top = client_rect->top + ((client_rect->bottom - client_rect->top) * 8) / 100;
      s.bottom = client_rect->top + ((client_rect->bottom - client_rect->top) * 0x23) / 100;
      GetObjectA(global_card_counters_pic, sizeof(BITMAP), &s.bitmap);
      s.bitmap_width = s.bitmap.bmWidth;
      s.text_height = s.bitmap.bmHeight / 0x19;
      s.icon_height = s.bottom - s.top;
      s.icon_width = (s.bitmap_width * s.icon_height) / s.text_height;
      s.rect_left = s.left;
      s.rect_top = s.top;
      s.step_width = s.icon_width;
      while (s.right_limit < s.step_width * (counters - 1) + s.icon_width + s.rect_left &&
             1 < s.step_width)
      {
        s.step_width--;
      }
      SetRect(&s.result_rect, s.rect_left, s.rect_top,
              s.step_width * (counters - 1) + s.icon_width + s.rect_left,
              s.icon_height + s.rect_top);
    }
    CopyRect(rect, &s.result_rect);
  }
}

// FUNCTION: MAGIC 0x0055ca6f
// FUNCTION: SHANDALAR 0x00572d8c
void get_ability_icon_rect(LPRECT rect, unsigned int ability, RECT *client_rect, unsigned int ability_flags)
{
  struct
  {
    BITMAP bitmap;
    int bitmap_row_height;
    int bitmap_width;
    int icon_width;
    int y;
    int index;
    int x;
    unsigned int ability_masks[18];
    RECT result_rect;
    int row;
    int icon_height;
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

  if (rect == NULL)
  {
    return;
  }

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
    s.icon_width = ((client_rect->right - client_rect->left) * 0x11) / 100;
    if (global_abilities_pic != NULL)
    {
      GetObjectA(global_abilities_pic, sizeof(BITMAP), &s.bitmap);
      s.bitmap_width = s.bitmap.bmWidth;
      s.bitmap_row_height = s.bitmap.bmHeight / (int)(s.ability_masks[17] + 1);
      s.icon_height = (s.icon_width * s.bitmap_row_height) / s.bitmap_width;
    }
    else
    {
      s.icon_height = s.icon_width;
    }
    s.x = client_rect->left + 1;
    s.y = (client_rect->bottom - 1) - s.icon_height;
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
             client_rect->right - ((client_rect->right - client_rect->left) * 0x19) / 100 < s.icon_width + s.x) ||
            (s.row >= 3 && client_rect->right < s.icon_width + s.x))
        {
          s.x = client_rect->left + 1;
          s.y -= s.icon_height + 1;
          s.row++;
        }
      }
    }
    CopyRect(rect, &s.result_rect);
  }
}

// FUNCTION: MAGIC 0x0055d471
// FUNCTION: SHANDALAR 0x0057378d
void get_will_untap_overlay_rect(LPRECT rect, RECT *client_rect)
{
  int width;
  int height;

  if (rect == NULL)
    return;

  if (client_rect == NULL)
  {
    SetRect(rect, 0, 0, 0, 0);
  }
  else
  {
    width = ((client_rect->right - client_rect->left) * 0x3c) / 100;
    height = ((client_rect->bottom - client_rect->top) * 0x3c) / 100;
    rect->left = client_rect->left + ((client_rect->right - client_rect->left) - width) / 2;
    rect->right = rect->left + width;
    rect->top = client_rect->top + ((client_rect->bottom - client_rect->top) - height) / 2;
    rect->bottom = rect->top + height;
  }
}

// FUNCTION: MAGIC 0x00448edf
// FUNCTION: SHANDALAR 0x00452a5c
int get_displayed_card_toughness(int player, int card)
{
  int toughness;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  toughness = global_displayed_card_instances[player][card].toughness;
  LeaveCriticalSection(&g_duel_render_lock);
  return toughness;
}

// FUNCTION: MAGIC 0x00448e62
// FUNCTION: SHANDALAR 0x004529df
int get_displayed_card_power(int player, int card)
{
  int power;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  power = global_displayed_card_instances[player][card].power;
  LeaveCriticalSection(&g_duel_render_lock);
  return power;
}

// FUNCTION: MAGIC 0x0055b806
// FUNCTION: SHANDALAR 0x00571b24
void draw_powertoughness_on_smallcard(HDC dc, RECT *rect, unsigned int power, unsigned int toughness)
{
  struct
  {
    unsigned int infinity_mask;
    char text[12];
    int saved_dc;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return;
  }
  s.saved_dc = SaveDC(dc);
  SetMapMode(dc, MM_ANISOTROPIC);
  SetWindowExtEx(dc, 200, 0x118, NULL);
  SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, NULL);
  SetWindowOrgEx(dc, 0, 0, NULL);
  SetViewportOrgEx(dc, rect->left, rect->top, NULL);

  s.infinity_mask = 0x4000;
  s.text[0] = '\0';
  if ((power & s.infinity_mask) != 0)
  {
    strcat(s.text, "*");
  }
  else
  {
    wsprintfA(&s.text[strlen(s.text)], "%d", power);
  }

  strcat(s.text, "/");
  if ((toughness & s.infinity_mask) != 0)
  {
    strcat(s.text, "*");
  }
  else
  {
    wsprintfA(&s.text[strlen(s.text)], "%d", toughness);
  }

  SelectObject(dc, global_smallcard_pt_font);
  SetTextAlign(dc, 10);
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, global_palette_col_c9);
  TextOutA(dc, 0xca, 0x11a, s.text, strlen(s.text));
  SetTextColor(dc, global_palette_col_9e_a);
  TextOutA(dc, 0xc6, 0x116, s.text, strlen(s.text));
  RestoreDC(dc, s.saved_dc);
}

// FUNCTION: MAGIC 0x0055bb59
// FUNCTION: SHANDALAR 0x00571e77
int draw_damage(HDC dc, RECT *rect, int damage)
{
  struct
  {
    int result;
    int saved_dc;
    char text[8];
    RECT damage_rect;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return 0;
  }

  s.saved_dc = SaveDC(dc);
  get_damage_overlay_rect(&s.damage_rect, rect);
  s.result = draw_masked_bitmap_left_half_to_rect(dc, &s.damage_rect, global_damage_pic);
  sprintf(s.text, "%d", damage);
  SelectObject(dc, global_damage_font);
  SetTextAlign(dc, 0);
  SetBkMode(dc, TRANSPARENT);
  SetMapMode(dc, MM_ANISOTROPIC);
  SetWindowExtEx(dc, s.damage_rect.right - s.damage_rect.left, 0x14, NULL);
  SetViewportExtEx(dc, s.damage_rect.right - s.damage_rect.left,
                   s.damage_rect.bottom - s.damage_rect.top, NULL);
  DPtoLP(dc, (LPPOINT)&s.damage_rect, 2);
  SetTextColor(dc, global_palette_col_c9);
  DrawTextA(dc, s.text, -1, &s.damage_rect, 0x25);
  OffsetRect(&s.damage_rect, -2, -2);
  SetTextColor(dc, global_palette_col_9e_b);
  DrawTextA(dc, s.text, -1, &s.damage_rect, 0x25);
  RestoreDC(dc, s.saved_dc);

  return s.result;
}

// FUNCTION: MAGIC 0x0055bfa9
// FUNCTION: SHANDALAR 0x005722c7
int draw_special_counter_icons(HDC dc, RECT *rect, int icon, int count)
{
  struct
  {
    int step_width;
    int unused_padding;
    int x;
    int result;
    BITMAP bitmap;
    int icon_row_height;
    int icon_index;
    int mask_y;
    int bitmap_width;
    RECT counters_rect;
    int icon_height;
    int src_y;
    int icon_width;
    RECT icon_rect;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return 0;
  }

  if (icon < 0 || 0x17 < icon)
  {
    return 0;
  }

  if (global_card_counters_pic != NULL)
  {
    get_special_counter_icons_rect(&s.counters_rect, rect, count);
    GetObjectA(global_card_counters_pic, sizeof(BITMAP), &s.bitmap);
    s.bitmap_width = s.bitmap.bmWidth;
    s.icon_row_height = s.bitmap.bmHeight / 0x19;
    s.icon_height = s.counters_rect.bottom - s.counters_rect.top;
    s.icon_width = (s.bitmap_width * s.icon_height) / s.icon_row_height;
    s.x = s.counters_rect.left;
    s.unused_padding = s.counters_rect.top;

    if (count > 1)
    {
      s.step_width = ((s.counters_rect.right - s.counters_rect.left) - s.icon_width) / (count - 1);
    }
    else
    {
      s.step_width = s.icon_width;
    }

    s.src_y = s.icon_row_height * icon;
    s.mask_y = s.bitmap.bmHeight - s.icon_row_height;
    s.x = s.counters_rect.right - s.icon_width;
    for (s.icon_index = 0; s.icon_index < count; s.icon_index++)
    {
      SetRect(&s.icon_rect, s.x, s.unused_padding, s.icon_width + s.x,
              s.icon_height + s.unused_padding);
      DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                             s.icon_row_height, 0, s.src_y, 0, s.mask_y);
      s.x -= s.step_width;
    }
    s.result = 1;
  }
  else
  {
    s.result = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0055c2e8
// FUNCTION: SHANDALAR 0x00572606
int draw_standard_counters(HDC dc, RECT *rect, int power, int m1m1,
                           int p1p1, int loyalty, int charge)
{
  struct
  {
    int min_step_width;
    int step_width;
    int counter_total;
    int y;
    int x;
    int bottom_y;
    int unused_result;
    BITMAP bitmap;
    int icon_row_height;
    int counter_index;
    int right_bottom_column;
    int mask_y;
    int bitmap_width;
    int left_bottom_column;
    int right_column;
    int card_height;
    int middle_column;
    int src_y;
    int column_width;
    int icon_width;
    int left_column;
    RECT icon_rect;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return 0;
  }

  s.counter_total = charge + loyalty + p1p1 + m1m1 + power;
  if (s.counter_total == 0)
  {
    return 1;
  }

  if (global_card_counters_pic == NULL)
  {
    return 0;
  }

  GetObjectA(global_card_counters_pic, sizeof(BITMAP), &s.bitmap);
  s.bitmap_width = s.bitmap.bmWidth;
  s.icon_row_height = s.bitmap.bmHeight / 0x19;
  s.card_height = rect->bottom - rect->top;
  s.icon_width = (s.bitmap_width * s.card_height) / s.icon_row_height;
  s.column_width = (rect->right - rect->left) / 3;
  s.left_column = rect->left;
  s.middle_column = s.left_column + s.column_width;
  s.right_column = s.middle_column + s.column_width;
  s.y = rect->top;
  s.bottom_y = s.y + (s.card_height * 2) / 3;
  s.left_bottom_column = (rect->left + (rect->right - rect->left) / 2) - s.column_width;
  s.right_bottom_column = s.left_bottom_column + s.column_width;
  s.min_step_width = 2;
  s.mask_y = s.bitmap.bmHeight - s.icon_row_height;

  s.step_width = s.icon_width;
  while (s.column_width < (p1p1 - 1) * s.step_width + s.icon_width && s.min_step_width < s.step_width)
  {
    s.step_width--;
  }
  s.x = (p1p1 - 1) * s.step_width + s.left_column;
  for (s.counter_index = 0; s.counter_index < p1p1; s.counter_index++)
  {
    s.src_y = s.icon_row_height * 0x14;
    SetRect(&s.icon_rect, s.x, s.y, s.icon_width + s.x, s.card_height + s.y);
    DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                           s.icon_row_height, 0, s.src_y, 0, s.mask_y);
    s.x -= s.step_width;
  }

  s.step_width = s.icon_width;
  while (s.column_width < (m1m1 - 1) * s.step_width + s.icon_width && s.min_step_width < s.step_width)
  {
    s.step_width--;
  }
  s.x = (m1m1 - 1) * s.step_width + s.middle_column;
  for (s.counter_index = 0; s.counter_index < m1m1; s.counter_index++)
  {
    s.src_y = s.icon_row_height * 0x16;
    SetRect(&s.icon_rect, s.x, s.y, s.icon_width + s.x, s.card_height + s.y);
    DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                           s.icon_row_height, 0, s.src_y, 0, s.mask_y);
    s.x -= s.step_width;
  }

  s.step_width = s.icon_width;
  while (s.column_width < (power - 1) * s.step_width + s.icon_width && s.min_step_width < s.step_width)
  {
    s.step_width--;
  }
  s.x = (power - 1) * s.step_width + s.right_column;
  for (s.counter_index = 0; s.counter_index < power; s.counter_index++)
  {
    s.src_y = s.icon_row_height * 0x15;
    SetRect(&s.icon_rect, s.x, s.y, s.icon_width + s.x, s.card_height + s.y);
    DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                           s.icon_row_height, 0, s.src_y, 0, s.mask_y);
    s.x -= s.step_width;
  }

  s.step_width = s.icon_width;
  while (s.column_width < (loyalty - 1) * s.step_width + s.icon_width && s.min_step_width < s.step_width)
  {
    s.step_width--;
  }
  s.x = (loyalty - 1) * s.step_width + s.left_bottom_column;
  for (s.counter_index = 0; s.counter_index < loyalty; s.counter_index++)
  {
    s.src_y = s.icon_row_height * 0x14;
    SetRect(&s.icon_rect, s.x, s.bottom_y, s.icon_width + s.x, s.card_height + s.bottom_y);
    DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                           s.icon_row_height, 0, s.src_y, 0, s.mask_y);
    s.x -= s.step_width;
  }

  s.step_width = s.icon_width;
  while (s.column_width < (charge - 1) * s.step_width + s.icon_width && s.min_step_width < s.step_width)
  {
    s.step_width--;
  }
  s.x = (charge - 1) * s.step_width + s.right_bottom_column;
  for (s.counter_index = 0; s.counter_index < charge; s.counter_index++)
  {
    s.src_y = s.icon_row_height * 0x12;
    SetRect(&s.icon_rect, s.x, s.bottom_y, s.icon_width + s.x, s.card_height + s.bottom_y);
    DrawMaskedBitmapToRect(dc, &s.icon_rect, global_card_counters_pic, s.bitmap_width,
                           s.icon_row_height, 0, s.src_y, 0, s.mask_y);
    s.x -= s.step_width;
  }

  return 1;
}

// FUNCTION: MAGIC 0x0055c7d8
// FUNCTION: SHANDALAR 0x00572af5
void draw_ability_icons_on_smallcard(HDC dc, RECT *rect, unsigned int ability_flags)
{
  struct
  {
    BITMAP bitmap;
    int unused_bottom;
    int saved_dc;
    int ability_index;
    int bitmap_width;
    int mask_y;
    int unused_left;
    int ability_masks[17];
    int ability_count;
    int src_x;
    int src_y;
    int icon_rows[17];
    RECT ability_rect;
    int mask_x;
  } s;
  int icon_height;

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
  s.icon_rows[0] = 0xb;
  s.icon_rows[1] = 0x10;
  s.icon_rows[2] = 0xd;
  s.icon_rows[3] = 0xc;
  s.icon_rows[4] = 0xe;
  s.icon_rows[5] = 0xf;
  s.icon_rows[6] = 3;
  s.icon_rows[7] = 2;
  s.icon_rows[8] = 0;
  s.icon_rows[9] = 1;
  s.icon_rows[10] = 4;
  s.icon_rows[11] = 8;
  s.icon_rows[12] = 7;
  s.icon_rows[13] = 5;
  s.icon_rows[14] = 6;
  s.icon_rows[15] = 9;
  s.icon_rows[16] = 10;
  s.ability_count = 0x11;

  if (dc == NULL || rect == NULL)
  {
    return;
  }

  if (ability_flags == 0)
  {
    return;
  }

  s.saved_dc = SaveDC(dc);
  if (global_abilities_pic != NULL)
  {
    GetObjectA(global_abilities_pic, sizeof(BITMAP), &s.bitmap);
    s.bitmap_width = s.bitmap.bmWidth;
    icon_height = s.bitmap.bmHeight / (s.ability_count + 1);
    s.mask_x = 0;
    s.mask_y = s.bitmap.bmHeight - icon_height;
    s.unused_left = rect->left;
    s.unused_bottom = rect->bottom - icon_height;
    for (s.ability_index = 0; s.ability_index < s.ability_count; s.ability_index++)
    {
      if ((ability_flags & s.ability_masks[s.ability_index]) != 0)
      {
        get_ability_icon_rect(&s.ability_rect, s.ability_masks[s.ability_index], rect, ability_flags);
        s.src_x = 0;
        s.src_y = s.icon_rows[s.ability_index] * icon_height;
        DrawMaskedBitmapToRect(dc, &s.ability_rect, global_abilities_pic, s.bitmap_width,
                               icon_height, s.src_x, s.src_y, s.mask_x, s.mask_y);
      }
    }
  }
  RestoreDC(dc, s.saved_dc);
}

// FUNCTION: MAGIC 0x0055ccd2
// FUNCTION: SHANDALAR 0x00572fef
void draw_manastripes(HDC dc, RECT *rect, int player, int card)
{
  struct
  {
    int attached_player_and_card[2];
    int active_card_counts[2];
    int scan_card;
    int scan_player;
    unsigned int mana_color_flags;
    BITMAP bitmap;
    int stripe_height;
    int saved_dc;
    int stripe_index;
    int mask_y;
    int stripe_width;
    int zero;
    int src_y;
    RECT stripe_rect;
    int mask_x;
  } s;

  if (dc == NULL || rect == NULL)
    return;

  if (get_displayed_card_internal_id(player, card) == -1)
    return;

  s.saved_dc = SaveDC(dc);
  if (get_displayed_card_mana_source_flag(player, card))
  {
    s.mana_color_flags = get_displayed_card_mana_color(player, card);
    get_displayed_player_card_counts(s.active_card_counts);
    for (s.scan_player = 0; s.scan_player < 2; s.scan_player++)
    {
      for (s.scan_card = 0; s.scan_card < s.active_card_counts[s.scan_player]; s.scan_card++)
      {
        get_displayed_card_attachment(s.attached_player_and_card, s.scan_player, s.scan_card);

        if (get_displayed_card_id(s.scan_player, s.scan_card) == CARD_ID_WILD_GROWTH &&
            s.attached_player_and_card[0] == player && s.attached_player_and_card[1] == card)
        {
          s.mana_color_flags |= 8;
        }
      }
    }

    GetObjectA(global_mana_stripes_pic, sizeof(BITMAP), &s.bitmap);
    s.stripe_width = s.bitmap.bmWidth / 2;
    s.stripe_height = s.bitmap.bmHeight / 6;
    SetRect(&s.stripe_rect,
            rect->right - ((s.stripe_width * 7) / 0x14) * 6 - s.stripe_width,
            rect->top + 1, -1, -1);
    IntersectClipRect(dc, rect->left, rect->top, rect->right,
                      rect->top + ((rect->bottom - rect->top) * 0xf) / 100);
    for (s.stripe_index = 0; s.stripe_index < 7; s.stripe_index++)
    {
      if ((s.mana_color_flags & (1 << (unsigned char)s.stripe_index)) != 0)
      {
        s.zero = 0;
        if (s.stripe_index == 5)
        {
          s.src_y = 0;
        }
        else if (s.stripe_index == 2)
        {
          s.src_y = s.stripe_height;
        }
        else if (s.stripe_index == 1)
        {
          s.src_y = s.stripe_height * 2;
        }
        else if (s.stripe_index == 4)
        {
          s.src_y = s.stripe_height * 3;
        }
        else if (s.stripe_index == 3)
        {
          s.src_y = s.stripe_height << 2;
        }
        else if (s.stripe_index == 0)
        {
          s.src_y = s.stripe_height * 5;
        }
        else if (s.stripe_index == 6)
        {
          s.src_y = s.stripe_height * 5;
        }
        s.mask_x = s.stripe_width;
        s.mask_y = s.src_y;
        DrawMaskedBitmapToRect(dc, &s.stripe_rect, global_mana_stripes_pic, s.stripe_width,
                               s.stripe_height, s.zero, s.src_y, s.mask_x, s.mask_y);
      }
      OffsetRect(&s.stripe_rect, (s.stripe_width * 7) / 0x14, 0);
    }
  }
  RestoreDC(dc, s.saved_dc);
}

// FUNCTION: MAGIC 0x0055cfca
// FUNCTION: SHANDALAR 0x005732e6
void draw_summoning_sickness_overlay(HDC dc, RECT *rect)
{
  RECT summon_rect;

  summon_rect.left = (rect->right * 5) / 100;
  summon_rect.top = (rect->bottom * 15) / 100;
  summon_rect.right = (rect->right * 95) / 100;
  summon_rect.bottom = (rect->bottom * 95) / 100;
  draw_masked_bitmap_left_half_to_rect(dc, rect, global_summon_pic);
}

// FUNCTION: MAGIC 0x0055d051
// FUNCTION: SHANDALAR 0x0057336d
void draw_card_ui_flag_20_overlay(HDC dc, RECT *rect)
{
  HBRUSH hatch_brush;
  RECT hatch_rect;

  hatch_rect.left = (rect->right * 5) / 100;
  hatch_rect.top = (rect->bottom * 15) / 100;
  hatch_rect.right = (rect->right * 95) / 100;
  hatch_rect.bottom = (rect->bottom * 95) / 100;
  SetROP2(dc, 0xd);
  hatch_brush = CreateHatchBrush(3, 0x808080);
  SetBkMode(dc, TRANSPARENT);
  SelectObject(dc, hatch_brush);
  SelectObject(dc, GetStockObject(8));
  Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
  SelectObject(dc, GetStockObject(0));
  DeleteObject(hatch_brush);
}

// FUNCTION: MAGIC 0x0055d14d
// FUNCTION: SHANDALAR 0x00573469
void draw_dying_overlay(HDC dc, RECT *rect)
{
  RECT dying_rect;

  dying_rect.left = (rect->right * 5) / 100;
  dying_rect.top = (rect->bottom * 15) / 100;
  dying_rect.right = (rect->right * 95) / 100;
  dying_rect.bottom = (rect->bottom * 95) / 100;
  draw_masked_bitmap_left_half_to_rect(dc, rect, global_dying_pic);
}

// FUNCTION: MAGIC 0x0055d54e
// FUNCTION: SHANDALAR 0x00573868
int get_card_special_counter_icon(card_id_t card_id)
{
  int result;

  if (card_id == CARD_ID_ARMAGEDDON_CLOCK)
    result = 0;
  else if (card_id == CARD_ID_BLACK_MANA_BATTERY)
    result = 1;
  else if (card_id == CARD_ID_BLUE_MANA_BATTERY)
    result = 2;
  else if (card_id == CARD_ID_GREEN_MANA_BATTERY)
    result = 3;
  else if (card_id == CARD_ID_RED_MANA_BATTERY)
    result = 4;
  else if (card_id == CARD_ID_WHITE_MANA_BATTERY)
    result = 5;
  else if (card_id == CARD_ID_CLOCKWORK_AVIAN)
    result = 6;
  else if (card_id == CARD_ID_CLOCKWORK_BEAST)
    result = 6;
  else if (card_id == CARD_ID_THRONE_OF_BONE)
    result = 7;
  else if (card_id == CARD_ID_CRYSTAL_ROD)
    result = 8;
  else if (card_id == CARD_ID_WOODEN_SPHERE)
    result = 9;
  else if (card_id == CARD_ID_IRON_STAR)
    result = 10;
  else if (card_id == CARD_ID_IVORY_CUP)
    result = 11;
  else if (card_id == CARD_ID_FUNGUSAUR)
    result = 12;
  else if (card_id == CARD_ID_WHIRLING_DERVISH)
    result = 12;
  else if (card_id == CARD_ID_LIVING_ARTIFACT)
    result = 13;
  else if (card_id == CARD_ID_OSAI_VULTURES)
    result = 14;
  else if (card_id == CARD_ID_SCAVENGING_GHOUL)
    result = 15;
  else if (card_id == CARD_ID_SENGIR_VAMPIRE)
    result = 16;
  else if (card_id == CARD_ID_NECROPOLIS_OF_AZAR)
    result = 17;
  else if (card_id == CARD_ID_TRISKELION)
    result = 18;
  else if (card_id == CARD_ID_TETRAVUS)
    result = 19;
  else if (card_id == CARD_ID_TIME_VAULT)
    result = 6;
  else if (card_id == CARD_ID_CYCLONE)
    result = 23;
  else if (card_id == CARD_ID_CITANUL_DRUID)
    result = 12;
  else if (card_id == CARD_ID_ROCK_HYDRA)
    result = 20;
  else if (card_id == CARD_ID_KHABAL_GHOUL)
    result = 16;
  else
    result = -1;

  return result;
}

// FUNCTION: MAGIC 0x0055a986
// FUNCTION: SHANDALAR 0x00570cbe
void draw_special_effect_small_card(HDC dc, RECT *rect, card_id_t card_id, unsigned int player, int card)
{
  struct
  {
    unsigned int display_flags;
    unsigned int ability_flags;
    card_id_t transformed_card_id;
    char temporary_name[52];
    int single_color;
    unsigned int display_info;
    int displayed_player_and_card[2];
    int i;
    card_ptr_t card_data;
    unsigned int display_version;
  } s;

  if (dc == (HDC)0 || rect == NULL)
    return;

  s.display_info = get_displayed_card_display_pic_info(s.displayed_player_and_card, player, card);
  s.display_version = (unsigned short)(s.display_info >> 0x10);
  s.card_data.id = s.display_info & 0xffff;

  if (card_id == g_damage_display_internal_card_id)
  {
    s.ability_flags = get_displayed_card_special_counters(player, card);
    sprintf(g_card_title_buffer_00709100, "%s: %d", gs_cardtitle_damage_008cfd30,
            s.ability_flags);
    s.single_color = single_color_test_bit_to_color_t(get_displayed_card_color_flags(player, card));
    if (s.single_color == 1 || s.single_color == 2 || s.single_color == 4 ||
        s.single_color == 3 || s.single_color == 5)
    {
      sprintf(g_card_title_buffer_00709100 + strlen(g_card_title_buffer_00709100), " (%s)",
              gs_color_word_capitalized_00709390[s.single_color]);
    }
  }
  else if (card_id == g_hunting_display_internal_card_id)
  {
    s.ability_flags = get_displayed_card_special_counters(player, card);
    sprintf(g_card_title_buffer_00709100, gs_cardtitle_hunting_00926750, get_hunting_subtype_name(s.ability_flags));
  }
  else if (card_id == g_multiblock_display_internal_card_id)
  {
    strcpy(g_card_title_buffer_00709100, gs_multiblock_creature_008cf040);
  }
  else if (card_id == g_effect_display_internal_card_id)
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.card_data.id].effect_title);
  }
  else if (card_id == g_legacy_display_internal_card_id)
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.card_data.id].legacy_title);
  }
  else if (card_id == g_activation_display_internal_card_id)
  {
    strcpy(g_card_title_buffer_00709100, gs_cardtitle_activation_007aaeb0);
  }
  else
  {
    g_card_title_buffer_00709100[0] = '\0';
  }

  if (card_id == g_effect_display_internal_card_id && get_displayed_card_eot_toughness(player, card) > 0)
  {
    strcpy(s.temporary_name, g_card_title_buffer_00709100);
    extract_numbered_text_choice(g_card_title_buffer_00709100, s.temporary_name, get_displayed_card_eot_toughness(player, card));
  }
  s.transformed_card_id = get_displayed_card_id(s.displayed_player_and_card[0], s.displayed_player_and_card[1]);
  if (card_id == g_effect_display_internal_card_id &&
      (s.transformed_card_id == CARD_ID_FAERIE_DRAGON ||
       s.transformed_card_id == CARD_ID_WHIMSY))
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.transformed_card_id].effect_title);
  }

  s.card_data.name = g_card_title_buffer_00709100;
  s.card_data.full_name = s.card_data.name;
  s.card_data.expansion = 0xffffffff;
  s.card_data.color = 0xffffffff;
  s.card_data.card_type = 0xffffffff;
  s.card_data.subtype = 0xffffffff;
  s.card_data.type_text = NULL;
  s.card_data.db_card_type_2 = 0xffffffff;
  s.card_data.rarity = 0;
  for (s.i = 0; s.i < 10; ++s.i)
  {
    s.card_data.abilities[s.i] = 0;
  }
  s.card_data.unused0x38 = 0xffffffff;
  s.card_data.unused0x3c = 0;
  s.card_data.artist = NULL;
  s.card_data.num_pics = 0;
  s.card_data.mana_cost_text = NULL;
  s.card_data.ai_modifiers1 = 0;
  s.card_data.ai_modifiers2 = 0;
  s.card_data.req.req_black = 0;
  s.card_data.req.req_blue = 0;
  s.card_data.req.req_colorless = 0;
  s.card_data.req.req_green = 0;
  s.card_data.req.req_red = 0;
  s.card_data.req.req_white = 0;
  if (card_id == g_damage_display_internal_card_id)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].damage_text);
  }
  else if (card_id == g_hunting_display_internal_card_id)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].effect_text);
  }
  else if (card_id == g_multiblock_display_internal_card_id)
  {
    strcpy(g_card_rules_text_buffer_00708da8, gs_multiblock_shadow_blocker_00925e70);
  }
  else if (card_id == g_effect_display_internal_card_id)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].effect_text);
  }
  else if (card_id == g_legacy_display_internal_card_id)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].legacy_text);
  }
  else
  {
    g_card_rules_text_buffer_00708da8[0] = '\0';
  }
  s.card_data.rules_text = g_card_rules_text_buffer_00708da8;
  s.card_data.flavor_text = "";
  s.card_data.power = 0;
  s.card_data.toughness = 0;

  DrawSmallCard(dc, rect, &s.card_data, s.display_version, 1);
  draw_manastripes(dc, rect, player, card);
  DrawSmallCardTitle(dc, rect, s.card_data.full_name, get_displayed_card_title_flags(player, card),
                     is_displayed_card_owned_by_opponent(player, card) == player);
  if (card_id == g_damage_display_internal_card_id)
  {
    s.display_flags = get_displayed_card_display_flags(player, card);
    s.ability_flags = 0;
    if ((s.display_flags & 0x100000) != 0)
    {
      s.ability_flags = 0x100;
    }
    if ((s.display_flags & 0x80000) != 0)
    {
      s.ability_flags |= 0x80;
    }
    if (g_duel_interface_options.show_abilities_on_cards != 0 && s.ability_flags != 0)
    {
      draw_ability_icons_on_smallcard(dc, rect, s.ability_flags);
    }
  }
}

// FUNCTION: MAGIC 0x0055af78
// FUNCTION: SHANDALAR 0x0057129f
void draw_activation_small_card(HDC dc, RECT *rect, card_id_t card_id, int player, int card,
                                int attached_player, int attached_card)
{
  struct
  {
    int displayed_player_and_card[2];
    unsigned int display_info;
    int i;
    card_ptr_t card_data;
    unsigned int display_version;
  } s;

  if (dc == (HDC)0)
  {
    return;
  }
  if (rect == NULL)
  {
    return;
  }
  if (card_id == g_activation_display_internal_card_id && get_displayed_card_original_internal_id(player, card) == (unsigned int)g_draw_card_placeholder_internal_card_id)
  {
    DrawCardBack(dc, rect);
    DrawSmallCardTitle(dc, rect, gs_cardtitle_activation_007aaeb0, 0, 1);
  }
  else
  {
    s.display_info = get_displayed_card_display_pic_info(s.displayed_player_and_card, player, card);
    s.display_version = (s.display_info >> 0x10) & 0xffff;
    s.card_data.id = s.display_info & 0xffff;
    if (card_id == g_activation_display_internal_card_id)
    {
      strcpy(g_activation_card_title_buffer_00709150, gs_cardtitle_activation_007aaeb0);
    }
    else
    {
      g_activation_card_title_buffer_00709150[0] = '\0';
    }
    s.card_data.name = g_activation_card_title_buffer_00709150;
    s.card_data.full_name = s.card_data.name;
    s.card_data.expansion = 0xffffffff;
    s.card_data.color = 0xffffffff;
    s.card_data.card_type = 0xffffffff;
    s.card_data.subtype = 0xffffffff;
    s.card_data.type_text = NULL;
    s.card_data.db_card_type_2 = 0xffffffff;
    s.card_data.rarity = 0;
    for (s.i = 0; s.i < 10; ++s.i)
    {
      ((unsigned char *)&s.card_data.req)[s.i] = 0;
    }
    s.card_data.unused0x3c = 0xffffffff;
    s.card_data.artist = NULL;
    s.card_data.num_pics = 0;
    s.card_data.ai_against_color = s.card_data.ai_for_color = s.card_data.ai_counts_as_color = 0;
    s.card_data.ai_against_land = s.card_data.ai_for_land = s.card_data.ai_counts_as_land = 0;
    s.card_data.expansion_rarity = 0;
    g_activation_rules_text_buffer_00708f48[0] = '\0';
    s.card_data.rules_text = g_activation_rules_text_buffer_00708f48;
    s.card_data.flavor_text = "";
    s.card_data.power = 0;
    s.card_data.toughness = 0;
    s.card_data.sleight_color = 0;
    *(uint32_t *)&s.card_data.hack_colors = 0;

    DrawSmallCard(dc, rect, &s.card_data, s.display_version, 1);
    draw_manastripes(dc, rect, attached_player, attached_card);
    DrawSmallCardTitle(dc, rect, s.card_data.name, get_displayed_card_title_flags(player, card),
                       is_displayed_card_owned_by_opponent(player, card) == player);
  }
}

// FUNCTION: MAGIC 0x0055b1e4
// FUNCTION: SHANDALAR 0x00571504
void draw_cardclass_small_card(HDC dc, RECT *rect, unsigned int player, int card, int ui_flags,
                               unsigned int target_ui_flags, int parent_card, int activation, int upkeep)
{
  struct
  {
    unsigned int counter_loyalty;
    unsigned int counter_p1p1;
    unsigned int counter_charge;
    unsigned int counter_m1m1;
    unsigned int counter_power;
    unsigned int special_counters;
    RECT counter_rect;
    int damage;
    unsigned int ability_flags;
    int color_count;
    unsigned int color_flags;
    int color_index;
    int saved_dc;
    card_ptr_t card_data;
    int card_version;
    card_id_t card_id;
  } s;

  (void)target_ui_flags;
  (void)parent_card;

  if (dc == (HDC)0 || rect == NULL)
    return;

  s.card_id = get_displayed_card_id(player, card);
  if (s.card_id == -1)
    return;

  s.saved_dc = SaveDC(dc);
  memcpy(&s.card_data, global_raw_cards_storage + s.card_id, sizeof(s.card_data));

  s.color_flags = get_displayed_card_color_flags(player, card);
  s.color_count = 0;
  for (s.color_index = 1; s.color_index <= 5; ++s.color_index)
  {
    if ((s.color_flags & (1 << (unsigned char)s.color_index)) != 0)
    {
      ++s.color_count;
    }
  }

  if (s.color_count > 1)
  {
    s.card_data.color = 4;
  }
  else
  {
    if (s.color_flags == 2)
    {
      s.card_data.color = 1;
    }
    else if (s.color_flags == 0x20)
    {
      s.card_data.color = 8;
    }
    else if (s.color_flags == 8)
    {
      s.card_data.color = 5;
    }
    else if (s.color_flags == 0x10)
    {
      s.card_data.color = 7;
    }
    else if (s.color_flags == 4)
    {
      s.card_data.color = 2;
    }
  }

  if (activation != 0)
  {
    s.card_data.name = gs_cardtitle_activation_007aaeb0;
    s.card_data.full_name = s.card_data.name;
    s.card_data.color = 0xffffffff;
  }
  if (upkeep != 0)
  {
    s.card_data.name = gs_cardtitle_upkeep_008b4100;
    s.card_data.full_name = s.card_data.name;
    s.card_data.color = 0xffffffff;
  }

  s.card_version = get_card_display_pic_num(s.card_id, player, card);
  DrawSmallCard(dc, rect, &s.card_data, s.card_version, 1);
  draw_manastripes(dc, rect, player, card);
  if (g_duel_interface_options.show_power_toughness_on_cards != 0 &&
      (get_displayed_card_type(player, card) & 2) != 0)
  {
    draw_powertoughness_on_smallcard(dc, rect, get_displayed_card_power(player, card),
                                     get_displayed_card_toughness(player, card));
  }
  if (((get_displayed_card_type(player, card) & 2) != 0 ||
       g_duel_interface_options.show_all_cards_summon_sickness != 0) &&
      (get_displayed_card_ui_flags(player, card) & 1) != 0)
  {
    draw_summoning_sickness_overlay(dc, rect);
  }
  if ((get_displayed_card_ui_flags(player, card) & 0x20) != 0)
  {
    draw_card_ui_flag_20_overlay(dc, rect);
  }
  if ((get_displayed_card_type(player, card) & 0x43) != 0 && get_displayed_card_kill_code(player, card) == 2)
  {
    draw_dying_overlay(dc, rect);
  }
  s.ability_flags = get_displayed_card_regen_status(player, card);
  if (g_duel_interface_options.show_abilities_on_cards != 0 && s.ability_flags != 0)
  {
    draw_ability_icons_on_smallcard(dc, rect, s.ability_flags);
  }
  s.damage = get_displayed_card_damage_on_card(player, card);
  if (s.damage != 0)
  {
    draw_damage(dc, rect, s.damage);
  }

  s.counter_rect.left = rect->left + ((rect->right - rect->left) * 7) / 100;
  s.counter_rect.right = rect->right - ((rect->right - rect->left) * 10) / 100;
  s.counter_rect.top = rect->top + ((rect->bottom - rect->top) * 8) / 100;
  s.counter_rect.bottom = rect->top + ((rect->bottom - rect->top) * 0x23) / 100;
  s.special_counters = get_displayed_card_special_counter_count(player, card);
  if (s.special_counters != 0)
  {
    draw_special_counter_icons(dc, rect, get_card_special_counter_icon(s.card_id),
                               s.special_counters);
  }
  s.counter_rect.top = rect->top + ((rect->bottom - rect->top) * 0x23) / 100;
  s.counter_rect.bottom = rect->top + ((rect->bottom - rect->top) * 0x3e) / 100;
  get_displayed_card_standard_counters(player, card, &s.counter_power, &s.counter_m1m1, &s.counter_p1p1,
                                       &s.counter_loyalty, &s.counter_charge);
  draw_standard_counters(dc, &s.counter_rect, s.counter_power, s.counter_m1m1,
                         s.counter_p1p1, s.counter_loyalty, s.counter_charge);
  DrawSmallCardTitle(dc, rect, s.card_data.name, get_displayed_card_title_flags(player, card),
                     is_displayed_card_owned_by_opponent(player, card) == player);
  if (player == 0 && (get_displayed_card_state(player, card) & 0x40000) != 0)
  {
    SelectObject(dc, global_pen_palette_5d);
    SelectObject(dc, GetStockObject(5));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
  }
  RestoreDC(dc, s.saved_dc);
}

// FUNCTION: MAGIC 0x0055bd85
// FUNCTION: SHANDALAR 0x005720a3
void draw_id_tag(HDC dc, RECT *rect, int player, int card, int enabled)
{
  int saved_dc;
  char text[8];
  RECT tag_rect;

  if (dc == (HDC)0 || rect == NULL)
  {
    return;
  }

  if ((player != 0 && player != 1) || card == -1)
  {
    return;
  }

  if (enabled != 0 && get_displayed_card_zone(player, card) == 1)
  {
    saved_dc = SaveDC(dc);
    tag_rect.left = rect->left + ((rect->right - rect->left) * 0x50) / 100;
    tag_rect.top = rect->top;
    tag_rect.right = rect->left + ((rect->right - rect->left) * 0x5f) / 100;
    tag_rect.bottom = rect->top + ((rect->bottom - rect->top) * 0xe) / 100;
    SetMapMode(dc, MM_ANISOTROPIC);
    SetWindowExtEx(dc, tag_rect.right - tag_rect.left, 0x14, NULL);
    SetViewportExtEx(dc, tag_rect.right - tag_rect.left, tag_rect.bottom - tag_rect.top,
                     NULL);
    DPtoLP(dc, (LPPOINT)&tag_rect, 2);
    sprintf(text, " %d ", card);
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

// FUNCTION: MAGIC 0x00497d70
// FUNCTION: SHANDALAR 0x00468800
unsigned int get_dib_pixel(unsigned char *bits, int bit_count, int width, int x, int y)
{
  struct
  {
    int bytes_per_pixel;
    unsigned int result;
    int row_padding;
    unsigned char *pixel;
  } s;

  if (bits == 0)
  {
    return 0;
  }

  if (bit_count != 0x20 && bit_count != 0x18 && bit_count != 0x10 && bit_count != 8)
  {
    return 0;
  }

  s.bytes_per_pixel = bit_count / 8;
  s.row_padding = (4 - ((s.bytes_per_pixel * width) % 4)) % 4;
  s.pixel = ((s.bytes_per_pixel * width + s.row_padding) * y) +
            s.bytes_per_pixel * x + bits;

  if (bit_count == 0x20)
  {
    s.result = *(unsigned int *)s.pixel;
  }
  else if (bit_count == 0x18)
  {
    s.result = ((unsigned int)s.pixel[1] << 8) | ((unsigned int)s.pixel[0] << 0x10) | s.pixel[2];
  }
  else if (bit_count == 0x10)
  {
    s.result = ((unsigned int)s.pixel[0] << 8) | s.pixel[1];
  }
  else if (bit_count == 8)
  {
    s.result = s.pixel[0];
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00497eb0
// FUNCTION: SHANDALAR 0x00468940
void set_dib_pixel(unsigned char *bits, int bit_count, int width, int x, int y, unsigned int color)
{
  struct
  {
    int bytes_per_pixel;
    int row_padding;
    unsigned char *pixel;
  } s;

  if (bits == 0)
  {
    return;
  }

  if (bit_count != 0x20 && bit_count != 0x18 && bit_count != 0x10 && bit_count != 8)
  {
    return;
  }

  s.bytes_per_pixel = bit_count / 8;
  s.row_padding = (4 - ((s.bytes_per_pixel * width) % 4)) % 4;
  s.pixel = ((s.bytes_per_pixel * width + s.row_padding) * y) +
            s.bytes_per_pixel * x + bits;

  if (bit_count == 0x20)
  {
    *(unsigned int *)s.pixel = color;
  }
  else if (bit_count == 0x18)
  {
    s.pixel[0] = (unsigned char)(color >> 0x10);
    s.pixel[1] = (unsigned char)(color >> 8);
    s.pixel[2] = (unsigned char)color;
  }
  else if (bit_count == 0x10)
  {
    s.pixel[0] = (unsigned char)(color >> 8);
    s.pixel[1] = (unsigned char)color;
  }
  else if (bit_count == 8)
  {
    s.pixel[0] = (unsigned char)color;
  }
}

// FUNCTION: MAGIC 0x00494cf0
// FUNCTION: SHANDALAR 0x0046578a
void rotate_dib_rect_clockwise(BITMAPINFO *bmi, unsigned char *bits, RECT *rect)
{
  struct
  {
    int x;
    int offset;
    unsigned int top_pixel;
    int width;
    unsigned int bit_count;
    RECT bounds;
  } s;

  if (bmi == NULL || bits == NULL || rect == NULL)
    return;

  CopyRect(&s.bounds, rect);
  --s.bounds.right;
  --s.bounds.bottom;
  s.width = bmi->bmiHeader.biWidth;
  s.bit_count = (unsigned int)bmi->bmiHeader.biBitCount;
  while (!(s.bounds.right <= s.bounds.left))
  {
    for (s.x = s.bounds.left; s.x < s.bounds.right; ++s.x)
    {
      s.offset = s.x - s.bounds.left;
      s.top_pixel = get_dib_pixel(bits, s.bit_count, s.width, s.bounds.left + s.offset, s.bounds.top);
      set_dib_pixel(bits, s.bit_count, s.width, s.bounds.left + s.offset, s.bounds.top,
                    get_dib_pixel(bits, s.bit_count, s.width, s.bounds.left, s.bounds.bottom - s.offset));
      set_dib_pixel(bits, s.bit_count, s.width, s.bounds.left, s.bounds.bottom - s.offset,
                    get_dib_pixel(bits, s.bit_count, s.width, s.bounds.right - s.offset, s.bounds.bottom));
      set_dib_pixel(bits, s.bit_count, s.width, s.bounds.right - s.offset, s.bounds.bottom,
                    get_dib_pixel(bits, s.bit_count, s.width, s.bounds.right, s.offset + s.bounds.top));
      set_dib_pixel(bits, s.bit_count, s.width, s.bounds.right, s.offset + s.bounds.top, s.top_pixel);
    }

    ++s.bounds.left;
    --s.bounds.right;
    ++s.bounds.top;
    --s.bounds.bottom;
  }
}

// FUNCTION: MAGIC 0x0055d26d
// FUNCTION: SHANDALAR 0x00573589
unsigned int draw_target_canttarget(HDC dc, RECT *rect, int is_target, int can_target)
{
  int saved_dc;
  unsigned int result;
  RECT target_rect;

  if (dc == (HDC)0 || rect == NULL)
  {
    return 0;
  }

  saved_dc = SaveDC(dc);
  target_rect.left = rect->left + ((rect->right - rect->left) * 10) / 100;
  target_rect.right = rect->right - ((rect->right - rect->left) * 10) / 100;
  target_rect.top = rect->top + ((rect->bottom - rect->top) * 10) / 100;
  target_rect.bottom = rect->bottom - ((rect->bottom - rect->top) * 10) / 100;
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
// FUNCTION: SHANDALAR 0x005736af
void draw_card_highlight_frame(HDC dc, RECT *rect, int highlighted)
{
  HPEN pen;
  int saved_dc;

  pen = CreatePen(PS_INSIDEFRAME, 3, global_palette_col_1f);
  if (highlighted != 0)
  {
    saved_dc = SaveDC(dc);
    SelectObject(dc, pen);
    SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, rect->left, rect->top, rect->right, rect->bottom);
    RestoreDC(dc, saved_dc);
  }
  DeleteObject(pen);
}

// FUNCTION: MAGIC 0x0055d429
// FUNCTION: SHANDALAR 0x00573745
void draw_will_untap_overlay(HDC dc, RECT *rect, unsigned int untap_status)
{
  RECT untap_rect;

  if ((untap_status & 1) != 0 && (untap_status & 2) != 0)
  {
    get_will_untap_overlay_rect(&untap_rect, rect);
    draw_masked_bitmap_left_half_to_rect(dc, &untap_rect, global_will_untap_pic);
  }
}

// FUNCTION: MAGIC 0x0055d910
// FUNCTION: SHANDALAR 0x00573c2a
void draw_summoning_sickness_dib_overlay(BITMAPINFO *bmi, unsigned char *bits, RECT *rect)
{
  (void)bmi;
  (void)bits;
  (void)rect;
}

// FUNCTION: MAGIC 0x004d2385
// FUNCTION: SHANDALAR 0x00578961
void draw_card_debug_id_tag(HDC dc, RECT *rect, int player, int card)
{
  struct
  {
    char text[12];
    int saved_dc;
    HFONT font;
  } s;

  s.saved_dc = SaveDC(dc);
  sprintf(s.text, "%d,%d", player, card);
  s.font = CreateFontA(0x12, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0x12,
                       "Times New Roman");
  SelectObject(dc, s.font);
  SetBkMode(dc, TRANSPARENT);
  SetTextAlign(dc, TA_RIGHT);
  SetTextColor(dc, 0xffffff);
  TextOutA(dc, rect->right, rect->top + 1, s.text, strlen(s.text));
  SetTextColor(dc, 0x808080);
  TextOutA(dc, rect->right - 1, rect->top, s.text, strlen(s.text));
  RestoreDC(dc, s.saved_dc);
  DeleteObject(s.font);
}

// FUNCTION: MAGIC 0x004d7953
// FUNCTION: SHANDALAR 0x004615b1
int can_card_join_banding_group(int player, int card, int unused_player, int unused_card)
{
  int result;

  result = 1;
  if (can_attack(player, card) == 0)
  {
    result = 0;
  }
  if ((get_displayed_card_regen_status(player, card) & 0x40) == 0)
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0044184b
// FUNCTION: SHANDALAR 0x0040d0cb
int can_block_attacker_with_landwalk_checks(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
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
  get_landwalk_evasion_masks(&s.tmp1, &s.tmp2);

  if (blocker_player == 1)
  {
    s.selected = s.tmp1;
  }
  else
  {
    s.selected = s.tmp2;
  }

  s.result = can_block_attacker_with_abilities(blocker_player,
                                               blocker_card,
                                               attacker_player,
                                               attacker_card,
                                               (unsigned int)s.attacker_abilities,
                                               s.selected);
  return s.result;
}

// FUNCTION: MAGIC 0x00441b94
// FUNCTION: SHANDALAR 0x0040d414
int assign_blocker_to_attacker(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  int saved_state;
  int saved_blocking;
  int result;

  saved_state = global_card_instances[blocker_player][blocker_card].state;
  saved_blocking = (int)(char)global_card_instances[blocker_player][blocker_card].blocking;
  global_card_instances[blocker_player][blocker_card].state &= ~8;
  global_card_instances[blocker_player][blocker_card].blocking = (unsigned char)0xff;

  result = can_block_attacker_with_landwalk_checks(blocker_player, blocker_card, attacker_player, attacker_card);
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
  AppendMenuA(g_cardclass_alternate_card_submenu, 0, 0x72, "");

  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_SMALLCARD");
  strcpy(g_cardclass_menu_alternate_card_text, g_text_lines[0]);
  strcpy(g_cardclass_menu_view_stats_text, g_text_lines[1]);
  strcpy(g_cardclass_menu_view_card_text, g_text_lines[2]);
  strcpy(g_cardclass_menu_protect_text, g_text_lines[3]);
  strcpy(g_cardclass_menu_id_tags_text, g_text_lines[4]);
  strcpy(g_cardclass_menu_autotarget_text, g_text_lines[5]);
  strcpy(g_cardclass_menu_show_damage_text, g_text_lines[6]);
  strcpy(g_cardclass_menu_help_text, g_text_lines[7]);

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

  load_text_with_tab_escapes(global_ui_strings_filename, "PROMPT_BANDWITHWHOM");
  strcpy(g_cardclass_prompt_band_whom_text, g_text_lines[0]);
  strcpy(g_cardclass_prompt_band_cannot_text, g_text_lines[1]);
  strcpy(g_cardclass_prompt_band_error_text, g_text_lines[2]);

  load_text_with_tab_escapes(global_ui_strings_filename, "PROMPT_DEFENDWHOM");
  strcpy(g_cardclass_prompt_defend_attacker_text, g_text_lines[0]);
  strcpy(g_cardclass_prompt_defend_error_text, g_text_lines[1]);
  strcpy(g_cardclass_prompt_defend_prompt_text, g_text_lines[2]);

  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_ArmageddonClock");
  strcpy(g_cardclass_cuecard_counter_armageddon_clock_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_ManaBattery");
  strcpy(g_cardclass_cuecard_counter_mana_battery_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_ClockworkAvian");
  strcpy(g_cardclass_cuecard_counter_clockwork_avian_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_ClockworkBeast");
  strcpy(g_cardclass_cuecard_counter_clockwork_beast_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_LuckyCharms");
  strcpy(g_cardclass_cuecard_counter_lucky_charms_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_Fungusaur");
  strcpy(g_cardclass_cuecard_counter_fungusaur_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_WhirlingDervish");
  strcpy(g_cardclass_cuecard_counter_whirling_dervish_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_LivingArtifact");
  strcpy(g_cardclass_cuecard_counter_living_artifact_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_OsaiVultures");
  strcpy(g_cardclass_cuecard_counter_osai_vultures_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_ScavengingGhouls");
  strcpy(g_cardclass_cuecard_counter_scavenging_ghouls_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_SengirVampire");
  strcpy(g_cardclass_cuecard_counter_sengir_vampire_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_NecropolisOfAzar");
  strcpy(g_cardclass_cuecard_counter_necropolis_of_azar_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_Triskelion");
  strcpy(g_cardclass_cuecard_counter_triskelion_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_Tetravus");
  strcpy(g_cardclass_cuecard_counter_tetravus_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_TimeVault");
  strcpy(g_cardclass_cuecard_counter_time_vault_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_Cyclone");
  strcpy(g_cardclass_cuecard_counter_cyclone_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_CitanulDruid");
  strcpy(g_cardclass_cuecard_counter_citanul_druid_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_RockHydra");
  strcpy(g_cardclass_cuecard_counter_rock_hydra_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_KhabalGhoul");
  strcpy(g_cardclass_cuecard_counter_khabal_ghoul_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_OrcishCatapult");
  strcpy(g_cardclass_cuecard_counter_orcish_catapult_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_UnstableMutation");
  strcpy(g_cardclass_cuecard_counter_unstable_mutation_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_SpiritShackle");
  strcpy(g_cardclass_cuecard_counter_spirit_shackle_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_DwarvenWeaponsmith");
  strcpy(g_cardclass_cuecard_counter_dwarven_weaponsmith_text, g_text_lines[0]);
  load_text_with_tab_escapes(global_ui_strings_filename, "CUECARD_COUNTERS_AshnodsTransmorgrant");
  strcpy(g_cardclass_cuecard_counter_ashnods_transmogrant_text, g_text_lines[0]);

  return s.registered;
}

// FUNCTION: MAGIC 0x004cdf7b
// FUNCTION: SHANDALAR 0x0057456b
void destroy_MAGICGAME_CardClass(LPCSTR class_name)
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
    char pad_0000_to_001b[0x1c];
    unsigned int paint_unused_878;
    int paint_parent_card;
    unsigned int paint_target_ui_flags;
    unsigned int paint_ui_flags;
    int paint_attached_player_and_card[2];
    int paint_displayed_player_and_card[2];
    HDC paint_dc;
    PAINTSTRUCT paint_paint_struct;
    RECT paint_rect;
    DWORD paint_start_tick;
    card_id_t paint_card_id;
    char pad_0098_to_00f3[0x5c];
    unsigned char command_saved_phase_flags;
    char pad_00f5_to_00f7[3];
    char command_defend_tooltip_text[100];
    int command_defend_selected_parent_card;
    int command_defend_select_ok;
    target_t command_defend_target;
    int command_defend_saved_modal_active;
    target_selection_request_t command_defend_action_request_copy;
    int command_defend_attach_card;
    int command_linked_scan_card;
    int command_linked_unique_card;
    char command_band_tooltip_text[100];
    int command_band_attach_card;
    int command_band_select_ok;
    target_t command_band_target;
    int command_band_saved_modal_active;
    target_selection_request_t command_band_action_request_copy;
    int command_selected_card;
    int command_attach_card;
    char command_help_path[264];
    int command_help_card_id;
    int command_protect_on;
    int command_preview_player_and_card[2];
    card_id_t command_preview_card_id;
    int command_player_and_card[2];
    HWND command_child_hwnd;
    int command_unused_3a0;
    HWND command_previous_z_order;
    int msg_404_compare_result;
    card_instance_t msg_404_snapshot;
    char pad_062c_to_0633[12];
    HDC msg_432_title_dc;
    RECT msg_432_title_rect;
    card_id_t msg_432_card_id;
    card_instance_t msg_432_snapshot;
    unsigned int temp_mouse;
    unsigned int can_target_again;
    unsigned int counter_p0p1;
    unsigned int ability_flags;
    RECT ability_rect;
    char cuecard_text[100];
    unsigned int untap_status;
    unsigned int counter_p1p1;
    POINT mouse_point;
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
    card_id_t card_id;
    RECT damage_rect;
    RECT untap_rect;
    int is_target;
    card_instance_t *snapshot;
    int card;
    int scratch;
    int player;
    HWND hidden_parent;
  } s;

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
    s.mouse_point.x = (short)LOWORD(lparam);
    s.mouse_point.y = (short)HIWORD(lparam);
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
        s.temp_mouse = s.mouse_point.x;
        s.mouse_point.x = s.mouse_point.y;
        s.mouse_point.y = s.client_rect.bottom - s.temp_mouse;
      }

      s.ability_flags = get_displayed_card_regen_status(s.player, s.card);
      s.ability_index = -1;
      if (s.ability_flags != 0)
      {
        for (s.ability_loop_index = 0; s.ability_loop_index < 0x11 && s.ability_index == -1;
             s.ability_loop_index++)
        {
          get_ability_icon_rect(&s.ability_rect, g_cardclass_ability_masks[s.ability_loop_index],
                                &s.client_rect, s.ability_flags);
          if (PtInRect(&s.ability_rect, s.mouse_point) != 0)
          {
            s.ability_index = s.ability_loop_index;
          }
        }
      }

      s.special_counters = get_displayed_card_special_counter_count(s.player, s.card);
      get_displayed_card_standard_counters(s.player, s.card, &s.counter_power, &s.counter_m1m1, &s.counter_p1p1,
                                           &s.counter_p0p1, &s.counter_p1p0);
      get_special_counter_icons_rect(&s.special_counter_rect, &s.client_rect, s.special_counters);
      s.is_target = is_displayed_card_targetted(s.player, s.card);
      s.can_target_again = is_displayed_card_untargetable(s.player, s.card) == 0;
      s.damage_on_card = get_displayed_card_damage_on_card(s.player, s.card);
      get_damage_overlay_rect(&s.damage_rect, &s.client_rect);
      s.untap_status = get_displayed_card_untap_status(s.player, s.card);
      get_will_untap_overlay_rect(&s.untap_rect, &s.client_rect);
      s.kill_code = get_displayed_card_kill_code(s.player, s.card);

      if ((s.untap_status & 1) != 0 &&
          (s.untap_status & 2) != 0 &&
          PtInRect(&s.untap_rect, s.mouse_point) != 0)
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
               PtInRect(&s.damage_rect, s.mouse_point) != 0)
      {
        sprintf(s.cuecard_text, gs_cuecard_damage_n_0091b290, s.damage_on_card);
        s.cuecard_found = 1;
      }
      else if ((int)s.special_counters > 0 &&
               PtInRect(&s.special_counter_rect, s.mouse_point) != 0)
      {
        format_special_counter_cuecard_text(s.cuecard_text, s.card_id, s.special_counters);
        s.cuecard_found = 1;
      }
      else if ((int)(s.counter_p1p1 + s.counter_m1m1 + s.counter_power) > 0 &&
               s.mouse_point.y > (s.client_rect.bottom * 0x23) / 100 &&
               (s.client_rect.bottom * 0x3e) / 100 > s.mouse_point.y)
      {
        s.counter_column_width = (s.client_rect.right - s.client_rect.left) / 3;
        if (s.mouse_point.x < s.client_rect.left + s.counter_column_width)
        {
          format_standard_counter_cuecard_text(s.cuecard_text, 1, s.counter_p1p1);
        }
        else if (s.mouse_point.x < s.counter_column_width * 2 + s.client_rect.left)
        {
          format_standard_counter_cuecard_text(s.cuecard_text, 2, s.counter_m1m1);
        }
        else
        {
          format_standard_counter_cuecard_text(s.cuecard_text, 3, s.counter_power);
        }
        s.cuecard_found = 1;
      }
      else if ((int)(s.counter_p1p0 + s.counter_p0p1) > 0 &&
               s.mouse_point.y > (s.client_rect.bottom * 0x35) / 100 &&
               (s.client_rect.bottom * 0x59) / 100 > s.mouse_point.y)
      {
        s.counter_column_width = (s.client_rect.right - s.client_rect.left) / 2;
        if (s.mouse_point.x < s.client_rect.left + (s.client_rect.right - s.client_rect.left) / 2)
        {
          format_standard_counter_cuecard_text(s.cuecard_text, 4, s.counter_p0p1);
        }
        else
        {
          format_standard_counter_cuecard_text(s.cuecard_text, 5, s.counter_p1p0);
        }
        s.cuecard_found = 1;
      }
      else
      {
        if (is_displayed_card_owned_by_opponent(s.player, s.card) != s.player && (s.client_rect.bottom * 0xc) / 100 > s.mouse_point.y)
        {
          strcpy(s.cuecard_text, gs_cuecard_not_controlled_by_owner_008a8e40);
          s.cuecard_found = 1;
        }
        else if (((s.is_target != 0 || s.can_target_again != 0) &&
                  s.mouse_point.x > (s.client_rect.right * 5) / 100 &&
                  (s.client_rect.right * 0x5f) / 100 > s.mouse_point.x &&
                  s.mouse_point.y > (s.client_rect.bottom * 0xf) / 100 &&
                  (s.client_rect.bottom * 0x5f) / 100 > s.mouse_point.y))
        {
          if (s.is_target != 0 && s.can_target_again != 0)
          {
            strcpy(s.cuecard_text, gs_cuecard_is_a_target_cant_target_again_008cf230);
          }
          else if (s.is_target != 0)
          {
            strcpy(s.cuecard_text, gs_cuecard_is_a_target_00789c40);
          }
          else if (s.can_target_again != 0)
          {
            strcpy(s.cuecard_text, gs_cuecard_cant_target_this_008b4990);
          }
          s.cuecard_found = 1;
        }
        else if (s.kill_code == 2 &&
                 (s.client_rect.right * 5) / 100 < s.mouse_point.x &&
                 s.mouse_point.x < (s.client_rect.right * 0x5f) / 100 &&
                 (s.client_rect.bottom * 0xf) / 100 < s.mouse_point.y &&
                 s.mouse_point.y < (s.client_rect.bottom * 0x5f) / 100)
        {
          strcpy(s.cuecard_text, gs_cuecard_dying_0091c640);
          s.cuecard_found = 1;
        }
        else if ((s.client_rect.right * 5) / 100 < s.mouse_point.x &&
                 s.mouse_point.x < (s.client_rect.right * 0x5f) / 100 &&
                 (s.client_rect.bottom * 0xf) / 100 < s.mouse_point.y &&
                 s.mouse_point.y < (s.client_rect.bottom * 0x5f) / 100)
        {
          if ((get_displayed_card_type(s.player, s.card) & 2) != 0 &&
              (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0 &&
              (get_displayed_card_ui_flags(s.player, s.card) & 0x20) != 0)
          {
            strcpy(s.cuecard_text, gs_cuecard_summoning_sickness_008a8e00);
            strcat(s.cuecard_text, ",");
            strcat(s.cuecard_text, gs_cuecard_phased_007aae00);
            s.cuecard_found = 1;
          }
          else
          {
            if ((get_displayed_card_type(s.player, s.card) & 2) != 0 && (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0)
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
    if (g_duel_interface_options.layout != 2)
    {
      SendMessageA(hwnd, WM_COMMAND, 0x6e, 0);
    }
    return s.cuecard_found;

  case 0x432:
    if (IsWindowVisible(hwnd) == 0)
    {
      return 0;
    }
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.snapshot = (card_instance_t *)GetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
    copy_displayed_card_snapshot(&s.msg_432_snapshot, s.player, s.card);
    if (s.player != -1 && s.card == -1)
    {
      return 0;
    }
    if (card_snapshots_match_for_redraw(s.snapshot, &s.msg_432_snapshot) == 0)
    {
      InvalidateRect(hwnd, (RECT *)0, 0);
    }
    else
    {
      if (g_duel_interface_options.show_abilities_on_cards != 0 &&
          card_snapshots_match_ability_icons(s.snapshot, &s.msg_432_snapshot) == 0)
      {
        InvalidateRect(hwnd, (RECT *)0, 0);
      }
      else if (s.snapshot->unknown0x70 != s.msg_432_snapshot.unknown0x70)
      {
        s.msg_432_card_id = get_displayed_card_id(s.player, s.card);
        if ((get_displayed_card_ui_flags(s.player, s.card) & 2) != 0)
        {
          InvalidateRect(hwnd, (RECT *)0, 0);
        }
        else if (s.msg_432_card_id == g_activation_display_internal_card_id ||
                 s.msg_432_card_id == g_card_back_display_internal_card_id ||
                 s.msg_432_card_id == g_effect_display_internal_card_id ||
                 s.msg_432_card_id == g_damage_display_internal_card_id ||
                 s.msg_432_card_id == g_legacy_display_internal_card_id ||
                 s.msg_432_card_id == g_hunting_display_internal_card_id ||
                 s.msg_432_card_id == g_multiblock_display_internal_card_id)
        {
          InvalidateRect(hwnd, (RECT *)0, 0);
        }
        else if (s.msg_432_card_id != -1)
        {
          s.msg_432_title_dc = GetDC(hwnd);
          ApplyCardArtPaletteToDc(s.msg_432_title_dc);
          GetClientRect(hwnd, &s.msg_432_title_rect);
          DrawSmallCardTitle(s.msg_432_title_dc, &s.msg_432_title_rect,
                             global_raw_cards_storage[s.msg_432_card_id].name,
                             get_displayed_card_title_flags(s.player, s.card),
                             (unsigned int)(is_displayed_card_owned_by_opponent(s.player, s.card) - s.player) < 1);
          ReleaseDC(hwnd, s.msg_432_title_dc);
          s.snapshot->unknown0x70 = s.msg_432_snapshot.unknown0x70;
        }
      }
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
    return (LRESULT)(s.hidden_parent = (HWND)GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET));

  case 0x404:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.snapshot = (card_instance_t *)GetWindowLongA(hwnd, CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
    if (s.player != -1 && s.card == -1)
    {
      s.msg_404_compare_result = 0;
    }
    else
    {
      copy_displayed_card_snapshot(&s.msg_404_snapshot, s.player, s.card);
      s.msg_404_compare_result = memcmp(s.snapshot, &s.msg_404_snapshot, sizeof(card_instance_t));
    }
    return s.msg_404_compare_result;

  case WM_COMMAND:
    s.player = GetWindowLongA(hwnd, CARDCLASS_PLAYER_WINDOW_LONG_OFFSET);
    s.card = GetWindowLongA(hwnd, CARDCLASS_CARD_WINDOW_LONG_OFFSET);
    s.hidden_parent = (HWND)GetWindowLongA(hwnd, CARDCLASS_HIDDEN_PARENT_WINDOW_LONG_OFFSET);
    s.command_player_and_card[0] = s.player;
    s.command_player_and_card[1] = s.card;
    if (GetParent(hwnd) == g_duel_attack_phase_window_hwnd)
    {
      s.command_child_hwnd = hwnd;
      find_battlefield_card_window(g_duel_player_battlefield_window_hwnd,
                                   s.command_player_and_card, (int *)0,
                                   &s.command_previous_z_order);
    }
    else
    {
      s.command_previous_z_order = hwnd;
      find_attack_phase_card_window(g_duel_attack_phase_window_hwnd,
                                    s.command_player_and_card, (int *)0,
                                    &s.command_child_hwnd, (int *)0);
    }
    switch ((unsigned int)wparam & 0xffff)
    {
    case 0x6e:
      s.command_preview_card_id = get_displayed_card_id(s.player, s.card);
      s.command_preview_player_and_card[0] = s.player;
      s.command_preview_player_and_card[1] = s.card;
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                   s.command_preview_card_id,
                   (LPARAM)s.command_preview_player_and_card);
      break;

    case 0x70:
      s.command_protect_on = (get_displayed_card_state(s.player, s.card) & 0x40000) != 0;
      s.command_protect_on = s.command_protect_on == 0;
      if (s.command_protect_on != 0)
      {
        global_card_instances[s.player][s.card].state |= STATE_NO_AUTO_TAPPING;
      }
      else
      {
        global_card_instances[s.player][s.card].state &= ~STATE_NO_AUTO_TAPPING;
      }
      EnterCriticalSection(&g_duel_render_lock);
      global_displayed_card_instances[s.player][s.card].state =
          global_card_instances[s.player][s.card].state;
      LeaveCriticalSection(&g_duel_render_lock);
      InvalidateRect(hwnd, (RECT *)0, 1);
      break;

    case 0x71:
      s.command_help_card_id = get_displayed_card_id(s.player, s.card);
      if (s.command_help_card_id == g_card_back_display_internal_card_id)
      {
        s.command_help_card_id = 0xc1b;
      }
      if (s.command_help_card_id != -1)
      {
        strcpy(s.command_help_path, global_base_directory);
        strcat(s.command_help_path, "\\duel.hlp");
        WinHelpA(g_duel_window_hwnd, s.command_help_path, HELP_CONTEXT,
                 s.command_help_card_id);
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
      get_current_duel_selection((int *)0, &s.command_selected_card);
      if (can_attack(s.player, s.card) != 0)
      {
        s.command_attach_card = -1;
        global_card_instances[s.player][s.card].state |= 4;
        global_card_instances[s.player][s.card].blocking =
            (unsigned char)s.command_attach_card;
        refresh_duel_display_cache();
        if (s.command_selected_card > 0x14 &&
            s.command_selected_card < 0x1e)
        {
          SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
        }
        else
        {
          SendMessageA(GetParent(hwnd), 0x436,
                       (WPARAM)s.command_player_and_card, 0);
        }
      }
      break;

    case 0x66:
      if (get_displayed_card_blocking(s.player, s.card) != -1)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x69, 0);
      }
      global_card_instances[s.player][s.card].state &= ~STATUS_SLEIGHTED;
      global_card_instances[s.player][s.card].blocking = (unsigned char)0xff;
      refresh_duel_display_cache();
      if (s.command_child_hwnd == hwnd)
      {
        SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
      }
      else
      {
        SendMessageA(GetParent(hwnd), 0x436,
                     (WPARAM)s.command_player_and_card, 0);
      }
      break;

    case 0x68:
      SendMessageA(hwnd, WM_COMMAND, 0x69, 0);
    case 0x67:
      if (can_attack(s.player, s.card) != 0)
      {
        memcpy(&s.command_band_action_request_copy, &g_duel_action_request_copy,
               sizeof(s.command_band_action_request_copy));
        GetWindowTextA((HWND)g_duel_tooltip_window, s.command_band_tooltip_text, 100);
        s.command_band_saved_modal_active = g_duel_modal_action_active;
        s.command_band_select_ok =
            C_real_select_target(0, 0, 1, 0x200, 2, 0, 0, 0, 0, 0,
                                 -1, -1, -1, -1, 0, 2, 0,
                                 g_cardclass_prompt_band_whom_text, 1,
                                 &s.command_band_target);
        g_duel_modal_action_active = s.command_band_saved_modal_active;
        if (s.command_band_select_ok != 0)
        {
          if ((get_displayed_card_ui_flags(s.command_band_target.player,
                                           s.command_band_target.card) &
               4) != 0)
          {
            if (can_card_join_banding_group(s.player, s.card,
                                            s.command_band_target.player,
                                            s.command_band_target.card) != 0)
            {
              s.command_band_attach_card =
                  get_displayed_card_blocking(s.command_band_target.player,
                                              s.command_band_target.card);
              if (s.command_band_attach_card != -1)
              {
                global_card_instances[s.player][s.card].state |= 4;
                global_card_instances[s.player][s.card].blocking =
                    (unsigned char)s.command_band_attach_card;
                refresh_duel_display_cache();
                SendMessageA(hwnd, 0x432, 0, 0);
                if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
                {
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
                }
              }
              else
              {
                s.command_band_attach_card = s.command_band_target.card;
                global_card_instances[s.player][s.card].state |= 4;
                global_card_instances[s.player][s.card].blocking =
                    (unsigned char)s.command_band_attach_card;
                global_card_instances[s.command_band_target.player]
                                     [s.command_band_target.card]
                                         .state |= 4;
                global_card_instances[s.command_band_target.player]
                                     [s.command_band_target.card]
                                         .blocking =
                    (unsigned char)s.command_band_attach_card;
                refresh_duel_display_cache();
                SendMessageA(hwnd, 0x432, 0, 0);
                if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
                {
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
                  SendMessageA(g_duel_attack_phase_window_hwnd, 0x436,
                               (WPARAM)&s.command_band_target, 0);
                }
                else
                {
                  SendMessageA(GetParent(s.command_previous_z_order), 0x436,
                               (WPARAM)&s.command_band_target, 0);
                }
              }
            }
            else
            {
              UpdateWindow(g_duel_window_hwnd);
              display_duel_prompt_text(g_cardclass_prompt_band_cannot_text);
              UpdateWindow(g_duel_prompt_context_hwnd);
              Sleep(2000);
            }
          }
          else
          {
            UpdateWindow(g_duel_window_hwnd);
            display_duel_prompt_text(g_cardclass_prompt_band_error_text);
            UpdateWindow(g_duel_prompt_context_hwnd);
            Sleep(2000);
          }
        }
        memcpy(&g_duel_action_request_copy, &s.command_band_action_request_copy,
               sizeof(g_duel_action_request_copy));
        set_duel_prompt_context(g_duel_prompt_context_hwnd,
                                s.command_band_action_request_copy.prompt,
                                s.command_band_action_request_copy.allow_cancel);
        set_duel_tooltip_text(s.command_band_tooltip_text);
      }
      break;

    case 0x69:
      s.command_linked_unique_card = get_displayed_card_blocking(s.player, s.card);
      for (s.command_linked_scan_card = 0;
           s.command_linked_scan_card < g_active_cards_count[s.player];
           s.command_linked_scan_card++)
      {
        if (get_displayed_card_internal_id(s.player, s.command_linked_scan_card) != -1 &&
            (get_displayed_card_ui_flags(s.player, s.command_linked_scan_card) & 4) != 0 &&
            get_displayed_card_blocking(s.player, s.command_linked_scan_card) ==
                s.command_linked_unique_card)
        {
          global_card_instances[s.player][s.command_linked_scan_card].blocking = (unsigned char)0xff;
          refresh_duel_display_cache();
          s.command_player_and_card[0] = s.player;
          s.command_player_and_card[1] = s.command_linked_scan_card;
          if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0)
          {
            SendMessageA(g_duel_attack_phase_window_hwnd, 0x436,
                         (WPARAM)s.command_player_and_card, 0);
          }
          else
          {
            SendMessageA(GetParent(s.command_previous_z_order), 0x436,
                         (WPARAM)s.command_player_and_card, 0);
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
      memcpy(&s.command_defend_action_request_copy, &g_duel_action_request_copy,
             sizeof(s.command_defend_action_request_copy));
      GetWindowTextA((HWND)g_duel_tooltip_window, s.command_defend_tooltip_text, 100);
      s.command_defend_saved_modal_active = g_duel_modal_action_active;
      s.command_defend_select_ok =
          C_real_select_target(0, 1, 0, 0x200, 2, 0, 0, 0, 0, 0,
                               -1, -1, -1, -1, 0, 2, 0,
                               g_cardclass_prompt_defend_attacker_text, 1,
                               &s.command_defend_target);
      g_duel_modal_action_active = s.command_defend_saved_modal_active;
      if (s.command_defend_select_ok != 0)
      {
        if ((get_displayed_card_ui_flags(s.command_defend_target.player,
                                         s.command_defend_target.card) &
             4) != 0)
        {
          if (assign_blocker_to_attacker(s.player, s.card,
                                         s.command_defend_target.player,
                                         s.command_defend_target.card) != 0)
          {
            s.command_defend_selected_parent_card =
                get_displayed_card_blocking(s.command_defend_target.player,
                                            s.command_defend_target.card);
            if (s.command_defend_selected_parent_card == -1)
            {
              s.command_defend_attach_card = s.command_defend_target.card;
            }
            else
            {
              s.command_defend_attach_card =
                  s.command_defend_selected_parent_card;
            }
            global_card_instances[s.player][s.card].state |= 8;
            global_card_instances[s.player][s.card].blocking =
                (unsigned char)s.command_defend_attach_card;
            refresh_duel_display_cache();
            if (IsWindowVisible(g_duel_attack_phase_window_hwnd) != 0 &&
                GetParent(hwnd) != g_duel_attack_phase_window_hwnd)
            {
              SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
            }
            else
            {
              SendMessageA(GetParent(hwnd), 0x436,
                           (WPARAM)s.command_player_and_card, 0);
            }
          }
          else
          {
            UpdateWindow(g_duel_window_hwnd);
            display_duel_prompt_text(g_cardclass_prompt_defend_error_text);
            UpdateWindow(g_duel_prompt_context_hwnd);
            Sleep(2000);
          }
        }
        else
        {
          UpdateWindow(g_duel_window_hwnd);
          display_duel_prompt_text(g_cardclass_prompt_defend_prompt_text);
          UpdateWindow(g_duel_prompt_context_hwnd);
          Sleep(2000);
        }
      }
      memcpy(&g_duel_action_request_copy, &s.command_defend_action_request_copy,
             sizeof(g_duel_action_request_copy));
      set_duel_prompt_context(g_duel_prompt_context_hwnd,
                              s.command_defend_action_request_copy.prompt,
                              s.command_defend_action_request_copy.allow_cancel);
      set_duel_tooltip_text(s.command_defend_tooltip_text);
      break;

    case 0x6c:
      global_card_instances[s.player][s.card].state &= ~STATUS_OBLITERATED;
      global_card_instances[s.player][s.card].blocking = (unsigned char)0xff;
      refresh_duel_display_cache();
      if (s.command_child_hwnd == hwnd)
      {
        SendMessageA(g_duel_attack_phase_window_hwnd, 0x412, 0, 0);
      }
      else
      {
        SendMessageA(GetParent(hwnd), 0x436,
                     (WPARAM)s.command_player_and_card, 0);
      }
      break;

    case 100:
    case 0x6d:
      g_recorded_action_player = 0;
      post_card_action_selection(hwnd);
      break;

    case 0x262:
      if (g_duel_cheats_state != 0)
      {
        s.command_unused_3a0 = global_card_instances[s.player][s.card].internal_card_id;
        produce_mana(s.player,
                     single_color_test_bit_to_color_t(
                         (int)global_cards_data[global_card_instances[s.player][s.card].internal_card_id].color),
                     (int)(char)global_cards_data[s.command_unused_3a0].cc[0]);
        produce_mana(s.player, 0,
                     abs((int)(char)global_cards_data[s.command_unused_3a0].cc[1]));
        if ((g_duel_network_flags & 2) != 0 && s.player == g_active_player)
        {
          for (s.scratch = 0; s.scratch < 8; s.scratch++)
          {
            g_xpool_network_packet.raw_mana_available[s.scratch] =
                g_raw_mana_available[g_active_player][s.scratch];
          }
          g_xpool_network_packet.packet_type = 0x11;
          TENTATIVE_send_network_result(g_active_player, 0x11);
        }
        copy_mana_pool_to_display();
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x263:
      if (g_duel_cheats_state != 0)
      {
        global_card_instances[s.player][s.card].state ^= 0x10;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x264:
      if (g_duel_cheats_state != 0)
      {
        s.command_saved_phase_flags = g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase];
        g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
            (unsigned char)((int)(char)g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] & ~PHASE_STOP_ENABLED);
        global_card_instances[s.player][s.card].token_status |= 8;
        kill_card(s.player, s.card, KILL_DESTROY);
        g_duel_phase_stop_settings[g_current_player].phase_flags[g_current_phase] =
            s.command_saved_phase_flags;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x266:
      if (g_duel_cheats_state != 0)
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
              abs(s.client_rect.left - s.damage_rect.left) <
          5)
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
    if (s.is_target == 0 && g_duel_modal_action_active != 0 && card_window_matches_action_request(hwnd) != 0)
    {
      g_recorded_action_player = s.cuecard_found;
      post_card_action_selection(hwnd);
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
    s.paint_start_tick = GetTickCount();
    GetClientRect(hwnd, &s.paint_rect);
    s.paint_dc = BeginPaint(hwnd, &s.paint_paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (g_duel_palette_refresh_pending != 0)
      {
        FillRect(s.paint_dc, &s.paint_rect, GetStockObject(0));
        Sleep(200);
      }
      if (s.player == -1 && s.card == -1)
      {
        DrawCardBack(s.paint_dc, &s.paint_rect);
      }
      else if (s.card == -1)
      {
        draw_duel_face_window(s.paint_dc, &s.paint_rect, s.player);
        FrameRect(s.paint_dc, &s.paint_rect, GetStockObject(4));
      }
      else
      {
        s.paint_card_id = get_displayed_card_id(s.player, s.card);
        if (s.paint_card_id != -1 && global_available_slots > s.paint_card_id)
        {
          if (s.paint_card_id == g_card_back_display_internal_card_id)
          {
            DrawCardBack(s.paint_dc, &s.paint_rect);
            DrawSmallCardTitle(s.paint_dc, &s.paint_rect,
                               gs_cardtitle_draw_a_card_008b4330,
                               get_displayed_card_title_flags(s.player, s.card), 1);
          }
          else if (s.paint_card_id == g_damage_display_internal_card_id ||
                   g_hunting_display_internal_card_id == s.paint_card_id ||
                   g_multiblock_display_internal_card_id == s.paint_card_id ||
                   s.paint_card_id == g_effect_display_internal_card_id ||
                   s.paint_card_id == g_legacy_display_internal_card_id)
          {
            get_displayed_card_display_pic_info(s.paint_displayed_player_and_card, s.player, s.card);
            draw_special_effect_small_card(s.paint_dc, &s.paint_rect,
                                           s.paint_card_id, s.player, s.card);
            draw_card_highlight_frame(s.paint_dc, &s.paint_rect,
                                      get_displayed_card_display_flags(s.player, s.card) & 0x20000);
            draw_target_canttarget(s.paint_dc, &s.paint_rect,
                                   is_displayed_card_targetted(s.player, s.card),
                                   is_displayed_card_untargetable(s.player, s.card));
            draw_id_tag(s.paint_dc, &s.paint_rect,
                        s.paint_displayed_player_and_card[0],
                        s.paint_displayed_player_and_card[1],
                        g_duel_interface_options.show_id_tags_on_cards);
          }
          else if (g_activation_display_internal_card_id == s.paint_card_id)
          {
            get_displayed_card_parent(s.paint_attached_player_and_card, s.player, s.card);
            draw_activation_small_card(g_shared_offscreen_dc, &s.paint_rect,
                                       s.paint_card_id, s.player, s.card,
                                       s.paint_attached_player_and_card[0],
                                       s.paint_attached_player_and_card[1]);
            draw_target_canttarget(g_shared_offscreen_dc, &s.paint_rect,
                                   is_displayed_card_targetted(s.paint_attached_player_and_card[0],
                                                               s.paint_attached_player_and_card[1]),
                                   is_displayed_card_untargetable(s.paint_attached_player_and_card[0],
                                                                  s.paint_attached_player_and_card[1]));
            draw_id_tag(g_shared_offscreen_dc, &s.paint_rect,
                        s.paint_attached_player_and_card[0],
                        s.paint_attached_player_and_card[1],
                        g_duel_interface_options.show_id_tags_on_cards);
            BitBlt(s.paint_dc, 0, 0, s.paint_rect.right,
                   s.paint_rect.bottom,
                   g_shared_offscreen_dc, 0, 0, SRCCOPY);
          }
          else
          {
            s.paint_ui_flags = get_displayed_card_ui_flags(s.player, s.card);
            if (GetParent(hwnd) == g_duel_attack_phase_window_hwnd)
            {
              s.paint_target_ui_flags = 0;
            }
            else
            {
              s.paint_target_ui_flags = s.paint_ui_flags & 4;
              s.paint_parent_card = get_displayed_card_blocking(s.player, s.card);
            }
            draw_cardclass_small_card(g_shared_offscreen_dc, &s.paint_rect, s.player, s.card,
                                      get_displayed_card_title_flags(s.player, s.card),
                                      s.paint_target_ui_flags,
                                      s.paint_parent_card, 0, 0);
            draw_target_canttarget(g_shared_offscreen_dc, &s.paint_rect,
                                   is_displayed_card_targetted(s.player, s.card),
                                   is_displayed_card_untargetable(s.player, s.card));
            draw_card_highlight_frame(g_shared_offscreen_dc, &s.paint_rect,
                                      get_displayed_card_display_flags(s.player, s.card) & 0x20000);
            draw_will_untap_overlay(g_shared_offscreen_dc, &s.paint_rect,
                                    get_displayed_card_untap_status(s.player, s.card));
            draw_id_tag(g_shared_offscreen_dc, &s.paint_rect, s.player, s.card,
                        g_duel_interface_options.show_id_tags_on_cards);
            if (((get_displayed_card_type(s.player, s.card) & 2) != 0 ||
                 g_duel_interface_options.show_all_cards_summon_sickness != 0) &&
                (get_displayed_card_ui_flags(s.player, s.card) & 1) != 0)
            {
              draw_summoning_sickness_dib_overlay(&g_duel_backbuffer_bmi, g_shared_offscreen_bits,
                                                  &s.paint_rect);
            }
            if ((s.paint_ui_flags & 2) != 0)
            {
              rotate_dib_rect_clockwise(&g_duel_backbuffer_bmi, g_shared_offscreen_bits,
                                        &s.paint_rect);
            }
            BitBlt(s.paint_dc, 0, 0, s.paint_rect.right,
                   s.paint_rect.bottom,
                   g_shared_offscreen_dc, 0, 0, SRCCOPY);
          }
          s.snapshot = (card_instance_t *)GetWindowLongA(hwnd,
                                                         CARDCLASS_SNAPSHOT_WINDOW_LONG_OFFSET);
          copy_displayed_card_snapshot(s.snapshot, s.player, s.card);
          if (g_duel_dialog_refresh_state != 0)
          {
            draw_card_debug_id_tag(s.paint_dc, &s.paint_rect, s.player, s.card);
          }
        }
      }
      EndPaint(hwnd, &s.paint_paint_struct);
    }
    g_duel_tick_adjustment += GetTickCount() - s.paint_start_tick;
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
      s.test_point.x += GetSystemMetrics(SM_CXCURSOR);
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
      get_displayed_card_blocking(s.player, s.card);
    }
    s.damage_on_card = get_displayed_card_blocking(s.player, s.card);
    s.is_target = card_window_matches_action_request(hwnd);
    s.ability_flags = get_displayed_card_regen_status(s.player, s.card) & 0x40;
    s.untap_status = get_displayed_card_zone(s.player, s.card);
    s.special_counters = get_displayed_card_internal_id(s.player, s.card);
    s.card_id = get_displayed_card_id(s.player, s.card);
    s.counter_p1p1 = get_displayed_card_state(s.player, s.card) & 0x40000;
    s.counter_p0p1 = global_cards_data[s.special_counters].extra_ability & 0x1000;
    s.type_flags = get_displayed_card_type(s.player, s.card);
    s.previous_z_order = GetParent(hwnd);
    s.counter_power = get_displayed_card_original_internal_id(s.player, s.card);
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
    if (g_duel_cheats_state != 0 && g_duel_modal_action_active != 0)
    {
      if (s.untap_status == 0)
      {
        AppendMenuA(g_cardclass_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x262, "&M: Add mana for this card");
        AppendMenuA(g_cardclass_popup_menu, 0, 0x264, "&B: Bury this card");
      }
      else
      {
        AppendMenuA(g_cardclass_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
        AppendMenuA(g_cardclass_popup_menu, 0, 0x262, "&M: Add mana for this card");
        AppendMenuA(g_cardclass_popup_menu, 0, 0x263, "&T: Tap/untap this card");
        AppendMenuA(g_cardclass_popup_menu, 0, 0x264, "&B: Bury this card");
        AppendMenuA(g_cardclass_popup_menu, 0, 0x266,
                    "&X: Increment counters for this card");
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
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  return 0;
}
