#ifndef SHANDALAR_GLOBAL_STRINGS_H
#define SHANDALAR_GLOBAL_STRINGS_H

#include "defs.h"

/* Single header library style */
#ifdef SHANDALAR_GLOBAL_STRINGS_IMPL
#define SHANDALAR_GLOBAL_STRINGS_EXTERN
#define SHANDALAR_GLOBAL_STRINGS_INIT(value) = value
#else
#define SHANDALAR_GLOBAL_STRINGS_EXTERN extern
#define SHANDALAR_GLOBAL_STRINGS_INIT(value)
#endif

typedef char sh_text_0x19_t[0x19];
typedef char sh_text_0x32_t[0x32];
typedef char sh_text_0x50_t[0x50];

/*
 * Runtime text globals loaded by FUN_00564ee7() (mostly from AdvStrings.txt),
 * plus block pointers loaded by FUN_00565dbc() (from AdvBlocks.txt).
 */

// GLOBAL: SHANDALAR 0x008bd200
SHANDALAR_GLOBAL_STRINGS_EXTERN int gs_skip_advstring_load_008bd200 SHANDALAR_GLOBAL_STRINGS_INIT(0);

// GLOBAL: SHANDALAR 0x00593934
SHANDALAR_GLOBAL_STRINGS_EXTERN unsigned int gs_creature_name_count_00593934 SHANDALAR_GLOBAL_STRINGS_INIT(0x39);

/* [CREATURENAMES] packed record at 0x00591a08, stride 0x8c */
typedef struct
{
  char article[0x0a];
  sh_text_0x32_t name;
  sh_text_0x50_t plural;
} sh_creature_name_entry_t;

// GLOBAL: SHANDALAR 0x00591a08
SHANDALAR_GLOBAL_STRINGS_EXTERN sh_creature_name_entry_t gs_creature_names_00591a08[0x39];

/* Pointer tables (arrays of char*) */
// GLOBAL: SHANDALAR 0x0077c5a0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_playernames_0077c5a0[0x0e];
// GLOBAL: SHANDALAR 0x0077d130
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_difficultylevels_0077d130[4];
// GLOBAL: SHANDALAR 0x00780820
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_dungeon_names_00780820[0x11];
// GLOBAL: SHANDALAR 0x0077c020
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_lair_names_0077c020[0x13];
// GLOBAL: SHANDALAR 0x0077e060
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_citynames_firsthalf_0077e060[0x10];
// GLOBAL: SHANDALAR 0x007653e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_citynames_secondhalf_007653e0[0x10];
// GLOBAL: SHANDALAR 0x00780660
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_worldmagic_names_00780660[0x0c];
// GLOBAL: SHANDALAR 0x0074b8f0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_worldmagic_explains_0074b8f0[0x0c];
// GLOBAL: SHANDALAR 0x0077c9a0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_logstrings_0077c9a0[0x10];
// GLOBAL: SHANDALAR 0x0077cf70
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_cardclassnames_0077cf70[9];
// GLOBAL: SHANDALAR 0x0077e1f0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_cardclassnames_plural_0077e1f0[9];
// GLOBAL: SHANDALAR 0x0077efa0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_cave_showclues_0077efa0[0x17];
// GLOBAL: SHANDALAR 0x0077f0d0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_encounter_preduel_0077f0d0[0x30];
// GLOBAL: SHANDALAR 0x0077f050
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_encounter_postduel_0077f050[0x20];
// GLOBAL: SHANDALAR 0x0077c4f0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_visit_0077c4f0[0x23];
// GLOBAL: SHANDALAR 0x0077f1d0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_visit_citybuy_0077f1d0[4];
// GLOBAL: SHANDALAR 0x0074b8c0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_castlewin_0074b8c0[0x0b];
// GLOBAL: SHANDALAR 0x0077f000
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_dungeon_0077f000[0x14];
// GLOBAL: SHANDALAR 0x0077e580
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_hinttext_0077e580[2];
// GLOBAL: SHANDALAR 0x0077c580
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_questfailed_0077c580[6];
// GLOBAL: SHANDALAR 0x0074cff0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_monsterlair_0074cff0[6];
// GLOBAL: SHANDALAR 0x0074ccd0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_buyanycard_0074ccd0[6];
// GLOBAL: SHANDALAR 0x0077e0a0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_queststatus_0077e0a0[0x1a];
// GLOBAL: SHANDALAR 0x0077d140
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_newsflash_0077d140[0x0b];
// GLOBAL: SHANDALAR 0x0074ccf0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_citycardtext_0074ccf0[0x31];
// GLOBAL: SHANDALAR 0x0074da20
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_browse_0074da20[0x11];
// GLOBAL: SHANDALAR 0x0074b920
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_showdeck_0074b920[3];
// GLOBAL: SHANDALAR 0x0077d1b0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_loadsave_0077d1b0[3];
// GLOBAL: SHANDALAR 0x0077cfa0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_stats_0077cfa0[9];
// GLOBAL: SHANDALAR 0x0074b870
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_analyze_0074b870[0x11];
// GLOBAL: SHANDALAR 0x0077cf20
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_riddle_0077cf20[0x11];
// GLOBAL: SHANDALAR 0x0077e180
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_lair_0077e180[0x13];
// GLOBAL: SHANDALAR 0x0074d840
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_wiseman_0074d840[0x13];
// GLOBAL: SHANDALAR 0x0074d800
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_citywiseman_0074d800[0x0d];
// GLOBAL: SHANDALAR 0x0077f5e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_cityscreen_buttons_0077f5e0[8];

/* Backing buffers (char storage) */
// GLOBAL: SHANDALAR 0x0077e2c0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_playernames_buf_0077e2c0[0x2bc];
// GLOBAL: SHANDALAR 0x0074d790
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_difficultylevels_buf_0074d790[0x64];
// GLOBAL: SHANDALAR 0x0074c970
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_dungeon_names_buf_0074c970[0x352];
// GLOBAL: SHANDALAR 0x0074bd30
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_lair_names_buf_0074bd30[0x3b6];
// GLOBAL: SHANDALAR 0x0074c950
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cityname_format_left_0074c950[0x20];
// GLOBAL: SHANDALAR 0x0077f190
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cityname_format_right_0077f190[0x40];
// GLOBAL: SHANDALAR 0x0077cfd0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cityname_village_0077cfd0[0x70];
// GLOBAL: SHANDALAR 0x00765dc0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cityname_castle_00765dc0[0x80];
typedef struct
{
  char citynames_buf_0077d610[0x640];
  char browse_buf_0077dc50[0x1a9];
  char pad_0077ddf9[7];
  char cityname_manacastle_0077de00[0x260];
} sh_city_text_cluster_t;
// GLOBAL: SHANDALAR 0x0077d610
SHANDALAR_GLOBAL_STRINGS_EXTERN sh_city_text_cluster_t gs_city_text_cluster_0077d610;
// GLOBAL: SHANDALAR 0x0074b160
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_worldmagic_buf_0074b160[0x708];
// GLOBAL: SHANDALAR 0x0077e1d0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_worldmagic_title_0077e1d0[0x50];
// GLOBAL: SHANDALAR 0x0077c680
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_logstrings_buf_0077c680[0x320];
// GLOBAL: SHANDALAR 0x0077ee70
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_wizardnames_0077ee70[0x130];
// GLOBAL: SHANDALAR 0x00765d50
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_directions_00765d50[0x64];
// GLOBAL: SHANDALAR 0x0074c5c0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cardclassnames_buf_0074c5c0[0x1c2];
// GLOBAL: SHANDALAR 0x0077e6e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_spellname_primary_0077e6e0[0x20];
// GLOBAL: SHANDALAR 0x0077e220
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_spellnames_0077e220[0x0a0];
// GLOBAL: SHANDALAR 0x0074d980
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_mananames_0074d980[0x0a0];
// GLOBAL: SHANDALAR 0x007806f0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_landwalks_007806f0[0x130];
// GLOBAL: SHANDALAR 0x0077d090
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_amuletnames_0077d090[0x0a0];
// GLOBAL: SHANDALAR 0x0077edd0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_amuletnames_plural_0077edd0[0x0a0];
// GLOBAL: SHANDALAR 0x0077c5e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_colorcards_0077c5e0[0x0a0];
// GLOBAL: SHANDALAR 0x007658d0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cave_showclues_buf_007658d0[0x47e];
// GLOBAL: SHANDALAR 0x0074da70
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_encounter_buf_0074da70[0x1770];
// GLOBAL: SHANDALAR 0x0077f610
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_visit_buf_0077f610[0x0aa5];
// GLOBAL: SHANDALAR 0x0077e700
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_castlewin_buf_0077e700[0x672];
// GLOBAL: SHANDALAR 0x00780870
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_dungeon_buf_00780870[0x3e8];
// GLOBAL: SHANDALAR 0x0077e5a0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_hinttext_buf_0077e5a0[0x12c];
// GLOBAL: SHANDALAR 0x0074b930
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_questfailed_buf_0074b930[0x384];
// GLOBAL: SHANDALAR 0x0074d010
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_monsterlair_buf_0074d010[0x258];
// GLOBAL: SHANDALAR 0x0077f1e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_buyanycard_buf_0077f1e0[0x258];
// GLOBAL: SHANDALAR 0x0074d270
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_queststatus_buf_0074d270[0x514];
// GLOBAL: SHANDALAR 0x0077d1c0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_newsflash_buf_0077d1c0[0x44c];
// GLOBAL: SHANDALAR 0x0074c0f0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_citycardtext_buf_0074c0f0[0x4c9];
// GLOBAL: SHANDALAR 0x0074d890
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_showdeck_buf_0074d890[0x0e1];
// GLOBAL: SHANDALAR 0x0077d040
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_loadsave_buf_0077d040[0x4b];
// GLOBAL: SHANDALAR 0x0074ce20
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_stats_buf_0074ce20[0x1c2];
// GLOBAL: SHANDALAR 0x0074afb0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_analyze_buf_0074afb0[0x1a9];
// GLOBAL: SHANDALAR 0x0077ca20
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_riddle_buf_0077ca20[0x4fb];
// GLOBAL: SHANDALAR 0x007800c0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_lair_buf_007800c0[0x591];
// GLOBAL: SHANDALAR 0x00780c60
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_wiseman_buf_00780c60[0x0aa5];
// GLOBAL: SHANDALAR 0x0077f450
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_cityscreen_buttons_buf_0077f450[0x190];
// GLOBAL: SHANDALAR 0x0077e110
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_showcard_name_0077e110[0x32];
// GLOBAL: SHANDALAR 0x0077e142
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_showcard_type_0077e142[0x32];
// GLOBAL: SHANDALAR 0x0074bcc0
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_showlibrary_title_0074bcc0[0x32];
// GLOBAL: SHANDALAR 0x0074bcf2
SHANDALAR_GLOBAL_STRINGS_EXTERN char gs_showlibrary_empty_0074bcf2[0x32];

/* AdvBlocks pointers */
// GLOBAL: SHANDALAR 0x0074c930
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_advblock_city_0074c930[4];
// GLOBAL: SHANDALAR 0x0077c9e0
SHANDALAR_GLOBAL_STRINGS_EXTERN char *gs_advblock_general_0077c9e0[0x0c];

/* Legacy aliases used by decomp code */
#define DAT_008bd200 gs_skip_advstring_load_008bd200
#define DAT_00593934 gs_creature_name_count_00593934

#define DAT_00591a08 ((char *)gs_creature_names_00591a08 + 0x00)
#define DAT_00591a12 ((char *)gs_creature_names_00591a08 + 0x0a)
#define DAT_00591a44 ((char *)gs_creature_names_00591a08 + 0x3c)

#define DAT_0077e2c0 gs_playernames_buf_0077e2c0
#define DAT_0074d790 gs_difficultylevels_buf_0074d790
#define DAT_0074c970 gs_dungeon_names_buf_0074c970
#define DAT_0074bd30 gs_lair_names_buf_0074bd30
#define DAT_0074c950 gs_cityname_format_left_0074c950
#define DAT_0077f190 gs_cityname_format_right_0077f190
#define DAT_0077cfd0 gs_cityname_village_0077cfd0
#define DAT_00765dc0 gs_cityname_castle_00765dc0
#define DAT_0077de00 gs_city_text_cluster_0077d610.cityname_manacastle_0077de00
#define DAT_0077d610 gs_city_text_cluster_0077d610.citynames_buf_0077d610
#define DAT_0074b160 gs_worldmagic_buf_0074b160
#define DAT_0077e1d0 gs_worldmagic_title_0077e1d0
#define DAT_0077c680 gs_logstrings_buf_0077c680
#define DAT_0077ee70 gs_wizardnames_0077ee70
#define DAT_00765d50 gs_directions_00765d50
#define DAT_0074c5c0 gs_cardclassnames_buf_0074c5c0
#define DAT_0077e6e0 gs_spellname_primary_0077e6e0
#define DAT_0077e220 gs_spellnames_0077e220
#define DAT_0074d980 gs_mananames_0074d980
#define DAT_007806f0 gs_landwalks_007806f0
#define DAT_0077d090 gs_amuletnames_0077d090
#define DAT_0077edd0 gs_amuletnames_plural_0077edd0
#define DAT_0077c5e0 gs_colorcards_0077c5e0
#define DAT_007658d0 gs_cave_showclues_buf_007658d0
#define DAT_0074da70 gs_encounter_buf_0074da70
#define DAT_0077f610 gs_visit_buf_0077f610
#define DAT_0077e700 gs_castlewin_buf_0077e700
#define DAT_00780870 gs_dungeon_buf_00780870
#define DAT_0077e5a0 gs_hinttext_buf_0077e5a0
#define DAT_0074b930 gs_questfailed_buf_0074b930
#define DAT_0074d010 gs_monsterlair_buf_0074d010
#define DAT_0077f1e0 gs_buyanycard_buf_0077f1e0
#define DAT_0074d270 gs_queststatus_buf_0074d270
#define DAT_0077d1c0 gs_newsflash_buf_0077d1c0
#define DAT_0074c0f0 gs_citycardtext_buf_0074c0f0
#define DAT_0074d890 gs_showdeck_buf_0074d890
#define DAT_0077d040 gs_loadsave_buf_0077d040
#define DAT_0074ce20 gs_stats_buf_0074ce20
#define DAT_0074afb0 gs_analyze_buf_0074afb0
#define DAT_0077ca20 gs_riddle_buf_0077ca20
#define DAT_007800c0 gs_lair_buf_007800c0
#define DAT_00780c60 gs_wiseman_buf_00780c60
#define DAT_0077f450 gs_cityscreen_buttons_buf_0077f450
#define DAT_0077e110 gs_showcard_name_0077e110
#define DAT_0077e142 gs_showcard_type_0077e142
#define DAT_0074bcc0 gs_showlibrary_title_0074bcc0
#define DAT_0074bcf2 gs_showlibrary_empty_0074bcf2

#define DAT_0074c930 gs_advblock_city_0074c930
#define DAT_0077c9e0 gs_advblock_general_0077c9e0

#undef SHANDALAR_GLOBAL_STRINGS_EXTERN
#undef SHANDALAR_GLOBAL_STRINGS_INIT

#endif /* SHANDALAR_GLOBAL_STRINGS_H */
