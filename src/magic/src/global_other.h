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

#if defined(GLOBALS_OTHERS_IMPL) && defined(SHANDALAR)
// GLOBAL: SHANDALAR 0x00591a08
ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39] = {
#include "shandalar/src/creature_names_init.inc"
};
#elif defined(GLOBALS_OTHERS_IMPL) && !defined(SHANDALAR)
// GLOBAL: MAGIC 0x00746880
ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39];
#else
extern ShandalarMonsterDefinition g_shandalar_monster_definitions[0x39];
#endif

// GLOBAL: MOK 0x543c10
// GLOBAL: MAGIC 0x00791560
GLOBAL_OTHERS_EXTERN card_instance_t global_displayed_card_instances[2][151];

// GLOBAL: MAGIC 0x00775d3c
GLOBAL_OTHERS_EXTERN int DAT_00775d3c;

// GLOBAL: MAGIC 0x00777aa0
GLOBAL_OTHERS_EXTERN int duel_trace_counter;

// GLOBAL: MAGIC 0x0093917c
GLOBAL_OTHERS_EXTERN int DAT_0093917c[33];

// GLOBAL: MAGIC 0x0064ee98
GLOBAL_OTHERS_EXTERN int DAT_0064ee98;

// GLOBAL: MAGIC 0x008b4980
GLOBAL_OTHERS_EXTERN HWND global_opponent_chat_hwnd;

// GLOBAL: MAGIC 0x00743030
GLOBAL_OTHERS_EXTERN int _DAT_00743030;

// GLOBAL: MAGIC 0x007161d4
GLOBAL_OTHERS_EXTERN phase_t previous_stop_phase;

// GLOBAL: MAGIC 0x00743034
GLOBAL_OTHERS_EXTERN int previous_stop_phase_player;

// GLOBAL: MAGIC 0x0074303c
GLOBAL_OTHERS_EXTERN int g_target_selection_status_code;

// GLOBAL: MAGIC 0x0072c440
GLOBAL_OTHERS_EXTERN int unk_0072c440[8];

// GLOBAL: MAGIC 0x00743000
GLOBAL_OTHERS_EXTERN int unk_00743000[2][8];

// GLOBAL: MAGIC 0x007ab290
// GLOBAL: SHANDALAR 0x007bf490
GLOBAL_OTHERS_EXTERN int g_duel_cached_raw_mana_player_1[7];

// GLOBAL: MAGIC 0x00715fa8
GLOBAL_OTHERS_EXTERN int unk_00715fa8;

// GLOBAL: MAGIC 0x00715fac
GLOBAL_OTHERS_EXTERN int phase_response_window_open;

// GLOBAL: MAGIC 0x00791550
GLOBAL_OTHERS_EXTERN int regenerate_or_graveyard_triggers_in_progress;

// GLOBAL: MAGIC 0x008cfd78
GLOBAL_OTHERS_EXTERN int hand_count[2];

// GLOBAL: MAGIC 0x008cfd80
GLOBAL_OTHERS_EXTERN int creature_cards_in_play[2];

// GLOBAL: MAGIC 0x008cfda8
GLOBAL_OTHERS_EXTERN int cards_drawn_count;

// GLOBAL: MAGIC 0x008cff10
GLOBAL_OTHERS_EXTERN int life_gained;

// GLOBAL: MAGIC 0x008cfd88
GLOBAL_OTHERS_EXTERN int artifact_cards_in_play[2];

// GLOBAL: MAGIC 0x008cfd90
GLOBAL_OTHERS_EXTERN int enchantments_in_play[2];

// GLOBAL: MAGIC 0x008cfdac
GLOBAL_OTHERS_EXTERN int unk_008cfdac;

// GLOBAL: MAGIC 0x008cfdb0
GLOBAL_OTHERS_EXTERN int unk_008cfdb0;

// GLOBAL: MAGIC 0x00743038
GLOBAL_OTHERS_EXTERN int unk_00743038;

// GLOBAL: MAGIC 0x0074308c
GLOBAL_OTHERS_EXTERN HWND DAT_0074308c;

// GLOBAL: MAGIC 0x00743090
GLOBAL_OTHERS_EXTERN HWND DAT_00743090;

// GLOBAL: MAGIC 0x007a7d88
GLOBAL_OTHERS_EXTERN int unk_007a7d88[2];

// GLOBAL: MAGIC 0x007a7d18
GLOBAL_OTHERS_EXTERN int unk_007a7d18[2];

// GLOBAL: MAGIC 0x00743094
GLOBAL_OTHERS_EXTERN int recorded_action_count;

// GLOBAL: MAGIC 0x007abc78
GLOBAL_OTHERS_EXTERN int battlefield_extra_ability_flags;

// GLOBAL: MAGIC 0x007abc80
GLOBAL_OTHERS_EXTERN int DAT_007abc80;

// GLOBAL: MAGIC 0x007a7d78
GLOBAL_OTHERS_EXTERN int DAT_007a7d78;

// GLOBAL: MAGIC 0x008ce534
GLOBAL_OTHERS_EXTERN HWND unk_008ce534;

// GLOBAL: MAGIC 0x008ce680
GLOBAL_OTHERS_EXTERN char DAT_008ce680[300];

// GLOBAL: MAGIC 0x00896670
// GLOBAL: SHANDALAR 0x008aa870
GLOBAL_OTHERS_EXTERN int g_duel_cached_raw_mana_player_0[7];

// GLOBAL: MAGIC 0x0064efa0
GLOBAL_OTHERS_EXTERN char current_duel_prompt_text[256];

// GLOBAL: MAGIC 0x0064f0a4
GLOBAL_OTHERS_EXTERN int _DAT_0064f0a4;

// GLOBAL: MAGIC 0x0064f0a8
GLOBAL_OTHERS_EXTERN int _DAT_0064f0a8;

// GLOBAL: MAGIC 0x00637558
GLOBAL_OTHERS_EXTERN int recorded_mana_payments[10][7];

#ifdef GLOBALS_OTHERS_IMPL
int mana_payment_record_depth = 0;
#else
// GLOBAL: MAGIC 0x005607e0
GLOBAL_OTHERS_EXTERN int mana_payment_record_depth;
#endif

#ifdef GLOBALS_OTHERS_IMPL
int affected_card_context_depth = 0;
#else
// GLOBAL: MAGIC 0x00561268
GLOBAL_OTHERS_EXTERN int affected_card_context_depth;
#endif

#ifdef GLOBALS_OTHERS_IMPL
int DAT_00561390 = 0;
#else
// GLOBAL: MAGIC 0x00561390
GLOBAL_OTHERS_EXTERN int DAT_00561390;
#endif

#ifdef GLOBALS_OTHERS_IMPL
char empty_duel_prompt_text[1] = "";
#else
// GLOBAL: MAGIC 0x0056e494
GLOBAL_OTHERS_EXTERN char empty_duel_prompt_text[1];
#endif

// GLOBAL: MAGIC 0x00939344
GLOBAL_OTHERS_EXTERN HWND unk_00939344;

// GLOBAL: MAGIC 0x0093a7b0
// GLOBAL: SHANDALAR 0x0094e8d0
GLOBAL_OTHERS_EXTERN HWND g_duel_prompt_context_hwnd;

// GLOBAL: MAGIC 0x0093a844
GLOBAL_OTHERS_EXTERN int phase_was_skipped;

// GLOBAL: MAGIC 0x0091bfb0
GLOBAL_OTHERS_EXTERN int DAT_0091bfb0;

// GLOBAL: MAGIC 0x008a915c
GLOBAL_OTHERS_EXTERN char *DAT_008a915c;

// GLOBAL: MAGIC 0x0074636c
GLOBAL_OTHERS_EXTERN int DAT_0074636c;

// GLOBAL: MAGIC 0x00776510
GLOBAL_OTHERS_EXTERN int DAT_00776510;

// GLOBAL: MAGIC 0x00776514
// GLOBAL: SHANDALAR 0x0073c7e4
#if defined(GLOBALS_OTHERS_IMPL) && defined(SHANDALAR)
extern int g_force_opponent_starts_duel;
#else
GLOBAL_OTHERS_EXTERN int g_force_opponent_starts_duel;
#endif

// GLOBAL: MAGIC 0x00775d40
GLOBAL_OTHERS_EXTERN int DAT_00775d40[500];

// GLOBAL: MAGIC 0x00896718
GLOBAL_OTHERS_EXTERN int DAT_00896718;

// GLOBAL: MAGIC 0x00925d3c
GLOBAL_OTHERS_EXTERN int unk_00925d3c;

// GLOBAL: MAGIC 0x00925d38
GLOBAL_OTHERS_EXTERN int unk_00925d38;

// GLOBAL: MAGIC 0x00791418
GLOBAL_OTHERS_EXTERN int DAT_00791418;

// GLOBAL: MAGIC 0x00926740
GLOBAL_OTHERS_EXTERN char ai_action_dialog_number_buffer[12];

// GLOBAL: MAGIC 0x0092674c
GLOBAL_OTHERS_EXTERN int g_waiting_for_network_packet;

// GLOBAL: MAGIC 0x0093f9c0
GLOBAL_OTHERS_EXTERN int unk_0093f9c0;

// GLOBAL: MAGIC 0x0093f4c0
GLOBAL_OTHERS_EXTERN int affected_card_context_stack[32][10];

// GLOBAL: MAGIC 0x0093f4b4
GLOBAL_OTHERS_EXTERN int unk_0093f4b4;

// GLOBAL: MAGIC 0x00925030
GLOBAL_OTHERS_EXTERN HINSTANCE g_app_instance;

// GLOBAL: MAGIC 0x008cf1d4
GLOBAL_OTHERS_EXTERN int landsofcolor_controlled[2][8];

// GLOBAL: MAGIC 0x00939530
GLOBAL_OTHERS_EXTERN int unk_00939530[2][8];

// GLOBAL: MAGIC 0x008cdab4
GLOBAL_OTHERS_EXTERN int DAT_008cdab4;

#ifdef GLOBALS_OTHERS_IMPL
int unk_0057a75c = 0;
#else
// GLOBAL: MAGIC 0x0057a75c
GLOBAL_OTHERS_EXTERN int unk_0057a75c;
#endif

#ifdef GLOBALS_OTHERS_IMPL
int ai_recorded_action_type = 0;
#else
// GLOBAL: MAGIC 0x0057aae8
GLOBAL_OTHERS_EXTERN int ai_recorded_action_type;
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

#ifdef GLOBALS_OTHERS_IMPL
// GLOBAL: MAGIC 0x0057f758
char unk_0057f758[4] = {0x7c, 0x23, 0, 0};
#else
GLOBAL_OTHERS_EXTERN char unk_0057f758[4];
#endif

#ifdef GLOBALS_OTHERS_IMPL
// GLOBAL: MAGIC 0x0057f75c
char unk_0057f75c[4] = {0x7c, 0x23, 0, 0};
#else
GLOBAL_OTHERS_EXTERN char unk_0057f75c[4];
#endif

// GLOBAL: MAGIC 0x00637670
GLOBAL_OTHERS_EXTERN char g_dialog_card_title_buffer[0x68];

// GLOBAL: MAGIC 0x006376d8
GLOBAL_OTHERS_EXTERN char g_prompt_stop_phase_anyway_text[300];

// GLOBAL: MAGIC 0x00638b30
GLOBAL_OTHERS_EXTERN HBITMAP g_mana_burn_dialog_background;

// GLOBAL: MAGIC 0x00638b60
GLOBAL_OTHERS_EXTERN COLORREF g_mana_burn_dialog_text_color;

// GLOBAL: MAGIC 0x00638c18
GLOBAL_OTHERS_EXTERN COLORREF g_mana_burn_dialog_shadow_color;

// GLOBAL: MAGIC 0x00638c80
GLOBAL_OTHERS_EXTERN int *g_mana_burn_dialog_data;

// GLOBAL: MAGIC 0x006a1db8
GLOBAL_OTHERS_EXTERN int unk_006a1db8[256];

// GLOBAL: MAGIC 0x006a21b8
GLOBAL_OTHERS_EXTERN int unk_006a21b8[256];

// GLOBAL: MAGIC 0x006a5f18
GLOBAL_OTHERS_EXTERN int unk_006a5f18[256];

// GLOBAL: MAGIC 0x006a6ae8
GLOBAL_OTHERS_EXTERN int recorded_action_codes[256];

// GLOBAL: MAGIC 0x006a6ee8
GLOBAL_OTHERS_EXTERN int unk_006a6ee8[256];

// GLOBAL: MAGIC 0x006a7e58
GLOBAL_OTHERS_EXTERN int saved_recorded_action_internal_card_ids[256];

// GLOBAL: MAGIC 0x006a8258
GLOBAL_OTHERS_EXTERN int unk_006a8258[256];

// GLOBAL: MAGIC 0x006a8660
GLOBAL_OTHERS_EXTERN int unk_006a8660[256];

// GLOBAL: MAGIC 0x006ab368
GLOBAL_OTHERS_EXTERN int saved_recorded_action_count;

// GLOBAL: MAGIC 0x00748770
GLOBAL_OTHERS_EXTERN char g_duel_text_scratch_buffer[3000];

// GLOBAL: MAGIC 0x0074309c
GLOBAL_OTHERS_EXTERN HDC unk_0074309c;

// GLOBAL: MAGIC 0x007462a0
GLOBAL_OTHERS_EXTERN int unk_007462a0;

// GLOBAL: MAGIC 0x007463dc
GLOBAL_OTHERS_EXTERN HPALETTE unk_007463dc;

// GLOBAL: MAGIC 0x007497b0
GLOBAL_OTHERS_EXTERN int unk_007497b0;

// GLOBAL: MAGIC 0x00789100
GLOBAL_OTHERS_EXTERN HWND g_duel_player_face_window_hwnd;

// GLOBAL: MAGIC 0x008b40c8
GLOBAL_OTHERS_EXTERN HWND g_duel_opponent_face_window_hwnd;

// GLOBAL: MAGIC 0x008cee70
// GLOBAL: SHANDALAR 0x008e2fc0
GLOBAL_OTHERS_EXTERN HWND g_duel_player_graveyard_window_hwnd;

// GLOBAL: MAGIC 0x0091ce30
GLOBAL_OTHERS_EXTERN HWND DAT_0091ce30;

// GLOBAL: MAGIC 0x00926804
// GLOBAL: SHANDALAR 0x0093a934
GLOBAL_OTHERS_EXTERN int g_duel_network_flags;

#ifdef _DEBUG
#define TRACE_ENABLED 1
#else
#define TRACE_ENABLED ((g_duel_network_flags & 2) != 0)
#endif

// GLOBAL: MAGIC 0x0092680c
GLOBAL_OTHERS_EXTERN HWND DAT_0092680c;

// GLOBAL: MAGIC 0x007abc74
GLOBAL_OTHERS_EXTERN int DAT_007abc74;

enum
{
  DUEL_PHASE_STOP_COUNT = 0x26,
  PHASE_STOP_ENABLED = 0x01,
  PHASE_STOP_SUPPRESSED = 0x02,
  PHASE_STOP_OPPONENT = 0x04
};

typedef struct duel_phase_stop_settings_struct
{
  unsigned char phase_flags[DUEL_PHASE_STOP_COUNT];
} duel_phase_stop_settings_t;

// GLOBAL: MAGIC 0x007abc90
GLOBAL_OTHERS_EXTERN duel_phase_stop_settings_t g_duel_phase_stop_settings[2];

// GLOBAL: MAGIC 0x0069c6f0
// GLOBAL: SHANDALAR 0x005a6198
GLOBAL_OTHERS_EXTERN int master_csv_offsets[0x4e2];

// GLOBAL: MAGIC 0x0091b280
// GLOBAL: SHANDALAR 0x0092f3d0
GLOBAL_OTHERS_EXTERN int g_duel_timer_id;

// GLOBAL: MAGIC 0x0094ca30
GLOBAL_OTHERS_EXTERN HWND DAT_0094ca30;

#ifdef GLOBALS_OTHERS_IMPL
int DAT_0055e17c = 0;
#else
// GLOBAL: MAGIC 0x0055e17c
GLOBAL_OTHERS_EXTERN int DAT_0055e17c;
#endif

// GLOBAL: SHANDALAR 0x007483f0
// GLOBAL: MAGIC 0x0074b630
GLOBAL_OTHERS_EXTERN int random_seed_initialized;

#undef GLOBAL_OTHERS_EXTERN
#endif
