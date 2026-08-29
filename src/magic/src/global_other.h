#ifndef __GLOBALS_OTHERS_H__
#define __GLOBALS_OTHERS_H__

#include <windows.h>
#include "defs.h"
#include "duel_engine.h"
#include "global_duel_ui_ids.h"

// Single header library style
#ifdef GLOBALS_OTHERS_IMPL
#define GLOBAL_OTHERS_EXTERN
#else
#define GLOBAL_OTHERS_EXTERN extern
#endif

typedef struct shandalar_monster_definition_struct
{
  char article[0x0a];
  char name[0x32];
  char plural[0x32];
  char gender;
  char tier;
  char base_strength;
  char encounter_type;
  char color_mask;
  char unknown_05;
  int deck_number;
  unsigned int flags_0a;
  char flags_0e;
  char flags_0f;
  char flags_10;
  char flags_11;
  unsigned int preduel_flags;
  unsigned int reward_flags;
  int reward_card_id;
} PACKED ShandalarMonsterDefinition;
STATIC_ASSERT(sizeof(ShandalarMonsterDefinition) == 0x8c, shandalar_monster_definition_wrong_size);

typedef struct
{
  int x;
  int y;
} int_pair_t;

typedef struct mana_burn_dialog_context_struct
{
  int player;
  int amount;
} mana_burn_dialog_context_t;
STATIC_ASSERT(sizeof(mana_burn_dialog_context_t) == 8, mana_burn_dialog_context_wrong_size);

#if defined(GLOBALS_OTHERS_IMPL) && defined(SHANDALAR)
// GLOBAL: SHANDALAR 0x00591a08
ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39] = {
#include "shandalar/src/creature_names_init.inc"
};
#elif defined(GLOBALS_OTHERS_IMPL) && !defined(SHANDALAR)
// GLOBAL: MAGIC 0x00746830
ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39];
#else
extern ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39];
#endif

// GLOBAL: MOK 0x543c10
// GLOBAL: MAGIC 0x00791560
// GLOBAL: SHANDALAR 0x007a82f0
GLOBAL_OTHERS_EXTERN card_instance_t global_displayed_card_instances[2][151];

// GLOBAL: MAGIC 0x00775d3c
// GLOBAL: SHANDALAR 0x0097c85c
GLOBAL_OTHERS_EXTERN int g_ai_land_score_modifier;

// GLOBAL: MAGIC 0x00777aa0
// GLOBAL: SHANDALAR 0x0078e830
GLOBAL_OTHERS_EXTERN int g_duel_trace_counter;

// GLOBAL: MAGIC 0x0093917c
// GLOBAL: SHANDALAR 0x0094d2ac
GLOBAL_OTHERS_EXTERN int g_duel_state_table_0093917c[33];

// GLOBAL: MAGIC 0x0064ee98
// GLOBAL: SHANDALAR 0x00669390
GLOBAL_OTHERS_EXTERN int g_ai_trace_enabled;

// GLOBAL: MAGIC 0x008b4980
// GLOBAL: SHANDALAR 0x008c8b00
GLOBAL_OTHERS_EXTERN HWND global_opponent_chat_hwnd;

// GLOBAL: MAGIC 0x00743030
// GLOBAL: SHANDALAR 0x0077f43c
GLOBAL_OTHERS_EXTERN int g_selected_response_internal_card_id;

// GLOBAL: MAGIC 0x007161d4
// GLOBAL: SHANDALAR 0x0074ccc4
GLOBAL_OTHERS_EXTERN phase_t g_previous_stop_phase;

// GLOBAL: MAGIC 0x00743034
// GLOBAL: SHANDALAR 0x0077f440
GLOBAL_OTHERS_EXTERN int g_previous_stop_phase_player;

// GLOBAL: MAGIC 0x0074303c
// GLOBAL: SHANDALAR 0x00780690
GLOBAL_OTHERS_EXTERN int g_target_selection_status_code;

// GLOBAL: MAGIC 0x0072c440
// GLOBAL: SHANDALAR 0x00765420
GLOBAL_OTHERS_EXTERN int g_mana_cost_modifiers_by_color[8];

// GLOBAL: MAGIC 0x00743000
// GLOBAL: SHANDALAR 0x0077eda0
GLOBAL_OTHERS_EXTERN int g_landwalk_basiclandtypes_controlled[2][8];

// GLOBAL: MAGIC 0x007ab290
// GLOBAL: SHANDALAR 0x007bf490
GLOBAL_OTHERS_EXTERN int g_duel_cached_raw_mana_player_1[7];

// GLOBAL: MAGIC 0x00715fa8
// GLOBAL: SHANDALAR 0x0074b868
GLOBAL_OTHERS_EXTERN int g_loaded_recorded_action_code;

// GLOBAL: MAGIC 0x00715fac
// GLOBAL: SHANDALAR 0x0074b8b4
GLOBAL_OTHERS_EXTERN int g_phase_response_window_open;

// GLOBAL: MAGIC 0x00791550
// GLOBAL: SHANDALAR 0x007a82dc
GLOBAL_OTHERS_EXTERN int g_regenerate_or_graveyard_triggers_in_progress;

// GLOBAL: MAGIC 0x008cff10
// GLOBAL: SHANDALAR 0x008e4060
GLOBAL_OTHERS_EXTERN int g_life_gained;

// GLOBAL: MAGIC 0x00743038
// GLOBAL: SHANDALAR 0x0077f600
GLOBAL_OTHERS_EXTERN int g_activation_scan_flags;

// GLOBAL: MAGIC 0x0074308c
// GLOBAL: SHANDALAR 0x0078df70
GLOBAL_OTHERS_EXTERN HWND g_duel_main_window;

// GLOBAL: MAGIC 0x00743090
// GLOBAL: SHANDALAR 0x0078df74
GLOBAL_OTHERS_EXTERN HWND g_duel_client_window;

// GLOBAL: MAGIC 0x007a7d88
// GLOBAL: SHANDALAR 0x007beb18
GLOBAL_OTHERS_EXTERN int g_global_toughness_bonus[2];

// GLOBAL: MAGIC 0x00743094
// GLOBAL: SHANDALAR 0x0074842c
GLOBAL_OTHERS_EXTERN int g_recorded_action_count;

// GLOBAL: MAGIC 0x007abc78
// GLOBAL: SHANDALAR 0x007bfe78
GLOBAL_OTHERS_EXTERN int g_battlefield_extra_ability_flags;

// GLOBAL: MAGIC 0x007abc80
// GLOBAL: SHANDALAR 0x007bfe80
GLOBAL_OTHERS_EXTERN int g_battlefield_ui_flags;

// GLOBAL: MAGIC 0x007a7d78
// GLOBAL: SHANDALAR 0x007beb04
GLOBAL_OTHERS_EXTERN int g_duel_state_007a7d78;

// GLOBAL: MAGIC 0x008ce534
// GLOBAL: SHANDALAR 0x008e26b4
GLOBAL_OTHERS_EXTERN HWND g_duel_active_popup_window;

// GLOBAL: MAGIC 0x008ce680
// GLOBAL: SHANDALAR 0x008e27d0
GLOBAL_OTHERS_EXTERN char g_duel_status_text[0x20];

// GLOBAL: MAGIC 0x00896670
// GLOBAL: SHANDALAR 0x008aa870
GLOBAL_OTHERS_EXTERN int g_duel_cached_raw_mana_player_0[7];

// GLOBAL: MAGIC 0x0064efa0
// GLOBAL: SHANDALAR 0x00669498
GLOBAL_OTHERS_EXTERN char g_current_duel_prompt_text[256];

// GLOBAL: MAGIC 0x0064f0a4
// GLOBAL: SHANDALAR 0x005a9d50
GLOBAL_OTHERS_EXTERN int g_response_activation_phase_mask;

// GLOBAL: MAGIC 0x0064f0a8
// GLOBAL: SHANDALAR 0x005a9d54
GLOBAL_OTHERS_EXTERN int g_response_source_phase_mask;

// GLOBAL: MAGIC 0x00637558
// GLOBAL: SHANDALAR 0x005a6080
GLOBAL_OTHERS_EXTERN int g_recorded_mana_payments[10][7];

#ifdef GLOBALS_OTHERS_IMPL
int g_mana_payment_record_depth = 0;
#else
// GLOBAL: MAGIC 0x005607e0
// GLOBAL: SHANDALAR 0x00580dac
GLOBAL_OTHERS_EXTERN int g_mana_payment_record_depth;
#endif

#ifdef GLOBALS_OTHERS_IMPL
int g_affected_card_context_depth = 0;
#else
// GLOBAL: MAGIC 0x00561268
// GLOBAL: SHANDALAR 0x00580678
GLOBAL_OTHERS_EXTERN int g_affected_card_context_depth;
#endif

#ifdef GLOBALS_OTHERS_IMPL
int g_affected_card_context_flags = 0;
#else
// GLOBAL: MAGIC 0x00561390
// GLOBAL: SHANDALAR 0x005807a0
GLOBAL_OTHERS_EXTERN int g_affected_card_context_flags;
#endif

#ifdef GLOBALS_OTHERS_IMPL
char g_empty_duel_prompt_text[1] = "";
#else
// GLOBAL: MAGIC 0x0056e494
// GLOBAL: SHANDALAR 0x00583fb8
GLOBAL_OTHERS_EXTERN char g_empty_duel_prompt_text[1];
#endif

// GLOBAL: MAGIC 0x00939344
// GLOBAL: SHANDALAR 0x0094d470
GLOBAL_OTHERS_EXTERN HWND g_duel_last_active_window;

// GLOBAL: MAGIC 0x0093a7b0
// GLOBAL: SHANDALAR 0x0094e8d0
GLOBAL_OTHERS_EXTERN HWND g_duel_prompt_context_hwnd;

// GLOBAL: MAGIC 0x0093a844
// GLOBAL: SHANDALAR 0x0094e964
GLOBAL_OTHERS_EXTERN int g_phase_was_skipped;

// GLOBAL: MAGIC 0x0091bfb0
// GLOBAL: SHANDALAR 0x009300f0
GLOBAL_OTHERS_EXTERN int g_adventure_world_exit_requested;

// GLOBAL: MAGIC 0x008a915c
// GLOBAL: SHANDALAR 0x008bd35c
GLOBAL_OTHERS_EXTERN char *g_duel_prompt_text_pointer;

// GLOBAL: MAGIC 0x0074636c
// GLOBAL: SHANDALAR 0x0078df88
GLOBAL_OTHERS_EXTERN int g_dungeon_life_reward_delta;

// GLOBAL: MAGIC 0x00776510
// GLOBAL: SHANDALAR 0x0073c7e0
GLOBAL_OTHERS_EXTERN int g_restore_deck_after_duel;

// GLOBAL: MAGIC 0x00776514
// GLOBAL: SHANDALAR 0x0073c7e4
GLOBAL_OTHERS_EXTERN int g_force_opponent_starts_duel;

// GLOBAL: MAGIC 0x00775d40
// GLOBAL: SHANDALAR 0x0073c010
GLOBAL_OTHERS_EXTERN int g_deck_restore_buffer[500];

// GLOBAL: MAGIC 0x00896718
// GLOBAL: SHANDALAR 0x008aa918
GLOBAL_OTHERS_EXTERN int g_duel_state_00896718;

// GLOBAL: MAGIC 0x00925d3c
// GLOBAL: SHANDALAR 0x0097db30
GLOBAL_OTHERS_EXTERN int g_ai_score;

// GLOBAL: MAGIC 0x00925d38
// GLOBAL: SHANDALAR 0x00939e64
GLOBAL_OTHERS_EXTERN int g_ai_score_baseline;

// GLOBAL: MAGIC 0x00791418
// GLOBAL: SHANDALAR 0x007a81a8
GLOBAL_OTHERS_EXTERN int g_duel_state_00791418;

// GLOBAL: MAGIC 0x00926740
// GLOBAL: SHANDALAR 0x0093a870
GLOBAL_OTHERS_EXTERN char g_itoa_buffer[12];

// GLOBAL: MAGIC 0x0092674c
GLOBAL_OTHERS_EXTERN int g_waiting_for_network_packet;

// GLOBAL: MAGIC 0x0093f9c0
// GLOBAL: SHANDALAR 0x0097f1a0
GLOBAL_OTHERS_EXTERN int g_affected_card_context_index;

// GLOBAL: MAGIC 0x0093f4c0
// GLOBAL: SHANDALAR 0x0097eca0
GLOBAL_OTHERS_EXTERN int g_affected_card_context_stack[32][10];

// GLOBAL: MAGIC 0x0093f4b4
GLOBAL_OTHERS_EXTERN int g_affected_card_context_flags_0093f4b4;

// GLOBAL: MAGIC 0x00925030
GLOBAL_OTHERS_EXTERN HINSTANCE g_app_instance;

// GLOBAL: MAGIC 0x008cdab4
// GLOBAL: SHANDALAR 0x008e1c34
GLOBAL_OTHERS_EXTERN int g_ai_side_score_player_1;

#ifdef GLOBALS_OTHERS_IMPL
int g_duel_state_0057a75c = 0;
#else
// GLOBAL: MAGIC 0x0057a75c
GLOBAL_OTHERS_EXTERN int g_duel_state_0057a75c;
#endif

#ifdef GLOBALS_OTHERS_IMPL
// GLOBAL: MAGIC 0x0057aae8
// GLOBAL: SHANDALAR 0x00589a38
int g_ai_recorded_action_type = 0;
#else
GLOBAL_OTHERS_EXTERN int g_ai_recorded_action_type;
#endif

#if defined(GLOBALS_OTHERS_IMPL) && defined(MAGIC)
// GLOBAL: MAGIC 0x0057d9f0
int global_screen_width = 0x280;
#else
GLOBAL_OTHERS_EXTERN int global_screen_width;
#endif

#if defined(GLOBALS_OTHERS_IMPL) && defined(MAGIC)
// GLOBAL: MAGIC 0x0057d9f4
int global_screen_height = 0x1e0;
#else
GLOBAL_OTHERS_EXTERN int global_screen_height;
#endif

// GLOBAL: MAGIC 0x00637670
// GLOBAL: SHANDALAR 0x005a8f70
GLOBAL_OTHERS_EXTERN char g_dialog_card_title_buffer[0x68];

// GLOBAL: MAGIC 0x006376d8
// GLOBAL: SHANDALAR 0x005a5da8
GLOBAL_OTHERS_EXTERN char g_prompt_stop_phase_anyway_text[300];

// GLOBAL: MAGIC 0x00638b30
GLOBAL_OTHERS_EXTERN HBITMAP g_mana_burn_dialog_background;

// GLOBAL: MAGIC 0x00638b60
GLOBAL_OTHERS_EXTERN COLORREF g_mana_burn_dialog_text_color;

// GLOBAL: MAGIC 0x00638c18
GLOBAL_OTHERS_EXTERN COLORREF g_mana_burn_dialog_shadow_color;

// GLOBAL: MAGIC 0x00638c80
GLOBAL_OTHERS_EXTERN mana_burn_dialog_context_t *g_mana_burn_dialog_context;

// GLOBAL: MAGIC 0x006a1db8
// GLOBAL: SHANDALAR 0x005ae0a0
GLOBAL_OTHERS_EXTERN int g_recorded_action_targets[256];

// GLOBAL: MAGIC 0x006a21b8
// GLOBAL: SHANDALAR 0x005ae4a0
GLOBAL_OTHERS_EXTERN int g_trial_recorded_action_targets[256];

// GLOBAL: MAGIC 0x006a5f18
// GLOBAL: SHANDALAR 0x005b2200
GLOBAL_OTHERS_EXTERN int g_recorded_action_types[256];

// GLOBAL: MAGIC 0x006a6ae8
// GLOBAL: SHANDALAR 0x005b2dd0
GLOBAL_OTHERS_EXTERN int g_recorded_action_codes[256];

// GLOBAL: MAGIC 0x006a6ee8
// GLOBAL: SHANDALAR 0x005b31d0
GLOBAL_OTHERS_EXTERN int g_trial_recorded_action_codes[256];

// GLOBAL: MAGIC 0x006a7e58
// GLOBAL: SHANDALAR 0x005b4140
GLOBAL_OTHERS_EXTERN int g_saved_recorded_action_internal_card_ids[256];

// GLOBAL: MAGIC 0x006a8258
// GLOBAL: SHANDALAR 0x005b4540
GLOBAL_OTHERS_EXTERN int g_trial_recorded_action_internal_card_ids[256];

// GLOBAL: MAGIC 0x006a8660
// GLOBAL: SHANDALAR 0x005b4948
GLOBAL_OTHERS_EXTERN int g_trial_recorded_action_types[256];

// GLOBAL: MAGIC 0x006ab368
// GLOBAL: SHANDALAR 0x005b7650
GLOBAL_OTHERS_EXTERN int g_saved_recorded_action_count;

#ifdef SHANDALAR
// GLOBAL: SHANDALAR 0x0078cf10
GLOBAL_OTHERS_EXTERN char g_ui_message_buffer[0x1000];
#else
// GLOBAL: MAGIC 0x00748770
GLOBAL_OTHERS_EXTERN char g_ui_message_buffer[3000];
#endif

// GLOBAL: MAGIC 0x0074309c
GLOBAL_OTHERS_EXTERN HDC g_duel_screen_dc;

// GLOBAL: MAGIC 0x007462a0
GLOBAL_OTHERS_EXTERN int g_duel_palette_state;

// GLOBAL: MAGIC 0x007463dc
GLOBAL_OTHERS_EXTERN HPALETTE g_duel_palette;

// GLOBAL: MAGIC 0x007497b0
GLOBAL_OTHERS_EXTERN int g_world_map_state;

// GLOBAL: MAGIC 0x00789100
// GLOBAL: SHANDALAR 0x0079fe90
GLOBAL_OTHERS_EXTERN HWND g_duel_player_face_window_hwnd;

// GLOBAL: MAGIC 0x008b40c8
// GLOBAL: SHANDALAR 0x008c8248
GLOBAL_OTHERS_EXTERN HWND g_duel_opponent_face_window_hwnd;

// GLOBAL: MAGIC 0x008cee70
// GLOBAL: SHANDALAR 0x008e2fc0
GLOBAL_OTHERS_EXTERN HWND g_duel_player_graveyard_window_hwnd;

// GLOBAL: MAGIC 0x0091ce30
// GLOBAL: SHANDALAR 0x00930f60
GLOBAL_OTHERS_EXTERN HWND g_duel_main_window_hwnd;

// GLOBAL: MAGIC 0x00926804
// GLOBAL: SHANDALAR 0x0093a934
GLOBAL_OTHERS_EXTERN int g_duel_network_flags;

#ifdef _DEBUG
#define TRACE_ENABLED 1
#else
#define TRACE_ENABLED ((g_duel_network_flags & 2) != 0)
#endif

// GLOBAL: MAGIC 0x0092680c
// GLOBAL: SHANDALAR 0x0093a93c
GLOBAL_OTHERS_EXTERN HWND g_duel_status_window_hwnd;

// GLOBAL: MAGIC 0x007abc74
// GLOBAL: SHANDALAR 0x007bfe74
GLOBAL_OTHERS_EXTERN int g_saved_stop_phase;

// GLOBAL: MAGIC 0x0069c6f0
// GLOBAL: SHANDALAR 0x005a6198
GLOBAL_OTHERS_EXTERN int g_master_csv_offsets[0x4e2];

// GLOBAL: MAGIC 0x0091b280
// GLOBAL: SHANDALAR 0x0092f3d0
GLOBAL_OTHERS_EXTERN int g_duel_timer_id;

// GLOBAL: MAGIC 0x0094ca30
// GLOBAL: SHANDALAR 0x00951be4
GLOBAL_OTHERS_EXTERN HWND g_attack_phase_window_hwnd;

#ifdef GLOBALS_OTHERS_IMPL
int g_duel_ui_initialized = 0;
#else
// GLOBAL: MAGIC 0x0055e17c
// GLOBAL: SHANDALAR 0x0057f060
GLOBAL_OTHERS_EXTERN int g_duel_ui_initialized;
#endif

// GLOBAL: SHANDALAR 0x007483f0
// GLOBAL: MAGIC 0x0074b630
GLOBAL_OTHERS_EXTERN int g_random_seed_initialized;

#undef GLOBAL_OTHERS_EXTERN
#endif
