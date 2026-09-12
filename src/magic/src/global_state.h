#ifndef __GLOBAL_STATE_H__
#define __GLOBAL_STATE_H__

#include <windows.h>
#include "defs.h"
#include "duel_engine.h"

// Single header library style
#ifdef GLOBAL_STATE_IMPL
#define GLOBAL_STATE_EXTERN
#else
#define GLOBAL_STATE_EXTERN extern
#endif

// See manalink.lds and ai.c for more some of these addresses and types
// A lot of these can potentially be correlated through save_or_load_ver1 and save_or_load_ver2

// GLOBAL: MOK 0x004ef540
// GLOBAL: MAGIC 0x008b60f0
// GLOBAL: SHANDALAR 0x008ca270
GLOBAL_STATE_EXTERN card_instance_t global_card_instances[2][151];

#define PLAYER_CARD_INSTANCE(player_, card_) global_card_instances[player_][card_]

#define PARENT_CARD_INSTANCE(player_, card_) \
  PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player_, card_).parent_controller, PLAYER_CARD_INSTANCE(player_, card_).parent_card)

#define DAMAGE_TARGET_CARD_INSTANCE(player_, card_) \
  PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player_, card_).damage_target_player, PLAYER_CARD_INSTANCE(player_, card_).damage_target_card)

#define DAMAGE_SOURCE_CARD_INSTANCE(player_, card_) \
  PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player_, card_).damage_source_player, PLAYER_CARD_INSTANCE(player_, card_).damage_source_card)

#define TARGET_CARD_INSTANCE(player_, card_, target_index_) \
  PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player_, card_).targets[target_index_].player, PLAYER_CARD_INSTANCE(player_, card_).targets[target_index_].card)

#define IS_SICK(player_, card_) (((PLAYER_CARD_INSTANCE(player_, card_).state & STATE_SUMMONSICK_BOTH) != 0) && \
                                 ((global_cards_data[PLAYER_CARD_INSTANCE(player_, card_).internal_card_id].type & TYPE_CREATURE) != 0))

#define IS_NOT_SICK(player_, card_) (((PLAYER_CARD_INSTANCE(player_, card_).state & STATE_SUMMONSICK_BOTH) == 0) || \
                                     ((global_cards_data[PLAYER_CARD_INSTANCE(player_, card_).internal_card_id].type & TYPE_CREATURE) == 0))

#define CAN_TAP(player_, card_) (IS_NOT_SICK(player_, card_) && ((PLAYER_CARD_INSTANCE(player_, card).state & STATE_TAPPED) == 0))

// GLOBAL: MAGIC 0x00939810
// GLOBAL: SHANDALAR 0x0094d930
GLOBAL_STATE_EXTERN int global_graveyard_slots[2][500];

// GLOBAL: MAGIC 0x008cecc0
// GLOBAL: SHANDALAR 0x008e2e10
GLOBAL_STATE_EXTERN int global_ante_cards[2][16];

typedef struct
{
  int csvid;
  int numcards;
} csvid_and_numcards;

typedef struct
{
  int show_cue_cards;                  // +0x00
  int directive_tracks_mouse;          // +0x04
  int show_power_toughness_on_cards;   // +0x08
  int show_abilities_on_cards;         // +0x0c
  int expand_text_box_on_big_card;     // +0x10
  int see_next_draws_at_end_of_duel;   // +0x14
  int show_id_tags_on_cards;           // +0x18
  int show_invisible_effect_cards;     // +0x1c
  int show_all_cards_summon_sickness;  // +0x20
  int show_coin_flips;                 // +0x24
  int layout;                          // +0x28
  int player_territory_color;          // +0x2c
  int player_territory_type;           // +0x30
} DuelInterfaceOptions;
STATIC_ASSERT(sizeof(DuelInterfaceOptions) == 0x34, duel_interface_options_wrong_size);

typedef struct
{
  int difficulty;              // +0x00
  int ante;                    // +0x04
  int best_of;                 // +0x08
  int allow_sideboarding;      // +0x0c
  char player_deck[0x34];      // +0x10
  int player_random;           // +0x44
  char opponent_deck[0x34];    // +0x48
  int opponent_random;         // +0x7c
} SoloDuelOptions;
STATIC_ASSERT(sizeof(SoloDuelOptions) == 0x80, solo_duel_options_wrong_size);

typedef struct
{
  int ante;                    // +0x00
  int best_of;                 // +0x04
  int allow_sideboarding;      // +0x08
  char player_deck[0x34];      // +0x0c
  int player_random;           // +0x40
  int gauntlet_length;         // +0x44
} GauntletOptions;
STATIC_ASSERT(sizeof(GauntletOptions) == 0x48, gauntlet_options_wrong_size);

typedef struct
{
  int rounds;                  // +0x00
  int starter_packs[2];        // +0x04
  int starter_count;           // +0x0c
  int booster_packs[4];        // +0x10
  int booster_count;           // +0x20
  int free_lands;              // +0x24
  int ante;                    // +0x28
  int best_of;                 // +0x2c
  int minimum_deck_size;       // +0x30
} SealedDeckOptions;
STATIC_ASSERT(sizeof(SealedDeckOptions) == 0x34, sealed_deck_options_wrong_size);

typedef enum
{
  SHANDALAR_ENTRY_NONE = -1,
  SHANDALAR_ENTRY_LAIR = 0x0,
  SHANDALAR_ENTRY_WITCH = 0x1,
  SHANDALAR_ENTRY_UNDEAD_KNIGHT = 0x2,
  SHANDALAR_ENTRY_WARLOCK = 0x3,
  SHANDALAR_ENTRY_VAMPIRELORD = 0x4,
  SHANDALAR_ENTRY_NETHER_FIEND = 0x5,
  SHANDALAR_ENTRY_NECROMANCER = 0x6,
  SHANDALAR_ENTRY_GREATER_LICH = 0x7,
  SHANDALAR_ENTRY_CLERIC = 0x8,
  SHANDALAR_ENTRY_PRIESTESS = 0x9,
  SHANDALAR_ENTRY_CRUSADER = 0xa,
  SHANDALAR_ENTRY_PALADIN = 0xb,
  SHANDALAR_ENTRY_ARCH_ANGEL = 0xc,
  SHANDALAR_ENTRY_HIGH_PRIEST = 0xd,
  SHANDALAR_ENTRY_SAINTED_ONE = 0xe,
  SHANDALAR_ENTRY_SEER = 0xf,
  SHANDALAR_ENTRY_MERFOLK_SHAMAN = 0x10,
  SHANDALAR_ENTRY_CONJURER = 0x11,
  SHANDALAR_ENTRY_SEA_DRAGON = 0x12,
  SHANDALAR_ENTRY_SHAPESHIFTER = 0x13,
  SHANDALAR_ENTRY_THOUGHT_INVOKER = 0x14,
  SHANDALAR_ENTRY_ASTRAL_VISIONARY = 0x15,
  SHANDALAR_ENTRY_DRUID = 0x16,
  SHANDALAR_ENTRY_ELVISH_MAGI = 0x17,
  SHANDALAR_ENTRY_ENCHANTRESS = 0x18,
  SHANDALAR_ENTRY_FOREST_DRAGON = 0x19,
  SHANDALAR_ENTRY_BEASTMASTER = 0x1a,
  SHANDALAR_ENTRY_SUMMONER = 0x1b,
  SHANDALAR_ENTRY_GREAT_DRUID = 0x1c,
  SHANDALAR_ENTRY_SORCERESS = 0x1d,
  SHANDALAR_ENTRY_SORCERER = 0x1e,
  SHANDALAR_ENTRY_TROLL_SHAMAN = 0x1f,
  SHANDALAR_ENTRY_GOBLIN_LORD = 0x20,
  SHANDALAR_ENTRY_HYDRA = 0x21,
  SHANDALAR_ENTRY_WAR_MAGE = 0x22,
  SHANDALAR_ENTRY_DRAGON_LORD = 0x23,
  SHANDALAR_ENTRY_TUSK_GUARDIAN = 0x24,
  SHANDALAR_ENTRY_SEDGE_BEAST = 0x25,
  SHANDALAR_ENTRY_APE_LORD = 0x26,
  SHANDALAR_ENTRY_CENTAUR_SHAMAN = 0x27,
  SHANDALAR_ENTRY_WINGED_STALLION = 0x28,
  SHANDALAR_ENTRY_FUNGUS_MASTER = 0x29,
  SHANDALAR_ENTRY_CENTAUR_WARCHIEF = 0x2a,
  SHANDALAR_ENTRY_MIND_STEALER = 0x2b,
  SHANDALAR_ENTRY_LORD_OF_FATE = 0x2c,
  SHANDALAR_ENTRY_ELEMENTALIST = 0x2d,
  SHANDALAR_ENTRY_AGA_GALNEER = 0x2e,
  SHANDALAR_ENTRY_ALT_A_KESH = 0x2f,
  SHANDALAR_ENTRY_QUELTOSH = 0x30,
  SHANDALAR_ENTRY_SALTREM_TOR = 0x31,
  SHANDALAR_ENTRY_MANDURANG = 0x32,
  SHANDALAR_ENTRY_WHIM = 0x33,
  SHANDALAR_ENTRY_PRISMAT = 0x34,
  SHANDALAR_ENTRY_DRACUR = 0x35,
  SHANDALAR_ENTRY_KISKA_RA = 0x36,
  SHANDALAR_ENTRY_ARZAKON = 0x37,
  SHANDALAR_ENTRY_ARZAKON_COPY = 0x38
} ShandalarEntryType;

typedef struct
{
  ShandalarEntryType entry_type;   // +0x00
  int world_x;                     // +0x04
  int world_y;                     // +0x08
  int color;                       // +0x0c
  signed char movement_heading;    // +0x10
  signed char movement_anim_frame; // +0x11
  unsigned char pad_0012[2];       // +0x12
  int respawn_timestamp;           // +0x14
} ShandalarMonsterSlot;
STATIC_ASSERT(sizeof(ShandalarMonsterSlot) == 0x18, shandalar_card_slot_wrong_size);

typedef struct
{
  int card_slot_1;             // +0x00
  int card_slot_2;             // +0x04
  int card_slot_3;             // +0x08
  int card_in_effect;          // +0x0c
  int world_x;                 // +0x10
  int world_y;                 // +0x14
  int north_of_town_index;     // +0x18
  unsigned char color;         // +0x1c
  unsigned char monster_flags; // +0x1d
  unsigned char pad_1e;        // +0x1e
  unsigned char pad_1f;        // +0x1f
  int clues_bitmap;            // +0x20
  unsigned int rules_bitmap;   // +0x24
  int times_entered;           // +0x28
  int reserved_2c;             // +0x2c
} ShandalarEncounterSlot;
STATIC_ASSERT(sizeof(ShandalarEncounterSlot) == 0x30, shandalar_encounter_slot_wrong_size);

typedef struct
{
  int location_type;              // +0x00
  int world_x;                    // +0x04
  int world_y;                    // +0x08
  int trade_color_and_type;       // +0x0c
  int status_and_ruling_wizard;   // +0x10
  int card_slots[8];              // +0x14
  int card_restock_timers[8];     // +0x34
  int quest_restock_timer;        // +0x54
  int special_card_restock_timer; // +0x58
  int visit_count;                // +0x5c
  int last_visit_timer;           // +0x60
} WorldNode;
STATIC_ASSERT(sizeof(WorldNode) == 100, world_node_wrong_size);

typedef enum
{
  JOURNAL_ENTRY_NONE = 0,
  JOURNAL_ENTRY_TOWN = 1,
  JOURNAL_ENTRY_CREATURE_DUEL = 2,
  JOURNAL_ENTRY_DUNGEON_ENTERED = 3,
  JOURNAL_ENTRY_CASTLE_DUNGEON_ENTERED = 4,
  JOURNAL_ENTRY_RANDOM_EVENT = 5,
  JOURNAL_ENTRY_WORLD_MAGIC_LEARNED = 6,
  JOURNAL_ENTRY_CITY_SAVED = 7,
  JOURNAL_ENTRY_CARD_FOUND_SET0 = 8,
  JOURNAL_ENTRY_CARD_FOUND_SET1 = 9,
  JOURNAL_ENTRY_CARD_FOUND_SET2 = 10,
  JOURNAL_ENTRY_CARD_FOUND_SET3 = 11,
  JOURNAL_ENTRY_CARD_FOUND_SET4 = 12,
  JOURNAL_ENTRY_LAIR_EXPLORED = 13,
  JOURNAL_ENTRY_QUEST_ACCEPTED = 15,
  JOURNAL_ENTRY_SPELL_FOUND = 16,
  JOURNAL_ENTRY_QUEST_FAILED = 17,
  JOURNAL_ENTRY_WORLD_MAGIC_EVENT = 18,
  JOURNAL_ENTRY_DUNGEON_TREASURE = 19
} JournalEntryType;

typedef enum
{
  JOURNAL_WORLD_MAGIC_EVENT_TELEPORT_RANDOM = 2,
  JOURNAL_WORLD_MAGIC_EVENT_DELAY_MONSTERS = 3,
  JOURNAL_WORLD_MAGIC_EVENT_BANISH_MONSTER = 4,
  JOURNAL_WORLD_MAGIC_EVENT_TELEPORT_TO_CASTLE = 5
} JournalWorldMagicEventType;

typedef struct
{
  JournalEntryType type; // +0x00
  unsigned int arg;      // +0x04
  int tile_x;            // +0x08
  int tile_y;            // +0x0c
} JournalEntry;
STATIC_ASSERT(sizeof(JournalEntry) == 0x10, journal_entry_wrong_size);

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN csvid_and_numcards g_initial_library[2][200] = {
#include "initial_library_init.inc"
};
#else
// GLOBAL: MAGIC 0x0056fb20
// GLOBAL: SHANDALAR 0x0058d3c8
GLOBAL_STATE_EXTERN csvid_and_numcards g_initial_library[2][200];
#endif

// GLOBAL: MAGIC 0x008cc980
// GLOBAL: SHANDALAR 0x008e0b00
GLOBAL_STATE_EXTERN int global_exile[2][500];

// GLOBAL: MAGIC 0x008951c8
// GLOBAL: SHANDALAR 0x008a93c8
GLOBAL_STATE_EXTERN int g_encounter_color_match_mode;

// GLOBAL: MAGIC 0x00895210
// GLOBAL: SHANDALAR 0x008a9410
GLOBAL_STATE_EXTERN int global_library[2][500];

// GLOBAL: MOK 0x507ec0
// GLOBAL: MAGIC 0x0093a990
// GLOBAL: SHANDALAR 0x0094eab0
GLOBAL_STATE_EXTERN int TENTATIVE_timestamp_player[500];

// GLOBAL: MOK 0x5076f0
// GLOBAL: MAGIC 0x8b5910
// GLOBAL: SHANDALAR 0x008c9a90
GLOBAL_STATE_EXTERN int TENTATIVE_timestamp_card[500];

// GLOBAL: MAGIC 0x005707f0
// GLOBAL: SHANDALAR 0x005854f8
GLOBAL_STATE_EXTERN int g_cyclopean_tomb_cast_sequence;

// GLOBAL: MOK 0x5096b0
// GLOBAL: MAGIC 0x9251e0
// GLOBAL: SHANDALAR 0x00939310
GLOBAL_STATE_EXTERN target_t global_stack_cards[32];

// GLOBAL: MOK 0x5097b0
// GLOBAL: MAGIC 0x926810
// GLOBAL: SHANDALAR 0x0093a940
GLOBAL_STATE_EXTERN target_t global_stack_damage_targets[32];

// GLOBAL: MAGIC 0x0078944c
// GLOBAL: SHANDALAR 0x007a01dc
GLOBAL_STATE_EXTERN int g_combat_assignment_cancelled;

// GLOBAL: MAGIC 0x00789450
// GLOBAL: SHANDALAR 0x007a01e0
GLOBAL_STATE_EXTERN int g_event_result;

// GLOBAL: MAGIC 0x00777850
// GLOBAL: SHANDALAR 0x0078e5dc
GLOBAL_STATE_EXTERN int g_ai_search_result;

// GLOBAL: MAGIC 0x00777854
// GLOBAL: SHANDALAR 0x0078e5e0
GLOBAL_STATE_EXTERN int g_ai_search_flags;

// GLOBAL: MAGIC 0x00789714
// GLOBAL: SHANDALAR 0x007a04a4
GLOBAL_STATE_EXTERN int g_response_processing_flags;

// GLOBAL: MAGIC 0x008b426c
// GLOBAL: SHANDALAR 0x008c83ec
GLOBAL_STATE_EXTERN int g_spell_fizzled;

// GLOBAL: MAGIC 0x00777a9c
// GLOBAL: SHANDALAR 0x0078e82c
GLOBAL_STATE_EXTERN int g_ai_decision_code;
// GLOBAL: MAGIC 0x00777aa8
// GLOBAL: SHANDALAR 0x0078e838
GLOBAL_STATE_EXTERN int g_ai_decision_parameter;

// GLOBAL: MAGIC 0x00777be0
// GLOBAL: SHANDALAR 0x0078e970
GLOBAL_STATE_EXTERN int g_active_cards_count[2];

// GLOBAL: MAGIC 0x00939048
// GLOBAL: SHANDALAR 0x0094d178
GLOBAL_STATE_EXTERN trigger_t g_trigger_condition;

// GLOBAL: MAGIC 0x00938e2c
// GLOBAL: SHANDALAR 0x0094cf5c
GLOBAL_STATE_EXTERN int g_required_mana_color_mask;

// GLOBAL: MAGIC 0x00939338
// GLOBAL: SHANDALAR 0x0094d464
GLOBAL_STATE_EXTERN phase_t g_current_phase;

// GLOBAL: MAGIC 0x00939340
// GLOBAL: SHANDALAR 0x0094d46c
GLOBAL_STATE_EXTERN int g_ai_recorded_choice;

// GLOBAL: MAGIC 0x00939510
// GLOBAL: SHANDALAR 0x0094d640
GLOBAL_STATE_EXTERN int g_trigger_dispatch_depth;

// GLOBAL: MAGIC 0x0093d850
// GLOBAL: SHANDALAR 0x00951970
GLOBAL_STATE_EXTERN int g_recorded_action_cursor;

// GLOBAL: MAGIC 0x00939180
// GLOBAL: SHANDALAR 0x0094d2b0
GLOBAL_STATE_EXTERN int g_stack_data[32];

typedef struct
{
  int amount;
  unsigned int source_type;
  char unknown_8;
  char padding_9[3];
  int unknown_c;
} damage_accumulator_t;

// GLOBAL: MOK 0x005098b0
// GLOBAL: MAGIC 0x0093b280
// GLOBAL: SHANDALAR 0x0094f3a0
GLOBAL_STATE_EXTERN damage_accumulator_t g_damage_accumulators[2][151][2];
STATIC_ASSERT(sizeof(g_damage_accumulators) == 0x25c0, g_damage_accumulators_wrong_size);
STATIC_ASSERT(sizeof(damage_accumulator_t) == 0x10, damage_accumulator_t_wrong_size);

// GLOBAL: MAGIC 0x00895030
// GLOBAL: SHANDALAR 0x008a9230
GLOBAL_STATE_EXTERN int g_stack_trigger_condition[32];

// GLOBAL: MAGIC 0x007a7750
// GLOBAL: SHANDALAR 0x007be4e0
GLOBAL_STATE_EXTERN int g_stack_prompt_flags[32];

// GLOBAL: MAGIC 0x008b497c
// GLOBAL: SHANDALAR 0x008c8afc
GLOBAL_STATE_EXTERN int g_current_turn;

// GLOBAL: MAGIC 0x008b32b8
// GLOBAL: SHANDALAR 0x008c7468
GLOBAL_STATE_EXTERN int g_other_player;

// GLOBAL: MAGIC 0x008b2934
// GLOBAL: SHANDALAR 0x008c6ae4
GLOBAL_STATE_EXTERN int g_stack_size;

// GLOBAL: MAGIC 0x00742fc4
// GLOBAL: SHANDALAR 0x0077e58c
GLOBAL_STATE_EXTERN int g_duel_active;

// GLOBAL: MAGIC 0x00742fc8
// GLOBAL: SHANDALAR 0x0077e590
GLOBAL_STATE_EXTERN int g_phase_stop_suppressed;
// GLOBAL: MAGIC 0x00742fcc
// GLOBAL: SHANDALAR 0x0077e6cc
GLOBAL_STATE_EXTERN int g_target_player_choice;
// GLOBAL: MAGIC 0x00742fb8
GLOBAL_STATE_EXTERN char *g_load_error_text;
// GLOBAL: MAGIC 0x00742fbc
// GLOBAL: SHANDALAR 0x0077e058
GLOBAL_STATE_EXTERN int g_response_selection_in_progress;

// GLOBAL: MAGIC 0x00743024
// GLOBAL: SHANDALAR 0x0077f1c4
GLOBAL_STATE_EXTERN int g_attack_action_available;

// GLOBAL: MAGIC 0x00743020
// GLOBAL: SHANDALAR 0x0077edc0
GLOBAL_STATE_EXTERN unsigned int g_response_action_mask;

// GLOBAL: MAGIC 0x00743028
// GLOBAL: SHANDALAR 0x0077f1c8
GLOBAL_STATE_EXTERN unsigned int g_load_menu_valid_slot_mask;

// GLOBAL: MAGIC 0x007161e0
// GLOBAL: SHANDALAR 0x0074cdc0
GLOBAL_STATE_EXTERN int g_mana_color_conversions[2][11];

// GLOBAL: MAGIC 0x007161d0
// GLOBAL: SHANDALAR 0x0074c960
GLOBAL_STATE_EXTERN int g_response_window_depth;
// GLOBAL: MAGIC 0x007161d8
// GLOBAL: SHANDALAR 0x0074cdb4
GLOBAL_STATE_EXTERN int g_hand_count_adjustment;
// GLOBAL: MAGIC 0x00742f68
// GLOBAL: SHANDALAR 0x0077cf1c
GLOBAL_STATE_EXTERN int g_response_card_type_mask;

// GLOBAL: MAGIC 0x00742f64
// GLOBAL: SHANDALAR 0x0077ca10
GLOBAL_STATE_EXTERN int g_response_window_serial;

// GLOBAL: MAGIC 0x00743088
// GLOBAL: SHANDALAR 0x007806e8
GLOBAL_STATE_EXTERN int g_response_action_selected;

// GLOBAL: MAGIC 0x00716030
// GLOBAL: SHANDALAR 0x0074c790
GLOBAL_STATE_EXTERN int g_raw_mana_available_hex[2][51];

// GLOBAL: MAGIC 0x00742fe0
// GLOBAL: SHANDALAR 0x0077ed80
GLOBAL_STATE_EXTERN int g_basiclandtypes_controlled[2][8];
// GLOBAL: MAGIC 0x00742f70
// GLOBAL: SHANDALAR 0x0077d170
GLOBAL_STATE_EXTERN int g_special_mana_pool[2][8];
// GLOBAL: MAGIC 0x00743080
// GLOBAL: SHANDALAR 0x007806e0
GLOBAL_STATE_EXTERN int g_mana_pool_colorless_components[2];

// GLOBAL: MAGIC 0x00743040
// GLOBAL: SHANDALAR 0x007806a0
GLOBAL_STATE_EXTERN int g_raw_mana_available[2][8];

// GLOBAL: MAGIC 0x00715fb0
// GLOBAL: SHANDALAR 0x0074b8b8
GLOBAL_STATE_EXTERN int g_recorded_action_player;
// GLOBAL: MAGIC 0x00715fa4
// GLOBAL: SHANDALAR 0x0074b15c
GLOBAL_STATE_EXTERN int g_recorded_action_controller;

// GLOBAL: MAGIC 0x00791554
// GLOBAL: SHANDALAR 0x007a82e0
GLOBAL_STATE_EXTERN int g_current_player;
// GLOBAL: MAGIC 0x00791410
// GLOBAL: SHANDALAR 0x007a81a0
GLOBAL_STATE_EXTERN int g_duel_state_current_card;
// GLOBAL: MAGIC 0x0072c8e0
// GLOBAL: SHANDALAR 0x007658c0
GLOBAL_STATE_EXTERN int g_recorded_action_phase;
// GLOBAL: MAGIC 0x0072c8e4
// GLOBAL: SHANDALAR 0x00765e24
GLOBAL_STATE_EXTERN int g_ai_action_replay_available;
// GLOBAL: MAGIC 0x00789a48
// GLOBAL: SHANDALAR 0x007a07d4
GLOBAL_STATE_EXTERN int g_opponent_starting_card_id_3;
// GLOBAL: MAGIC 0x00789308
// GLOBAL: SHANDALAR 0x007a0098
GLOBAL_STATE_EXTERN int g_opponent_starting_card_id_2;

// GLOBAL: MAGIC 0x0078930c
// GLOBAL: SHANDALAR 0x007a009c
GLOBAL_STATE_EXTERN int g_current_casting_player;

// GLOBAL: MAGIC 0x007ab2b8
// GLOBAL: SHANDALAR 0x007bf4b8
GLOBAL_STATE_EXTERN int g_trigger_cause_controller;
// GLOBAL: MAGIC 0x007ab2bc
// GLOBAL: SHANDALAR 0x007bf4bc
GLOBAL_STATE_EXTERN int g_activation_dispatch_flags;
// GLOBAL: MAGIC 0x007ab2cc
// GLOBAL: SHANDALAR 0x007bf4c8
GLOBAL_STATE_EXTERN int(__cdecl *pending_killed_card_handler)(int, int);

// GLOBAL: MAGIC 0x008cee74
// GLOBAL: SHANDALAR 0x008e2fc4
GLOBAL_STATE_EXTERN int g_trigger_cause;

// GLOBAL: MAGIC 0x00742f60
// GLOBAL: SHANDALAR 0x0077c57c
GLOBAL_STATE_EXTERN int g_event_player;

// GLOBAL: MAGIC 0x007a7c1c
// GLOBAL: SHANDALAR 0x007be9ac
GLOBAL_STATE_EXTERN int g_upkeep_payment_completed;

// GLOBAL: MAGIC 0x007a7d80
// GLOBAL: SHANDALAR 0x007beb10
GLOBAL_STATE_EXTERN int g_global_stats_bonus[4];

// Layout: player power bonuses, followed by player toughness bonuses.
#define g_global_power_bonus (g_global_stats_bonus)
#define g_global_toughness_bonus (g_global_stats_bonus + 2)

// GLOBAL: MAGIC 0x007a7874
// GLOBAL: SHANDALAR 0x007be604
GLOBAL_STATE_EXTERN int g_current_encounter_strength;
// GLOBAL: MAGIC 0x007aadec
// GLOBAL: SHANDALAR 0x007befec
GLOBAL_STATE_EXTERN int g_current_action_event_code;

// GLOBAL: MAGIC 0x007abc00
// GLOBAL: SHANDALAR 0x007bfe00
GLOBAL_STATE_EXTERN unsigned int g_ai_action_score_table[16];

enum
{
  DUEL_PHASE_STOP_COUNT = 0x26,
  PHASE_STOP_ENABLED = 0x01,
  PHASE_STOP_SUPPRESSED = 0x02,
  PHASE_STOP_OPPONENT = 0x04
};

typedef struct duel_phase_stop_settings_struct
{
  char phase_flags[DUEL_PHASE_STOP_COUNT];
} duel_phase_stop_settings_t;

// GLOBAL: MAGIC 0x007abc90
// GLOBAL: SHANDALAR 0x007bfe90
GLOBAL_STATE_EXTERN duel_phase_stop_settings_t g_duel_phase_stop_settings[2];

// GLOBAL: MAGIC 0x008b3270
// GLOBAL: SHANDALAR 0x008c7420
GLOBAL_STATE_EXTERN int g_activation_event_flags;

// GLOBAL: MAGIC 0x008b35ec
// GLOBAL: SHANDALAR 0x008c776c
GLOBAL_STATE_EXTERN int g_active_player;

// GLOBAL: MAGIC 0x008b4270
// GLOBAL: SHANDALAR 0x008c83f0
GLOBAL_STATE_EXTERN int g_life[2];

// GLOBAL: MAGIC 0x008b4278
// GLOBAL: SHANDALAR 0x008c83f8
GLOBAL_STATE_EXTERN land_can_be_played_t g_land_can_be_played;

// GLOBAL: MAGIC 0x008b4dd0
// GLOBAL: SHANDALAR 0x008c8f50
GLOBAL_STATE_EXTERN int g_affected_internal_card_id;

// GLOBAL: MAGIC 0x00789b7c
// GLOBAL: SHANDALAR 0x007a090c
GLOBAL_STATE_EXTERN int g_current_casting_internal_card_id;

// GLOBAL: MAGIC 0x008a8d74
// GLOBAL: SHANDALAR 0x008bcf74
GLOBAL_STATE_EXTERN int g_ai_search_force_pass;

// GLOBAL: MAGIC 0x008a8d7c
// GLOBAL: SHANDALAR 0x008bcf7c
GLOBAL_STATE_EXTERN int g_ai_search_target_depth;

// GLOBAL: MAGIC 0x008a8de4
// GLOBAL: SHANDALAR 0x008bcfe4
GLOBAL_STATE_EXTERN int g_duel_state_008a8de4;

// GLOBAL: MAGIC 0x008ce4ec
// GLOBAL: SHANDALAR 0x008e266c
GLOBAL_STATE_EXTERN int g_max_trigger_dispatch_depth;

// GLOBAL: MAGIC 0x008ce510
// GLOBAL: SHANDALAR 0x008e2690
GLOBAL_STATE_EXTERN int g_mana_charge[8];
#define MANA_CHARGE_POWER_ARTIFACT_REDUCTION 7

// GLOBAL: MAGIC 0x008ce538
// GLOBAL: SHANDALAR 0x008e26b8
GLOBAL_STATE_EXTERN int g_current_encounter_color;

// GLOBAL: MAGIC 0x0091a6d0
// GLOBAL: SHANDALAR 0x0092e820
GLOBAL_STATE_EXTERN int g_nested_trigger_depth;

// GLOBAL: MAGIC 0x0091bf70
// GLOBAL: SHANDALAR 0x009300b0
GLOBAL_STATE_EXTERN int g_nested_trigger_stack[16];

// GLOBAL: MAGIC 0x0091bfb4
// GLOBAL: SHANDALAR 0x009300f4
GLOBAL_STATE_EXTERN int g_adventure_world_state;

// GLOBAL: MAGIC 0x008b60e0
// GLOBAL: SHANDALAR 0x008ca260
GLOBAL_STATE_EXTERN int g_attacking_creature_count;

// GLOBAL: MAGIC 0x00926668
// GLOBAL: SHANDALAR 0x0093a798
GLOBAL_STATE_EXTERN int g_duel_extra_turn_player;

// GLOBAL: MAGIC 0x00926664
// GLOBAL: SHANDALAR 0x0093a794
GLOBAL_STATE_EXTERN int g_produced_mana_color;

// GLOBAL: MAGIC 0x008cc840
// GLOBAL: SHANDALAR 0x008e09c0
GLOBAL_STATE_EXTERN int g_ai_search_try_count;
// GLOBAL: MAGIC 0x008cc844
// GLOBAL: SHANDALAR 0x008e09c4
GLOBAL_STATE_EXTERN int g_ai_search_retry_state;

// GLOBAL: MAGIC 0x0093f4b8
// GLOBAL: SHANDALAR 0x0073c00c
GLOBAL_STATE_EXTERN int g_card_count;

// GLOBAL: MAGIC 0x008b2880
// GLOBAL: SHANDALAR 0x008c6a30
GLOBAL_STATE_EXTERN int g_duel_state_008b2880;

// GLOBAL: MAGIC 0x00925bb8
// GLOBAL: SHANDALAR 0x00939ce8
GLOBAL_STATE_EXTERN int g_ai_recorded_action;

// GLOBAL: MAGIC 0x008b40f4
// GLOBAL: SHANDALAR 0x008c8274
GLOBAL_STATE_EXTERN int g_x_value;

// GLOBAL: MAGIC 0x008b44d0
// GLOBAL: SHANDALAR 0x008c8650
GLOBAL_STATE_EXTERN int g_lich_active[2];

// GLOBAL: MAGIC 0x008ce568
// GLOBAL: SHANDALAR 0x008e26bc
GLOBAL_STATE_EXTERN int g_max_x_value;

// GLOBAL: MAGIC 0x008cf6d4
// GLOBAL: SHANDALAR 0x008e3824
GLOBAL_STATE_EXTERN int g_affected_card_color;
// GLOBAL: MAGIC 0x008cf1b8
// GLOBAL: SHANDALAR 0x008e3308
GLOBAL_STATE_EXTERN int g_last_single_card_event_result;

// GLOBAL: MAGIC 0x008cf1c0
// GLOBAL: SHANDALAR 0x008e3310
GLOBAL_STATE_EXTERN int g_creature_power_by_color[2][8];

// GLOBAL: MAGIC 0x00939520
// GLOBAL: SHANDALAR 0x0094d650
GLOBAL_STATE_EXTERN int g_ai_mana_demand_by_color[2][8];

// GLOBAL: MAGIC 0x0093950c
// GLOBAL: SHANDALAR 0x0094d63c
GLOBAL_STATE_EXTERN int g_ai_modifier;

// GLOBAL: MAGIC 0x008cfd20
// GLOBAL: SHANDALAR 0x008e3e70
GLOBAL_STATE_EXTERN int g_duel_state_008cfd20;

// GLOBAL: MAGIC 0x0093dabc
// GLOBAL: SHANDALAR 0x00951bdc
GLOBAL_STATE_EXTERN int g_affected_card;

// GLOBAL: MAGIC 0x008cd928
// GLOBAL: SHANDALAR 0x008e1aa8
GLOBAL_STATE_EXTERN int g_affected_card_controller;
// GLOBAL: MAGIC 0x0093a808
// GLOBAL: SHANDALAR 0x0094e928
GLOBAL_STATE_EXTERN int g_attacking_card_controller;

// GLOBAL: MAGIC 0x008cdab0
// GLOBAL: SHANDALAR 0x008e1c30
GLOBAL_STATE_EXTERN int g_ai_side_scores[2];
#define g_ai_side_score_player_0 (g_ai_side_scores[0])
#define g_ai_side_score_player_1 (g_ai_side_scores[1])

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_opponent_initial_library_index = -1;
#else
// GLOBAL: MAGIC 0x0057a750
// GLOBAL: SHANDALAR 0x00591200
GLOBAL_STATE_EXTERN int g_opponent_initial_library_index;
#endif

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_selected_wizard_color = -1;
#else
// GLOBAL: MAGIC 0x0057a754
// GLOBAL: SHANDALAR 0x00591204
GLOBAL_STATE_EXTERN int g_selected_wizard_color;
#endif

// GLOBAL: MAGIC 0x008ce530
// GLOBAL: SHANDALAR 0x008e26b0
GLOBAL_STATE_EXTERN int g_attacking_card;
// GLOBAL: MAGIC 0x0093b160
// GLOBAL: SHANDALAR 0x0094f280
GLOBAL_STATE_EXTERN int g_card_on_stack_controller;
// GLOBAL: MAGIC 0x0091ce2c
// GLOBAL: SHANDALAR 0x00930f5c
GLOBAL_STATE_EXTERN int g_card_on_stack;
// GLOBAL: MAGIC 0x00939330
// GLOBAL: SHANDALAR 0x0094d45c
GLOBAL_STATE_EXTERN int g_ai_speculation_depth;
// GLOBAL: MAGIC 0x007a7c58
// GLOBAL: SHANDALAR 0x007be9e8
GLOBAL_STATE_EXTERN int g_graveyard_card_types[2];
// GLOBAL: MAGIC 0x007a7c60
// GLOBAL: SHANDALAR 0x007be9f0
GLOBAL_STATE_EXTERN char g_duel_message_text[100];
// GLOBAL: MAGIC 0x009266d0
// GLOBAL: SHANDALAR 0x0093a800
GLOBAL_STATE_EXTERN char g_saved_player_name[0x32];

// GLOBAL: MAGIC 0x00926702
// GLOBAL: SHANDALAR 0x0093a832
GLOBAL_STATE_EXTERN char g_saved_player_name_terminator;

// GLOBAL: MAGIC 0x00926790
// GLOBAL: SHANDALAR 0x0093a8c0
GLOBAL_STATE_EXTERN int g_response_action_taken;

// GLOBAL: MAGIC 0x007a79b0
// GLOBAL: SHANDALAR 0x007be740
GLOBAL_STATE_EXTERN int g_player_special_effect_flags[2];

// GLOBAL: MAGIC 0x0091c4fc
// GLOBAL: SHANDALAR 0x0093063c
GLOBAL_STATE_EXTERN int g_duel_state_0091c4fc;

// GLOBAL: MAGIC 0x0091c500
// GLOBAL: SHANDALAR 0x00930640
GLOBAL_STATE_EXTERN int g_ai_search_time_limit;

// GLOBAL: MAGIC 0x006abe30
// GLOBAL: SHANDALAR 0x005b8138
GLOBAL_STATE_EXTERN int global_saveload_loading;

// GLOBAL: MAGIC 0x006abe38
// GLOBAL: SHANDALAR 0x005b8140
GLOBAL_STATE_EXTERN int g_save_file_fd;

// GLOBAL: MAGIC 0x006abe40
// GLOBAL: SHANDALAR 0x005b8148
GLOBAL_STATE_EXTERN unsigned char g_savegame_data_buffer[0x4ae64];

// GLOBAL: MAGIC 0x006f6cac
// GLOBAL: SHANDALAR 0x00602fb4
GLOBAL_STATE_EXTERN int g_save_errno;

// GLOBAL: MAGIC 0x0093f4b0
// GLOBAL: SHANDALAR 0x0097f1b0
GLOBAL_STATE_EXTERN int g_duel_state_0093f4b0;

// GLOBAL: MAGIC 0x0093a848
// GLOBAL: SHANDALAR 0x0094e968
GLOBAL_STATE_EXTERN int g_duel_state_0093a848;

// GLOBAL: MAGIC 0x0091b2c4
// GLOBAL: SHANDALAR 0x0092f414
GLOBAL_STATE_EXTERN int g_duel_state_0091b2c4;

// GLOBAL: MAGIC 0x00925d30
// GLOBAL: SHANDALAR 0x00939e5c
GLOBAL_STATE_EXTERN int g_duel_state_00925d30;

// GLOBAL: MAGIC 0x00925ac4
// GLOBAL: SHANDALAR 0x00939bf4
GLOBAL_STATE_EXTERN int g_duel_state_00925ac4;

// GLOBAL: MAGIC 0x0091c970
// GLOBAL: SHANDALAR 0x00930ab0
GLOBAL_STATE_EXTERN DuelInterfaceOptions g_duel_interface_options;

// GLOBAL: MAGIC 0x0091c9a8
// GLOBAL: SHANDALAR 0x00930ae8
GLOBAL_STATE_EXTERN int g_duel_options_state;

// GLOBAL: MAGIC 0x0091d254
// GLOBAL: SHANDALAR 0x00931384
GLOBAL_STATE_EXTERN int g_last_duel_enemy_primary_color;

// GLOBAL: MAGIC 0x0092636c
GLOBAL_STATE_EXTERN int g_sealed_deck_state;

// GLOBAL: MAGIC 0x00926340
// GLOBAL: SHANDALAR 0x0093a470
GLOBAL_STATE_EXTERN SealedDeckOptions g_sealed_deck_options;

// GLOBAL: MAGIC 0x00925bf8
// GLOBAL: SHANDALAR 0x00939d28
GLOBAL_STATE_EXTERN int g_duel_state_00925bf8;

// GLOBAL: MAGIC 0x0093d844
// GLOBAL: SHANDALAR 0x00951964
GLOBAL_STATE_EXTERN void *g_savegame_data_pointer;

// GLOBAL: MAGIC 0x0089652c
// GLOBAL: SHANDALAR 0x008aa72c
GLOBAL_STATE_EXTERN int g_ai_search_best_score;

// GLOBAL: MAGIC 0x008a8c38
// GLOBAL: SHANDALAR 0x008bce38
GLOBAL_STATE_EXTERN unsigned int g_card_types_in_play[2];

// GLOBAL: MAGIC 0x008a8d6c
// GLOBAL: SHANDALAR 0x008bcf6c
GLOBAL_STATE_EXTERN int g_produced_mana_card;

// GLOBAL: MAGIC 0x008a8ffc
// GLOBAL: SHANDALAR 0x008bd1fc
GLOBAL_STATE_EXTERN int g_starting_player_was_random;

// GLOBAL: MAGIC 0x008b2874
// GLOBAL: SHANDALAR 0x008c6a24
GLOBAL_STATE_EXTERN int g_last_duel_player_primary_color;

// GLOBAL: MAGIC 0x008b3240
// GLOBAL: SHANDALAR 0x008c73f0
GLOBAL_STATE_EXTERN int g_opponent_deck_color_filter_by_color[12];

// GLOBAL: MAGIC 0x008b323c
// GLOBAL: SHANDALAR 0x008c73ec
GLOBAL_STATE_EXTERN int g_opponent_deck_color_filter_count;

// GLOBAL: MAGIC 0x008b32c0
// GLOBAL: SHANDALAR 0x008c746c
GLOBAL_STATE_EXTERN int g_last_duel_result_state;

// GLOBAL: MAGIC 0x008b33fc
// GLOBAL: SHANDALAR 0x008c759c
GLOBAL_STATE_EXTERN int g_duel_initialized;

// GLOBAL: MAGIC 0x008b3fc0
// GLOBAL: SHANDALAR 0x008c8140
GLOBAL_STATE_EXTERN unsigned char g_duel_state_block_008b3fc0[0x104];

// GLOBAL: MAGIC 0x008b42e8
// GLOBAL: SHANDALAR 0x008c8468
GLOBAL_STATE_EXTERN int g_creature_count_summary[2];

// GLOBAL: MAGIC 0x008b42e4
// GLOBAL: SHANDALAR 0x008c8464
GLOBAL_STATE_EXTERN int g_creature_count_total;

// GLOBAL: MAGIC 0x008b484c
// GLOBAL: SHANDALAR 0x008c89cc
GLOBAL_STATE_EXTERN int g_first_turn_draw_pending;

// GLOBAL: MAGIC 0x008cd930
// GLOBAL: SHANDALAR 0x008e1ab0
GLOBAL_STATE_EXTERN GauntletOptions g_gauntlet_options;

// GLOBAL: MAGIC 0x008ce4f0
// GLOBAL: SHANDALAR 0x008e2670
GLOBAL_STATE_EXTERN int g_current_spell_state;

// GLOBAL: MAGIC 0x008ce4f4
// GLOBAL: SHANDALAR 0x008e2674
GLOBAL_STATE_EXTERN int g_current_spell_card;

// GLOBAL: MAGIC 0x008ce4fc
// GLOBAL: SHANDALAR 0x008e267c
GLOBAL_STATE_EXTERN int g_current_spell_internal_card_id;

// GLOBAL: MAGIC 0x008ce504
// GLOBAL: SHANDALAR 0x008e2684
GLOBAL_STATE_EXTERN int g_has_expansion_10;

// GLOBAL: MAGIC 0x008ce508
// GLOBAL: SHANDALAR 0x008e2688
GLOBAL_STATE_EXTERN int g_current_spell_player;

// GLOBAL: MAGIC 0x008ce570
// GLOBAL: SHANDALAR 0x008e26c0
GLOBAL_STATE_EXTERN unsigned char g_duel_state_block_008ce570[0x104];

// GLOBAL: MAGIC 0x008cec7c
// GLOBAL: SHANDALAR 0x008e2dcc
GLOBAL_STATE_EXTERN int g_ante_card_count;

// GLOBAL: MAGIC 0x008cee6c
// GLOBAL: SHANDALAR 0x008e2fbc
GLOBAL_STATE_EXTERN int g_duel_state_008cee6c;

// GLOBAL: MAGIC 0x008cefc0
// GLOBAL: SHANDALAR 0x008e3110
GLOBAL_STATE_EXTERN SoloDuelOptions g_solo_duel_options;

// GLOBAL: MAGIC 0x008cf690
// GLOBAL: SHANDALAR 0x008e37e0
GLOBAL_STATE_EXTERN int g_creature_toughness_by_color[2][8];

// GLOBAL: MAGIC 0x008cf6d0
// GLOBAL: SHANDALAR 0x008e3820
GLOBAL_STATE_EXTERN int g_opponent_starting_card_id_1;

// GLOBAL: MAGIC 0x008cf6e0
// GLOBAL: SHANDALAR 0x008e3830
GLOBAL_STATE_EXTERN unsigned char g_opponent_deck_cards[0x640];

// GLOBAL: MAGIC 0x00925450
// GLOBAL: SHANDALAR 0x00939580
GLOBAL_STATE_EXTERN unsigned char g_player_deck_cards[0x640];

// GLOBAL: MAGIC 0x0070a840
// GLOBAL: SHANDALAR 0x007817f0
GLOBAL_STATE_EXTERN JournalEntry g_journal_entries[2000];

// GLOBAL: MAGIC 0x00712540
// GLOBAL: SHANDALAR 0x007894f0
GLOBAL_STATE_EXTERN int g_current_quest_color;

// GLOBAL: MAGIC 0x00712550
// GLOBAL: SHANDALAR 0x00789500
GLOBAL_STATE_EXTERN char g_duel_victory_log[1000];

// GLOBAL: MAGIC 0x0071293c
// GLOBAL: SHANDALAR 0x00789904
GLOBAL_STATE_EXTERN int g_defeated_wizards_bitmap;

// GLOBAL: MAGIC 0x00712940
// GLOBAL: SHANDALAR 0x00789910
GLOBAL_STATE_EXTERN int g_amulet_inventory[5];

// GLOBAL: MAGIC 0x00712954
// GLOBAL: SHANDALAR 0x0078992c
GLOBAL_STATE_EXTERN int g_current_quest_deadline;

// GLOBAL: MAGIC 0x00712958
// GLOBAL: SHANDALAR 0x00789930
GLOBAL_STATE_EXTERN int g_journal_entry_count;

// GLOBAL: MAGIC 0x00712960
// GLOBAL: SHANDALAR 0x00789940
GLOBAL_STATE_EXTERN WorldNode g_town_slots[128];

// GLOBAL: MAGIC 0x00715b60
// GLOBAL: SHANDALAR 0x0078cb40
GLOBAL_STATE_EXTERN ShandalarEncounterSlot g_castle_dungeon_slots[15];

// GLOBAL: MAGIC 0x00715e30
// GLOBAL: SHANDALAR 0x0078ce10
GLOBAL_STATE_EXTERN int g_current_quest_type;

// GLOBAL: MAGIC 0x00715e40
// GLOBAL: SHANDALAR 0x0078ce20
GLOBAL_STATE_EXTERN ShandalarMonsterSlot g_lair_or_monster_slots[8];

// GLOBAL: MAGIC 0x00715f00
// GLOBAL: SHANDALAR 0x0078cee4
GLOBAL_STATE_EXTERN int g_world_magic_bitmap;

// GLOBAL: MAGIC 0x00715f04
// GLOBAL: SHANDALAR 0x0078cf00
GLOBAL_STATE_EXTERN int g_monster_timer;

// GLOBAL: MAGIC 0x00715f08
// GLOBAL: SHANDALAR 0x0078cf04
GLOBAL_STATE_EXTERN int g_shandalar_difficulty;

// GLOBAL: MAGIC 0x00715f0c
// GLOBAL: SHANDALAR 0x0078df30
GLOBAL_STATE_EXTERN int g_lairs_explored;

// GLOBAL: MAGIC 0x00715f10
// GLOBAL: SHANDALAR 0x0078df34
GLOBAL_STATE_EXTERN int g_skip_new_game_state_initialization;

// GLOBAL: MAGIC 0x00715f18
// GLOBAL: SHANDALAR 0x0078df6c
GLOBAL_STATE_EXTERN int g_current_quest_data;

// GLOBAL: MAGIC 0x00716024
// GLOBAL: SHANDALAR 0x0074c0e8
GLOBAL_STATE_EXTERN int g_player_starting_life;

// GLOBAL: MAGIC 0x007161c8
// GLOBAL: SHANDALAR 0x0074c940
GLOBAL_STATE_EXTERN int g_duel_state_007161c8;

// GLOBAL: MAGIC 0x007161cc
// GLOBAL: SHANDALAR 0x0074c95c
GLOBAL_STATE_EXTERN int g_duel_state_007161cc;

// GLOBAL: MAGIC 0x00716238
// GLOBAL: SHANDALAR 0x0074cfe4
GLOBAL_STATE_EXTERN int g_selected_save_slot_index;

// GLOBAL: MAGIC 0x0071623c
// GLOBAL: SHANDALAR 0x0074cfe8
GLOBAL_STATE_EXTERN int g_next_duel_life_delta;

// GLOBAL: MAGIC 0x00716244
// GLOBAL: SHANDALAR 0x0074d26c
GLOBAL_STATE_EXTERN int g_stop_phase_player;

// GLOBAL: MAGIC 0x00716248
// GLOBAL: SHANDALAR 0x0074f1e0
GLOBAL_STATE_EXTERN phase_t g_stop_phase;

// GLOBAL: MAGIC 0x00742fc0
// GLOBAL: SHANDALAR 0x0077e588
GLOBAL_STATE_EXTERN int g_duel_state_00742fc0;

// GLOBAL: MAGIC 0x00742fd0
// GLOBAL: SHANDALAR 0x0077e6d0
GLOBAL_STATE_EXTERN int g_encounter_opening_hand_size_modifier;

// GLOBAL: MAGIC 0x0074302c
// GLOBAL: SHANDALAR 0x0077f438
GLOBAL_STATE_EXTERN int g_one_deck_mode;

// GLOBAL: MAGIC 0x00748768
// GLOBAL: SHANDALAR 0x0073ea20
GLOBAL_STATE_EXTERN int g_quest_restock_timer;

#if defined(GLOBAL_STATE_IMPL) && defined(SHANDALAR)
// GLOBAL: MAGIC 0x007497a8
// GLOBAL: SHANDALAR 0x005911f8
GLOBAL_STATE_EXTERN int g_world_player_x = 1296;
#else
GLOBAL_STATE_EXTERN int g_world_player_x;
#endif

#if defined(GLOBAL_STATE_IMPL) && defined(SHANDALAR)
// GLOBAL: MAGIC 0x007497ac
// GLOBAL: SHANDALAR 0x005911fc
GLOBAL_STATE_EXTERN int g_world_player_y = 816;
#else
GLOBAL_STATE_EXTERN int g_world_player_y;
#endif

// GLOBAL: MAGIC 0x007497bc
// GLOBAL: SHANDALAR 0x0073ea64
GLOBAL_STATE_EXTERN int g_siege_indicator;

#if defined(GLOBAL_STATE_IMPL) && defined(SHANDALAR)
GLOBAL_STATE_EXTERN int g_siege_timer = 1;
#else
// GLOBAL: MAGIC 0x007497c0
// GLOBAL: SHANDALAR 0x0059120c
GLOBAL_STATE_EXTERN int g_siege_timer;
#endif

#if defined(GLOBAL_STATE_IMPL) && defined(SHANDALAR)
GLOBAL_STATE_EXTERN char g_name_entry_buffer[0x40] = "Ned Way the Ratiocinator";
#else
// GLOBAL: MAGIC 0x007497d0
// GLOBAL: SHANDALAR 0x00591228
GLOBAL_STATE_EXTERN char g_name_entry_buffer[0x40];
#endif

// GLOBAL: MAGIC 0x0074a22c
// GLOBAL: SHANDALAR 0x0073ea88
GLOBAL_STATE_EXTERN int g_starting_color;

// GLOBAL: MAGIC 0x0077784c
// GLOBAL: SHANDALAR 0x0078e5d8
GLOBAL_STATE_EXTERN int g_duel_state_0077784c;

// GLOBAL: MAGIC 0x00777c08
// GLOBAL: SHANDALAR 0x0078e998
GLOBAL_STATE_EXTERN int g_duel_state_00777c08;

// GLOBAL: MAGIC 0x00777c10
// GLOBAL: SHANDALAR 0x0078e9a0
GLOBAL_STATE_EXTERN int g_duel_state_00777c10;

// GLOBAL: MAGIC 0x00789498
// GLOBAL: SHANDALAR 0x007a0228
GLOBAL_STATE_EXTERN int g_player_duel_state_00789498[2];

// GLOBAL: MAGIC 0x00789104
// GLOBAL: SHANDALAR 0x0079fe94
GLOBAL_STATE_EXTERN int g_duel_state_00789104;

// GLOBAL: MAGIC 0x007899e0
// GLOBAL: SHANDALAR 0x007a0770
GLOBAL_STATE_EXTERN char g_player_name[0x32];

// GLOBAL: MAGIC 0x00789a12
// GLOBAL: SHANDALAR 0x007a07a2
GLOBAL_STATE_EXTERN char g_player_name_terminator;

// GLOBAL: MAGIC 0x007a7d10
// GLOBAL: SHANDALAR 0x007beaa0
GLOBAL_STATE_EXTERN int g_ai_combat_value_weights[4];

// GLOBAL: MAGIC 0x007a7d68
// GLOBAL: SHANDALAR 0x007beaf8
GLOBAL_STATE_EXTERN int g_save_format_version;

// GLOBAL: MAGIC 0x007aadf0
// GLOBAL: SHANDALAR 0x007beff0
GLOBAL_STATE_EXTERN int g_produced_mana_color_valid;

// GLOBAL: MAGIC 0x007abce0
// GLOBAL: SHANDALAR 0x007bfee0
GLOBAL_STATE_EXTERN int g_poison_counters[2];

// GLOBAL: MAGIC 0x00938e38
// GLOBAL: SHANDALAR 0x0094cf68
GLOBAL_STATE_EXTERN int g_player_duel_state_00938e38[2];

// GLOBAL: MAGIC 0x00939504
// GLOBAL: SHANDALAR 0x0094d634
GLOBAL_STATE_EXTERN int g_player_is_male;

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_save_path_needs_init = -1;

GLOBAL_STATE_EXTERN int g_savegame_version = 200;
#else
// GLOBAL: MAGIC 0x0057b174
// GLOBAL: SHANDALAR 0x0058c13c
GLOBAL_STATE_EXTERN int g_save_path_needs_init;

// GLOBAL: MAGIC 0x0057b178
// GLOBAL: SHANDALAR 0x0058c140
GLOBAL_STATE_EXTERN int g_savegame_version;
#endif

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_food = 50;
#else
// GLOBAL: MAGIC 0x0057d9e0
// GLOBAL: SHANDALAR 0x005863a8
GLOBAL_STATE_EXTERN int g_food;
#endif

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_current_quest_destination = -1;
#else
// GLOBAL: MAGIC 0x0057d9e8
// GLOBAL: SHANDALAR 0x005863b0
GLOBAL_STATE_EXTERN int g_current_quest_destination;
#endif
#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_next_duel_card_id = -1;
#else
// GLOBAL: MAGIC 0x0057d9ec
// GLOBAL: SHANDALAR 0x005863b4
GLOBAL_STATE_EXTERN int g_next_duel_card_id;
#endif

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int g_deck_color_bitmap = 1;
#else
// GLOBAL: MAGIC 0x0057a758
// GLOBAL: SHANDALAR 0x00591208
GLOBAL_STATE_EXTERN int g_deck_color_bitmap;
#endif

typedef struct duel_summary_struct
{
  int life_totals[2];
  int hand_counts[2];
  int creature_counts[2];
  int artifact_counts[2];
  int enchantment_counts[2];
  int unknown_28[2];
  int player_damage_totals[2];
  int cards_drawn;
  int creatures_died;
  int land_entries;
  int unknown_44[7];
} duel_summary_t;
STATIC_ASSERT(sizeof(duel_summary_t) == 0x60, duel_summary_t_wrong_size);

// GLOBAL: MAGIC 0x008cfd70
// GLOBAL: SHANDALAR 0x008e3ec0
GLOBAL_STATE_EXTERN duel_summary_t g_duel_summary;

// GLOBAL: MAGIC 0x0094f790
// GLOBAL: SHANDALAR 0x0097f1ac
GLOBAL_STATE_EXTERN int _PlayerFace;

// GLOBAL: MAGIC 0x0094efb4
// GLOBAL: SHANDALAR 0x0097f1a4
GLOBAL_STATE_EXTERN int _OpponFace;

// GLOBAL: MAGIC 0x0094efc0
// GLOBAL: SHANDALAR 0x0097f1c0
GLOBAL_STATE_EXTERN int deck[500];

// GLOBAL: MAGIC 0x00748764
// GLOBAL: SHANDALAR 0x007490e8
GLOBAL_STATE_EXTERN int _currentDeck;

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN int Gold = 100;
#else
// GLOBAL: MAGIC 0x0057d9dc
// GLOBAL: SHANDALAR 0x005863a4
GLOBAL_STATE_EXTERN int Gold;
#endif

#ifdef GLOBAL_STATE_IMPL
GLOBAL_STATE_EXTERN shandalar_worldmagic_t Scards[12] = {
    {0x01af, 800, 0, 0},
    {0x0072, 1000, 0, 0},
    {0x003e, 700, 0, 0},
    {0x0018, 200, 0, 0},
    {0x0085, 800, 0, 0},
    {0x0019, 1500, 0, 0},
    {0x0079, 400, 0, 0},
    {0x011d, 500, 0, 0},
    {0x0091, 600, 0, 0},
    {0x001a, 300, 0, 0},
    {0x00cd, 600, 0, 0},
    {0x001c, 1200, 0, 0},
};
#else

// GLOBAL: MAGIC 0x0057d9f8
// GLOBAL: SHANDALAR 0x005863c0
GLOBAL_STATE_EXTERN shandalar_worldmagic_t Scards[12];
#endif

unsigned int load_gametype0(char *path);
int load_selected_duel_save_slot(int player);
unsigned int load_or_probe_duel_save_slot(char *path, int probe_only);
unsigned int load_duel_run_mode_1_save(char *path);
unsigned int load_duel_run_mode_2_save(char *path);
unsigned int load_duel_run_mode_3_save(char *path);
void save_duel_interface_options_to_registry(void);

#ifdef GLOBAL_STATE_IMPL
// GLOBAL: MAGIC 0x0057b070
// GLOBAL: SHANDALAR 0x0058c038
GLOBAL_STATE_EXTERN char g_save_file_path[13] = "D:MAGIC0.SVE";
#else
GLOBAL_STATE_EXTERN char g_save_file_path[13];
#endif

#undef GLOBAL_STATE_EXTERN

#endif
