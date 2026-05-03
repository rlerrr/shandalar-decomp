#ifndef __GLOBAL_STATE_H__
#define __GLOBAL_STATE_H__

#include "defs.h"

// Single header library style
#ifdef GLOBAL_STATE_IMPL

#define GLOBAL_STATE_INIT(value) = value
#else
#define extern 
#define GLOBAL_STATE_INIT(value)
#endif

// See manalink.lds and ai.c for more some of these addresses and types
// A lot of these can potentially be correlated through save_or_load_ver1 and save_or_load_ver2

// GLOBAL: MOK 0x4ef540
// GLOBAL: MAGIC 0x8b60f0
// GLOBAL: SHANDALAR 0x8ca270
extern card_instance_t global_card_instances[2][151];

// GLOBAL: MAGIC 0x00789110
extern char unk_00789110[24];

// GLOBAL: MOK 0x543c10
// GLOBAL: MAGIC 0x00791560
extern card_instance_t global_displayed_card_instances[2][151];

#define PLAYER_CARD_INSTANCE(player_, card_) global_card_instances[player_][card_]

// GLOBAL: MAGIC 0x939810
// GLOBAL: SHANDALAR 0x94d930
extern int global_graveyard_slots[2][500];

// TODO: is this actually int???
// GLOBAL: MAGIC 0x8cecc0
extern char global_ante_cards[2][16];

typedef struct
{
  int csvid;
  int numcards;
} csvid_and_numcards;

typedef struct
{
  char *name_at_0;
  int unk_4;
  char *name_at_8;
  int unk_c;
  int unk_10;
} name_table_entry_t;

// GLOBAL: MAGIC 0x0056fb20
extern csvid_and_numcards initial_library[2][200];

// GLOBAL: MAGIC 0x008cc980
extern int global_exile[2][500];

// GLOBAL: MAGIC 0x00895210
extern int global_library[2][500];

// GLOBAL: MOK 0x507ec0
// GLOBAL: MAGIC 0x0093a990
extern int TENTATIVE_timestamp_player[500];

// GLOBAL: MOK 0x5076f0
// GLOBAL: MAGIC 0x8b5910
extern int TENTATIVE_timestamp_card[500];

// GLOBAL: MOK 0x5096b0
// GLOBAL: MAGIC 0x9251e0
extern target_t global_stack_cards[32];

// GLOBAL: MOK 0x5097b0
// GLOBAL: MAGIC 0x926810
extern target_t global_stack_damage_targets[32];

// GLOBAL: MAGIC 0x0078944c
extern int unk_0078944c;

// GLOBAL: MAGIC 0x00789450
extern int event_result;

// GLOBAL: MAGIC 0x007894a0
extern int unk_007894a0;

// GLOBAL: MAGIC 0x00789314
extern int unk_00789314;

// GLOBAL: MAGIC 0x008b426c
extern int spell_fizzled;

// GLOBAL: MAGIC 0x00777be0
extern int active_cards_count[2];

// GLOBAL: MAGIC 0x00939048
extern trigger_t trigger_condition;

// GLOBAL: MAGIC 0x00938e2c
extern int unk_00938e2c;

// GLOBAL: MAGIC 0x00939338
extern int current_phase;

// GLOBAL: MAGIC 0x0093933c
extern int unk_0093933c;

// GLOBAL: MAGIC 0x00939340
extern int unk_00939340;

// GLOBAL: MAGIC 0x00939348
extern int unk_00939348;

// GLOBAL: MAGIC 0x00939180
extern int unk_00939180[32];

// GLOBAL: MAGIC 0x0093b280
extern int unk_0093b280[2][151][2][4];

// GLOBAL: MAGIC 0x00895030
extern int unk_00895030[32];

// GLOBAL: MAGIC 0x007a7750
extern int unk_007a7750[32];

// GLOBAL: MAGIC 0x00777e64
extern name_table_entry_t unk_00777e64[866];

// GLOBAL: MAGIC 0x008b497c
extern int current_turn;

// GLOBAL: MAGIC 0x008b32b8
extern int active_player;

// GLOBAL: MAGIC 0x008b2934
extern int unk_008b2934;
// GLOBAL: MAGIC 0x008b3104
extern int unk_008b3104;

// GLOBAL: MAGIC 0x00742f80
extern int unk_00742f80[2][8];

// GLOBAL: MAGIC 0x00742fc4
extern int unk_00742fc4;

// GLOBAL: MAGIC 0x007161e0
extern int unk_007161e0[2][11];
// GLOBAL: MAGIC 0x007161d8
extern int unk_007161d8;

// GLOBAL: MAGIC 0x00716030
extern int raw_mana_available_hex[2][51];

// GLOBAL: MAGIC 0x0072c440
extern int unk_0072c440[8];

// GLOBAL: MAGIC 0x00742fe0
extern int basiclandtypes_controlled[2][8];

// GLOBAL: MAGIC 0x00743040
extern int raw_mana_available[2][8];

// GLOBAL: MAGIC 0x0074305c
extern int total_raw_mana_available[2];

// GLOBAL: MAGIC 0x00715fb0
extern int unk_00715fb0;

// GLOBAL: MAGIC 0x00715fa8
extern int unk_00715fa8;

// GLOBAL: MAGIC 0x00791554
extern int human_player;

// GLOBAL: MAGIC 0x008cfd78
extern int hand_count[2];

// GLOBAL: MAGIC 0x007ab2b8
extern int trigger_cause_controller;

// GLOBAL: MAGIC 0x008cee74
extern int trigger_cause;

// GLOBAL: MAGIC 0x008cfd88
extern int artifact_cards_in_play[2];

// GLOBAL: MAGIC 0x008cfd90
extern int enchantments_in_play[2];
// GLOBAL: MAGIC 0x008cfdac
extern int unk_008cfdac;
// GLOBAL: MAGIC 0x008cfdb0
extern int unk_008cfdb0;

// GLOBAL: MAGIC 0x00742f60
extern int unk_00742f60;

// GLOBAL: MAGIC 0x00743038
extern int unk_00743038;

// GLOBAL: MAGIC 0x007a7c1c
extern int unk_007a7c1c;

// GLOBAL: MAGIC 0x007a7d88
extern int unk_007a7d88[2];

// GLOBAL: MAGIC 0x007a7d80
extern int unk_007a7d80[2];

// GLOBAL: MAGIC 0x008b3270
extern int unk_008b3270;

// GLOBAL: MAGIC 0x008b35ec
extern int unk_008b35ec;

// GLOBAL: MAGIC 0x008b3bd4
extern int unk_008b3bd4;

// GLOBAL: MAGIC 0x008b4270
extern int life[2];

// GLOBAL: MAGIC 0x008b49c4
extern int unk_008b49c4;

// GLOBAL: MAGIC 0x008b4278
extern int unk_008b4278;

// GLOBAL: MAGIC 0x00743094
extern int unk_00743094;

// GLOBAL: MAGIC 0x00789734
extern int unk_00789734;

// GLOBAL: MAGIC 0x00789b80
extern int unk_00789b80;

// GLOBAL: MAGIC 0x007abc78
extern int unk_007abc78;

// GLOBAL: MAGIC 0x007a7d64
extern int unk_007a7d64;

// GLOBAL: MAGIC 0x007a7d08
extern char unk_007a7d08;

// GLOBAL: MAGIC 0x007a7d0c
extern int unk_007a7d0c;

extern int unk_00896534;
// GLOBAL: MAGIC 0x008a8de8
extern int unk_008a8de8;
// GLOBAL: MAGIC 0x008a8df0
extern int unk_008a8df0;

// GLOBAL: MAGIC 0x008a8c38
extern unsigned int card_types_in_play[2];

// GLOBAL: MAGIC 0x008a8c20
extern int unk_008a8c20;
// GLOBAL: MAGIC 0x008a8c34
extern int unk_008a8c34;

// GLOBAL: MAGIC 0x008ce4f4
extern int unk_008ce4f4 GLOBAL_STATE_INIT(-1);

// GLOBAL: MAGIC 0x008ce508
extern int unk_008ce508 GLOBAL_STATE_INIT(-1);

// GLOBAL: MAGIC 0x008ce510
extern int unk_008ce510;

// GLOBAL: MAGIC 0x008ce518
extern int unk_008ce518;

// GLOBAL: MAGIC 0x0091a80c
extern int unk_0091a80c;

// GLOBAL: MAGIC 0x0091bfb4
extern int unk_0091bfb4;

// GLOBAL: MAGIC 0x00925d3c
extern int unk_00925d3c;
// GLOBAL: MAGIC 0x00925d38
extern int unk_00925d38;

// GLOBAL: MAGIC 0x009252e0
extern int unk_009252e0;

// GLOBAL: MAGIC 0x008b60e0
extern int unk_008b60e0;

// GLOBAL: MAGIC 0x0079140c
extern int unk_0079140c;

// GLOBAL: MAGIC 0x007abc7c
extern int unk_007abc7c;

// GLOBAL: MAGIC 0x0092664c
extern int unk_0092664c[8];

// GLOBAL: MAGIC 0x0092666c
extern int unk_0092666c;

// GLOBAL: MAGIC 0x00926664
extern int produced_mana_color;
extern int unk_009266a4;
extern int unk_009266ac;

// GLOBAL: MAGIC 0x00926750
extern char unk_00926750[300];

// GLOBAL: MAGIC 0x00926930
extern char unk_00926930[212][300];

// GLOBAL: MAGIC 0x0093d848
extern int unk_0093d848;

// GLOBAL: MAGIC 0x008cc840
extern int unk_008cc840;

// GLOBAL: MAGIC 0x0093f4b8
extern int unk_0093f4b8;

// GLOBAL: MAGIC 0x0055e0cc
extern int unk_0055e0cc;

// GLOBAL: MAGIC 0x00572920
extern char unk_00572920[16];

// GLOBAL: MAGIC 0x00572930
extern char unk_00572930[16];

// GLOBAL: MAGIC 0x00950810
extern int (__stdcall *IAT_SelectPalette)(int hdc, int hpalette, int force_background);

// GLOBAL: SHANDALAR 0x008a8df8
extern int LEGACY_EFFECT_PUMP;

// GLOBAL: MAGIC 0x008b3d10
extern int unk_008b3d10;

// GLOBAL: MAGIC 0x008b28f8
extern int unk_008b28f8;

// GLOBAL: MAGIC 0x0091ca90
extern char unk_0091ca90;

// GLOBAL: MAGIC 0x0091ca92
extern short unk_0091ca92;

// GLOBAL: MAGIC 0x0091ca94
extern short unk_0091ca94;

// GLOBAL: MAGIC 0x0091ca96
extern short unk_0091ca96;

// GLOBAL: MAGIC 0x0091ca98
extern short *unk_0091ca98;

// GLOBAL: MAGIC 0x0091d07c
extern int unk_0091d07c;

// GLOBAL: MAGIC 0x00925bb8
extern int unk_00925bb8;

// GLOBAL: MAGIC 0x00925030
extern int unk_00925030;

// GLOBAL: MAGIC 0x008b40e0
extern int unk_008b40e0;

// GLOBAL: MAGIC 0x008b40f4
extern int x_value;

// GLOBAL: MAGIC 0x008b44d0
extern int unk_008b44d0[2];

// GLOBAL: MAGIC 0x008ce568
extern int max_x_value;

// GLOBAL: MAGIC 0x008cf1ac
extern int unk_008cf1ac;

// GLOBAL: MAGIC 0x008cf1b4
extern int unk_008cf1b4;

// GLOBAL: MAGIC 0x008cf1bc
extern int unk_008cf1bc;

// GLOBAL: MAGIC 0x008cf1c0
extern int unk_008cf1c0[2][8];

// GLOBAL: MAGIC 0x008cf1d4
extern int landsofcolor_controlled[2][8];

// GLOBAL: MAGIC 0x00939520
extern int unk_00939520[2][8];

// GLOBAL: MAGIC 0x00939530
extern int unk_00939530[2][8];

// GLOBAL: MAGIC 0x0093950c
extern int ai_modifier;

// GLOBAL: MAGIC 0x00896720
// GLOBAL: SHANDALAR 0x008aa920
extern char text_lines[249][300];

// GLOBAL: MAGIC 0x008b4330
extern char unk_008b4330[300];

// GLOBAL: MAGIC 0x008cfd30
extern char unk_008cfd30[300];

// GLOBAL: MAGIC 0x008cf040
extern char unk_008cf040[300];

// GLOBAL: MAGIC 0x0093dabc
// GLOBAL: SHANDALAR 0x008cd928
extern int affected_card;

// GLOBAL: MAGIC 0x008cd928
extern int affected_card_controller;
// GLOBAL: MAGIC 0x0093a808
extern int attacking_card_controller;

// GLOBAL: MAGIC 0x0056e49c
extern char unk_0056e49c[64];

// GLOBAL: MAGIC 0x00573364
extern char unk_00573364[12];

// GLOBAL: MAGIC 0x00573370
extern char unk_00573370[12];

// GLOBAL: MAGIC 0x00573380
extern char unk_00573380[12];

// GLOBAL: MAGIC 0x005724ac
extern char unk_005724ac[300];

// GLOBAL: MAGIC 0x0057a75c
extern int unk_0057a75c;

// GLOBAL: MAGIC 0x0057aae8
extern int unk_0057aae8;

// GLOBAL: MAGIC 0x0057d9f0
extern int unk_0057d9f0;

// GLOBAL: MAGIC 0x0057d9f4
extern int unk_0057d9f4;

// GLOBAL: MAGIC 0x0057f758
extern char unk_0057f758[4];

// GLOBAL: MAGIC 0x0057f75c
extern char unk_0057f75c[4];

// GLOBAL: MAGIC 0x00637670
extern char unk_00637670[300];

// GLOBAL: MAGIC 0x006a1db8
extern int unk_006a1db8[256];

// GLOBAL: MAGIC 0x006a21b8
extern int unk_006a21b8[256];

// GLOBAL: MAGIC 0x006a5f18
extern int unk_006a5f18[256];

// GLOBAL: MAGIC 0x006a6ae8
extern int unk_006a6ae8[256];

// GLOBAL: MAGIC 0x006a6ee8
extern int unk_006a6ee8[256];

// GLOBAL: MAGIC 0x006a8258
extern int unk_006a8258[256];

// GLOBAL: MAGIC 0x006a8660
extern int unk_006a8660[256];

// GLOBAL: MAGIC 0x00748770
extern char unk_00748770[3000];

// GLOBAL: MAGIC 0x0074309c
extern int unk_0074309c;

// GLOBAL: MAGIC 0x007462a0
extern int unk_007462a0;

// GLOBAL: MAGIC 0x007463dc
extern int unk_007463dc;

// GLOBAL: MAGIC 0x007497b0
extern int unk_007497b0;

// GLOBAL: MAGIC 0x008ce530
extern int attacking_card;
// GLOBAL: MAGIC 0x0093b160
extern int card_on_stack_controller;
// GLOBAL: MAGIC 0x0091ce2c
extern int card_on_stack;
// GLOBAL: MAGIC 0x008a9000
extern int unk_008a9000;
// GLOBAL: MAGIC 0x008a9194
extern int unk_008a9194;
// GLOBAL: MAGIC 0x00926804
extern int unk_00926804;
// GLOBAL: MAGIC 0x00939330
extern int unk_00939330;
// GLOBAL: MAGIC 0x007a7c58
extern int unk_007a7c58[2];
extern int unk_00926664;
// GLOBAL: MAGIC 0x009266d0
extern char unk_009266d0[260];

// GLOBAL: MAGIC 0x007a79b0
extern int unk_007a79b0[2];

// GLOBAL: MAGIC 0x00789108
extern int unk_00789108;

// GLOBAL: MAGIC 0x008cefb8
extern int unk_008cefb8;

// GLOBAL: MAGIC 0x008d0340
extern int unk_008d0340;

// GLOBAL: MAGIC 0x0091c4fc
extern int unk_0091c4fc;

#undef extern
#undef GLOBAL_STATE_INIT
#endif
