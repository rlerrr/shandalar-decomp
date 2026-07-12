#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "deckdll/src/card_db.h"
#include "game_support.h"
#include "global_strings.h"
#include "duel_engine.h"

typedef ptrdiff_t INT_PTR;
typedef struct
{
  int bigcard_player;
  int bigcard_card;
  int smallcard_player;
  int smallcard_card;
  char *prompt;
  int dialog_mode;
} dialog_context_t;

typedef struct
{
  int player;
  int card;
  target_t targets[20];
  int number_of_targets;
} dialog_stack_entry_t;
STATIC_ASSERT(sizeof(dialog_stack_entry_t) == 0xac, dialog_stack_entry_t_wrong_size);

#define DISPLAYED_PLAYER_CARD_INSTANCE(player_, card_) global_displayed_card_instances[(player_)][(card_)]
#define DISPLAYED_PIC_INFO(player_, card_) (*(unsigned int *)&DISPLAYED_PLAYER_CARD_INSTANCE((player_), (card_)).display_pic_csv_id)

extern card_ptr_t global_raw_cards_storage[2000];
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern int DAT_0091c998;

// GLOBAL: MAGIC 0x006f6df8
int DAT_006f6df8[4];
// GLOBAL: MAGIC 0x006f6e08
int *DAT_006f6e08;
// GLOBAL: MAGIC 0x006f6e10
int DAT_006f6e10[4];
// GLOBAL: MAGIC 0x006f6e20
int DAT_006f6e20;
// GLOBAL: MAGIC 0x008950b4
int DAT_008950b4;

// GLOBAL: MAGIC 0x00777848
int DAT_00777848;
// GLOBAL: MAGIC 0x007ab14c
int DAT_007ab14c;
// GLOBAL: MAGIC 0x007ab430
int DAT_007ab430[500];
// GLOBAL: MAGIC 0x007abce0
int DAT_007abce0;
// GLOBAL: MAGIC 0x007abce4
int DAT_007abce4;
// GLOBAL: MAGIC 0x008a8c18
int DAT_008a8c18;
// GLOBAL: MAGIC 0x008a99f8
int DAT_008a99f8;
// GLOBAL: MAGIC 0x008a99fc
int DAT_008a99fc;
// GLOBAL: MAGIC 0x008b1140
dialog_stack_entry_t DAT_008b1140[32];
// GLOBAL: MAGIC 0x008b2940
int DAT_008b2940;
// GLOBAL: MAGIC 0x008b3400
int DAT_008b3400[0x26];
// GLOBAL: MAGIC 0x008b5140
int DAT_008b5140[500];
// GLOBAL: MAGIC 0x008cdac0
int DAT_008cdac0[500];
// GLOBAL: MAGIC 0x008ce4f8
int DAT_008ce4f8;
// GLOBAL: MAGIC 0x008ce500
int DAT_008ce500;
// GLOBAL: MAGIC 0x008ced00
int DAT_008ced00[16];
// GLOBAL: MAGIC 0x008cefb4
int DAT_008cefb4;
// GLOBAL: MAGIC 0x008966d0
int DAT_008966d0;
// GLOBAL: MAGIC 0x0091a940
int DAT_0091a940[500];
// GLOBAL: MAGIC 0x0091b400
int DAT_0091b400[500];
// GLOBAL: MAGIC 0x0091c4f4
int DAT_0091c4f4;
// GLOBAL: MAGIC 0x00924820
int DAT_00924820[500];
// GLOBAL: MAGIC 0x00924ff0
int DAT_00924ff0[16];
// GLOBAL: MAGIC 0x00925d34
int DAT_00925d34;
// GLOBAL: MAGIC 0x00925f9c
int DAT_00925f9c;
// GLOBAL: MAGIC 0x00925360
int DAT_00925360[0x26];
// GLOBAL: MAGIC 0x009266a8
int DAT_009266a8;
// GLOBAL: MAGIC 0x009397d0
int DAT_009397d0[16];
// GLOBAL: MAGIC 0x0093a7f4
int DAT_0093a7f4;
// GLOBAL: MAGIC 0x0093a97c
int DAT_0093a97c;
// GLOBAL: MAGIC 0x0093d84c
int DAT_0093d84c;
// GLOBAL: MAGIC 0x0091c980
int DAT_0091c980;
// GLOBAL: MAGIC 0x00708da8
char DAT_00708da8[0x358];
// GLOBAL: MAGIC 0x00709100
char DAT_00709100[0x290];

// GLOBAL: MAGIC 0x0057248c
char s_HackWordSearchFormat_0057248c[8] = "|H%s";
// GLOBAL: MAGIC 0x00572494
char s_HackWordReplacementFormat_00572494[8] = "|H%s";
// GLOBAL: MAGIC 0x0057249c
char s_ColorWordSearchFormat_0057249c[8] = "|S%s";
// GLOBAL: MAGIC 0x005724a4
char s_ColorWordReplacementFormat_005724a4[8] = "|S%s";
// GLOBAL: MAGIC 0x0057f630
char s_CardValueFormat_0057f630[8] = "%s: %d";
// GLOBAL: MAGIC 0x0057f638
char s_ParenthesizedColorFormat_0057f638[8] = " (%s)";
// GLOBAL: MAGIC 0x0057f640
char s_Empty_0057f640[4] = "";
// GLOBAL: MAGIC 0x0057f644
char s_HackWord1_0057f644[4] = "|H1";
// GLOBAL: MAGIC 0x0057f648
char s_Empty_0057f648[4] = "";
// GLOBAL: MAGIC 0x0057f64c
char s_HackWord2_0057f64c[4] = "|H2";
// GLOBAL: MAGIC 0x0057f650
char s_Empty_0057f650[4] = "";
// GLOBAL: MAGIC 0x0057f654
char s_HackWord3_0057f654[4] = "|H3";
// GLOBAL: MAGIC 0x0057f658
char s_Empty_0057f658[4] = "";
// GLOBAL: MAGIC 0x0057f65c
char s_HackWord4_0057f65c[4] = "|H4";
// GLOBAL: MAGIC 0x0057f660
char s_Empty_0057f660[4] = "";
// GLOBAL: MAGIC 0x0057f664
char s_HackWordAny_0057f664[4] = "|H";
// GLOBAL: MAGIC 0x0057f668
char s_Empty_0057f668[4] = "";
// GLOBAL: MAGIC 0x0057f66c
char s_ColorWord1_0057f66c[4] = "|S1";
// GLOBAL: MAGIC 0x0057f670
char s_Empty_0057f670[4] = "";
// GLOBAL: MAGIC 0x0057f674
char s_ColorWord2_0057f674[4] = "|S2";
// GLOBAL: MAGIC 0x0057f678
char s_Empty_0057f678[4] = "";
// GLOBAL: MAGIC 0x0057f67c
char s_ColorWord3_0057f67c[4] = "|S3";
// GLOBAL: MAGIC 0x0057f680
char s_Empty_0057f680[4] = "";
// GLOBAL: MAGIC 0x0057f684
char s_ColorWord4_0057f684[4] = "|S4";
// GLOBAL: MAGIC 0x0057f688
char s_Empty_0057f688[4] = "";
// GLOBAL: MAGIC 0x0057f68c
char s_ColorWordAny_0057f68c[4] = "|S";
// GLOBAL: MAGIC 0x0057f690
char s_PlusPowerToughnessFormat_0057f690[8] = "+%d/+%d";
// GLOBAL: MAGIC 0x0057f698
char s_PlusPowerToughnessToken_0057f698[8] = "+|n/+|n";
// GLOBAL: MAGIC 0x0057f6a0
char s_PlusZeroToughnessFormat_0057f6a0[8] = "+0/+%d";
// GLOBAL: MAGIC 0x0057f6a8
char s_PlusZeroToughnessToken_0057f6a8[8] = "+0/+|n";
// GLOBAL: MAGIC 0x0057f6b0
char s_PlusPowerZeroFormat_0057f6b0[8] = "+%d/+0";
// GLOBAL: MAGIC 0x0057f6b8
char s_PlusPowerZeroToken_0057f6b8[8] = "+|n/+0";
// GLOBAL: MAGIC 0x0057f6c0
char s_MinusZeroToughnessFormat_0057f6c0[8] = "%d/-0";
// GLOBAL: MAGIC 0x0057f6c8
char s_MinusZeroToughnessToken_0057f6c8[8] = "-|n/-0";
// GLOBAL: MAGIC 0x0057f6d0
char s_PlusPowerMinusToughnessFormat_0057f6d0[8] = "+%d/%d";
// GLOBAL: MAGIC 0x0057f6d8
char s_PlusPowerMinusToughnessToken_0057f6d8[8] = "+|n/-|n";
// GLOBAL: MAGIC 0x0057f6e0
char s_NumberFormat_0057f6e0[4] = "%d";
// GLOBAL: MAGIC 0x0057f6e4
char s_NumberToken_0057f6e4[4] = "|n";
// GLOBAL: MAGIC 0x0057f6e8
char s_Empty_0057f6e8[4] = "";
// GLOBAL: MAGIC 0x0057f6ec
char s_CommaSpace_0057f6ec[4] = ", ";
// GLOBAL: MAGIC 0x0057f6f0
char s_CommaSpace_0057f6f0[4] = ", ";
// GLOBAL: MAGIC 0x0057f6f4
char s_CommaSpace_0057f6f4[4] = ", ";
// GLOBAL: MAGIC 0x0057f6f8
char s_CommaSpace_0057f6f8[4] = ", ";
// GLOBAL: MAGIC 0x0057f6fc
char s_AbilityListMarker_0057f6fc[4] = "|A";
// GLOBAL: MAGIC 0x0057f700
char s_MinusPowerToughnessFormat_0057f700[8] = "-%d/-%d";
// GLOBAL: MAGIC 0x0057f708
char s_MinusPowerToughnessToken_0057f708[8] = "-|n/-|n";
// GLOBAL: MAGIC 0x0057f710
char s_Newline_0057f710[4] = "\n";
// GLOBAL: MAGIC 0x0057f714
char s_Newline_0057f714[4] = "\n";
// GLOBAL: MAGIC 0x0057f718
char s_Empty_0057f718[64] = "";

// GLOBAL: MAGIC 0x008b4e44
char DAT_008b4e44[50];
// GLOBAL: MAGIC 0x008b4e76
char DAT_008b4e76[50];
// GLOBAL: MAGIC 0x008b4ea8
char DAT_008b4ea8[50];

unsigned int FUN_00446e4b(void);
unsigned int FUN_00449ac3(int player, int card);
unsigned int FUN_00448c93(int *displayed_player_and_card, int player, int card);
unsigned int FUN_00448490(int player, int card);
int FUN_00449057(int player, int card);
int FUN_004491cd(int player, int card);
void FUN_00449249(int player, int card, int *power, int *toughness);
void FUN_00449645(int player, int card, char *text);
void FUN_00449706(int player, int card, char *text);
unsigned int FUN_00449898(int player, int card);
unsigned int FUN_00449990(int player, int card);
void FUN_00449a0f(int *displayed_player_and_card, int player, int card);
void FUN_00494e91(char *text, char *search, int case_sensitive, char *replace);
void FUN_0049511c(char *text, int source_color, int target_color, int parenthesize);
void FUN_00495217(char *text, int source_color, int target_color, int parenthesize);
int FUN_004964dd(int card_id, int player, int card);
unsigned int FUN_00559999(int dc, int rect, int raw_card, int player, int card, int param_6, int param_7);
void FUN_00559bc1(int dc, int rect, int player, int card);
void FUN_00559e9c(int dc, int rect, int card_id, int player, int card);
int FUN_004a583e(int player, int card);
int FUN_004a587c(int player, int card);
int FUN_004a58ba(int player, int card);
INT_PTR CALLBACK FUN_00506fa0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode);

// FUNCTION: MAGIC 0x0044a3bf
char *FUN_0044a3bf(int player, int card)
{
  struct
  {
    int transformed_csvid;
    char temporary_name[52];
    int source_card_data;
    int source_player;
    unsigned int choice;
    char *card_name;
    unsigned int type;
    int csvid;
  } s;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1)
  {
    s.type = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  }
  else
  {
    s.type = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  }

  s.csvid = CardIDFromType(s.type);
  s.card_name = unk_0056e49c;
  if (s.csvid != -1)
  {
    if (s.csvid == unk_0092666c)
    {
      s.csvid = CardIDFromType(FUN_004a583e(player, card));
    }

    s.choice = (unsigned int)PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id;
    s.source_player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
    s.source_card_data = PLAYER_CARD_INSTANCE(player, card).damage_source_card;

    if (s.csvid == unk_007a7d64)
    {
      strcpy(unk_00637670, gs_cardtitle_damage_008cfd30);
    }
    else if (s.csvid == unk_00789b80)
    {
      sprintf(unk_00637670, gs_cardtitle_hunting_00926750, FUN_00495311(PLAYER_CARD_INSTANCE(player, card).info_slot));
    }
    else if (s.csvid == unk_008cf1ac)
    {
      strcpy(unk_00637670, gs_multiblock_creature_008cf040);
    }
    else if (s.csvid == unk_00789734)
    {
      strcpy(unk_00637670, unk_00777e60[s.choice].damage_text);
    }
    else if (s.csvid == unk_008a8de8)
    {
      strcpy(unk_00637670, unk_00777e60[s.choice].effect_text);
    }
    else if (s.csvid == unk_009266ac)
    {
      strcpy(unk_00637670, gs_cardtitle_draw_a_card_008b4330);
    }
    else
    {
      unk_00637670[0] = '\0';
    }

    if (s.csvid == unk_00789734 && 0 < (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness)
    {
      strcpy(s.temporary_name, unk_00637670);
      FUN_0055d802(unk_00637670, s.temporary_name, PLAYER_CARD_INSTANCE(player, card).eot_toughness);
    }

    s.transformed_csvid = FUN_004a58ba(s.source_player, s.source_card_data);
    if (s.transformed_csvid == 0x361 || s.transformed_csvid == 0x360)
    {
      strcpy(unk_00637670, unk_00777e60[s.transformed_csvid].damage_text);
    }

    if (unk_00637670[0] == '\0')
    {
      s.card_name = global_raw_cards_storage[s.csvid].full_name;
    }
    else
    {
      s.card_name = unk_00637670;
    }
  }

  return s.card_name;
}

// FUNCTION: MAGIC 0x0044a796
void FUN_0044a796(int player, int card)
{
  char *card_name;

  card_name = FUN_0044a3bf(player, card);
  if (card_name != NULL)
  {
    strcat(unk_00748770, card_name);
  }
}

// FUNCTION: MAGIC 0x004483be
int FUN_004483be(int player, int card)
{
  if (player != 0 && player != 1)
  {
    return 1;
  }

  if (card < 0 || card > 0x96)
  {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004487d8
int FUN_004487d8(int player, int card)
{
  int internal_card_id;

  if (FUN_004483be(player, card) != 0)
  {
    return -1;
  }

  EnterCriticalSection(&g_duel_render_lock);
  internal_card_id = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return internal_card_id;
}

// FUNCTION: MAGIC 0x00448857
int FUN_00448857(int player, int card)
{
  int internal_card_id;

  if (FUN_004483be(player, card) != 0)
  {
    return -1;
  }

  internal_card_id = FUN_004487d8(player, card);
  if (internal_card_id == -1)
  {
    return -1;
  }
  else
    return *(int *)&global_cards_data[internal_card_id].id;
}

// FUNCTION: MAGIC 0x00449057
int FUN_00449057(int player, int card)
{
  int displayed_color;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_color = (int)(char)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_color;
}

// FUNCTION: MAGIC 0x00449645
void FUN_00449645(int player, int card, char *text)
{
  int color_index;

  EnterCriticalSection(&g_duel_render_lock);
  for (color_index = 1; color_index < 6; ++color_index)
  {
    if (DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color_id[color_index] != '\0')
    {
      FUN_00495217(text, color_index, (int)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color_id[color_index], 0);
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00449706
void FUN_00449706(int player, int card, char *text)
{
  int color_index;

  EnterCriticalSection(&g_duel_render_lock);
  for (color_index = 1; color_index < 6; ++color_index)
  {
    if (DISPLAYED_PLAYER_CARD_INSTANCE(player, card).hack_mode[color_index] != '\0')
    {
      FUN_0049511c(text, color_index, (int)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).hack_mode[color_index], 0);
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00449990
unsigned int FUN_00449990(int player, int card)
{
  int result;

  if (FUN_004483be(player, card) != 0)
    return -1;

  EnterCriticalSection(&g_duel_render_lock);
  result = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x00449a0f
void FUN_00449a0f(int *displayed_player_and_card, int player, int card)
{
  if (FUN_004483be(player, card) != 0)
    return;

  if (displayed_player_and_card == NULL)
    return;

  EnterCriticalSection(&g_duel_render_lock);
  displayed_player_and_card[0] = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).parent_controller;
  displayed_player_and_card[1] = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).parent_card;
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00448c93
unsigned int FUN_00448c93(int *displayed_player_and_card, int player, int card)
{
  unsigned int display_info;

  if (FUN_004483be(player, card) == 0)
  {
    EnterCriticalSection(&g_duel_render_lock);
    if (displayed_player_and_card != NULL)
    {
      displayed_player_and_card[0] = (int)(char)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).damage_source_player;
      displayed_player_and_card[1] = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).damage_source_card;
    }
    display_info = DISPLAYED_PIC_INFO(player, card);
    LeaveCriticalSection(&g_duel_render_lock);
  }
  else
  {
    display_info = 0xffffffff;
  }

  return display_info;
}

// FUNCTION: MAGIC 0x00448490
unsigned int FUN_00448490(int player, int card)
{
  unsigned int info_slot;

  if (FUN_004483be(player, card) == 0)
  {
    EnterCriticalSection(&g_duel_render_lock);
    info_slot = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).info_slot;
    LeaveCriticalSection(&g_duel_render_lock);
  }
  else
  {
    info_slot = 0;
  }

  return info_slot;
}

// FUNCTION: MAGIC 0x004491cd
int FUN_004491cd(int player, int card)
{
  int displayed_value;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_value = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_value;
}

// FUNCTION: MAGIC 0x00449249
void FUN_00449249(int player, int card, int *power, int *toughness)
{
  if (power == NULL || toughness == NULL)
  {
  }
  else if (FUN_004483be(player, card))
  {
  }
  else
  {
    EnterCriticalSection(&g_duel_render_lock);
    *power = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).counter_power;
    *toughness = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).counter_toughness;
    LeaveCriticalSection(&g_duel_render_lock);
  }
}

// FUNCTION: MAGIC 0x00449898
unsigned int FUN_00449898(int player, int card)
{
  unsigned int displayed_flags;

  if (FUN_004483be(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_flags = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).token_status;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_flags;
}

// FUNCTION: MAGIC 0x00494e91
void FUN_00494e91(char *text, char *search, int case_sensitive, char *replace)
{
  struct
  {
    int write_pos;
    char rewritten_text[1000];
    size_t search_length;
    int found_match;
    char *cursor;
  } s;

  if (text == NULL || search == NULL || replace == NULL)
  {
  }
  else if (strlen(text) == 0 || strlen(search) == 0)
  {
  }
  else
  {
    s.search_length = strlen(search);
    s.cursor = text;
    s.rewritten_text[0] = '\0';
    s.write_pos = 0;
    while (*s.cursor != '\0')
    {
      s.found_match = 0;
      if ((case_sensitive != 0 && strncmp(s.cursor, search, s.search_length) == 0) ||
          (case_sensitive == 0 && _strnicmp(s.cursor, search, s.search_length) == 0))
      {
        s.found_match = 1;
      }

      if (s.found_match != 0)
      {
        strcat(s.rewritten_text, replace);
        s.write_pos += strlen(replace);
        s.cursor += s.search_length;
      }
      else
      {
        s.rewritten_text[s.write_pos] = *s.cursor;
        ++s.cursor;
        ++s.write_pos;
        s.rewritten_text[s.write_pos] = '\0';
      }
    }
    strcpy(text, s.rewritten_text);
  }
}

// FUNCTION: MAGIC 0x0049511c
void FUN_0049511c(char *text, color_t source_color, color_t target_color, int parenthesize)
{
  char replacement_text[100];
  int color_index;
  char search_text[100];

  for (color_index = 0; color_index < gs_words_to_hack_count_008cd924; ++color_index)
  {
    sprintf(search_text, s_HackWordSearchFormat_0057248c, gs_words_to_hack_00789c80[source_color][color_index]);
    if (parenthesize == 0)
    {
      strcpy(replacement_text, gs_words_hacked_0091d260[target_color][color_index]);
    }
    else
    {
      sprintf(replacement_text, s_HackWordReplacementFormat_00572494, gs_words_hacked_0091d260[target_color][color_index]);
    }
    FUN_00494e91(text, search_text, 1, replacement_text);
  }
}

// FUNCTION: MAGIC 0x00495217
void FUN_00495217(char *text, color_t source_color, int target_color, int parenthesize)
{
  char replacement_text[100];
  int color_index;
  char search_text[100];

  for (color_index = 0; color_index < gs_words_to_sleight_count_00789c34; ++color_index)
  {
    sprintf(search_text, s_ColorWordSearchFormat_0057249c, gs_words_to_sleight_00781bd0[source_color][color_index]);
    if (parenthesize == 0)
    {
      strcpy(replacement_text, gs_words_sleighted_008a9c10[target_color][color_index]);
    }
    else
    {
      sprintf(replacement_text, s_ColorWordReplacementFormat_005724a4, gs_words_sleighted_008a9c10[target_color][color_index]);
    }
    FUN_00494e91(text, search_text, 1, replacement_text);
  }
}

// FUNCTION: MAGIC 0x004a583e
int FUN_004a583e(int player, int card)
{
  return *(int *)((char *)&global_card_instances[0][card] + player * 0xb0f4 + 0x3c);
}

// FUNCTION: MAGIC 0x004a587c
int FUN_004a587c(int player, int card)
{
  return PLAYER_CARD_INSTANCE(player, card).internal_card_id;
}

// FUNCTION: MAGIC 0x004a58ba
int FUN_004a58ba(int player, int card)
{
  if (player == -1 || card == -1)
  {
    return -1;
  }
  else
  {
    int internal_card_id = FUN_004a587c(player, card);
    if (internal_card_id == -1)
    {
      return -1;
    }
    else
    {
      return *(int *)&global_cards_data[internal_card_id].id;
    }
  }
}

// FUNCTION: MAGIC 0x004a63b8
int do_dialog(int who_chooses,
              int bigcard_player,
              int bigcard_card,
              int smallcard_player,
              int smallcard_card,
              const char *options,
              int ai_choice)
{
  int dialog_mode;
  char displayed_options[600];
  int remaining_choice;
  int dialog_result;
  int option_index;
  unsigned int at_start_of_line;

  if (who_chooses == active_player && (unk_00926804 & 2) != 0)
  {
    TENTATIVE_wait_for_network_result(who_chooses, 0xd);
    ai_choice = DAT_008b293c;
  }

  if (g_duel_ai_mode_state == 1)
  {
    return ai_choice;
  }

  if (who_chooses < 0 || bigcard_player < 0 || bigcard_card < 0)
  {
    return ai_choice;
  }

  strcpy(displayed_options, options);
  strcpy(unk_00748770, "");
  if (who_chooses == active_player)
  {
    sprintf(unk_00748770, gs_prompt_new_full_card_0091b150, DAT_007a7c60);
    strcat(unk_00748770, "\n\n");
    at_start_of_line = 1;
    remaining_choice = ai_choice;
    for (option_index = 0; option_index < 1000; ++option_index)
    {
      if (at_start_of_line != 0 && displayed_options[option_index] == ' ')
      {
        if (remaining_choice-- == 0)
        {
          displayed_options[option_index] = '>';
          break;
        }
      }

      at_start_of_line = displayed_options[option_index] == '\n';
    }
  }
  else
  {
    FUN_0044a796(bigcard_player, bigcard_card);
    strcat(unk_00748770, "...\n");
  }

  strcat(unk_00748770, displayed_options);
  if (PLAYER_CARD_INSTANCE(bigcard_player, bigcard_card).internal_card_id == -1 ||
      PLAYER_CARD_INSTANCE(smallcard_player, smallcard_card).internal_card_id == -1)
  {
    TENTATIVE_reassess_all_cards();
  }

  if (who_chooses == unk_008b35ec && g_duel_network_state == 0)
  {
    dialog_mode = 1;
  }
  else
  {
    dialog_mode = 0;
  }

  dialog_result =
      raw_do_dialog(bigcard_player, bigcard_card, smallcard_player, smallcard_card, unk_00748770, dialog_mode);

  if (who_chooses == unk_008b35ec && (unk_00926804 & 2) != 0)
  {
    unk_008b2938 = '\r';
    DAT_008b293c = dialog_result;
    TENTATIVE_send_network_result(who_chooses, 0xd);
  }

  if (who_chooses == unk_008b35ec && g_duel_network_state == 0)
  {
    ai_choice = dialog_result;
  }

  return ai_choice;
}

// FUNCTION: MAGIC 0x004a55c7
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode)
{
  INT_PTR dialog_result;
  int card_image_number;
  dialog_context_t s;

  KillTimer((HWND)g_duel_window_hwnd, g_duel_timer_id);
  if (smallcard_card == 0xff)
  {
    smallcard_card = -1;
  }

  if (bigcard_player != -1 && bigcard_card != -1)
  {
    card_image_number = FUN_00448857(bigcard_player, bigcard_card);
    if (card_image_number == -1)
    {
      FUN_00446e4b();
    }
  }

  if (smallcard_player != -1 && smallcard_card != -1)
  {
    card_image_number = FUN_00448857(smallcard_player, smallcard_card);
    if (card_image_number == -1)
    {
      FUN_00446e4b();
    }
  }

  s.bigcard_player = bigcard_player;
  s.bigcard_card = bigcard_card;
  s.smallcard_player = smallcard_player;
  s.smallcard_card = smallcard_card;
  s.prompt = prompt;
  s.dialog_mode = dialog_mode;

  dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xdf, (HWND)g_duel_window_hwnd, FUN_00506fa0, (LPARAM)&s);
  if (dialog_result == 0)
  {
    return -1;
  }

  return dialog_result - 1;
}

// FUNCTION: MAGIC 0x00446e4b
unsigned int FUN_00446e4b(void)
{
  int stack_player;
  int stack_card;
  unsigned int mana_changed_player_0;
  unsigned int mana_changed_player_1;
  unsigned int graveyard_changed_player_0;
  unsigned int graveyard_changed_player_1;
  int converted_card_id;
  unsigned int exile_changed_player_0;
  unsigned int exile_changed_player_1;
  unsigned int library_changed_player_0;
  unsigned int library_changed_player_1;
  int target_index;
  int zone_index;
  unsigned int needs_refresh;
  int *stack_entry_count;

  // stack_entry_count = (int *)&gs_multiblock_creature_008cf040[52];

  EnterCriticalSection(&g_duel_render_lock);
  needs_refresh = memcmp(global_displayed_card_instances, global_card_instances, 0x161e8);
  memcpy(global_displayed_card_instances, global_card_instances, 0x161e8);
  if (DAT_008a99f8 != active_cards_count[0] || DAT_008a99fc != active_cards_count[1])
  {
    needs_refresh = 1;
  }
  DAT_008a99f8 = active_cards_count[0];
  DAT_008a99fc = active_cards_count[1];
  if (life[0] != DAT_008b2940 || life[1] != DAT_00925d34)
  {
    needs_refresh = 1;
  }
  DAT_008b2940 = life[0];
  DAT_00925d34 = life[1];
  if (DAT_007abce0 != DAT_007ab14c || DAT_007abce4 != DAT_0093a97c)
  {
    needs_refresh = 1;
  }
  DAT_007ab14c = DAT_007abce0;
  DAT_0093a97c = DAT_007abce4;
  if (unk_008b44d0[0] != DAT_0091c4f4 || unk_008b44d0[1] != DAT_008a8c18)
  {
    needs_refresh = 1;
  }
  DAT_0091c4f4 = unk_008b44d0[0];
  DAT_008a8c18 = unk_008b44d0[1];

  mana_changed_player_0 = memcmp(unk_00896670, raw_mana_available, 0x1c);
  mana_changed_player_1 = memcmp(unk_007ab290, raw_mana_available + 1, 0x1c);
  memcpy(unk_00896670, raw_mana_available, 0x1c);
  memcpy(unk_007ab290, raw_mana_available + 1, 0x1c);

  graveyard_changed_player_0 = memcmp(DAT_007ab430, global_graveyard_slots, 2000);
  graveyard_changed_player_1 = memcmp(DAT_0091a940, global_graveyard_slots[1], 2000);
  DAT_00925f9c = 0;
  for (zone_index = 0; zone_index < 500 && global_graveyard_slots[0][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_graveyard_slots[0][zone_index]);
    DAT_007ab430[DAT_00925f9c] = converted_card_id;
    ++DAT_00925f9c;
  }
  DAT_0093a7f4 = 0;
  for (zone_index = 0; zone_index < 500 && global_graveyard_slots[1][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_graveyard_slots[1][zone_index]);
    DAT_0091a940[DAT_0093a7f4] = converted_card_id;
    ++DAT_0093a7f4;
  }

  exile_changed_player_0 = memcmp(DAT_00924820, global_exile, 2000);
  exile_changed_player_1 = memcmp(DAT_008b5140, global_exile[1], 2000);
  DAT_009266a8 = 0;
  for (zone_index = 0; zone_index < 500 && global_exile[0][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_exile[0][zone_index]);
    DAT_00924820[DAT_009266a8] = converted_card_id;
    ++DAT_009266a8;
  }
  DAT_008ce500 = 0;
  for (zone_index = 0; zone_index < 500 && global_exile[1][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_exile[1][zone_index]);
    DAT_008b5140[DAT_008ce500] = converted_card_id;
    ++DAT_008ce500;
  }

  library_changed_player_0 = memcmp(DAT_008cdac0, global_library, 2000);
  library_changed_player_1 = memcmp(DAT_0091b400, global_library[1], 2000);
  needs_refresh |= mana_changed_player_0 | mana_changed_player_1 | graveyard_changed_player_0 |
                   graveyard_changed_player_1 | exile_changed_player_0 | exile_changed_player_1 |
                   library_changed_player_0 | library_changed_player_1;
  DAT_008ce4f8 = 0;
  for (zone_index = 0; zone_index < 500 && global_library[0][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_library[0][zone_index]);
    DAT_008cdac0[DAT_008ce4f8] = converted_card_id;
    ++DAT_008ce4f8;
  }
  DAT_008cefb4 = 0;
  for (zone_index = 0; zone_index < 500 && global_library[1][zone_index] != -1; ++zone_index)
  {
    converted_card_id = CardIDFromType(global_library[1][zone_index]);
    DAT_0091b400[DAT_008cefb4] = converted_card_id;
    ++DAT_008cefb4;
  }

  *stack_entry_count = 0;
  for (zone_index = 0; zone_index < 0x20 && global_stack_cards[zone_index].player != -1; ++zone_index)
  {
    if (unk_007a7750[zone_index] != 0)
    {
      stack_player = global_stack_cards[zone_index].player;
      stack_card = global_stack_cards[zone_index].card;
      DAT_008b1140[*stack_entry_count].player = stack_player;
      DAT_008b1140[*stack_entry_count].card = stack_card;
      DAT_008b1140[*stack_entry_count].number_of_targets = (int)PLAYER_CARD_INSTANCE(stack_player, stack_card).number_of_targets;
      for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(stack_player, stack_card).number_of_targets; ++target_index)
      {
        DAT_008b1140[*stack_entry_count].targets[target_index].player =
            PLAYER_CARD_INSTANCE(stack_player, stack_card).targets[target_index].player;
        DAT_008b1140[*stack_entry_count].targets[target_index].card =
            PLAYER_CARD_INSTANCE(stack_player, stack_card).targets[target_index].card;
      }
      ++*stack_entry_count;
    }
  }

  DAT_008966d0 = 0;
  for (zone_index = 0; zone_index < 0x10 && DAT_008ced00[zone_index] != -1; ++zone_index)
  {
    ++DAT_008966d0;
  }
  memcpy(DAT_00924ff0, DAT_008ced00, 0x40);
  DAT_0093d84c = 0;
  for (zone_index = 0; zone_index < 0x10 && ((int *)global_ante_cards[0])[zone_index] != -1; ++zone_index)
  {
    ++DAT_0093d84c;
  }
  memcpy(DAT_009397d0, global_ante_cards, 0x40);

  for (zone_index = 0; zone_index < 0x26; ++zone_index)
  {
    if ((int)(char)DAT_007abc90[zone_index] != DAT_00925360[zone_index])
    {
      needs_refresh |= 1;
    }
    if ((int)(char)DAT_007abc90[zone_index + 0x26] != DAT_008b3400[zone_index])
    {
      needs_refresh |= 1;
    }
    DAT_00925360[zone_index] = (int)(char)DAT_007abc90[zone_index] & 1;
    DAT_008b3400[zone_index] = (int)(char)DAT_007abc90[zone_index + 0x26] & 1;
  }

  if (DAT_00777848 != _DAT_00742fbc)
  {
    needs_refresh = 1;
  }
  DAT_00777848 = _DAT_00742fbc;
  LeaveCriticalSection(&g_duel_render_lock);
  return needs_refresh;
}

// FUNCTION: MAGIC 0x00449ac3
unsigned int FUN_00449ac3(int player, int card)
{
  int displayed_card_type;

  if (FUN_004483be(player, card) != 0)
  {
    return -1;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_card_type = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return displayed_card_type;
}

// FUNCTION: MAGIC 0x00559999
unsigned int FUN_00559999(int dc, int rect, int raw_card, int player, int card, int param_6, int param_7)
{
  struct
  {
    char rules_text[1000];
    int number_of_colors;
    unsigned int color_test;
    int color_index;
    card_ptr_t card_data;
    int card_version;
    int card_id;
  } s;

  if (dc == 0 || rect == 0 || raw_card == 0)
  {
    return 0;
  }

  s.card_id = FUN_00448857(player, card);
  if (s.card_id == -1)
  {
    return 0;
  }

  memcpy(&s.card_data, global_raw_cards_storage + s.card_id, 0x98);
  s.color_test = FUN_00449057(player, card);
  s.number_of_colors = 0;
  for (s.color_index = 1; s.color_index <= 5; ++s.color_index)
  {
    if ((s.color_test & (1 << (unsigned char)s.color_index)) != 0)
    {
      ++s.number_of_colors;
    }
  }

  if (s.number_of_colors > 1)
  {
    s.card_data.color = 4;
  }
  else
  {
    if ((s.color_test & 2) != 0)
    {
      s.card_data.color = 1;
    }
    else if ((s.color_test & 0x20) != 0)
    {
      s.card_data.color = 8;
    }
    else if ((s.color_test & 8) != 0)
    {
      s.card_data.color = 5;
    }
    else if ((s.color_test & 0x10) != 0)
    {
      s.card_data.color = 7;
    }
    else if ((s.color_test & 4) != 0)
    {
      s.card_data.color = 2;
    }
  }

  strcpy(s.rules_text, global_raw_cards_storage[s.card_id].rules_text);
  FUN_00449645(player, card, s.rules_text);
  FUN_00449706(player, card, s.rules_text);
  s.card_data.rules_text = s.rules_text;
  s.card_version = FUN_004964dd(s.card_id, player, card);
  return DrawFullCard((HDC)dc,
                      (RECT *)rect,
                      &s.card_data,
                      s.card_version,
                      param_6,
                      param_7,
                      &gs_illus_00789130);
}

// FUNCTION: MAGIC 0x00559bc1
void FUN_00559bc1(int dc, int rect, int player, int card)
{
  struct
  {
    int displayed_player;
    int displayed_card;
    int number_of_colors;
    unsigned int color_test;
    int color_index;
    card_ptr_t card_data;
    int card_version;
    int card_id;
    int displayed_type;
  } s;

  if (dc == 0 || rect == 0)
  {
    return;
  }

  s.displayed_type = FUN_00449990(player, card);
  s.card_id = CardIDFromType(s.displayed_type);
  if ((s.card_id == unk_0092666c && FUN_00449ac3(player, card) == unk_008b28f8) || s.card_id == unk_009266ac)
  {
    DrawCardBack((HDC)dc, (RECT *)rect);
    return;
  }

  FUN_00449a0f(&s.displayed_player, player, card);
  s.card_version = FUN_004964dd(s.card_id, s.displayed_player, s.displayed_card);
  if (s.card_id == unk_007a7d64 ||
      s.card_id == unk_00789b80 ||
      s.card_id == unk_008cf1ac ||
      s.card_id == unk_00789734 ||
      s.card_id == unk_008a8de8)
  {
    if (FUN_00448857(s.displayed_player, s.displayed_card) != -1)
    {
      FUN_00559e9c(dc, rect, s.card_id, s.displayed_player, s.displayed_card);
    }
    return;
  }

  memcpy(&s.card_data, global_raw_cards_storage + s.card_id, 0x98);
  s.color_test = FUN_00449057(player, card);
  s.number_of_colors = 0;
  for (s.color_index = 1; s.color_index < 6; ++s.color_index)
  {
    if ((s.color_test & (1 << (unsigned char)s.color_index)) != 0)
    {
      ++s.number_of_colors;
    }
  }

  if (s.number_of_colors >= 2)
  {
    s.card_data.color = 4;
  }
  else
  {
    if ((s.color_test & 2) != 0)
    {
      s.card_data.color = 1;
    }
    else if ((s.color_test & 0x20) != 0)
    {
      s.card_data.color = 8;
    }
    else if ((s.color_test & 8) != 0)
    {
      s.card_data.color = 5;
    }
    else if ((s.color_test & 0x10) != 0)
    {
      s.card_data.color = 7;
    }
    else if ((s.color_test & 4) != 0)
    {
      s.card_data.color = 2;
    }
  }

  DrawFullCard((HDC)dc, (RECT *)rect, &s.card_data, s.card_version, 2, DAT_0091c980, &gs_illus_00789130);
}

// FUNCTION: MAGIC 0x00559e9c
void FUN_00559e9c(int dc, int rect, int card_id, int player, int card)
{
  struct
  {
    char transformed_rules_text[400];
    int appended_text;
    int displayed_power;
    char value_text[100];
    int displayed_toughness;
    int transformed_csvid;
    char temporary_name[52];
    int single_color;
    unsigned int display_info;
    int displayed_player;
    int displayed_card;
    int ability_index;
    card_ptr_t card_data;
    unsigned int display_flags;
    unsigned int display_version;
  } s;

  if (dc != 0 && rect != 0)
  {
    s.display_info = FUN_00448c93(&s.displayed_player, player, card);
    s.display_version = s.display_info >> 0x10;
    memset(&s.card_data, 0, sizeof(s.card_data));
    s.card_data.id = s.display_info & 0xffff;

    if (card_id == unk_007a7d64)
    {
      sprintf(DAT_00709100, s_CardValueFormat_0057f630, gs_cardtitle_damage_008cfd30, FUN_00448490(player, card));
      s.single_color = single_color_test_bit_to_color_t(FUN_00449057(player, card));
      if (s.single_color == 1 ||
          s.single_color == 2 ||
          s.single_color == 4 ||
          s.single_color == 3 ||
          s.single_color == 5)
      {
        sprintf(DAT_00709100 + strlen(DAT_00709100), s_ParenthesizedColorFormat_0057f638, gs_color_word_capitalized_00709390[s.single_color]);
      }
    }
    else if (card_id == unk_00789b80)
    {
      sprintf(DAT_00709100, unk_009266d0 + 0x80, FUN_00495311(FUN_00448490(player, card)));
    }
    else if (card_id == unk_008cf1ac)
    {
      strcpy(DAT_00709100, gs_multiblock_creature_008cf040);
    }
    else if (card_id == unk_00789734)
    {
      strcpy(DAT_00709100, unk_00777e60[s.card_data.id].damage_text);
    }
    else if (card_id == unk_008a8de8)
    {
      strcpy(DAT_00709100, unk_00777e60[s.card_data.id].effect_text);
    }
    else
    {
      DAT_00709100[0] = '\0';
    }

    if (card_id == unk_00789734 && FUN_004491cd(player, card) > 0)
    {
      strcpy(s.temporary_name, DAT_00709100);
      FUN_0055d802(DAT_00709100, s.temporary_name, FUN_004491cd(player, card));
    }

    s.transformed_csvid = FUN_00448857(s.displayed_player, s.displayed_card);
    if (card_id == unk_00789734 && (s.transformed_csvid == 0x361 || s.transformed_csvid == 0x360))
    {
      strcpy(DAT_00709100, unk_00777e60[s.transformed_csvid].damage_text);
    }

    s.card_data.full_name = DAT_00709100;
    s.card_data.name = DAT_00709100;
    s.card_data.expansion = 0xffffffff;
    s.card_data.color = 0xffffffff;
    s.card_data.card_type = 0;
    s.card_data.subtype = 0xffffffff;
    s.card_data.type_text = NULL;
    s.card_data.db_card_type_2 = 0;
    s.card_data.rarity = 0;
    memset(&s.card_data.req, 0, sizeof(s.card_data.req));
    s.card_data.flags = 0;
    s.card_data.unused0x38 = 0xffffffff;
    s.card_data.unused0x3c = 0;
    s.card_data.artist = NULL;
    s.card_data.num_pics = 0xffffffff;
    s.card_data.mana_cost_text = NULL;
    s.card_data.ai_modifiers1 = 0;
    s.card_data.ai_modifiers2 = 0;
    memset(s.card_data.abilities, 0, sizeof(s.card_data.abilities));
    s.card_data.flavor_text = s_Empty_0057f718;
    s.card_data.power = 0;
    s.card_data.toughness = 0;

    if (card_id == unk_007a7d64)
    {
      strcpy(DAT_00708da8, *(char **)(gs_words_to_sleight_00781bd0 + s.card_data.id * 0x14));
    }
    else if (card_id == unk_00789b80)
    {
      strcpy(DAT_00708da8, unk_00777e60[s.card_data.id].effect_title);
    }
    else if (card_id == unk_008cf1ac)
    {
      strcpy(DAT_00708da8, gs_multiblock_shadow_blocker_00925e70);
    }
    else if (card_id == unk_00789734)
    {
      strcpy(DAT_00708da8, unk_00777e60[s.card_data.id].effect_title);
    }
    else if (card_id == unk_008a8de8)
    {
      strcpy(DAT_00708da8, unk_00777e60[s.card_data.id].legacy_title);
    }
    else
    {
      DAT_00708da8[0] = '\0';
    }

    s.display_flags = FUN_00449898(player, card);
    if ((s.display_flags & 0x2000000) == 0)
    {
      FUN_00449645(player, card, DAT_00708da8);
      FUN_00449706(player, card, DAT_00708da8);
    }
    else
    {
      FUN_00449645(s.displayed_player, s.displayed_card, DAT_00708da8);
      FUN_00449706(s.displayed_player, s.displayed_card, DAT_00708da8);
    }

    FUN_00494e91(DAT_00708da8, s_HackWord1_0057f644, 1, s_Empty_0057f640);
    FUN_00494e91(DAT_00708da8, s_HackWord2_0057f64c, 1, s_Empty_0057f648);
    FUN_00494e91(DAT_00708da8, s_HackWord3_0057f654, 1, s_Empty_0057f650);
    FUN_00494e91(DAT_00708da8, s_HackWord4_0057f65c, 1, s_Empty_0057f658);
    FUN_00494e91(DAT_00708da8, s_HackWordAny_0057f664, 1, s_Empty_0057f660);
    FUN_00494e91(DAT_00708da8, s_ColorWord1_0057f66c, 1, s_Empty_0057f668);
    FUN_00494e91(DAT_00708da8, s_ColorWord2_0057f674, 1, s_Empty_0057f670);
    FUN_00494e91(DAT_00708da8, s_ColorWord3_0057f67c, 1, s_Empty_0057f678);
    FUN_00494e91(DAT_00708da8, s_ColorWord4_0057f684, 1, s_Empty_0057f680);
    FUN_00494e91(DAT_00708da8, s_ColorWordAny_0057f68c, 1, s_Empty_0057f688);

    FUN_00449249(player, card, &s.displayed_power, &s.displayed_toughness);
    sprintf(s.value_text, s_PlusPowerToughnessFormat_0057f690, s.displayed_power, s.displayed_toughness);
    FUN_00494e91(DAT_00708da8, s_PlusPowerToughnessToken_0057f698, 0, s.value_text);
    sprintf(s.value_text, s_PlusZeroToughnessFormat_0057f6a0, s.displayed_toughness);
    FUN_00494e91(DAT_00708da8, s_PlusZeroToughnessToken_0057f6a8, 0, s.value_text);
    sprintf(s.value_text, s_PlusPowerZeroFormat_0057f6b0, s.displayed_power);
    FUN_00494e91(DAT_00708da8, s_PlusPowerZeroToken_0057f6b8, 0, s.value_text);
    sprintf(s.value_text, s_MinusZeroToughnessFormat_0057f6c0, s.displayed_power);
    FUN_00494e91(DAT_00708da8, s_MinusZeroToughnessToken_0057f6c8, 0, s.value_text);
    sprintf(s.value_text, s_PlusPowerMinusToughnessFormat_0057f6d0, s.displayed_power, s.displayed_toughness);
    FUN_00494e91(DAT_00708da8, s_PlusPowerMinusToughnessToken_0057f6d8, 0, s.value_text);
    sprintf(s.value_text, s_NumberFormat_0057f6e0, s.displayed_toughness);
    FUN_00494e91(DAT_00708da8, s_NumberToken_0057f6e4, 0, s.value_text);

    if (s.card_data.id == 0x21b)
    {
      s.display_flags = FUN_00448490(player, card);
      s.appended_text = 0;
      strcpy(s.value_text, s_Empty_0057f6e8);
      if ((s.display_flags & 0x20) != 0)
      {
        if (s.appended_text != 0)
        {
          strcat(s.value_text, s_CommaSpace_0057f6ec);
        }
        strcat(s.value_text, gs_ability_word_008b4de0);
        s.appended_text = 1;
      }
      if ((s.display_flags & 0x100) != 0)
      {
        if (s.appended_text != 0)
        {
          strcat(s.value_text, s_CommaSpace_0057f6f0);
        }
        strcat(s.value_text, DAT_008b4ea8);
        s.appended_text = 1;
      }
      if ((s.display_flags & 0x40) != 0)
      {
        if (s.appended_text != 0)
        {
          strcat(s.value_text, s_CommaSpace_0057f6f4);
        }
        strcat(s.value_text, DAT_008b4e44);
        s.appended_text = 1;
      }
      if ((s.display_flags & 0x80) != 0)
      {
        if (s.appended_text != 0)
        {
          strcat(s.value_text, s_CommaSpace_0057f6f8);
        }
        strcat(s.value_text, DAT_008b4e76);
        s.appended_text = 1;
      }
      FUN_00494e91(DAT_00708da8, s_AbilityListMarker_0057f6fc, 0, s.value_text);
      FUN_00449249(player, card, &s.displayed_power, &s.displayed_toughness);
      sprintf(s.value_text, s_MinusPowerToughnessFormat_0057f700, s.displayed_power, s.displayed_toughness);
      FUN_00494e91(DAT_00708da8, s_MinusPowerToughnessToken_0057f708, 0, s.value_text);
    }

    if (card_id == unk_00789734 && FUN_004491cd(player, card) > 0)
    {
      strcpy(s.transformed_rules_text, DAT_00708da8);
      FUN_0055d802(DAT_00708da8, s.transformed_rules_text, FUN_004491cd(player, card));
    }

    if (card_id == unk_007a7d64)
    {
      s.display_flags = FUN_00449898(player, card);
      if (DAT_00708da8[0] != '\0' && DAT_00708da8[0] != '\n')
      {
        strcat(DAT_00708da8, s_Newline_0057f710);
      }
      if ((s.display_flags & 0x100000) != 0)
      {
        strcat(DAT_00708da8, DAT_008b4ea8);
        strcat(DAT_00708da8, s_Newline_0057f714);
      }
      if ((s.display_flags & 0x80000) != 0)
      {
        strcat(DAT_00708da8, DAT_008b4e76);
      }
    }

    s.card_data.rules_text = DAT_00708da8;
    DrawFullCard((HDC)dc, (RECT *)rect, &s.card_data, s.display_version, 2, DAT_0091c980, &gs_illus_00789130);
  }
}

// FUNCTION: MAGIC 0x00506fa0
INT_PTR CALLBACK FUN_00506fa0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  POINT cursor_point;
  POINT secondary_point;
  LRESULT listbox_selection;
  UINT notification_code;
  HBRUSH background_brush;
  int drag_y;
  int drag_x;
  BOOL point_in_rect;
  HWND child_window;
  unsigned int card_type;
  size_t coord_text_length;
  HGDIOBJ font_handle;
  char card_coordinates[12];
  HDC paint_dc;
  PAINTSTRUCT paint_struct;
  int card_image_number;
  int card_id;
  int picture_number;
  int hovered_player;
  int hovered_card;
  unsigned int mouse_x;
  unsigned int mouse_y;
  WPARAM bigcard_image;
  WPARAM smallcard_image;
  RECT before_drag_rect;
  unsigned int was_dragged;
  RECT after_drag_rect;
  HDC erase_dc;
  RECT erase_rect;
  LRESULT current_selection;
  HGDIOBJ old_font;
  LOGFONTA listbox_font_info;
  HFONT bold_font;
  HANDLE listbox_font_handle;
  int smallcard_player;
  int smallcard_card;
  HWND smallcard_window;
  RECT listbox_rect;
  int listbox_top;
  int listbox_right_margin;
  RECT client_rect;
  dialog_context_t *dialog_context;

  dialog_context = (dialog_context_t *)DAT_006f6e08;

  if (msg == WM_ERASEBKGND)
  {
    erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(erase_dc);
    GetClientRect(hwnd, &erase_rect);
    if (g_magicgame_big_card_choice_pic == 0)
    {
      background_brush = GetStockObject(BLACK_BRUSH);
      FillRect(erase_dc, &erase_rect, background_brush);
    }
    else
    {
      DrawBitmapToRect(erase_dc, &erase_rect, (HBITMAP)g_magicgame_big_card_choice_pic);
    }
    return 1;
  }

  if (msg == WM_INITDIALOG)
  {
    DAT_006f6e20 = 0;
    if (g_duel_network_state != 0)
    {
      SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
    }

    child_window = GetDlgItem(hwnd, 0x3fd);
    GetWindowRect(child_window, &listbox_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&listbox_rect, 2);
    GetClientRect(hwnd, &client_rect);
    listbox_top = listbox_rect.top;
    listbox_right_margin = client_rect.right - listbox_rect.right;
    InflateRect(&client_rect, -listbox_rect.top, -listbox_right_margin);
    CopyRect((RECT *)DAT_006f6df8, &client_rect);
    DAT_006f6df8[2] = listbox_rect.left - listbox_top;
    CopyRect((RECT *)DAT_006f6e10, &client_rect);
    DAT_006f6e10[0] = listbox_rect.left;
    DAT_006f6e10[1] = listbox_rect.bottom;
    if (DAT_006f6e10[3] - listbox_rect.bottom < DAT_006f6e10[2] - listbox_rect.left)
    {
      DAT_006f6e10[2] = DAT_006f6e10[3] - listbox_rect.bottom + listbox_rect.left;
    }
    else
    {
      DAT_006f6e10[3] = DAT_006f6e10[2] - listbox_rect.left + listbox_rect.bottom;
    }

    DAT_006f6e08 = (int *)lparam;
    dialog_context = (dialog_context_t *)DAT_006f6e08;
    SetDlgItemTextA(hwnd, 0x3fd, dialog_context->prompt);
    SendDlgItemMessageA(hwnd, 0x3fd, 0x401, dialog_context->dialog_mode, 0);
    if (dialog_context->smallcard_player != -1)
    {
      smallcard_player = dialog_context->smallcard_player;
      smallcard_card = dialog_context->smallcard_card;
      smallcard_window = CreateWindowExA(0,
                                         "MAGICGAME_BigCardCardClass",
                                         "BigCard small card",
                                         0x50000000,
                                         DAT_006f6e10[0],
                                         DAT_006f6e10[1],
                                         g_showlist_smallcard_width,
                                         g_showlist_smallcard_height,
                                         hwnd,
                                         (HMENU)1,
                                         g_app_instance,
                                         &smallcard_player);
      (void)smallcard_window;
    }

    listbox_font_handle = (HANDLE)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
    GetObjectA(listbox_font_handle, sizeof(listbox_font_info), &listbox_font_info);
    listbox_font_info.lfWeight = 700;
    bold_font = CreateFontIndirectA(&listbox_font_info);
    SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, (WPARAM)bold_font, 0);
    child_window = GetDlgItem(hwnd, 0x3fd);
    SetFocus(child_window);
    listbox_selection = SendDlgItemMessageA(hwnd, 0x3fd, 0x400, 0, 0);
    if (listbox_selection == 0 || dialog_context->dialog_mode == 0)
    {
      SetTimer(hwnd, 1, DAT_008950b4, (TIMERPROC)0);
    }
    return 0;
  }

  if (msg == WM_COMMAND)
  {
    if (LOWORD(wparam) == 1 || LOWORD(wparam) == 2)
    {
      child_window = GetDlgItem(hwnd, 0x3fd);
      SendMessageA(hwnd, WM_COMMAND, 0x3fd, (LPARAM)child_window);
    }
    else if (LOWORD(wparam) == 0x3fd)
    {
      notification_code = HIWORD(wparam);
      current_selection = SendDlgItemMessageA(hwnd, 0x3fd, 0x400, 0, 0);
      if (notification_code == 0)
      {
        if (current_selection == 0 || dialog_context->dialog_mode == 0)
        {
          old_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
          SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
          DeleteObject(old_font);
          EndDialog(hwnd, notification_code);
        }
      }
      else
      {
        old_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
        SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
        DeleteObject(old_font);
        EndDialog(hwnd, notification_code);
      }
    }
    return 1;
  }

  if (msg == WM_TIMER)
  {
    font_handle = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
    DeleteObject(font_handle);
    EndDialog(hwnd, 0);
    return 1;
  }

  if (msg == WM_LBUTTONDOWN)
  {
    GetWindowRect(hwnd, &before_drag_rect);
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    GetWindowRect(hwnd, &after_drag_rect);
    drag_y = abs(before_drag_rect.top - after_drag_rect.top);
    drag_x = abs(before_drag_rect.left - after_drag_rect.left);
    was_dragged = (drag_y + drag_x > 4);
    if (was_dragged == 0)
    {
      child_window = GetDlgItem(hwnd, 0x3fd);
      SendMessageA(hwnd, WM_COMMAND, 0x3fd, (LPARAM)child_window);
    }
    return 1;
  }

  if (msg == WM_MOUSEMOVE || msg == WM_RBUTTONDOWN)
  {
    mouse_x = LOWORD(lparam);
    mouse_y = HIWORD(lparam);
    if ((msg == WM_MOUSEMOVE && DAT_0091c998 != 2) || (msg == WM_RBUTTONDOWN && DAT_0091c998 == 2))
    {
      bigcard_image = FUN_00448857(dialog_context->bigcard_player, dialog_context->bigcard_card);
      if (dialog_context->smallcard_player == -1 || dialog_context->smallcard_card == -1)
      {
        smallcard_image = 0xffffffff;
      }
      else
      {
        smallcard_image = FUN_00448857(dialog_context->smallcard_player, dialog_context->smallcard_card);
      }

      cursor_point.x = mouse_x;
      cursor_point.y = mouse_y;
      point_in_rect = PtInRect((RECT *)DAT_006f6df8, cursor_point);
      if (bigcard_image != 0xffffffff && point_in_rect != 0)
      {
        hovered_player = dialog_context->bigcard_player;
        hovered_card = dialog_context->bigcard_card;
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, bigcard_image, (LPARAM)&hovered_player);
      }
      else if (smallcard_image != 0xffffffff)
      {
        secondary_point.x = mouse_x;
        secondary_point.y = mouse_y;
        point_in_rect = PtInRect((RECT *)DAT_006f6e10, secondary_point);
        if (point_in_rect != 0)
        {
          hovered_player = dialog_context->smallcard_player;
          hovered_card = dialog_context->smallcard_card;
          SendMessageA(g_duel_card_preview_window_hwnd, 0x401, smallcard_image, (LPARAM)&hovered_player);
        }
      }
    }
    return 0;
  }

  if (msg == WM_PAINT)
  {
    child_window = GetDlgItem(hwnd, 0x3fd);
    UpdateWindow(child_window);
    paint_dc = BeginPaint(hwnd, &paint_struct);
    if (paint_dc != 0)
    {
      ApplyCardArtPaletteToDc(paint_dc);
      card_image_number = FUN_00448857(dialog_context->bigcard_player, dialog_context->bigcard_card);
      card_type = FUN_00449ac3(dialog_context->bigcard_player, dialog_context->bigcard_card);
      card_id = CardIDFromType(card_type);
      if (card_image_number == -1)
      {
        if (card_id != -1 &&
            card_id != unk_009266ac &&
            card_id != unk_007a7d64 &&
            card_id != unk_00789b80 &&
            card_id != unk_008cf1ac &&
            card_id != unk_00789734 &&
            card_id != unk_008a8de8 &&
            card_id != unk_0092666c)
        {
          DrawFullCard(paint_dc, (RECT *)DAT_006f6df8, global_raw_cards_storage + card_id, 0, 0x12, 0, &gs_illus_00789130);
        }
        else if (card_image_number == unk_0092666c)
        {
          FUN_00559bc1((int)paint_dc, (int)DAT_006f6df8, dialog_context->bigcard_player, dialog_context->bigcard_card);
        }
        else
        {
          FUN_00559999((int)paint_dc,
                       (int)DAT_006f6df8,
                       (int)(global_raw_cards_storage + card_image_number),
                       dialog_context->bigcard_player,
                       dialog_context->bigcard_card,
                       0x12,
                       0);
        }

        if (g_duel_dialog_refresh_state != 0)
        {
          sprintf(card_coordinates, "%d,%d", dialog_context->bigcard_player, dialog_context->bigcard_card);
          SetBkMode(paint_dc, 1);
          SetTextColor(paint_dc, 0);
          coord_text_length = strlen(card_coordinates);
          TextOutA(paint_dc,
                   DAT_006f6df8[0] + 5,
                   DAT_006f6df8[1] + ((DAT_006f6df8[3] - DAT_006f6df8[1]) * 0x14) / 100,
                   card_coordinates,
                   coord_text_length);
          SetTextColor(paint_dc, 0xffffff);
          coord_text_length = strlen(card_coordinates);
          TextOutA(paint_dc,
                   DAT_006f6df8[0] + 4,
                   DAT_006f6df8[1] + ((DAT_006f6df8[3] - DAT_006f6df8[1]) * 0x14) / 100 - 1,
                   card_coordinates,
                   coord_text_length);
        }

        (void)picture_number;
      }

      EndPaint(hwnd, &paint_struct);
    }
    return 1;
  }

  if (msg > 0x30e && msg < 0x312)
  {
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, (int)lparam);
  }

  return 0;
}
