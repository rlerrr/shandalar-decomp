#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "deckdll/src/card_db.h"
#include "game_support.h"
#include "global_state.h"
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
#define DISPLAYED_PLAYER_CARD_ROW(player_) global_displayed_card_instances[(player_)]

extern card_ptr_t global_raw_cards_storage[2000];
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;

// GLOBAL: MAGIC 0x006f6df8
// GLOBAL: SHANDALAR 0x005b7658
RECT g_big_card_choice_full_card_rect;
// GLOBAL: MAGIC 0x006f6e08
// GLOBAL: SHANDALAR 0x005b7668
dialog_context_t *g_big_card_choice_dialog_context;
// GLOBAL: MAGIC 0x006f6e10
// GLOBAL: SHANDALAR 0x005b7670
RECT g_big_card_choice_small_card_rect;
// GLOBAL: MAGIC 0x006f6e20
// GLOBAL: SHANDALAR 0x005b7680
int g_big_card_choice_state_006f6e20;
// GLOBAL: MAGIC 0x006f6e28
// GLOBAL: SHANDALAR 0x005b7688
char g_big_card_choice_window_text_buffer[0xc8];
// GLOBAL: MAGIC 0x008950b4
// GLOBAL: SHANDALAR 0x008a92b4
int g_big_card_choice_timeout_ms;

// GLOBAL: MAGIC 0x00777848
// GLOBAL: SHANDALAR 0x0078e5d4
int DAT_00777848;
// GLOBAL: MAGIC 0x007ab14c
// GLOBAL: SHANDALAR 0x007bf34c
int g_duel_cached_state_007abce0;
// GLOBAL: MAGIC 0x007ab430
// GLOBAL: SHANDALAR 0x007bf630
int DAT_007ab430[500];
// GLOBAL: MAGIC 0x008a8c18
// GLOBAL: SHANDALAR 0x008bce18
int g_duel_cached_unk_008b44d0_player_1;
// GLOBAL: MAGIC 0x008a99f8
// GLOBAL: SHANDALAR 0x008bdbf8
int g_duel_cached_active_cards_count_player_0;
// GLOBAL: MAGIC 0x008a99fc
// GLOBAL: SHANDALAR 0x008bdbfc
int g_duel_cached_active_cards_count_player_1;
// GLOBAL: MAGIC 0x008b1140
// GLOBAL: SHANDALAR 0x008c5340
dialog_stack_entry_t DAT_008b1140[32];
// GLOBAL: MAGIC 0x008b2940
// GLOBAL: SHANDALAR 0x008c6af0
int g_duel_cached_life_player_0;
// GLOBAL: MAGIC 0x008b3400
// GLOBAL: SHANDALAR 0x008c75a0
int DAT_008b3400[0x26];
// GLOBAL: MAGIC 0x008b5140
// GLOBAL: SHANDALAR 0x008c92c0
int DAT_008b5140[500];
// GLOBAL: MAGIC 0x008cdac0
// GLOBAL: SHANDALAR 0x008e1c40
int DAT_008cdac0[500];
// GLOBAL: MAGIC 0x008ce4f8
// GLOBAL: SHANDALAR 0x008e2678
int g_duel_cached_library_count_player_0;
// GLOBAL: MAGIC 0x008ce500
// GLOBAL: SHANDALAR 0x008e2680
int g_duel_cached_exile_count_player_1;
// GLOBAL: MAGIC 0x008cefb4
// GLOBAL: SHANDALAR 0x008e3100
int g_duel_cached_library_count_player_1;
// GLOBAL: MAGIC 0x008966d0
// GLOBAL: SHANDALAR 0x008aa8d0
int DAT_008966d0;
// GLOBAL: MAGIC 0x0091a940
// GLOBAL: SHANDALAR 0x0092ea90
int DAT_0091a940[500];
// GLOBAL: MAGIC 0x0091b400
// GLOBAL: SHANDALAR 0x0092f550
int DAT_0091b400[500];
// GLOBAL: MAGIC 0x0091c4f4
// GLOBAL: SHANDALAR 0x00930634
int g_duel_cached_unk_008b44d0_player_0;
// GLOBAL: MAGIC 0x00924820
// GLOBAL: SHANDALAR 0x00938950
int DAT_00924820[500];
// GLOBAL: MAGIC 0x00924ff0
// GLOBAL: SHANDALAR 0x00939120
int DAT_00924ff0[16];
// GLOBAL: MAGIC 0x00925d34
// GLOBAL: SHANDALAR 0x00939e60
int g_duel_cached_life_player_1;
// GLOBAL: MAGIC 0x00925f9c
// GLOBAL: SHANDALAR 0x0093a0cc
int g_duel_cached_graveyard_count_player_0;
// GLOBAL: MAGIC 0x00925360
// GLOBAL: SHANDALAR 0x00939490
int DAT_00925360[0x26];
// GLOBAL: MAGIC 0x009266a8
// GLOBAL: SHANDALAR 0x0093a7d8
int g_duel_cached_exile_count_player_0;
// GLOBAL: MAGIC 0x009397d0
// GLOBAL: SHANDALAR 0x0094d8f0
int DAT_009397d0[16];
// GLOBAL: MAGIC 0x0093a7f4
// GLOBAL: SHANDALAR 0x0094e914
int g_duel_cached_graveyard_count_player_1;
// GLOBAL: MAGIC 0x0093a97c
// GLOBAL: SHANDALAR 0x0094ea9c
int g_duel_cached_state_007abce4;
// GLOBAL: MAGIC 0x0093d84c
// GLOBAL: SHANDALAR 0x0095196c
int DAT_0093d84c;
// GLOBAL: MAGIC 0x00708da8
char g_card_rules_text_buffer_00708da8[0x190];
// GLOBAL: MAGIC 0x00708f48
// GLOBAL: SHANDALAR 0x007325c0
char g_activation_rules_text_buffer_00708f48[0x190];
// GLOBAL: MAGIC 0x00709100
char g_card_title_buffer_00709100[0x34];
// GLOBAL: MAGIC 0x00709150
// GLOBAL: SHANDALAR 0x007327c8
char g_activation_card_title_buffer_00709150[0x34];

// GLOBAL: MAGIC 0x008b4e44
char DAT_008b4e44[50];
// GLOBAL: MAGIC 0x008b4e76
char DAT_008b4e76[50];
// GLOBAL: MAGIC 0x008b4ea8
char DAT_008b4ea8[50];

unsigned int refresh_duel_display_cache(void);
unsigned int get_displayed_card_original_internal_id(int player, int card);
unsigned int get_displayed_card_display_pic_info(int *displayed_player_and_card, int player, int card);
unsigned int get_displayed_card_special_counters(int player, int card);
int get_displayed_card_color_flags(int player, int card);
int get_displayed_card_eot_toughness(int player, int card);
void get_displayed_card_counter_power_toughness(int player, int card, int *power, int *toughness);
void apply_displayed_card_sleight_words(int player, int card, char *text);
void apply_displayed_card_hack_words(int player, int card, char *text);
unsigned int get_displayed_card_display_flags(int player, int card);
unsigned int get_displayed_card_base_internal_id(int player, int card);
void get_displayed_card_parent(int *displayed_player_and_card, int player, int card);
void replace_text_token(char *text, char *search, int case_sensitive, char *replace);
void replace_hack_color_words(char *text, int source_color, int target_color, int parenthesize);
void replace_sleight_color_words(char *text, int source_color, int target_color, int parenthesize);
int get_card_display_pic_num(card_id_t card_id, int player, int card);
unsigned int draw_displayed_full_card(HDC dc, RECT *rect, card_ptr_t *raw_card, int player, int card, int draw_mode, int expand_text_box);
void draw_displayed_card_overlaid_full_card(HDC dc, RECT *rect, int player, int card);
void draw_special_effect_full_card(HDC dc, RECT *rect, card_id_t card_id, int player, int card);
int get_original_internal_card_id(int player, int card);
int get_card_internal_id(int player, int card);
card_id_t get_card_id(int player, int card);
INT_PTR CALLBACK big_card_choice_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode);

// FUNCTION: MAGIC 0x0044a3bf
// FUNCTION: SHANDALAR 0x00453f42
char *get_displayed_card_name(int player, int card)
{
  struct
  {
    card_id_t transformed_card_id;
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
  s.card_name = "";
  if (s.csvid != -1)
  {
    if (s.csvid == unk_0092666c)
    {
      s.csvid = CardIDFromType(get_original_internal_card_id(player, card));
    }

    s.choice = (unsigned int)PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id;
    s.source_player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
    s.source_card_data = PLAYER_CARD_INSTANCE(player, card).damage_source_card;

    if (s.csvid == unk_007a7d64)
    {
      strcpy(g_dialog_card_title_buffer, gs_cardtitle_damage_008cfd30);
    }
    else if (s.csvid == unk_00789b80)
    {
      sprintf(g_dialog_card_title_buffer, gs_cardtitle_hunting_00926750, get_hunting_subtype_name(PLAYER_CARD_INSTANCE(player, card).info_slot));
    }
    else if (s.csvid == unk_008cf1ac)
    {
      strcpy(g_dialog_card_title_buffer, gs_multiblock_creature_008cf040);
    }
    else if (s.csvid == unk_00789734)
    {
      strcpy(g_dialog_card_title_buffer, global_legacy_names[s.choice].effect_title);
    }
    else if (s.csvid == unk_008a8de8)
    {
      strcpy(g_dialog_card_title_buffer, global_legacy_names[s.choice].legacy_title);
    }
    else if (s.csvid == unk_009266ac)
    {
      strcpy(g_dialog_card_title_buffer, gs_cardtitle_draw_a_card_008b4330);
    }
    else
    {
      g_dialog_card_title_buffer[0] = '\0';
    }

    if (s.csvid == unk_00789734 && 0 < (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness)
    {
      strcpy(s.temporary_name, g_dialog_card_title_buffer);
      extract_numbered_text_choice(g_dialog_card_title_buffer, s.temporary_name, PLAYER_CARD_INSTANCE(player, card).eot_toughness);
    }

    s.transformed_card_id = get_card_id(s.source_player, s.source_card_data);
    if (s.transformed_card_id == CARD_ID_FAERIE_DRAGON ||
        s.transformed_card_id == CARD_ID_WHIMSY)
    {
      strcpy(g_dialog_card_title_buffer, global_legacy_names[s.transformed_card_id].effect_title);
    }

    if (g_dialog_card_title_buffer[0] == '\0')
    {
      s.card_name = global_raw_cards_storage[s.csvid].full_name;
    }
    else
    {
      s.card_name = g_dialog_card_title_buffer;
    }
  }

  return s.card_name;
}

// FUNCTION: MAGIC 0x0044a796
// FUNCTION: SHANDALAR 0x00454311
void append_displayed_card_name(int player, int card)
{
  char *card_name;

  card_name = get_displayed_card_name(player, card);
  if (card_name != NULL)
  {
    strcat(g_ui_message_buffer, card_name);
  }
}

// FUNCTION: MAGIC 0x004483be
// FUNCTION: SHANDALAR 0x00451f3b
int displayed_card_indices_invalid(int player, int card)
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

// FUNCTION: MAGIC 0x004486de
// FUNCTION: SHANDALAR 0x0045225b
int get_displayed_card_blocking(int player, int card)
{
  int blocking;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  blocking = (int)(char)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).blocking;
  LeaveCriticalSection(&g_duel_render_lock);
  return blocking;
}

// FUNCTION: MAGIC 0x004487d8
// FUNCTION: SHANDALAR 0x00452355
int get_displayed_card_internal_id(int player, int card)
{
  int internal_card_id;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return -1;
  }

  EnterCriticalSection(&g_duel_render_lock);
  internal_card_id = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return internal_card_id;
}

// FUNCTION: MAGIC 0x00448857
// FUNCTION: SHANDALAR 0x004523d4
card_id_t get_displayed_card_id(int player, int card)
{
  int internal_card_id;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return -1;
  }

  internal_card_id = get_displayed_card_internal_id(player, card);
  if (internal_card_id == -1)
  {
    return -1;
  }
  else
    return global_cards_data[internal_card_id].id;
}

// FUNCTION: MAGIC 0x00449057
// FUNCTION: SHANDALAR 0x00452bd4
int get_displayed_card_color_flags(int player, int card)
{
  int displayed_color;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_color = (int)(char)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_color;
}

// FUNCTION: MAGIC 0x00449645
// FUNCTION: SHANDALAR 0x004531c2
void apply_displayed_card_sleight_words(int player, int card, char *text)
{
  int color_index;

  EnterCriticalSection(&g_duel_render_lock);
  for (color_index = 1; color_index < 6; ++color_index)
  {
    if (DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color_id[color_index] != '\0')
    {
      replace_sleight_color_words(text, color_index, (int)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).color_id[color_index], 0);
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00449706
// FUNCTION: SHANDALAR 0x00453287
void apply_displayed_card_hack_words(int player, int card, char *text)
{
  int color_index;

  EnterCriticalSection(&g_duel_render_lock);
  for (color_index = 1; color_index < 6; ++color_index)
  {
    if (DISPLAYED_PLAYER_CARD_INSTANCE(player, card).hack_mode[color_index] != '\0')
    {
      replace_hack_color_words(text, color_index, (int)DISPLAYED_PLAYER_CARD_INSTANCE(player, card).hack_mode[color_index], 0);
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00449990
// FUNCTION: SHANDALAR 0x00453511
unsigned int get_displayed_card_base_internal_id(int player, int card)
{
  int result;

  if (displayed_card_indices_invalid(player, card) != 0)
    return -1;

  EnterCriticalSection(&g_duel_render_lock);
  result = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return result;
}

// FUNCTION: MAGIC 0x00449a0f
// FUNCTION: SHANDALAR 0x00453590
void get_displayed_card_parent(int *displayed_player_and_card, int player, int card)
{
  if (displayed_card_indices_invalid(player, card) != 0)
    return;

  if (displayed_player_and_card == NULL)
    return;

  EnterCriticalSection(&g_duel_render_lock);
  displayed_player_and_card[0] = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).parent_controller;
  displayed_player_and_card[1] = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).parent_card;
  LeaveCriticalSection(&g_duel_render_lock);
}

// FUNCTION: MAGIC 0x00448c93
// FUNCTION: SHANDALAR 0x00452810
unsigned int get_displayed_card_display_pic_info(int *displayed_player_and_card, int player, int card)
{
  unsigned int display_info;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    if (displayed_player_and_card != NULL)
    {
      displayed_player_and_card[0] = (int)(char)DISPLAYED_PLAYER_CARD_ROW(player)[card].damage_source_player;
      displayed_player_and_card[1] = DISPLAYED_PLAYER_CARD_ROW(player)[card].damage_source_card;
    }
    return 0xffffffff;
  }
  
  EnterCriticalSection(&g_duel_render_lock);
  if (displayed_player_and_card != NULL)
  {
    displayed_player_and_card[0] = (int)(char)DISPLAYED_PLAYER_CARD_ROW(player)[card].damage_source_player;
    displayed_player_and_card[1] = DISPLAYED_PLAYER_CARD_ROW(player)[card].damage_source_card;
  }
  display_info = DISPLAYED_PLAYER_CARD_ROW(player)[card].display_pic_info;
  LeaveCriticalSection(&g_duel_render_lock);

  return display_info;
}

// FUNCTION: MAGIC 0x00448490
// FUNCTION: SHANDALAR 0x0045200d
unsigned int get_displayed_card_special_counters(int player, int card)
{
  unsigned int info_slot;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  info_slot = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).info_slot;
  LeaveCriticalSection(&g_duel_render_lock);

  return info_slot;
}

// FUNCTION: MAGIC 0x004491cd
// FUNCTION: SHANDALAR 0x00452d4a
int get_displayed_card_eot_toughness(int player, int card)
{
  int displayed_value;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_value = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_value;
}

// FUNCTION: MAGIC 0x00449249
// FUNCTION: SHANDALAR 0x00452dc6
void get_displayed_card_counter_power_toughness(int player, int card, int *power, int *toughness)
{
  if (power == NULL || toughness == NULL)
  {
  }
  else if (displayed_card_indices_invalid(player, card))
  {
  }
  else
  {
    EnterCriticalSection(&g_duel_render_lock);
    *power = DISPLAYED_PLAYER_CARD_ROW(player)[card].counter_power;
    *toughness = DISPLAYED_PLAYER_CARD_ROW(player)[card].counter_toughness;
    LeaveCriticalSection(&g_duel_render_lock);
  }
}

// FUNCTION: MAGIC 0x00449898
// FUNCTION: SHANDALAR 0x00453419
unsigned int get_displayed_card_display_flags(int player, int card)
{
  unsigned int displayed_flags;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_flags = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).token_status;
  LeaveCriticalSection(&g_duel_render_lock);

  return displayed_flags;
}

// FUNCTION: MAGIC 0x00494e91
// FUNCTION: SHANDALAR 0x0046592b
void replace_text_token(char *text, char *search, int case_sensitive, char *replace)
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
// FUNCTION: SHANDALAR 0x00465bb6
void replace_hack_color_words(char *text, color_t source_color, color_t target_color, int parenthesize)
{
  struct
  {
    char search_text[100];
    int color_index;
    char replacement_text[100];
  } s;

  for (s.color_index = 0; gs_words_to_hack_count_008cd924 > s.color_index; ++s.color_index)
  {
    sprintf(s.search_text, "|H%s", gs_words_to_hack_00789c80[source_color][s.color_index]);
    if (parenthesize != 0)
    {
      sprintf(s.replacement_text, "|H%s", gs_words_hacked_0091d260[target_color][s.color_index]);
    }
    else
    {
      strcpy(s.replacement_text, gs_words_hacked_0091d260[target_color][s.color_index]);
    }
    replace_text_token(text, s.search_text, 1, s.replacement_text);
  }
}

// FUNCTION: MAGIC 0x00495217
// FUNCTION: SHANDALAR 0x00465cb1
void replace_sleight_color_words(char *text, color_t source_color, int target_color, int parenthesize)
{
  struct
  {
    char search_text[100];
    int color_index;
    char replacement_text[100];
  } s;

  for (s.color_index = 0; gs_words_to_sleight_count_00789c34 > s.color_index; ++s.color_index)
  {
    sprintf(s.search_text, "|S%s", gs_words_to_sleight_00781bd0[source_color][s.color_index]);
    if (parenthesize != 0)
    {
      sprintf(s.replacement_text, "|S%s", gs_words_sleighted_008a9c10[target_color][s.color_index]);
    }
    else
    {
      strcpy(s.replacement_text, gs_words_sleighted_008a9c10[target_color][s.color_index]);
    }
    replace_text_token(text, s.search_text, 1, s.replacement_text);
  }
}

// FUNCTION: MAGIC 0x004a583e
// FUNCTION: SHANDALAR 0x005579be
int get_original_internal_card_id(int player, int card)
{
  return PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
}

// FUNCTION: MAGIC 0x004a587c
// FUNCTION: SHANDALAR 0x005579fc
int get_card_internal_id(int player, int card)
{
  return PLAYER_CARD_INSTANCE(player, card).internal_card_id;
}

// FUNCTION: MAGIC 0x004a58ba
// FUNCTION: SHANDALAR 0x00557a3a
card_id_t get_card_id(int player, int card)
{
  if (player == -1 || card == -1)
  {
    return -1;
  }
  else
  {
    int internal_card_id = get_card_internal_id(player, card);
    if (internal_card_id == -1)
    {
      return -1;
    }
    else
    {
      return global_cards_data[internal_card_id].id;
    }
  }
}

// FUNCTION: MAGIC 0x004a63b8
// FUNCTION: SHANDALAR 0x00558538
int do_dialog(int who_chooses,
              int bigcard_player,
              int bigcard_card,
              int smallcard_player,
              int smallcard_card,
              const char *options,
              int ai_choice)
{
  struct
  {
    int dialog_mode;
    int unused_padding;
    char displayed_options[600];
    int remaining_choice;
    int dialog_result;
    int option_index;
    unsigned int at_start_of_line;
  } s;

  if (who_chooses == g_other_player && (g_duel_network_flags & 2) != 0)
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

  strcpy(s.displayed_options, options);
  strcpy(g_ui_message_buffer, "");
  if (who_chooses == g_other_player)
  {
    sprintf(g_ui_message_buffer, gs_prompt_new_full_card_0091b150, DAT_007a7c60);
    strcat(g_ui_message_buffer, "\n\n");
    s.at_start_of_line = 1;
    s.remaining_choice = ai_choice;
    for (s.option_index = 0; s.option_index < 1000; ++s.option_index)
    {
      if (s.at_start_of_line != 0 && s.displayed_options[s.option_index] == ' ')
      {
        s.unused_padding = s.remaining_choice;
        --s.remaining_choice;
        if (s.unused_padding == 0)
        {
          s.displayed_options[s.option_index] = '>';
          break;
        }
      }

      s.at_start_of_line = s.displayed_options[s.option_index] == '\n';
    }
  }
  else
  {
    append_displayed_card_name(bigcard_player, bigcard_card);
    strcat(g_ui_message_buffer, "...\n");
  }

  strcat(g_ui_message_buffer, s.displayed_options);
  if (PLAYER_CARD_INSTANCE(bigcard_player, bigcard_card).internal_card_id == -1 ||
      PLAYER_CARD_INSTANCE(smallcard_player, smallcard_card).internal_card_id == -1)
  {
    TENTATIVE_reassess_all_cards(1, 0xff);
  }

  if (who_chooses == g_active_player && g_duel_network_state == 0)
  {
    s.dialog_mode = 1;
  }
  else
  {
    s.dialog_mode = 0;
  }

  s.dialog_result =
      raw_do_dialog(bigcard_player, bigcard_card, smallcard_player, smallcard_card, g_ui_message_buffer, s.dialog_mode);

  if (who_chooses == g_active_player && (g_duel_network_flags & 2) != 0)
  {
    unk_008b2938 = '\r';
    DAT_008b293c = s.dialog_result;
    TENTATIVE_send_network_result(who_chooses, 0xd);
  }

  if (who_chooses == g_active_player && g_duel_network_state == 0)
  {
    return s.dialog_result;
  }
  else
  {
    return ai_choice;
  }
}

// FUNCTION: MAGIC 0x004a55c7
// FUNCTION: SHANDALAR 0x00540f38
int raw_do_dialog(int bigcard_player,
                  int bigcard_card,
                  int smallcard_player,
                  int smallcard_card,
                  char *prompt,
                  int dialog_mode)
{
  struct
  {
    dialog_context_t context;
    INT_PTR dialog_result;
  } s;

  KillTimer((HWND)g_duel_window_hwnd, g_duel_timer_id);
  if (smallcard_card == 0xff)
  {
    smallcard_card = -1;
  }

  if (bigcard_player != -1 && bigcard_card != -1)
  {
    if (get_displayed_card_id(bigcard_player, bigcard_card) == -1)
    {
      refresh_duel_display_cache();
    }
  }

  if (smallcard_player != -1 && smallcard_card != -1)
  {
    if (get_displayed_card_id(smallcard_player, smallcard_card) == -1)
    {
      refresh_duel_display_cache();
    }
  }

  s.context.bigcard_player = bigcard_player;
  s.context.bigcard_card = bigcard_card;
  s.context.smallcard_player = smallcard_player;
  s.context.smallcard_card = smallcard_card;
  s.context.prompt = prompt;
  s.context.dialog_mode = dialog_mode;

  s.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xdf, (HWND)g_duel_window_hwnd, big_card_choice_dialog_proc, (LPARAM)&s.context);
  if (s.dialog_result == 0)
  {
    return -1;
  }
  else
  {
    return s.dialog_result - 1;
  }
}

// FUNCTION: MAGIC 0x00446e4b
// FUNCTION: SHANDALAR 0x004509c9
unsigned int refresh_duel_display_cache(void)
{
  struct
  {
    int stack_card;
    int stack_player;
    int target_index;
    int zone_index;
    unsigned int needs_refresh;
  } s;

  EnterCriticalSection(&g_duel_render_lock);
  s.needs_refresh = memcmp(global_displayed_card_instances, global_card_instances, 0x161e8);
  memcpy(global_displayed_card_instances, global_card_instances, 0x161e8);
  if (g_active_cards_count[0] != g_duel_cached_active_cards_count_player_0 ||
      g_active_cards_count[1] != g_duel_cached_active_cards_count_player_1)
  {
    s.needs_refresh = 1;
  }
  g_duel_cached_active_cards_count_player_0 = g_active_cards_count[0];
  g_duel_cached_active_cards_count_player_1 = g_active_cards_count[1];
  if (g_life[0] != g_duel_cached_life_player_0 || g_life[1] != g_duel_cached_life_player_1)
  {
    s.needs_refresh = 1;
  }
  g_duel_cached_life_player_0 = g_life[0];
  g_duel_cached_life_player_1 = g_life[1];
  if (g_poison_counters[0] != g_duel_cached_state_007abce0 ||
      g_duel_cached_state_007abce4 != g_poison_counters[1])
  {
    s.needs_refresh = 1;
  }
  g_duel_cached_state_007abce0 = g_poison_counters[0];
  g_duel_cached_state_007abce4 = g_poison_counters[1];
  if (unk_008b44d0[0] != g_duel_cached_unk_008b44d0_player_0 ||
      unk_008b44d0[1] != g_duel_cached_unk_008b44d0_player_1)
  {
    s.needs_refresh = 1;
  }
  g_duel_cached_unk_008b44d0_player_0 = unk_008b44d0[0];
  g_duel_cached_unk_008b44d0_player_1 = unk_008b44d0[1];

  s.needs_refresh |= memcmp(g_duel_cached_raw_mana_player_0, g_raw_mana_available, 0x1c);
  s.needs_refresh |= memcmp(g_duel_cached_raw_mana_player_1, g_raw_mana_available + 1, 0x1c);
  memcpy(g_duel_cached_raw_mana_player_0, g_raw_mana_available, 0x1c);
  memcpy(g_duel_cached_raw_mana_player_1, g_raw_mana_available + 1, 0x1c);

  s.needs_refresh |= memcmp(DAT_007ab430, global_graveyard_slots, 2000);
  s.needs_refresh |= memcmp(DAT_0091a940, global_graveyard_slots[1], 2000);
  g_duel_cached_graveyard_count_player_0 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_graveyard_slots[0][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_007ab430[g_duel_cached_graveyard_count_player_0] = CardIDFromType(global_graveyard_slots[0][s.zone_index]);
    ++g_duel_cached_graveyard_count_player_0;
  }
  g_duel_cached_graveyard_count_player_1 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_graveyard_slots[1][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_0091a940[g_duel_cached_graveyard_count_player_1] = CardIDFromType(global_graveyard_slots[1][s.zone_index]);
    ++g_duel_cached_graveyard_count_player_1;
  }

  s.needs_refresh |= memcmp(DAT_00924820, global_exile, 2000);
  s.needs_refresh |= memcmp(DAT_008b5140, global_exile[1], 2000);
  g_duel_cached_exile_count_player_0 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_exile[0][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_00924820[g_duel_cached_exile_count_player_0] = CardIDFromType(global_exile[0][s.zone_index]);
    ++g_duel_cached_exile_count_player_0;
  }
  g_duel_cached_exile_count_player_1 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_exile[1][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_008b5140[g_duel_cached_exile_count_player_1] = CardIDFromType(global_exile[1][s.zone_index]);
    ++g_duel_cached_exile_count_player_1;
  }

  s.needs_refresh |= memcmp(DAT_008cdac0, global_library, 2000);
  s.needs_refresh |= memcmp(DAT_0091b400, global_library[1], 2000);
  g_duel_cached_library_count_player_0 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_library[0][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_008cdac0[g_duel_cached_library_count_player_0] = CardIDFromType(global_library[0][s.zone_index]);
    ++g_duel_cached_library_count_player_0;
  }
  g_duel_cached_library_count_player_1 = 0;
  for (s.zone_index = 0; s.zone_index < 500 && global_library[1][s.zone_index] != -1; ++s.zone_index)
  {
    DAT_0091b400[g_duel_cached_library_count_player_1] = CardIDFromType(global_library[1][s.zone_index]);
    ++g_duel_cached_library_count_player_1;
  }

  g_multiblock_creature_count = 0;
  for (s.zone_index = 0; global_stack_cards[s.zone_index].player != -1 && s.zone_index < 0x20; ++s.zone_index)
  {
    if (g_stack_prompt_flags[s.zone_index] != 0)
    {
      s.stack_player = global_stack_cards[s.zone_index].player;
      s.stack_card = global_stack_cards[s.zone_index].card;
      DAT_008b1140[g_multiblock_creature_count].player = s.stack_player;
      DAT_008b1140[g_multiblock_creature_count].card = s.stack_card;
      DAT_008b1140[g_multiblock_creature_count].number_of_targets =
          (int)(char)PLAYER_CARD_INSTANCE(s.stack_player, s.stack_card).number_of_targets;
      for (s.target_index = 0;
           s.target_index < (int)(char)PLAYER_CARD_INSTANCE(s.stack_player, s.stack_card).number_of_targets;
           ++s.target_index)
      {
        DAT_008b1140[g_multiblock_creature_count].targets[s.target_index].player =
            PLAYER_CARD_INSTANCE(s.stack_player, s.stack_card).targets[s.target_index].player;
        DAT_008b1140[g_multiblock_creature_count].targets[s.target_index].card =
            PLAYER_CARD_INSTANCE(s.stack_player, s.stack_card).targets[s.target_index].card;
      }
      ++g_multiblock_creature_count;
    }
  }

  s.zone_index = 0;
  DAT_008966d0 = 0;
  for (; s.zone_index < 0x10 && global_ante_cards[1][s.zone_index] != -1; ++s.zone_index)
  {
    ++DAT_008966d0;
  }
  memcpy(DAT_00924ff0, global_ante_cards[1], 0x40);
  s.zone_index = 0;
  DAT_0093d84c = 0;
  for (; s.zone_index < 0x10 && global_ante_cards[0][s.zone_index] != -1; ++s.zone_index)
  {
    ++DAT_0093d84c;
  }
  memcpy(DAT_009397d0, global_ante_cards, 0x40);

  for (s.zone_index = 0; s.zone_index <= 0x25; ++s.zone_index)
  {
    if ((int)(char)g_duel_phase_stop_settings[0].phase_flags[s.zone_index] != DAT_00925360[s.zone_index])
    {
      s.needs_refresh |= 1;
    }
    if ((int)(char)g_duel_phase_stop_settings[1].phase_flags[s.zone_index] != DAT_008b3400[s.zone_index])
    {
      s.needs_refresh |= 1;
    }
    DAT_00925360[s.zone_index] = (int)(char)g_duel_phase_stop_settings[0].phase_flags[s.zone_index] & PHASE_STOP_ENABLED;
    DAT_008b3400[s.zone_index] = (int)(char)g_duel_phase_stop_settings[1].phase_flags[s.zone_index] & PHASE_STOP_ENABLED;
  }

  if (DAT_00777848 != _DAT_00742fbc)
  {
    s.needs_refresh = 1;
  }
  DAT_00777848 = _DAT_00742fbc;
  LeaveCriticalSection(&g_duel_render_lock);
  return s.needs_refresh;
}

// FUNCTION: MAGIC 0x00449ac3
// FUNCTION: SHANDALAR 0x00453644
unsigned int get_displayed_card_original_internal_id(int player, int card)
{
  int displayed_card_type;

  if (displayed_card_indices_invalid(player, card) != 0)
  {
    return -1;
  }

  EnterCriticalSection(&g_duel_render_lock);
  displayed_card_type = DISPLAYED_PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  LeaveCriticalSection(&g_duel_render_lock);
  return displayed_card_type;
}

// FUNCTION: MAGIC 0x00559999
// FUNCTION: SHANDALAR 0x0056fce9
unsigned int draw_displayed_full_card(HDC dc, RECT *rect, card_ptr_t *raw_card, int player, int card, int draw_mode, int expand_text_box)
{
  struct
  {
    char rules_text[1000];
    int number_of_colors;
    unsigned int color_test;
    int color_index;
    card_ptr_t card_data;
    int card_version;
    card_id_t card_id;
  } s;

  if (dc == NULL || rect == NULL || raw_card == NULL)
  {
    return 0;
  }

  s.card_id = get_displayed_card_id(player, card);
  if (s.card_id == -1)
  {
    return 0;
  }

  memcpy(&s.card_data, global_raw_cards_storage + s.card_id, 0x98);
  s.color_test = get_displayed_card_color_flags(player, card);
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
  apply_displayed_card_sleight_words(player, card, s.rules_text);
  apply_displayed_card_hack_words(player, card, s.rules_text);
  s.card_data.rules_text = s.rules_text;
  s.card_version = get_card_display_pic_num(s.card_id, player, card);
  return DrawFullCard(dc,
                      rect,
                      &s.card_data,
                      s.card_version,
                      draw_mode,
                      expand_text_box,
                      gs_illus_00789130);
}

// FUNCTION: MAGIC 0x00559bc1
// FUNCTION: SHANDALAR 0x0056ff11
void draw_displayed_card_overlaid_full_card(HDC dc, RECT *rect, int player, int card)
{
  struct
  {
    card_id_t card_id;
    int displayed_player_and_card[2];
    int number_of_colors;
    unsigned int color_test;
    int color_index;
    card_ptr_t card_data;
    int card_version;
    int displayed_type;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return;
  }

  s.displayed_type = get_displayed_card_base_internal_id(player, card);
  s.card_id = CardIDFromType(s.displayed_type);
  if ((s.card_id == unk_0092666c && get_displayed_card_original_internal_id(player, card) == g_draw_card_placeholder_internal_card_id) || s.card_id == unk_009266ac)
  {
    DrawCardBack(dc, rect);
    return;
  }

  get_displayed_card_parent(s.displayed_player_and_card, player, card);
  s.card_version = get_card_display_pic_num(s.card_id, s.displayed_player_and_card[0], s.displayed_player_and_card[1]);
  if (s.card_id == unk_007a7d64 ||
      s.card_id == unk_00789b80 ||
      s.card_id == unk_008cf1ac ||
      s.card_id == unk_00789734 ||
      s.card_id == unk_008a8de8)
  {
    if (get_displayed_card_id(s.displayed_player_and_card[0], s.displayed_player_and_card[1]) != -1)
    {
      draw_special_effect_full_card(dc, rect, s.card_id, s.displayed_player_and_card[0], s.displayed_player_and_card[1]);
    }
    return;
  }

  memcpy(&s.card_data, global_raw_cards_storage + s.card_id, 0x98);
  s.color_test = get_displayed_card_color_flags(player, card);
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

  DrawFullCard(dc, rect, &s.card_data, s.card_version, 2, g_duel_interface_options.expand_text_box_on_big_card, gs_illus_00789130);
}

// FUNCTION: MAGIC 0x00559e9c
// FUNCTION: SHANDALAR 0x005701e5
void draw_special_effect_full_card(HDC dc, RECT *rect, card_id_t card_id, int player, int card)
{
  struct
  {
    unsigned int final_display_flags;
    char transformed_rules_text[400];
    int appended_text;
    int displayed_power;
    char value_text[100];
    int displayed_toughness;
    unsigned int display_flags;
    card_id_t transformed_card_id;
    char temporary_name[52];
    int single_color;
    unsigned int display_info;
    int displayed_player_and_card[2];
    int ability_index;
    card_ptr_t card_data;
    unsigned int display_version;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return;
  }

  s.display_info = get_displayed_card_display_pic_info(s.displayed_player_and_card, player, card);
  s.display_version = (unsigned short)((s.display_info >> 0x10) & 0xffff);
  s.card_data.id = s.display_info & 0xffff;

  if (card_id == unk_007a7d64)
  {
    sprintf(g_card_title_buffer_00709100, "%s: %d", gs_cardtitle_damage_008cfd30, get_displayed_card_special_counters(player, card));
    s.single_color = single_color_test_bit_to_color_t(get_displayed_card_color_flags(player, card));
    if (s.single_color == 1 ||
        s.single_color == 2 ||
        s.single_color == 4 ||
        s.single_color == 3 ||
        s.single_color == 5)
    {
      sprintf(g_card_title_buffer_00709100 + strlen(g_card_title_buffer_00709100), " (%s)", gs_color_word_capitalized_00709390[s.single_color]);
    }
  }
  else if (card_id == unk_00789b80)
  {
    sprintf(g_card_title_buffer_00709100, gs_cardtitle_hunting_00926750, get_hunting_subtype_name(get_displayed_card_special_counters(player, card)));
  }
  else if (card_id == unk_008cf1ac)
  {
    strcpy(g_card_title_buffer_00709100, gs_multiblock_creature_008cf040);
  }
  else if (card_id == unk_00789734)
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.card_data.id].effect_title);
  }
  else if (card_id == unk_008a8de8)
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.card_data.id].legacy_title);
  }
  else
  {
    g_card_title_buffer_00709100[0] = '\0';
  }

  if (card_id == unk_00789734 && get_displayed_card_eot_toughness(player, card) > 0)
  {
    strcpy(s.temporary_name, g_card_title_buffer_00709100);
    extract_numbered_text_choice(g_card_title_buffer_00709100, s.temporary_name, get_displayed_card_eot_toughness(player, card));
  }

  s.transformed_card_id = get_displayed_card_id(s.displayed_player_and_card[0], s.displayed_player_and_card[1]);
  if (card_id == unk_00789734 &&
      (s.transformed_card_id == CARD_ID_FAERIE_DRAGON ||
       s.transformed_card_id == CARD_ID_WHIMSY))
  {
    strcpy(g_card_title_buffer_00709100, global_legacy_names[s.transformed_card_id].effect_title);
  }

  s.card_data.name = g_card_title_buffer_00709100;
  s.card_data.full_name = s.card_data.name;
  s.card_data.expansion = 0xffffffff;
  s.card_data.color = 0xffffffff;
  s.card_data.card_type = 0;
  s.card_data.subtype = 0xffffffff;
  s.card_data.type_text = NULL;
  s.card_data.db_card_type_2 = 0xffffffff;
  s.card_data.rarity = 0;
  for (s.ability_index = 0; s.ability_index < 10; ++s.ability_index)
  {
    ((char *)&s.card_data.req)[s.ability_index] = 0;
  }
  s.card_data.unused0x3c = 0xffffffff;
  s.card_data.artist = NULL;
  s.card_data.num_pics = 0;
  s.card_data.ai_against_color = s.card_data.ai_for_color = s.card_data.ai_counts_as_color = 0;
  s.card_data.ai_against_land = s.card_data.ai_for_land = s.card_data.ai_counts_as_land = 0;
  s.card_data.expansion_rarity = 0;

  if (card_id == unk_007a7d64)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].damage_text);
  }
  else if (card_id == unk_00789b80)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].effect_text);
  }
  else if (card_id == unk_008cf1ac)
  {
    strcpy(g_card_rules_text_buffer_00708da8, gs_multiblock_shadow_blocker_00925e70);
  }
  else if (card_id == unk_00789734)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].effect_text);
  }
  else if (card_id == unk_008a8de8)
  {
    strcpy(g_card_rules_text_buffer_00708da8, global_legacy_names[s.card_data.id].legacy_text);
  }
  else
  {
    g_card_rules_text_buffer_00708da8[0] = '\0';
  }

  if ((get_displayed_card_display_flags(player, card) & 0x2000000) != 0)
  {
    apply_displayed_card_sleight_words(s.displayed_player_and_card[0], s.displayed_player_and_card[1], g_card_rules_text_buffer_00708da8);
    apply_displayed_card_hack_words(s.displayed_player_and_card[0], s.displayed_player_and_card[1], g_card_rules_text_buffer_00708da8);
  }
  else
  {
    apply_displayed_card_sleight_words(player, card, g_card_rules_text_buffer_00708da8);
    apply_displayed_card_hack_words(player, card, g_card_rules_text_buffer_00708da8);
  }

  replace_text_token(g_card_rules_text_buffer_00708da8, "|H1", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|H2", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|H3", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|H4", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|H", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|S1", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|S2", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|S3", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|S4", 1, "");
  replace_text_token(g_card_rules_text_buffer_00708da8, "|S", 1, "");

  get_displayed_card_counter_power_toughness(player, card, &s.displayed_power, &s.displayed_toughness);
  sprintf(s.value_text, "+%d/+%d", s.displayed_power, s.displayed_toughness);
  replace_text_token(g_card_rules_text_buffer_00708da8, "+|n/+|n", 0, s.value_text);
  sprintf(s.value_text, "+0/+%d", s.displayed_toughness);
  replace_text_token(g_card_rules_text_buffer_00708da8, "+0/+|n", 0, s.value_text);
  sprintf(s.value_text, "+%d/+0", s.displayed_power);
  replace_text_token(g_card_rules_text_buffer_00708da8, "+|n/+0", 0, s.value_text);
  sprintf(s.value_text, "%d/-0", s.displayed_power);
  replace_text_token(g_card_rules_text_buffer_00708da8, "-|n/-0", 0, s.value_text);
  sprintf(s.value_text, "+%d/%d", s.displayed_power, s.displayed_toughness);
  replace_text_token(g_card_rules_text_buffer_00708da8, "+|n/-|n", 0, s.value_text);
  sprintf(s.value_text, "%d", s.displayed_toughness);
  replace_text_token(g_card_rules_text_buffer_00708da8, "|n", 0, s.value_text);

  if (s.card_data.id == 0x21b)
  {
    s.display_flags = get_displayed_card_special_counters(player, card);
    s.appended_text = 0;
    strcpy(s.value_text, "");
    if ((s.display_flags & 0x20) != 0)
    {
      if (s.appended_text != 0)
      {
        strcat(s.value_text, ", ");
      }
      strcat(s.value_text, gs_ability_word_008b4de0[0]);
      s.appended_text = 1;
    }
    if ((s.display_flags & 0x100) != 0)
    {
      if (s.appended_text != 0)
      {
        strcat(s.value_text, ", ");
      }
      strcat(s.value_text, gs_ability_word_008b4de0[4]);
      s.appended_text = 1;
    }
    if ((s.display_flags & 0x40) != 0)
    {
      if (s.appended_text != 0)
      {
        strcat(s.value_text, ", ");
      }
      strcat(s.value_text, gs_ability_word_008b4de0[2]);
      s.appended_text = 1;
    }
    if ((s.display_flags & 0x80) != 0)
    {
      if (s.appended_text != 0)
      {
        strcat(s.value_text, ", ");
      }
      strcat(s.value_text, gs_ability_word_008b4de0[3]);
      s.appended_text = 1;
    }
    replace_text_token(g_card_rules_text_buffer_00708da8, "|A", 0, s.value_text);
    get_displayed_card_counter_power_toughness(player, card, &s.displayed_power, &s.displayed_toughness);
    sprintf(s.value_text, "-%d/-%d", s.displayed_power, s.displayed_toughness);
    replace_text_token(g_card_rules_text_buffer_00708da8, "-|n/-|n", 0, s.value_text);
  }

  if (card_id == unk_00789734 && get_displayed_card_eot_toughness(player, card) > 0)
  {
    strcpy(s.transformed_rules_text, g_card_rules_text_buffer_00708da8);
    extract_numbered_text_choice(g_card_rules_text_buffer_00708da8, s.transformed_rules_text, get_displayed_card_eot_toughness(player, card));
  }

  if (card_id == unk_007a7d64)
  {
    s.final_display_flags = get_displayed_card_display_flags(player, card);
    if (g_card_rules_text_buffer_00708da8[0] != '\0' && g_card_rules_text_buffer_00708da8[0] != '\n')
    {
      strcat(g_card_rules_text_buffer_00708da8, "\n");
    }
    if ((s.final_display_flags & 0x100000) != 0)
    {
      strcat(g_card_rules_text_buffer_00708da8, gs_ability_word_008b4de0[4]);
      strcat(g_card_rules_text_buffer_00708da8, "\n");
    }
    if ((s.final_display_flags & 0x80000) != 0)
    {
      strcat(g_card_rules_text_buffer_00708da8, gs_ability_word_008b4de0[3]);
    }
  }

  s.card_data.rules_text = g_card_rules_text_buffer_00708da8;
  s.card_data.flavor_text = "";
  s.card_data.power = 0;
  s.card_data.toughness = 0;
  s.card_data.sleight_color = 0;
  *(int *)&s.card_data.currently_zero_for_all_cards = 0;
  DrawFullCard(dc, rect, &s.card_data, s.display_version, 2, g_duel_interface_options.expand_text_box_on_big_card, gs_illus_00789130);
}

// FUNCTION: MAGIC 0x00506fa0
// FUNCTION: SHANDALAR 0x004c5ca0
INT_PTR CALLBACK big_card_choice_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HGDIOBJ timer_font;
    char card_coordinates[12];
    HDC paint_dc;
    PAINTSTRUCT paint_struct;
    int picture_number;
    card_id_t card_id;
    card_id_t card_image_number;
    int hovered_player_and_card[2];
    POINT cursor_point;
    card_id_t bigcard_image;
    card_id_t smallcard_image;
    RECT before_drag_rect;
    unsigned int was_dragged;
    RECT after_drag_rect;
    HDC erase_dc;
    RECT erase_rect;
    LRESULT current_selection;
    UINT notification_code;
    HGDIOBJ old_font;
    LOGFONTA listbox_font_info;
    HFONT bold_font;
    HANDLE listbox_font_handle;
    int smallcard_player_and_card[2];
    HWND smallcard_window;
    RECT listbox_rect;
    int listbox_top;
    int listbox_right_margin;
    RECT client_rect;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_big_card_choice_state_006f6e20 = 0;
    if (g_duel_network_state != 0)
    {
      SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
    }

    GetWindowRect(GetDlgItem(hwnd, 0x3fd), &s.listbox_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.listbox_rect, 2);
    GetClientRect(hwnd, &s.client_rect);
    s.listbox_top = s.listbox_rect.top;
    s.listbox_right_margin = s.client_rect.right - s.listbox_rect.right;
    InflateRect(&s.client_rect, -s.listbox_top, -s.listbox_right_margin);
    CopyRect(&g_big_card_choice_full_card_rect, &s.client_rect);
    g_big_card_choice_full_card_rect.right = s.listbox_rect.left - s.listbox_top;
    CopyRect(&g_big_card_choice_small_card_rect, &s.client_rect);
    g_big_card_choice_small_card_rect.left = s.listbox_rect.left;
    g_big_card_choice_small_card_rect.top = s.listbox_rect.bottom;
    if (g_big_card_choice_small_card_rect.bottom - g_big_card_choice_small_card_rect.top <
        g_big_card_choice_small_card_rect.right - g_big_card_choice_small_card_rect.left)
    {
      g_big_card_choice_small_card_rect.right =
          g_big_card_choice_small_card_rect.bottom - g_big_card_choice_small_card_rect.top + g_big_card_choice_small_card_rect.left;
    }
    else
    {
      g_big_card_choice_small_card_rect.bottom =
          g_big_card_choice_small_card_rect.right - g_big_card_choice_small_card_rect.left + g_big_card_choice_small_card_rect.top;
    }

    g_big_card_choice_dialog_context = (dialog_context_t *)lparam;
    SetDlgItemTextA(hwnd, 0x3fd, g_big_card_choice_dialog_context->prompt);
    SendDlgItemMessageA(hwnd, 0x3fd, 0x401, g_big_card_choice_dialog_context->dialog_mode, 0);
    if (g_big_card_choice_dialog_context->smallcard_player != -1)
    {
      s.smallcard_player_and_card[0] = g_big_card_choice_dialog_context->smallcard_player;
      s.smallcard_player_and_card[1] = g_big_card_choice_dialog_context->smallcard_card;
      s.smallcard_window = CreateWindowExA(0,
                                           "MAGICGAME_BigCardCardClass",
                                           "BigCard small card",
                                           0x50000000,
                                           g_big_card_choice_small_card_rect.left,
                                           g_big_card_choice_small_card_rect.top,
                                           g_showlist_smallcard_width,
                                           g_showlist_smallcard_height,
                                           hwnd,
                                           (HMENU)1,
                                           g_app_instance,
                                           s.smallcard_player_and_card);
    }

    s.listbox_font_handle = (HANDLE)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
    GetObjectA(s.listbox_font_handle, sizeof(s.listbox_font_info), &s.listbox_font_info);
    s.listbox_font_info.lfWeight = 700;
    s.bold_font = CreateFontIndirectA(&s.listbox_font_info);
    SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, (WPARAM)s.bold_font, 0);
    SetFocus(GetDlgItem(hwnd, 0x3fd));
    if (SendDlgItemMessageA(hwnd, 0x3fd, 0x400, 0, 0) == 0 || g_big_card_choice_dialog_context->dialog_mode == 0)
    {
      SetTimer(hwnd, 1, g_big_card_choice_timeout_ms, (TIMERPROC)0);
    }
    return 0;

  case WM_COMMAND:
    if ((lparam & 0xffff) == 1 || (lparam & 0xffff) == 2)
    {
      SendMessageA(hwnd, WM_COMMAND, 0x3fd, (LPARAM)GetDlgItem(hwnd, 0x3fd));
    }
    else if ((wparam & 0xffff) == 0x3fd)
    {
      s.notification_code = HIWORD(wparam);
      s.current_selection = SendDlgItemMessageA(hwnd, 0x3fd, 0x400, 0, 0);
      if (s.notification_code != 0)
      {
        s.old_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
        SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
        DeleteObject(s.old_font);
        EndDialog(hwnd, s.notification_code);
      }
      else if (s.current_selection == 0 || g_big_card_choice_dialog_context->dialog_mode == 0)
      {
        s.old_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
        SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
        DeleteObject(s.old_font);
        EndDialog(hwnd, s.notification_code);
      }
    }
    return 1;

  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
  case WM_QUERYNEWPALETTE:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, (int)lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_magicgame_big_card_choice_pic != 0)
    {
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, (HBITMAP)g_magicgame_big_card_choice_pic);
    }
    else
    {
      FillRect(s.erase_dc, &s.erase_rect, GetStockObject(0));
    }
    return 1;

  case WM_LBUTTONDOWN:
    GetWindowRect(hwnd, &s.before_drag_rect);
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    GetWindowRect(hwnd, &s.after_drag_rect);
    if (abs(s.before_drag_rect.top - s.after_drag_rect.top) +
            abs(s.before_drag_rect.left - s.after_drag_rect.left) <
        5)
    {
      s.was_dragged = 0;
    }
    else
    {
      s.was_dragged = 1;
    }
    if (s.was_dragged == 0)
    {
      SendMessageA(hwnd, WM_COMMAND, 0x3fd, (LPARAM)GetDlgItem(hwnd, 0x3fd));
    }
    return 1;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    s.cursor_point.x = lparam & 0xffff;
    s.cursor_point.y = HIWORD(lparam);
    if ((msg == WM_MOUSEMOVE && g_duel_interface_options.layout != 2) || (msg == WM_RBUTTONDOWN && g_duel_interface_options.layout == 2))
    {
      s.bigcard_image = get_displayed_card_id(g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card);
      if (g_big_card_choice_dialog_context->smallcard_player == -1 || g_big_card_choice_dialog_context->smallcard_card == -1)
      {
        s.smallcard_image = -1;
      }
      else
      {
        s.smallcard_image = get_displayed_card_id(g_big_card_choice_dialog_context->smallcard_player, g_big_card_choice_dialog_context->smallcard_card);
      }

      if (s.bigcard_image != -1 && PtInRect(&g_big_card_choice_full_card_rect, s.cursor_point) != 0)
      {
        s.hovered_player_and_card[0] = g_big_card_choice_dialog_context->bigcard_player;
        s.hovered_player_and_card[1] = g_big_card_choice_dialog_context->bigcard_card;
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, (WPARAM)s.bigcard_image, (LPARAM)s.hovered_player_and_card);
      }
      else if (s.smallcard_image != -1)
      {
        if (PtInRect(&g_big_card_choice_small_card_rect, s.cursor_point) != 0)
        {
          s.hovered_player_and_card[0] = g_big_card_choice_dialog_context->smallcard_player;
          s.hovered_player_and_card[1] = g_big_card_choice_dialog_context->smallcard_card;
          SendMessageA(g_duel_card_preview_window_hwnd, 0x401, (WPARAM)s.smallcard_image, (LPARAM)s.hovered_player_and_card);
        }
      }
    }
    return 0;

  case WM_PAINT:
    UpdateWindow(GetDlgItem(hwnd, 0x3fd));
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != 0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      s.card_image_number = get_displayed_card_id(g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card);
      s.card_id = CardIDFromType(get_displayed_card_original_internal_id(g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card));
      if (s.card_image_number != -1)
      {
        s.picture_number = get_card_display_pic_num(s.card_image_number,
                                                    g_big_card_choice_dialog_context->bigcard_player,
                                                    g_big_card_choice_dialog_context->bigcard_card);
        if (s.card_image_number == unk_009266ac)
        {
          DrawCardBack(s.paint_dc, &g_big_card_choice_full_card_rect);
        }
        else if (s.card_image_number == unk_007a7d64 ||
                 s.card_image_number == unk_00789b80 ||
                 s.card_image_number == unk_008cf1ac ||
                 s.card_image_number == unk_00789734 ||
                 s.card_image_number == unk_008a8de8)
        {
          draw_special_effect_full_card(s.paint_dc,
                                        &g_big_card_choice_full_card_rect,
                                        s.card_image_number,
                                        g_big_card_choice_dialog_context->bigcard_player,
                                        g_big_card_choice_dialog_context->bigcard_card);
        }
        else if (s.card_image_number == unk_0092666c)
        {
          draw_displayed_card_overlaid_full_card(s.paint_dc, &g_big_card_choice_full_card_rect, g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card);
        }
        else
        {
          draw_displayed_full_card(s.paint_dc,
                                   &g_big_card_choice_full_card_rect,
                                   global_raw_cards_storage + s.card_image_number,
                                   g_big_card_choice_dialog_context->bigcard_player,
                                   g_big_card_choice_dialog_context->bigcard_card,
                                   0x12,
                                   0);
        }

        if (g_duel_dialog_refresh_state != 0)
        {
          sprintf(s.card_coordinates, "%d,%d", g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card);
          SetBkMode(s.paint_dc, 1);
          SetTextColor(s.paint_dc, 0);
          TextOutA(s.paint_dc,
                   g_big_card_choice_full_card_rect.left + 5,
                   g_big_card_choice_full_card_rect.top +
                       ((g_big_card_choice_full_card_rect.bottom - g_big_card_choice_full_card_rect.top) * 0x14) / 100,
                   s.card_coordinates,
                   strlen(s.card_coordinates));
          SetTextColor(s.paint_dc, 0xffffff);
          TextOutA(s.paint_dc,
                   g_big_card_choice_full_card_rect.left + 4,
                   g_big_card_choice_full_card_rect.top +
                       ((g_big_card_choice_full_card_rect.bottom - g_big_card_choice_full_card_rect.top) * 0x14) / 100 - 1,
                   s.card_coordinates,
                   strlen(s.card_coordinates));
        }

        (void)s.picture_number;
      }
      else if (s.card_id != -1 &&
               s.card_id != unk_009266ac &&
               s.card_id != unk_007a7d64 &&
               s.card_id != unk_00789b80 &&
               s.card_id != unk_008cf1ac &&
               s.card_id != unk_00789734 &&
               s.card_id != unk_008a8de8 &&
               s.card_id != unk_0092666c)
      {
        DrawFullCard(s.paint_dc, &g_big_card_choice_full_card_rect, global_raw_cards_storage + s.card_id, 0, 0x12, 0, gs_illus_00789130);
        if (g_duel_dialog_refresh_state != 0)
        {
          sprintf(s.card_coordinates, "%d,%d", g_big_card_choice_dialog_context->bigcard_player, g_big_card_choice_dialog_context->bigcard_card);
          SetBkMode(s.paint_dc, 1);
          SetTextColor(s.paint_dc, 0);
          TextOutA(s.paint_dc,
                   g_big_card_choice_full_card_rect.left + 5,
                   g_big_card_choice_full_card_rect.top +
                       ((g_big_card_choice_full_card_rect.bottom - g_big_card_choice_full_card_rect.top) * 0x14) / 100,
                   s.card_coordinates,
                   strlen(s.card_coordinates));
          SetTextColor(s.paint_dc, 0xffffff);
          TextOutA(s.paint_dc,
                   g_big_card_choice_full_card_rect.left + 4,
                   g_big_card_choice_full_card_rect.top +
                       ((g_big_card_choice_full_card_rect.bottom - g_big_card_choice_full_card_rect.top) * 0x14) / 100 - 1,
                   s.card_coordinates,
                   strlen(s.card_coordinates));
        }
      }

      EndPaint(hwnd, &s.paint_struct);
    }
    return 1;

  case WM_TIMER:
    s.timer_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x3fd, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x3fd, WM_SETFONT, 0, 0);
    DeleteObject(s.timer_font);
    EndDialog(hwnd, 0);
    return 1;

  default:
    return 0;
  }
}
