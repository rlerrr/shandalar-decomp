#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/magsnd.h"
#include "game_support.h"

typedef ptrdiff_t INT_PTR;

typedef int(__cdecl *in_play_card_callback_t)(int source_player,
                                              int source_card,
                                              int player,
                                              int card,
                                              int internal_card_id);

extern card_ptr_t global_raw_cards_storage[2000];
extern int DAT_00777854;

int GetNextManaSymbol(char **param_1);

int can_target(target_definition_t *td)
{
  return td != NULL;
}

// FUNCTION: MAGIC 0x00551ed7
int FUN_00551ed7(int player, unsigned int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  if (C_real_select_target(player,
                           2,
                           preferred_controller,
                           TARGET_ZONE_IN_PLAY,
                           TYPE_ARTIFACT,
                           TYPE_NONE,
                           0,
                           get_protections_from(player, card),
                           COLOR_TEST_0,
                           COLOR_TEST_0,
                           -1,
                           ~SUB_WALL,
                           -1,
                           -1,
                           0,
                           0,
                           0,
                           text_lines[0],
                           1,
                           &selected_target))
  {

    global_card_instances[player][card].targets[global_card_instances[player][card].number_of_targets].player = selected_target.player;
    global_card_instances[player][card].targets[global_card_instances[player][card].number_of_targets].card = selected_target.card;
    ++global_card_instances[player][card].number_of_targets;

    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x00404c4c
int FUN_00404c4c(int player, int internal_card_id)
{
  int card;
  int result;

  result = 1;
  for (card = 0; card < active_cards_count[player]; ++card)
  {
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id && (PLAYER_CARD_INSTANCE(player, card).state & 2) == 0)
    {
      ++result;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00404cff
int FUN_00404cff(int player, int internal_card_id, int who_to_check)
{
  int card;
  int result;

  result = 0;
  for (player = 0; player < 2; ++player)
  {
    if (who_to_check == -1 || who_to_check == player)
    {
      for (card = 0; card < active_cards_count[player]; ++card)
      {
        if (is_in_play(player, card) && PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id)
        {
          ++result;
        }
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00449bef
void FUN_00449bef(char *name)
{
  char *source;

  if (name == 0)
  {
    return;
  }

  source = unk_009266d0;
  while (*source != '\0' && *source != '-')
  {
    *name = *source;
    ++source;
    ++name;
  }
  *name = '\0';
}

int charge_mana_for_activated_ability(int player, int card, int colorless, int black, int blue, int green, int red, int white)
{
  (void)player;
  (void)card;
  (void)colorless;
  (void)black;
  (void)blue;
  (void)green;
  (void)red;
  (void)white;
  return 0;
}

void default_target_definition(int player, int card, target_definition_t *td, int type)
{
  memset(td, 0, sizeof(*td));
  td->who_chooses = player;
  td->allowed_controller = ANYBODY;
  td->preferred_controller = 1 - player;
  td->zone = TARGET_ZONE_IN_PLAY;
  td->required_type = type;
  td->player = player;
  td->card = card;
}

// FUNCTION: MAGIC 0x004464f0
int is_in_play(int player, int card)
{
  return player >= 0 && player < 2 && card >= 0 && card < active_cards_count[player] && global_card_instances[player][card].internal_card_id != -1 && (global_card_instances[player][card].state & STATE_IN_PLAY) != 0;
}

// FUNCTION: MAGIC 0x005001e0
int get_hacked_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].hack_mode[orig_color] != 0)
  {
    orig_color = (char)global_card_instances[player][card].hack_mode[orig_color];
  }

  return orig_color;
}

// FUNCTION: MAGIC 0x0050026d
int get_sleighted_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].color_id[orig_color] == 0)
  {
    return orig_color;
  }
  else
  {
    return (char)global_card_instances[player][card].color_id[orig_color];
  }
}

// FUNCTION: MOK 0x004358f0
// FUNCTION: MAGIC 0x00442763
color_t single_color_test_bit_to_color_t(color_test_t color_test)
{
  if ((color_test & COLOR_TEST_BLACK) != 0)
  {
    return COLOR_BLACK;
  }
  if ((color_test & COLOR_TEST_BLUE) != 0)
  {
    return COLOR_BLUE;
  }
  if ((color_test & COLOR_TEST_GREEN) != 0)
  {
    return COLOR_GREEN;
  }
  if ((color_test & COLOR_TEST_RED) != 0)
  {
    return COLOR_RED;
  }
  if ((color_test & COLOR_TEST_WHITE) != 0)
  {
    return COLOR_WHITE;
  }

  return COLOR_COLORLESS;
}

// FUNCTION: MAGIC 0x004087cc
int FUN_004087cc(int player, unsigned int type)
{
  int score;
  int best_score;
  int graveyard_index;
  int best_index;
  int internal_card_id;

  best_score = 0;
  best_index = -1;
  if (player == -1)
  {
    return -1;
  }

  for (graveyard_index = 0; graveyard_index < 500 && global_graveyard_slots[player][graveyard_index] != -1;
       ++graveyard_index)
  {
    internal_card_id = global_graveyard_slots[player][graveyard_index];
    if (type == -1 || (type & global_cards_data[internal_card_id].type) != 0)
    {
      score = (char)global_cards_data[internal_card_id].cc[0] * 3 + abs((int)(char)global_cards_data[internal_card_id].cc[1]) * 2;
      if (best_score < score)
      {
        best_index = graveyard_index;
        best_score = score;
      }
    }
  }

  return best_index;
}

// FUNCTION: MAGIC 0x004832f4
int FUN_004832f4(int player, unsigned char type)
{
  int graveyard_index;
  int result;

  result = 0;
  if (player == -1 || player == unk_008b35ec)
  {
    graveyard_index = 0;
    while (graveyard_index < 500)
    {
      if (global_graveyard_slots[unk_008b35ec][graveyard_index] == -1)
      {
        break;
      }
      else if ((type & global_cards_data[global_graveyard_slots[unk_008b35ec][graveyard_index]].type) != 0)
      {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  if ((player == -1 || player == active_player) && result == 0)
  {
    graveyard_index = 0;
    while (graveyard_index < 500)
    {
      if (global_graveyard_slots[active_player][graveyard_index] == -1)
      {
        break;
      }
      else if ((type & global_cards_data[global_graveyard_slots[active_player][graveyard_index]].type) != 0)
      {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00484581
int FUN_00484581(int internal_card_id, color_t color)
{
  int result;

  result = 0;
  if ((char)global_cards_data[internal_card_id].subtype == 13 && *(int *)&global_cards_data[internal_card_id].id == unk_0092664c[color])
  {
    result = 1;
  }
  if ((char)global_cards_data[internal_card_id].subtype == 12 && ((1 << color) & (int)*(char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).mana_color) != 0)
  {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x0053aa74
unsigned int get_protections_from(int player, int card)
{
  unsigned char type;
  keyword_t illegal_abilities;
  int internal_card_id;

  illegal_abilities = 0;
  internal_card_id = global_card_instances[player][card].internal_card_id;
  if (internal_card_id == -1 || internal_card_id == unk_0091a80c)
  {
    type = global_cards_data[global_card_instances[player][card].original_internal_card_id].type;
  }
  else
  {
    type = global_cards_data[internal_card_id].type;
  }

  if ((type & TYPE_SORCERY) != 0)
  {
    illegal_abilities = KEYWORD_PROT_SORCERIES;
  }
  if ((type & TYPE_INSTANT) != 0)
  {
    illegal_abilities |= KEYWORD_PROT_INSTANTS;
  }
  if ((type & TYPE_INTERRUPT) != 0)
  {
    illegal_abilities |= KEYWORD_PROT_INTERRUPTS;
  }
  if ((type & TYPE_ENCHANTMENT) != 0)
  {
    illegal_abilities |= KEYWORD_PROT_ENCHANTMENTS;
  }
  if ((type & TYPE_ARTIFACT) != 0)
  {
    illegal_abilities |= KEYWORD_PROT_ARTIFACTS;
  }

  return (0x800 << (((char)single_color_test_bit_to_color_t(global_card_instances[player][card].color) - 1U) & 0x1f)) | illegal_abilities;
}

// FUNCTION: MAGIC 0x004a62d7
int FUN_004a62d7(int player,
                 int *graveyard,
                 void *available,
                 int count,
                 int prompt,
                 int a6,
                 int selected,
                 int a8,
                 int a9)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }
  return FUN_0049e8bb(player, graveyard, 0, available, count, prompt, a6, selected, a8, a9);
}

int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  (void)player;
  (void)card;
  return orig_color_test;
}

// FUNCTION: MAGIC 0x004b5f03
int FUN_004b5f03(int card_id)
{
  int internal_card_id;

  if (card_id != -1)
  {
    for (internal_card_id = 0; internal_card_id < unk_0093f4b8 + 0x10; ++internal_card_id)
    {
      if (global_cards_data[internal_card_id].id == card_id)
      {
        return internal_card_id;
      }
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x004b41f2
int FUN_004b41f2(int player,
                 int *graveyard,
                 int *allowed_cards,
                 int count,
                 const char *prompt,
                 int allow_cancel,
                 int *title)
{
  int current_card;

  (void)player;
  (void)prompt;
  (void)allow_cancel;
  (void)title;

  for (current_card = 0; current_card < count && graveyard[current_card] != -1; ++current_card)
  {
    if (allowed_cards[current_card] != 0)
    {
      return current_card;
    }
  }

  return -1;
}

// FUNCTION: MAGIC 0x00483e3e
int FUN_00483e3e(int player, unsigned int type_mask)
{
  int best_score;
  int best_slot;
  int current_slot;
  int current_internal_id;
  int current_score;
  unsigned int abilities;

  if (player == -1)
  {
    return -1;
  }

  best_score = -999;
  best_slot = -1;

  for (current_slot = 0; current_slot < 500 && global_library[player][current_slot] != -1; ++current_slot)
  {
    current_internal_id = global_library[player][current_slot];
    if (type_mask == 0xffffffff || (type_mask & global_cards_data[current_internal_id].type) != 0)
    {
      current_score = global_cards_data[current_internal_id].power + global_cards_data[current_internal_id].toughness;
      if ((global_cards_data[current_internal_id].type & TYPE_ENCHANTMENT) != 0)
      {
        ++current_score;
      }
      if ((global_cards_data[current_internal_id].type & TYPE_ARTIFACT) != 0)
      {
        ++current_score;
      }
      abilities = global_cards_data[current_internal_id].static_ability;
      while (abilities != 0)
      {
        if ((abilities & 1) != 0)
        {
          ++current_score;
        }
        abilities >>= 1;
      }
      if ((global_cards_data[current_internal_id].extra_ability & 0x1000) != 0)
      {
        current_score += 2;
      }
      if ((global_cards_data[current_internal_id].extra_ability & 1) != 0)
      {
        current_score += 2;
      }
      if (best_score < current_score)
      {
        best_score = current_score;
        best_slot = current_slot;
      }
    }
  }

  return best_slot;
}

// FUNCTION: MAGIC 0x0040a2ae
int drain_power_draw_mana_from_land(int player, int card, int internal_card_id)
{
  unk_00938e2c = 0x3e;
  produced_mana_color = -1;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0)
  {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if (spell_fizzled == 1)
    {
      spell_fizzled = -1;
      unk_00938e2c = 1;
      dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
      unk_00938e2c = 0x3e;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }

  unk_00938e2c = 0;
  return 0;
}

// FUNCTION: MAGIC 0x00481e25
int FUN_00481e25(int player, int card, int event)
{
  int parent_player;
  int parent_card;

  parent_player = *((char *)&PLAYER_CARD_INSTANCE(player, card) + 8);
  parent_card = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x44));

  if (event == 0x78 && card == affected_card && player == affected_card_controller && (*((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x20) == attacking_card || *((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x20) == *((char *)&PLAYER_CARD_INSTANCE(attacking_card_controller, attacking_card) + 0x20)))
  {
    event_result = 1;
  }

  dispatch_function_to_all_cards_in_play(player, card, FUN_004821f5, -1);

  if (trigger_condition == 0xdf && card == affected_card && player == affected_card_controller && current_turn == player && player != human_player && player == trigger_cause_controller && card == trigger_cause)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e)
    {
      unk_007abc78 |= 4;
      *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x14)) |= 0x8000000;
    }
  }

  if ((trigger_condition == 0xcc || event == 199) && card == affected_card && player == affected_card_controller && current_turn == player)
  {
    if (event == 0x7d)
    {
      event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      kill_card(player, card, KILL_BURY);
      unk_007abc78 &= 0xfffffffb;
      *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x14)) &= 0xf7ffffff;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0048194e
int FUN_0048194e(int preferred_player, int only_player, int *target_data)
{
  int mana_colors[8];
  int card_colors[8];
  int land_scores[8];
  int player;
  int card;
  int color;
  int best_land_color;
  int best_card_color;
  int best_score;
  int current_score;
  unsigned int colors;
  int internal_card_id;

  target_data[1] = -1;
  target_data[0] = -1;

  for (color = 0; color < 6; ++color)
  {
    card_colors[color] = 0;
    land_scores[color] = 0;
    mana_colors[color] = 0;
  }

  for (player = 0; player < 2; ++player)
  {
    if (only_player == -1 || player == only_player)
    {
      for (card = 0; card < active_cards_count[player]; ++card)
      {
        if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1)
        {
          if ((PLAYER_CARD_INSTANCE(player, card).state & 0x800022) == 2)
          {
            if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0)
            {
              for (color = 1; color < 6; ++color)
              {
                if (((int)(char)PLAYER_CARD_INSTANCE(player, card).mana_color & (1 << (((unsigned char)color) & 0x1f))) != 0)
                {
                  ++mana_colors[color];
                }
              }
            }
            if ((PLAYER_CARD_INSTANCE(player, card).color & 0x3e) != 0)
            {
              for (color = 1; color < 6; ++color)
              {
                if ((PLAYER_CARD_INSTANCE(player, card).color & (1 << (((unsigned char)color) & 0x1f))) != 0)
                {
                  ++land_scores[color];
                }
              }
            }
          }
          else if (preferred_player == only_player || only_player == -1)
          {
            colors = (unsigned int)(char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color;
            for (color = 1; color < 6; ++color)
            {
              if ((colors & (1 << (((unsigned char)color) & 0x1f))) != 0)
              {
                ++card_colors[color];
              }
            }
          }
        }
      }
    }
  }

  best_card_color = 0;
  best_land_color = 0;
  for (color = 1; color < 6; ++color)
  {
    if (land_scores[best_land_color] < land_scores[color])
    {
      best_land_color = color;
    }
    if (card_colors[best_card_color] < card_colors[color])
    {
      best_card_color = color;
    }
  }

  best_score = -1;
  for (player = 0; player < 2; ++player)
  {
    if (only_player == -1 || player == only_player)
    {
      for (card = 0; card < active_cards_count[player]; ++card)
      {
        if (is_in_play(player, card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_LAND) != 0)
        {
          current_score = 0;
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 1) != 0)
          {
            current_score = 2;
            if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
            {
              current_score = 5;
            }
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0)
          {
            current_score += 1;
            colors = (unsigned int)(char)PLAYER_CARD_INSTANCE(player, card).mana_color;
            for (color = 1; color < 6; ++color)
            {
              if ((colors & (1 << (((unsigned char)color) & 0x1f))) != 0)
              {
                internal_card_id = 4 - mana_colors[color];
                if (internal_card_id < 1)
                {
                  internal_card_id = 0;
                }
                current_score += internal_card_id;
              }
            }
            if (best_land_color != 0 && (colors & (1 << (((unsigned char)best_land_color) & 0x1f))) != 0)
            {
              ++current_score;
            }
            if ((preferred_player == only_player || only_player == -1) && best_card_color != 0 && (colors & (1 << (((unsigned char)best_card_color) & 0x1f))) != 0)
            {
              ++current_score;
            }
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0)
          {
            ++current_score;
          }
          if (best_score < current_score)
          {
            best_score = current_score;
            target_data[0] = player;
            target_data[1] = card;
          }
        }
      }
    }
  }

  return target_data[0] != -1;
}

// FUNCTION: MAGIC 0x004821f5
int FUN_004821f5(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  (void)internal_card_id;

  if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == source_player && PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == source_card)
  {
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player = PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_player;
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card = PLAYER_CARD_INSTANCE(source_player, source_card).damage_source_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b59b2
void FUN_004b59b2(int player, int deck_owner)
{
  int deck_size;
  int current_slot;
  int swap_slot;
  int temp;

  (void)player;

  if (unk_008a9000 != 1)
  {
    play_sound_effect(WAV_SHUFFLE);
  }

  deck_size = 0;
  while (deck_size < 500 && global_library[deck_owner][deck_size] != -1)
  {
    ++deck_size;
  }

  for (current_slot = 0; current_slot < deck_size; ++current_slot)
  {
    swap_slot = current_slot + internal_rand(deck_size - current_slot);
    if (swap_slot < deck_size && global_library[deck_owner][swap_slot] != -1)
    {
      temp = global_library[deck_owner][swap_slot];
      global_library[deck_owner][swap_slot] = global_library[deck_owner][current_slot];
      global_library[deck_owner][current_slot] = temp;
    }
  }

  for (current_slot = deck_size; current_slot < 500; ++current_slot)
  {
    global_library[deck_owner][current_slot] = -1;
  }
}

// FUNCTION: MAGIC 0x004b5c94
void remove_card_from_deck(int player, int position)
{
  int current_slot;

  for (current_slot = position; current_slot < 499; ++current_slot)
  {
    global_library[player][current_slot] = global_library[player][current_slot + 1];
  }

  global_library[player][499] = -1;
}

// FUNCTION: MAGIC 0x004b0b53
int regenerate_or_graveyard_triggers(void)
{
  TENTATIVE_reassess_all_cards();
  return 0;
}

// FUNCTION: MAGIC 0x0052d761
int FUN_0052d761(int source_player, int source_card, int test_player, int test_card, int internal_card_id)
{
  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    damage_creature(test_player, test_card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dccd
int FUN_0054dccd(int player, int card)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
  {
    if (current_card != card && is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005510dc
int FUN_005510dc(int player, int type_mask)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) && (type_mask & global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b15f7
void FUN_004b15f7(int player, int graveyard_index)
{
  for (; graveyard_index < 499; ++graveyard_index)
  {
    global_graveyard_slots[player][graveyard_index] = global_graveyard_slots[player][graveyard_index + 1];
  }
  global_graveyard_slots[player][499] = -1;
}

// FUNCTION: MOK 0x0047936d
// FUNCTION: MAGIC 0x004b42aa
int show_deck(int player, int *cards, int count, void *context, int suppress_done_txt, char *prompt)
{
  struct
  {
    int result;
    int display_count;
    int y_positions[500];
    int x_positions[500];
    int y;
    int x;
    int valid_count;
    int valid_graveyards[488];
    int draw_args[6];
    int padding[6];
    int local_c;
    int local_8;
  } s;

  if (((active_player == player) && ((unk_00926804 & 2) != 0)) && unk_00742fc4 != 0)
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    s.result = unk_007a7d0c;
  }
  else if (((active_player == player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1)
  {
    s.valid_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1)
      {
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
      }
    }

    unk_00939340 = internal_rand(s.valid_count);
    if (unk_008b35ec != player)
    {
      if (unk_008a9000 == 1)
      {
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
      }
    }

    s.result = s.valid_graveyards[unk_00939340];
  }
  else if (unk_00742fc4 == 0)
  {
    ReadPalette(unk_00573364, (char *)0);
    FUN_004e1c81(0, 0);
    SelectPalette(unk_0074309c, unk_007463dc, 0);
    FUN_004e1da6(unk_00573370);
    FUN_004e1d91(1, 0, 0, unk_00573380, &unk_007462a0);
    FUN_004e1d6d(unk_0057a75c, 0, 0, 0x280, 0x1e0, unk_007497b0, 0, 0, unk_0057d9f0, unk_0057d9f4);

    s.display_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8]))
      {
        ++s.display_count;
      }
    }

    s.valid_count = (s.display_count - 1) / 5;
    if (s.valid_count == 0)
    {
      s.valid_count = 1;
    }

    s.x = 0x60;
    s.valid_count = 0;
    s.y = 0x10;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8)
    {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8]))
      {
        s.x_positions[s.valid_count] = s.x + 4;
        s.y_positions[s.valid_count] = s.y;
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
        s.x += 0x38;
        if (s.x > 0x13f)
        {
          s.x = 0x60;
          s.y += 0x54 / s.display_count;
        }
      }
    }

    if (s.valid_count > 0)
    {
      s.result = s.valid_graveyards[0];
    }
    else
    {
      s.result = -1;
    }
  }
  else
  {
    s.result = FUN_004a61fe(cards, count, context, suppress_done_txt, prompt);
    if ((unk_008b35ec == player) && ((unk_00926804 & 2) != 0))
    {
      unk_007a7d08 = 0x19;
      unk_007a7d0c = s.result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return s.result;
}

// FUNCTION: MOK 0x00459AA0
// FUNCTION: MAGIC 0x00485060
int dispatch_function_to_all_cards_in_play(int player, int card, in_play_card_callback_t callback, int who_to_check)
{
  int saved_event_result;
  int result;
  int callback_result;
  int test_card;
  int test_player;

  saved_event_result = event_result;
  callback_result = 0;
  event_result = -1;
  test_player = player;

  while (test_player >= 0 && test_player < 2 && callback_result == 0)
  {
    if (who_to_check == -1 || test_player == who_to_check)
    {
      test_card = 0;
      while (test_card < active_cards_count[test_player] && callback_result == 0)
      {
        if (is_in_play(test_player, test_card))
        {
          callback_result = callback(player,
                                     card,
                                     test_player,
                                     test_card,
                                     global_card_instances[test_player][test_card].internal_card_id);
        }
        ++test_card;
      }
    }

    if (player == 0)
    {
      ++test_player;
    }
    else
    {
      --test_player;
    }
  }

  result = event_result;
  event_result = saved_event_result;
  return result;
}

// FUNCTION: MAGIC 0x004964dd
int FUN_004964dd(int card_id, int player, int card)
{
  if (card_id == -1)
  {
    return 0;
  }
  if (player == -1 || card == -1)
  {
    return 0;
  }
  if (global_raw_cards_storage[card_id].num_pics < 2)
  {
    return 0;
  }

  return (player + card) % global_raw_cards_storage[card_id].num_pics;
}

// FUNCTION: MAGIC 0x0055b9f0
void FUN_0055b9f0(int dc, int *rect, int value)
{
  struct
  {
    char text[12];
    int text_x;
    int text_y;
    int saved_dc;
  } s;
  size_t text_len;

  if (dc != 0 && rect != NULL)
  {
    s.saved_dc = SaveDC((HDC)dc);
    SetMapMode((HDC)dc, 8);
    SetWindowExtEx((HDC)dc, 100, 0x8c, NULL);
    SetViewportExtEx((HDC)dc, rect[2] - rect[0], rect[3] - rect[1], NULL);
    SetWindowOrgEx((HDC)dc, 0, 0, NULL);
    SetViewportOrgEx((HDC)dc, rect[0], rect[1], NULL);
    sprintf(s.text, "%d", value);
    SelectObject((HDC)dc, (HGDIOBJ)DAT_00708d98);
    SetTextAlign((HDC)dc, 10);
    SetBkMode((HDC)dc, 1);
    s.text_x = 100;
    s.text_y = 0x8c;
    SetTextColor((HDC)dc, DAT_00708d94);
    text_len = strlen(s.text);
    TextOutA((HDC)dc, s.text_x - 1, s.text_y - 1, s.text, text_len);
    SetTextColor((HDC)dc, DAT_00708d88);
    text_len = strlen(s.text);
    TextOutA((HDC)dc, s.text_x - 3, s.text_y - 3, s.text, text_len);
    RestoreDC((HDC)dc, s.saved_dc);
  }
}

// FUNCTION: MAGIC 0x0050047c
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card)
{
  card_instance_t *damage;
  int source_internal_card_id;
  int damage_player;
  int result;

  if (target_player == -1 || source_player == -1 || amount < 1)
  {
    return -1;
  }

  if (target_card == -1)
  {
    damage_player = target_player;
  }
  else
  {
    damage_player = source_player;
  }

  result = add_card_to_hand(damage_player, unk_009266a4);
  if (result != -1)
  {
    damage = &PLAYER_CARD_INSTANCE(damage_player, result);
    damage->state |= 2;
    if (damage_player == 0)
    {
      damage->state |= 0x1000;
    }
    damage->damage_target_player = (char)target_player;
    damage->damage_target_card = target_card;
    damage->info_slot = amount;
    damage->damage_source_player = (char)source_player;
    damage->damage_source_card = source_card;
    if (source_card == -1)
    {
      *(int *)((char *)damage + 0x64) = 0xef;
    }
    else
    {
      if (PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == -1 || PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == unk_0091a80c)
      {
        source_internal_card_id = *(int *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x3c);
      }
      else
      {
        source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id;
      }
      *(unsigned char *)((char *)damage + 0x1e) =
          *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x1e);
      if ((global_cards_data[source_internal_card_id].type & TYPE_INTERRUPT) != 0)
      {
        *(unsigned char *)((char *)damage + 0x1e) |= 0x40;
      }
      damage->eot_toughness = (unsigned int)global_cards_data[source_internal_card_id].type;
      if ((PLAYER_CARD_INSTANCE(source_player, source_card).state & 4) != 0)
      {
        if (current_phase == 0x19)
        {
          damage->token_status |= 0x100000;
        }
        if (current_phase == 0x1a)
        {
          damage->token_status |= 0x40000;
        }
      }
      if (*(int *)&global_cards_data[source_internal_card_id].id == unk_00789734 || *(int *)&global_cards_data[source_internal_card_id].id == unk_008a8de8 || *(int *)&global_cards_data[source_internal_card_id].id == unk_008cf1ac)
      {
        *(int *)((char *)damage + 0x64) = *(int *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x64);
      }
      else
      {
        *(int *)((char *)damage + 0x64) =
            FUN_004964dd(*(int *)&global_cards_data[source_internal_card_id].id, source_player, source_card) << 16 | *(unsigned short *)&global_cards_data[source_internal_card_id].id;
      }
    }
    unk_008b4278 |= 2;
  }

  return result;
}

int has_mana_for_activated_ability(int player, int card, int colorless, int black, int blue, int green, int red, int white)
{
  (void)player;
  (void)card;
  (void)colorless;
  (void)black;
  (void)blue;
  (void)green;
  (void)red;
  (void)white;
  return 0;
}

// FUNCTION: MAGIC 0x004b4654
int add_card_to_hand(int player, int internal_card_id)
{
  int card;

  card = active_cards_count[player];
  if (player < 0 || player >= 2 || card < 0)
  {
    return -1;
  }

  ++active_cards_count[player];
  memset(&global_card_instances[player][card], 0, sizeof(global_card_instances[player][card]));
  global_card_instances[player][card].internal_card_id = internal_card_id;
  global_card_instances[player][card].original_internal_card_id = internal_card_id;

  return card;
}

// FUNCTION: MAGIC 0x00500135
int create_a_card_type(int internal_card_id)
{
  return internal_card_id;
}

// FUNCTION: MAGIC 0x00442b42
int dispatch_event_to_single_card(int player,
                                  int card,
                                  event_t event,
                                  int new_attacking_card_controller,
                                  int new_attacking_card)
{
  int saved_affected_card;
  int saved_affected_card_controller;
  int saved_attacking_card;
  int saved_attacking_card_controller;
  int result;
  card_function_pointer code_pointer;

  if (player < 0 || player >= 2 || card < 0 || card >= active_cards_count[player])
  {
    return 0;
  }
  if (global_card_instances[player][card].internal_card_id == -1)
  {
    return 0;
  }

  code_pointer = global_cards_data[global_card_instances[player][card].internal_card_id].code_pointer;
  if (code_pointer == 0)
  {
    return 0;
  }

  saved_affected_card = affected_card;
  saved_affected_card_controller = affected_card_controller;
  saved_attacking_card = attacking_card;
  saved_attacking_card_controller = attacking_card_controller;

  affected_card = card;
  affected_card_controller = player;
  attacking_card = new_attacking_card;
  attacking_card_controller = new_attacking_card_controller;
  result = code_pointer(player, card, event);

  affected_card = saved_affected_card;
  affected_card_controller = saved_affected_card_controller;
  attacking_card = saved_attacking_card;
  attacking_card_controller = saved_attacking_card_controller;

  return result;
}

// FUNCTION: MAGIC 0x004428be
int dispatch_event(int player, int card, event_t event)
{
  return dispatch_event_to_single_card(player, card, event, 1 - player, -1);
}

// FUNCTION: MAGIC 0x0043e18b
int FUN_0043e18b(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x0040246a
void FUN_0040246a(int player, int amount)
{
  int cards_drawn;

  for (cards_drawn = 0; cards_drawn < amount; ++cards_drawn)
  {
    FUN_0043e18b(player);
    if (player != 0)
    {
      unk_007161d8 = 0;
    }
  }
  hand_count[player] = amount;
}

// FUNCTION: MAGIC 0x004a6968
void TENTATIVE_reassess_all_cards()
{
  int player;
  int card;
  int card_status;
  card_instance_t *instance;

  count_mana();
  if (_DAT_00742fbc == 0 && trigger_condition == -1)
  {
    _DAT_00743024 = 0;
  }

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, card);
      if (instance->internal_card_id != -1 && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0)
      {
        C_get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
      }
    }
  }

  C_count_colors_of_lands_in_play();
  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, card);
      if (instance->internal_card_id != -1)
      {
        if ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) != 0 || (instance->counter_power & 0x400) != 0)
        {
          C_get_abilities(player, card, EVENT_ABILITIES, -1);
        }
        if (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE)
        {
          C_get_abilities(player, card, EVENT_POWER, -1);
          C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
        }
      }
    }
  }

  if (unk_008a9000 != 1)
  {
    unk_008b3270 = 0;
    for (player = 0; player < 2; ++player)
    {
      for (card = 0; card < active_cards_count[player]; ++card)
      {
        instance = &PLAYER_CARD_INSTANCE(player, card);
        if (instance->internal_card_id != -1)
        {
          card_status = FUN_004b0047(player, card);
          instance->kill_code = (unsigned char)card_status;
        }
      }
    }

    if (unk_00742fc4 == 0)
    {
      FUN_004e1cd1();
    }
    else
    {
      SendMessageA((HWND)unk_008cf1b4, 0x464, 0xffff, 0);
    }
  }
}

// FUNCTION: MAGIC 0x004b0047
int FUN_004b0047(int player, int card)
{
  int result;
  int trigger_result;
  int in_play;
  card_instance_t *instance;
  card_data_t *card_data;
  unsigned int state;
  unsigned int upkeep_flags;
  unsigned int type;
  unsigned int extra_ability;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  state = instance->state;
  upkeep_flags = instance->upkeep_flags;
  in_play = is_in_play(player, card);
  card_data = &global_cards_data[instance->internal_card_id];
  type = (unsigned int)card_data->type;
  extra_ability = (unsigned int)card_data->extra_ability;

  unk_00743038 = 1;
  instance->state |= 0x800;

  if (in_play != 0 && current_phase == 0x15 && player == human_player && (state & 0x8000) != 0 && (state & 4) == 0 && FUN_0044125c(player, card) != 0)
  {
    result = 2;
  }
  else if (in_play != 0 && trigger_condition != -1)
  {
    if (instance->unknown0x14 == trigger_condition)
    {
      if (current_turn == player)
      {
        trigger_result = 2;
      }
      else
      {
        trigger_result = 0;
      }
    }
    else
    {
      trigger_result = FUN_004c0a36(FUN_004b082f(player, card, EVENT_TRIGGER, player), 0, 2);
    }

    if (trigger_result == 0)
    {
      result = 0;
    }
    else
    {
      DAT_007ab2bc |= 1 << ((unsigned char)trigger_result & 0x1f);
      DAT_00791410 += 1;
      result = trigger_result;
    }
  }
  else
  {
    if (in_play == 0 && trigger_condition != -1 && card_data->code_pointer != card_death_ward)
    {
      result = 0;
    }
    else if (in_play != 0 && current_phase == 1)
    {
      push_affected_card_stack();
      affected_card_controller = player;
      affected_card = card;
      event_result = 0;
      C_dispatch_event_raw(EVENT_TRIGGER);
      result = event_result;
      pop_affected_card_stack();
    }
    else if (player == active_player)
    {
      if ((DAT_007aadec == 4 && (upkeep_flags & 1) != 0) || (DAT_007aadec == 10 && unk_008b28f8 == instance->internal_card_id))
      {
        unk_008b3270 |= 3;
        DAT_007ab2bc |= 4;
        result = 2;
      }
      else
      {
        instance->state &= ~0x800;
        result = 0;
      }
    }
    else if (_DAT_00742fbc == 0 && (current_phase == 0x15 || current_phase == 0x17))
    {
      if (in_play != 0 && (state & 0x10) == 0 && ((type & TYPE_CREATURE) != 0 || (state & 0x3000000) != 0))
      {
        if (player == human_player && FUN_0044125c(player, card) != 0 && (state & 0x10000) == 0)
        {
          unk_00743038 = 0;
          return 0x10;
        }
        if (player != human_player && unk_008b60e0 != 0 && (state & 8) == 0)
        {
          unk_00743038 = 0;
          return 0x20;
        }
      }

      instance->state &= ~0x800;
      result = 0;
    }
    else
    {
      if (in_play == 0)
      {
        if ((state & 0xa0) != 0)
        {
          instance->state &= ~0x800;
          unk_00743038 = 0;
          return 0;
        }

        single_color_test_bit_to_color_t((int)(char)card_data->color);
        if (_DAT_00742fbc == 0 || (type & DAT_00742f68) != 0)
        {
          if ((type & TYPE_LAND) != 0)
          {
            if (player == human_player && (unk_008b4278 & 1) == 0 && (current_phase == 0x14 || current_phase == 0x1e))
            {
              unk_00743038 = 0;
              return 4;
            }

            instance->state &= ~0x800;
            unk_00743038 = 0;
            return 0;
          }

          if ((((unk_008b35ec == human_player && (((_DAT_00742fbc != 0 && (type & 0x30) != 0) || current_phase == 0x14) || current_phase == 0x1e)) || (unk_008b35ec != human_player && _DAT_00742fbc != 0 && ((type & 0x10) != 0 || (type & 0x20) != 0))) && FUN_0043fdb3(player, player, card) != 0 && (((unk_008b4278 & 4) == 0 || (extra_ability & 0x3004) != 0) && ((type & 0x42) != 0 || dispatch_event_to_single_card(player, card, EVENT_CAN_CAST, 1 - player, -1) != 0))))
          {
            unk_00743038 = 0;
            return 4;
          }
        }
      }
      else
      {
        if (DAT_007aadec == 4 && (upkeep_flags & 1) != 0)
        {
          unk_008b3270 |= 3;
          DAT_007ab2bc |= 4;
          unk_00743038 = 0;
          return 2;
        }

        if (DAT_007aadec == 4 && (upkeep_flags & 0x10) != 0 && (upkeep_flags & 0x88) == 0 && FUN_00445b56(player, card) != 0)
        {
          DAT_007ab2bc |= 2;
          unk_00743038 = 0;
          return 8;
        }

        if ((state & 0x10) == 0 && (type & TYPE_CREATURE) != 0 && _DAT_00742fbc == 0 && player == human_player && current_phase < 0x1b && FUN_0044125c(player, card) != 0 && (((instance->token_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0))
        {
          _DAT_00743024 = 1;
        }

        if ((((extra_ability & 0x1000) != 0 && (state & 0x10) == 0 && (((instance->token_status & 3) == 0) || (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)) || ((extra_ability & 1) != 0 && (DAT_00742f68 & 0x10) != 0) || ((extra_ability & 2) != 0 && (DAT_00742f68 & 0x20) != 0)) && (((unk_008b4278 & 4) == 0 || (extra_ability & 0x5004) != 0) && ((unk_008b3270 &= ~2), (state & 0x20) == 0) && dispatch_event_to_single_card(player, card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0))
        {
          if ((unk_008b3270 & 2) != 0)
          {
            DAT_007ab2bc |= 4;
            unk_00743038 = 0;
            return 2;
          }

          DAT_007ab2bc |= 2;
          unk_00743038 = 0;
          return 8;
        }
      }

      instance->state &= ~0x800;
      result = 0;
    }
  }

  unk_00743038 = 0;
  return result;
}

// FUNCTION: MAGIC 0x004e1cd1
void FUN_004e1cd1(void)
{
}

// FUNCTION: MOK 0x0049d510
// FUNCTION: MAGIC 0x004eaf09
int has_mana(int player, color_t color, int amount)
{
  int result;
  int available_mana;
  unsigned int interchangeable_colors;
  color_t effective_color;
  int current_color;
  int special_mana;
  int xmana;
  unsigned char mana_source_color;

  if (color == COLOR_ARTIFACT || color == COLOR_COLORLESS)
  {
    effective_color = COLOR_ANY;
  }
  else
  {
    effective_color = color;
  }

  if (amount == 0)
  {
    return 1;
  }

  interchangeable_colors = 0;
  for (current_color = 0; current_color < 10 && unk_007161e0[player][current_color] != -1; ++current_color)
  {
    if (effective_color == (color_t)((unsigned int)unk_007161e0[player][current_color] >> 16))
    {
      mana_source_color = (unsigned char)(unsigned short)unk_007161e0[player][current_color];
      interchangeable_colors |= 1 << (mana_source_color & 0x1f);
    }
  }

  available_mana = raw_mana_available[player][effective_color];
  for (current_color = COLOR_COLORLESS; current_color < 7; ++current_color)
  {
    if (effective_color != current_color && (interchangeable_colors & (1 << ((unsigned char)current_color & 0x1f))) != 0)
    {
      available_mana += raw_mana_available[player][current_color];
    }
  }

  special_mana = unk_00742f70[player * 8 + effective_color];
  for (current_color = COLOR_COLORLESS; current_color < 7; ++current_color)
  {
    if (effective_color != current_color && (interchangeable_colors & (1 << ((unsigned char)current_color & 0x1f))) != 0)
    {
      special_mana += unk_00742f70[player * 8 + current_color];
    }
  }

  xmana = 0;
  for (current_color = 0; current_color < 0x32 && raw_mana_available_hex[player][current_color] != -1; ++current_color)
  {
    if (effective_color == COLOR_ANY)
    {
      xmana += raw_mana_available_hex[player][current_color] >> 16;
    }
    else if ((raw_mana_available_hex[player][current_color] & (1 << ((unsigned char)effective_color & 0x1f))) == 0)
    {
      if ((interchangeable_colors & raw_mana_available_hex[player][current_color]) != 0)
      {
        xmana += raw_mana_available_hex[player][current_color] >> 16;
      }
    }
    else
    {
      xmana += raw_mana_available_hex[player][current_color] >> 16;
    }
  }

  if (color == COLOR_ANY || color == COLOR_COLORLESS)
  {
    result = (((available_mana - raw_mana_available[player][COLOR_ARTIFACT]) + special_mana) - unk_00742f70[player * 8 + COLOR_ARTIFACT]) + xmana;
  }
  else
  {
    result = xmana + special_mana + available_mana;
  }

  if (result < amount)
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0043fdb3
int FUN_0043fdb3(int player, int target_player, int target_card)
{
  int internal_card_id;
  int color;
  int colored_cost;
  int colorless_cost;

  if (player == -1 || target_player == -1 || target_card == -1)
  {
    return 0;
  }

  internal_card_id = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
  if (internal_card_id == -1)
  {
    return 0;
  }

  colored_cost = (int)(char)global_cards_data[internal_card_id].cc[0];
  colorless_cost = (int)(char)global_cards_data[internal_card_id].cc[1];
  if ((global_cards_data[internal_card_id].type & TYPE_ARTIFACT) == 0)
  {
    color = single_color_test_bit_to_color_t((int)(char)global_cards_data[internal_card_id].color);
    if (has_mana_w_global_cost_mod(player, target_card, color, colored_cost) == 0)
    {
      return 0;
    }

    if (colorless_cost > 0 && has_mana_w_global_cost_mod(player, target_card, COLOR_ANY, colored_cost + colorless_cost) == 0)
    {
      return 0;
    }
  }
  else if (has_mana(player, COLOR_ARTIFACT, colored_cost + colorless_cost) == 0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00442dac
void push_affected_card_stack(void)
{
  if (DAT_00561268 >= 0x20)
    return;

  DAT_0093f4c0[DAT_00561268][0] = affected_card_controller;
  DAT_0093f4c0[DAT_00561268][1] = affected_card;
  DAT_0093f4c0[DAT_00561268][2] = unk_008b4dd0;
  DAT_0093f4c0[DAT_00561268][3] = unk_008cf6d4;
  DAT_0093f4c0[DAT_00561268][4] = attacking_card_controller;
  DAT_0093f4c0[DAT_00561268][5] = attacking_card;
  DAT_0093f4c0[DAT_00561268][6] = event_result;
  DAT_00561268 += 1;
}

// FUNCTION: MAGIC 0x00442e62
void pop_affected_card_stack(void)
{
  if (DAT_00561268 > 0)
  {
    DAT_00561268 -= 1;
  }

  affected_card_controller = DAT_0093f4c0[DAT_00561268][0];
  affected_card = DAT_0093f4c0[DAT_00561268][1];
  unk_008b4dd0 = DAT_0093f4c0[DAT_00561268][2];
  unk_008cf6d4 = DAT_0093f4c0[DAT_00561268][3];
  attacking_card_controller = DAT_0093f4c0[DAT_00561268][4];
  attacking_card = DAT_0093f4c0[DAT_00561268][5];
  event_result = DAT_0093f4c0[DAT_00561268][6];
}

// FUNCTION: MAGIC 0x00445b56
int FUN_00445b56(int player, int card)
{
  int total_paid;
  int current_color;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((instance->state & 0x10) == 0)
  {
    return 0;
  }

  total_paid = 0;
  for (current_color = COLOR_BLACK; current_color < 7; ++current_color)
  {
    if ((char)instance->mana_to_untap[current_color] != 0)
    {
      if (has_mana(player, (color_t)current_color, (int)(char)instance->mana_to_untap[current_color]) == 0)
      {
        return 0;
      }
      total_paid += (int)(char)instance->mana_to_untap[current_color];
    }
  }

  if (has_mana(player, COLOR_ANY, (int)(char)instance->mana_to_untap[0] + total_paid) == 0)
  {
    return 0;
  }

  dispatch_event_to_single_card(player, card, EVENT_CHECK_UNTAP_PAYMENT, 1 - player, -1);
  return DAT_008cf1b8 == 0;
}

// FUNCTION: MAGIC 0x00445918
int TENTATIVE_set_timestamps(int player, int card)
{
  int slot;

  for (slot = 0; slot < 500; slot++)
  {
    if (TENTATIVE_timestamp_player[slot] == -1)
    {
      TENTATIVE_timestamp_player[slot] = player;
      TENTATIVE_timestamp_card[slot] = card;
      global_card_instances[player][card].timestamp = slot;
      return slot;
    }
  }
  return -1;
}

// FUNCTION: MAGIC 0x005493a6
int FUN_005493a6(int player, int card, int internal_card_id)
{
  unsigned int *recalc_abilities_ptr;

  if (global_cards_data[internal_card_id].subtype == SUB_ZOMBIE)
  {
    recalc_abilities_ptr =
        (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x28);
    *recalc_abilities_ptr |= 0x8000000;
  }

  return 1;
}

// FUNCTION: MAGIC 0x0055117d
void FUN_0055117d(int(__cdecl *callback)(int, int, int), int who_to_check)
{
  int test_card;
  int test_player;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    if (who_to_check == -1 || test_player == who_to_check)
    {
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
      {
        if (is_in_play(test_player, test_card))
        {
          callback(test_player,
                   test_card,
                   global_card_instances[test_player][test_card].internal_card_id);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00551638
int FUN_00551638(int player, unsigned int preferred_controller, int card)
{
  int result;
  unsigned char target_number;
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  result = C_real_select_target(player,
                                2,
                                preferred_controller,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                0,
                                0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                text_lines[0],
                                1,
                                &selected_target);
  if (result != 0)
  {
    target_number = global_card_instances[player][card].number_of_targets;
    global_card_instances[player][card].targets[target_number] = selected_target;
    ++global_card_instances[player][card].number_of_targets;
  }

  return result != 0;
}

// FUNCTION: MAGIC 0x0052dd74
int FUN_0052dd74(int player, int card, event_t event, int power_modifier, int toughness_modifier)
{
  int preferred_controller;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 2,
                                 0,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    if (power_modifier + toughness_modifier < 0)
    {
      preferred_controller = 1 - player;
    }
    else
    {
      preferred_controller = player;
    }

    if (FUN_00551638(player, preferred_controller, card) == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      ai_modifier += -0x18;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(global_card_instances[player][card].targets[0].player,
                               global_card_instances[player][card].targets[0].card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0) == 0)
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      global_card_instances[player][card].damage_target_player =
          (char)global_card_instances[player][card].targets[0].player;
      global_card_instances[player][card].damage_target_card =
          global_card_instances[player][card].targets[0].card;
    }
    global_card_instances[player][card].number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER) && (global_card_instances[player][card].damage_target_card == affected_card)) && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller) && (affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      event_result += power_modifier;
    }
  }
  if (((event == EVENT_TOUGHNESS) && (global_card_instances[player][card].damage_target_card == affected_card)) && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller) && (affected_card != -1)))
  {
    if (is_in_play(player, card))
    {
      event_result += toughness_modifier;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004eab5c
void declare_mana_available_hex(int player, color_test_t colors, int amount)
{
  int i;

  if ((int)colors > 0)
  {
    for (i = 0; i < 0x32; ++i)
    {
      if (raw_mana_available_hex[player][i] == -1)
      {
        raw_mana_available_hex[player][i] = (amount << 16) | colors;
        raw_mana_available_hex[player][i + 1] = -1;
        break;
      }
    }
  }
}

// FUNCTION: MAGIC 0x004eac0c
void undeclare_mana_available_hex(int player, color_test_t color, int amount)
{
  int i;
  int j;
  int target;

  target = (amount << 16) | color;
  for (i = 0; i < 0x32 && raw_mana_available_hex[player][i] != -1; ++i)
  {
    if (raw_mana_available_hex[player][i] == target)
    {
      for (j = i; j < 0x31; ++j)
      {
        raw_mana_available_hex[player][j] = raw_mana_available_hex[player][j + 1];
      }
      break;
    }
  }
}

// FUNCTION: MAGIC 0x004eada9
void declare_mana_available(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
}

// FUNCTION: MAGIC 0x004eaec1
void undeclare_mana_available_and_produce_it(int player, color_t color, int amount)
{
  if (raw_mana_available[player][color] >= amount)
  {
    raw_mana_available[player][color] -= amount;
  }
  else
  {
    raw_mana_available[player][color] = 0;
  }
}

// FUNCTION: MAGIC 0x0051a41c
void FUN_0051a41c(int player, int card)
{
  dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
  dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
}

// FUNCTION: MAGIC 0x005513d7
void FUN_005513d7(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
}

// FUNCTION: MAGIC 0x004a686a
int choose_a_color(int player, const char *prompt, int unused1, int unused2, unsigned int available_colors)
{
  (void)player;
  (void)prompt;
  (void)unused1;
  (void)unused2;

  if ((available_colors & COLOR_TEST_WHITE) != 0)
  {
    return COLOR_WHITE;
  }
  if ((available_colors & COLOR_TEST_BLUE) != 0)
  {
    return COLOR_BLUE;
  }
  if ((available_colors & COLOR_TEST_BLACK) != 0)
  {
    return COLOR_BLACK;
  }
  if ((available_colors & COLOR_TEST_RED) != 0)
  {
    return COLOR_RED;
  }
  if ((available_colors & COLOR_TEST_GREEN) != 0)
  {
    return COLOR_GREEN;
  }

  return -1;
}

// FUNCTION: MOK 0x0044e050
// FUNCTION: MAGIC 0x004e1d6d
int FUN_004e1d6d()
{
  return 0;
}

// FUNCTION: MAGIC 0x004e1d91
int FUN_004e1d91()
{
  return 1;
}

// FUNCTION: MAGIC 0x004e1da6
int FUN_004e1da6()
{
  return 1;
}

// FUNCTION: MAGIC 0x004e1c81
int FUN_004e1c81()
{
}

// FUNCTION: MAGIC 0x00464a57
int internal_rand(int maximum)
{
  int result;

  if (maximum > 1)
  {
    result = rand() % maximum;
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x004e4f11
void FUN_004e4f11(void)
{
  if (unk_00743094 < 0x100)
  {
    unk_006a21b8[unk_00743094] = unk_00925bb8;
    unk_006a8258[unk_00743094] = PLAYER_CARD_INSTANCE((unk_00925bb8 & 0x100) >> 8, unk_00925bb8 & 0xff).internal_card_id;
    unk_006a8660[unk_00743094] = unk_0057aae8;
    unk_006a6ee8[unk_00743094] = unk_00939340;
    ++unk_00743094;
    if (unk_006a6ee8[0] == 99 || unk_006a6ae8[0] == 99)
    {
      unk_00925bb8 = -1;
    }
  }
  else
  {
    spell_fizzled = 1;
  }

  unk_0057aae8 = 0;
}

// FUNCTION: MAGIC 0x004e5089
void FUN_004e5089(void)
{
  if (unk_006a5f18[unk_00743094] != unk_0057aae8)
  {
    unk_0057aae8 |= 0x100;
  }

  unk_00925bb8 = unk_006a1db8[unk_00743094];
  unk_00939340 = unk_006a6ae8[unk_00743094];
  if (unk_00939340 != 99)
  {
    ++unk_00743094;
  }

  unk_0057aae8 = 0;
}

// FUNCTION: MAGIC 0x0048463d
int FUN_0048463d(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x004817fd
void FUN_004817fd(int player)
{
  int ante_slot;
  int card_in_hand;

  for (ante_slot = 0; ante_slot < 0x10 && *(int *)&global_ante_cards[player][ante_slot * 4] != -1; ++ante_slot)
  {
  }

  if (ante_slot < 0x10)
  {
    card_in_hand = FUN_0043e18b(player);
    *(int *)&global_ante_cards[player][ante_slot * 4] = PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id;
    --hand_count[player];
    load_text("promptsX1.txt", "ANTE_A_CARD");
    if (unk_008b35ec == player)
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[0], 0);
    }
    else
    {
      do_dialog(player, player, card_in_hand, -1, -1, text_lines[1], 0);
    }

    PLAYER_CARD_INSTANCE(player, card_in_hand).internal_card_id = -1;
  }
}

// FUNCTION: MAGIC 0x00483489
int FUN_00483489(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x004bff5a
int FUN_004bff5a(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

// FUNCTION: MAGIC 0x004c081a
int FUN_004c081a(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

// FUNCTION: MAGIC 0x004c0880
int FUN_004c0880(int internal_card_id, int extra)
{
  (void)internal_card_id;
  (void)extra;
  return 0;
}

// FUNCTION: MAGIC 0x004848a0
void gain_life(int player, int amount)
{
  life[player] += amount;
}

int is_animated_and_sick(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

int is_tapped(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

// FUNCTION: MAGIC 0x0054276d
int FUN_0054276d(int player, int card, event_t event, unsigned int color, int amount)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_ACTIVATE && (unk_008b4278 & 0x200) != 0 && instance->info_slot == 0)
  {
    if (instance->kill_code == 2 && (instance->state & 0x800002) == 2 && (instance->regen_status & 0x200) != 0)
    {
      if (has_mana(player, color, amount) == 0)
      {
        return 0;
      }

      return 99;
    }

    return 0;
  }
  else if (event == 0x90)
  {
    FUN_004e4ff3(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE && (unk_008b4278 & 0x200) != 0)
  {
    charge_mana(player, color, amount);
    if (spell_fizzled != 1)
    {
      unk_007a7c1c = 1;
      ++instance->info_slot;
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && (unk_008b4278 & 0x200) != 0)
  {
    PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).info_slot = 0;
    FUN_00542a2a(card_on_stack_controller, card_on_stack);
    return 0;
  }

  return 0;
}

int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4);
int FUN_004b0c12(int player, int card);
void FUN_004b117e(int player, int card);
void FUN_004b1670(int player, int internal_card_id);

// FUNCTION: MAGIC 0x00445663
int dispatch_trigger_twice_once_with_each_player_as_reason(int reason_for_trig, trigger_t trig, const char *prompt, int a4)
{
  (void)reason_for_trig;
  (void)trig;
  (void)prompt;
  (void)a4;
  return 1;
}

// FUNCTION: MAGIC 0x004b1670
void FUN_004b1670(int player, int internal_card_id)
{
  int exile_index;

  if (unk_008a9000 != 1 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    play_sound_effect(0x17);
  }

  exile_index = 0;
  while (exile_index < 500)
  {
    if (global_exile[player][exile_index] == -1)
    {
      global_exile[player][exile_index] = internal_card_id;
      return;
    }

    ++exile_index;
  }
}

// FUNCTION: MAGIC 0x004b0c12
int FUN_004b0c12(int player, int card)
{
  card_instance_t *instance;
  int internal_card_id;
  int saved_trigger_cause;
  int saved_trigger_cause_controller;
  int original_internal_card_id;
  char kill_mode;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  internal_card_id = instance->internal_card_id;
  kill_mode = instance->kill_code;
  if (kill_mode == 0)
  {
    return 0;
  }

  if ((instance->token_status & 8) == 0)
  {
    if (kill_mode != 4 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 && (instance->state & 0x20) == 0)
    {
      ++*(int *)(unk_008cfd30 + 0x7c);
    }

    if ((global_cards_data[internal_card_id].type & 0xc7) != 0)
    {
      push_affected_card_stack();
      event_result = 0;
      affected_card_controller = player;
      affected_card = card;
      attacking_card_controller = 1 - player;
      attacking_card = -1;
      C_dispatch_event_raw(0x77);
      if (event_result > 0)
      {
        instance->token_status &= 0xffffff7f;
        pop_affected_card_stack();
        return 0;
      }

      kill_mode = instance->kill_code;
      pop_affected_card_stack();
    }

    if ((global_cards_data[internal_card_id].type & 0x80) == 0 && (instance->token_status & 0x10) == 0)
    {
      if (kill_mode == 4)
      {
        FUN_004b1670((instance->state & 0x1000) >> 12, instance->original_internal_card_id);
      }
      else
      {
        if (kill_mode != 3 && unk_008a9000 != 1)
        {
          original_internal_card_id = instance->original_internal_card_id;
          if ((global_cards_data[original_internal_card_id].type & TYPE_CREATURE) == 0)
          {
            if ((global_cards_data[original_internal_card_id].type & 0x38) == 0)
            {
              play_sound_effect(1);
            }
          }
          else
          {
            play_sound_effect(0x19);
          }
        }

        FUN_004b14f5(player, card);
        if (kill_mode == 3)
        {
          dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_GRAVEYARD_FROM_PLAY, &DAT_008a8ed0, 0);
        }
      }
    }
  }

  push_affected_card_stack();
  saved_trigger_cause = trigger_cause;
  saved_trigger_cause_controller = trigger_cause_controller;
  trigger_cause_controller = player;
  trigger_cause = card;
  if ((global_cards_data[internal_card_id].type & 0x47) != 0)
  {
    dispatch_trigger_twice_once_with_each_player_as_reason(human_player, TRIGGER_LEAVE_PLAY, &DAT_007aaef0, 0);
  }
  trigger_cause_controller = saved_trigger_cause_controller;
  trigger_cause = saved_trigger_cause;
  pop_affected_card_stack();

  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0)
  {
    --*(int *)(unk_008cfd30 + player * 4 + 0x50);
  }
  if ((global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0)
  {
    --*(int *)(unk_008cfd30 + player * 4 + 0x58);
  }
  if ((global_cards_data[internal_card_id].type & TYPE_ARTIFACT) != 0)
  {
    --*(int *)(unk_008cfd30 + player * 4 + 0x60);
  }

  instance->internal_card_id = -1;
  instance->kill_code = 0;
  instance->unknown0x14 = 0;
  if (unk_008a9000 != 1)
  {
    FUN_004a61a1(player, card, 7, 2);
  }

  FUN_004b117e(player, card);
  if ((global_cards_data[internal_card_id].type & 0x47) != 0)
  {
    FUN_00441d78();
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b08e6
void kill_card(int player, int card, kill_t kill_mode)
{
  card_instance_t *instance;
  int internal_card_id;

  if (player == -1 || card == -1)
  {
    return;
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((instance->token_status & 0x80) != 0)
  {
    return;
  }

  instance->token_status |= 0x80;
  internal_card_id = instance->internal_card_id;
  if (internal_card_id == -1)
  {
    return;
  }

  if ((instance->state & 2) == 0)
  {
    kill_mode = 3;
  }

  if ((instance->token_status & 8) == 0 && kill_mode != 3 && kill_mode != 4 && (global_cards_data[internal_card_id].type & 0x43) != 0 && global_cards_data[internal_card_id].type != 0x80)
  {
    instance->kill_code = (char)kill_mode;
    instance->state |= 2;
    instance->unknown0x14 = 0xd6;
    DAT_007ab2cc = FUN_004b0c12;
  }
  else
  {
    instance->kill_code = (char)kill_mode;
    FUN_004b0c12(player, card);
  }
}

// FUNCTION: MAGIC 0x004b117e
void FUN_004b117e(int player, int card)
{
  struct
  {
    int linked_cards[600];
    int linked_count;
    int current_card;
    int current_player;
    int in_play;
  } s;
  card_instance_t *instance;
  int linked_internal_card_id;

  s.linked_count = 0;
  for (s.current_player = 0; s.current_player < 2; ++s.current_player)
  {
    for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; ++s.current_card)
    {
      s.in_play = is_in_play(s.current_player, s.current_card);
      instance = &PLAYER_CARD_INSTANCE(s.current_player, s.current_card);
      if (s.in_play != 0 && instance->damage_target_player == player && instance->damage_target_card == card && (s.current_player != player || s.current_card != card))
      {
        linked_internal_card_id = instance->internal_card_id;
        if ((global_cards_data[linked_internal_card_id].type & 0x43) == 0)
        {
          s.linked_cards[s.linked_count * 2] = s.current_player;
          s.linked_cards[s.linked_count * 2 + 1] = s.current_card;
          ++s.linked_count;
        }
        else
        {
          instance->damage_target_player = -1;
          instance->damage_target_card = -1;
        }
      }
    }
  }

  while (s.linked_count != 0)
  {
    --s.linked_count;
    kill_card(s.linked_cards[s.linked_count * 2], s.linked_cards[s.linked_count * 2 + 1], 2);
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  instance->damage_on_card = 0;
  instance->counter_toughness = instance->damage_on_card;
  instance->counter_power = instance->counter_toughness;
  instance->blocking = 0xff;
}

// FUNCTION: MAGIC 0x004ec830
int load_text(const char *file_name, const char *section_name)
{
  struct
  {
    FILE *f;
    char line[300];
    char path[128];
    char section_line[300]; // ebp - 0x134
    unsigned int num_text;
    int i;
  } s;

  strcpy(s.section_line, "@");
  strcat(s.section_line, section_name);
  strcat(s.section_line, "\n");

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\");
  strcat(s.path, file_name);
  // if (!strchr(file_name, '.'))
  //   strcat(s.path, ".txt");

  s.f = fopen(s.path, "rt");
  if (s.f == NULL)
    return -1;

  while (strcmp(s.section_line, s.line))
  {
    if (!fgets(s.line, 300, s.f))
    {
      fclose(s.f);
      return -1;
    }
  }

  fscanf(s.f, "%d", &s.num_text);
  fgets(s.line, 300, s.f);
  if (s.num_text > 225)
  {
    fclose(s.f);
    return -1;
  }

  for (s.i = 0; s.i < (int)s.num_text; s.i++)
  {
    if (!fgets(text_lines[s.i], 300, s.f))
    {
      fclose(s.f);
      return -1;
    }
    text_lines[s.i][strlen(text_lines[s.i]) - 1] = 0;
  }

  fclose(s.f);
  return s.num_text;
}

// FUNCTION: MAGIC 0x0043e668
void discard(int player, int flags, int player_who_controls_effect)
{
  struct
  {
    int tries;         // ebp - 0x4cc
    int selected_card; // ebp - 0x4c8
    target_t target;
    char prompt[600];
    char prompt_line_2[300];
    int dialog_result;
    char prompt_line_1[300];
    int current_card;
    unsigned int found;
  } s;

  s.tries = 0;

  if (hand_count[player] <= 0)
    return;

  if ((active_player == player && (unk_00926804 & 2) == 0 && (hand_count[player] + unk_007161d8) <= 0))
    return;

  if (((unk_008b35ec == player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1) && flags == 0)
  {
    load_text("", "PROMPT_DISCARDACARD");
    C_real_select_target(player,
                         player,
                         player,
                         TARGET_ZONE_HAND,
                         TYPE_NONE,
                         TYPE_NONE,
                         0,
                         0,
                         COLOR_TEST_0,
                         COLOR_TEST_0,
                         -1,
                         ~SUB_WALL,
                         -1,
                         -1,
                         0,
                         0,
                         0,
                         text_lines[0],
                         0,
                         &s.target);
    s.selected_card = s.target.card;
  }
  else
  {
    s.found = 0;
    if ((unk_00926804 & 2) != 0 && active_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x14);
      s.selected_card = unk_007a7d0c;
      if (s.selected_card == -1)
        s.found = 0;
      else
        s.found = 1;
    }
    else
    {
      s.tries = 0;
      do
      {
        s.selected_card = internal_rand(active_cards_count[player]);
        if (PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & 0x20) == 0)
        {
          s.found = 1;
        }
      } while (s.found == 0 && ++s.tries < 999);

      if (s.found == 0)
      {
        for (s.current_card = 0; s.current_card < active_cards_count[player] && s.found == 0; s.current_card++)
        {
          if (PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 2) == 0 && (PLAYER_CARD_INSTANCE(player, s.current_card).state & 0x20) == 0)
          {
            s.found = 1;
            s.selected_card = s.current_card;
          }
        }
      }

      if ((unk_00926804 & 2) != 0 && unk_008b35ec == player)
      {
        if (s.found == 0)
        {
          s.selected_card = -1;
        }
        unk_007a7d0c = s.selected_card;
        unk_007a7d08 = 0x14;
        TENTATIVE_send_network_result(player, 0x14);
      }
    }

    if ((unk_007a79b0[player] & 2) == 0 || player_who_controls_effect != 0)
    {
      if (active_player == player && (unk_00926804 & 2) == 0 && unk_008a9000 != 1)
      {
        if (flags)
        {
          load_text("", "PROMPT_DISCARDACARD");
          do_dialog(player, player, s.selected_card, -1, -1, text_lines[1], 0);
        }
        else
        {
          load_text("", "PROMPT_DISCARDACARD");
          do_dialog(player, player, s.selected_card, -1, -1, text_lines[2], 0);
        }
      }
      FUN_0043ec38(player, s.selected_card);
    }
    else
    {
      load_text("", "PROMPT_DISCARDACARD");
      strcpy(s.prompt_line_1, text_lines[3]);
      strcpy(s.prompt_line_2, text_lines[4]);
      sprintf(s.prompt, " %s\n %s", s.prompt_line_1, s.prompt_line_2);
      s.dialog_result = do_dialog(player, player, s.selected_card, -1, -1, s.prompt, 0);
      if (s.dialog_result == 0)
      {
        real_put_on_top_of_deck(player, PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id);
        PLAYER_CARD_INSTANCE(player, s.selected_card).internal_card_id = -1;
        FUN_004a61a1(player, s.selected_card, 10, 1);
        if (unk_008a9000 != 1)
        {
          play_sound_effect(WAV_DISCARD);
        }
        --hand_count[player];
      }
      else
      {
        FUN_0043ec38(player, s.selected_card);
      }
    }
  }
}

// FUNCTION: MAGIC 0x004b99d0
int mana_producer_sound_on_resolve(int player, int card, event_t event, color_t color)
{
  if (event == EVENT_COUNT_MANA && card == affected_card && player == affected_card_controller)
  {
    if (((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      declare_mana_available(player, color, 1);
    }
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
      return 1;
    else
      return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    undeclare_mana_available_and_produce_it(player, color, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    produced_mana_color = color;
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL && unk_008a9000 != 1)
  {
    play_sound_effect(color + WAV_GREY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004eae35
int produce_mana(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
  total_raw_mana_available[player] += amount;
  return raw_mana_available[player][color];
}

int select_target(int player, int card, target_definition_t *td, const char *prompt, target_t *ret_location)
{
  (void)player;
  (void)card;
  (void)td;
  (void)prompt;
  if (ret_location != NULL)
  {
    ret_location->player = -1;
    ret_location->card = -1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004c0a36
int FUN_004c0a36(int a1, int a2, int a3)
{
  if (a1 < a2)
  {
    return a2;
  }
  if (a1 > a3)
  {
    return a3;
  }
  return a1;
}

// FUNCTION: MOK 0x0042de60
// FUNCTION: MAGIC 0x00432f00
int charge_mana(int player, color_t color, int amount)
{
  struct
  {
    int saved_raw_mana[8];
    int saved_mana_cost[8];
    int mana_paid_by_color[8];
    int mana_paid_total;
    int max_colorless;
    int include_auto_payment;
    int allow_special_x_payment;
    int allow_generic_payment;
    int allow_mana_sources;
    int allow_interrupt_speed_mana_sources;
    int any_variable_costs;
    int done;
    int only_variable_costs;
    int max_variable_mana;
    int mana_to_pay;
    int selected_card;
    int current_internal_card_id;
    int cost_color;
    int amount_to_take;
    int saved_x_value;
    int saved_max_x_value;
    int slot;
    int color_count;
    int chosen_color;
    int old_tapped_state;
    color_test_t produced_special_colors;
    color_test_t payable_colors;
  } s;
  card_instance_t *instance;
  card_data_t *card_data;
  int result;
  int available_mana;
  int i;
  unsigned int special_mana_entry;
  unsigned int produced_color;

  if ((unk_008b4278 & LCBP_DAMAGE_PREVENTION) == 0)
  {
    if (DAT_008ce52c > 0)
    {
      if (color == COLOR_COLORLESS)
      {
        if (amount != -1)
        {
          amount = amount + DAT_008ce52c * -2;
          if (amount < 2)
          {
            amount = 1;
          }
        }
      }
      else if (unk_008ce510[COLOR_COLORLESS] > 0)
      {
        unk_008ce510[COLOR_COLORLESS] = unk_008ce510[COLOR_COLORLESS] + DAT_008ce52c * -2;
        if (unk_008ce510[COLOR_COLORLESS] < 2)
        {
          unk_008ce510[COLOR_COLORLESS] = 1;
        }
      }
    }

    unk_008ce510[color] += amount;
    s.mana_paid_total = 0;
    for (i = 0; i < 7; ++i)
    {
      s.mana_paid_by_color[i] = 0;
    }

    s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
    result = has_mana(player, COLOR_ANY, 1);
    s.max_colorless -= result;

    s.include_auto_payment = 1;
    if (unk_00715fb0 == 0 && ((player != active_player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1 && unk_009252e0 == 0))
    {
      s.allow_interrupt_speed_mana_sources = 0;
      s.allow_mana_sources = 0;
      s.allow_generic_payment = 0;
      s.allow_special_x_payment = 0;
    }
    else
    {
      s.allow_special_x_payment = 1;
      s.allow_generic_payment = 1;
      s.allow_mana_sources = 1;
      if ((player == active_player && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
      {
        s.allow_interrupt_speed_mana_sources = 1;
      }
      else
      {
        s.allow_interrupt_speed_mana_sources = 0;
      }
    }

    if (player == active_player && (unk_00926804 & 2) == 0)
    {
      s.max_variable_mana = max_x_value;
      for (i = COLOR_COLORLESS; i < 7; ++i)
      {
        if (unk_008ce510[i] == -1)
        {
          available_mana = has_mana(player, i, 1);
          if (unk_008a9000 == 1)
          {
            result = internal_rand(3);
            if (result == 0 || available_mana < 2)
            {
              s.mana_to_pay = available_mana;
            }
            else
            {
              s.mana_to_pay = internal_rand(available_mana - 1) + 1;
            }

            if (max_x_value != -1 && s.mana_to_pay > max_x_value)
            {
              s.mana_to_pay = max_x_value;
            }

            unk_00939340 = s.mana_to_pay;
            FUN_004e4f11();
          }
          else
          {
            FUN_004e5089();
            if (unk_00939340 == 99)
            {
              unk_00939340 = 0;
            }
            s.mana_to_pay = unk_00939340;
          }

          s.max_variable_mana = s.mana_to_pay;
        }
      }

      max_x_value = s.max_variable_mana;
    }

    if (DAT_008ce52c < 1)
    {
      x_value = 0;
    }
    else
    {
      x_value = DAT_008ce52c * 2;
    }

    if (max_x_value == 0)
    {
      for (i = 0; i < 7; ++i)
      {
        if (unk_008ce510[i] == -1)
        {
          unk_008ce510[i] = 0;
        }
      }
    }

    s.any_variable_costs = 0;
    for (i = 0; i < 7; ++i)
    {
      if (unk_008ce510[i] == -1)
      {
        s.any_variable_costs = 1;
      }
    }

    if (s.include_auto_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      FUN_0043410a(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless);
      copy_mana_pool_to_display();
    }

    if (s.allow_special_x_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0 && s.any_variable_costs != 0)
    {
      FUN_004342b3(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_generic_payment != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      FUN_004344b0(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
      copy_mana_pool_to_display();
    }

    if (s.allow_mana_sources != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1e, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1c, 1);
      }
    }

    if (s.allow_interrupt_speed_mana_sources != 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x14, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x04, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x1a, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x18, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x10, 1);
      }
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        FUN_00434af9(player, s.mana_paid_by_color, &s.mana_paid_total, 0x00, 1);
      }
    }

    if ((player == active_player && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
    {
      if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0 && s.any_variable_costs == 0)
      {
        spell_fizzled = 1;
      }
    }
    else if (FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
    {
      s.done = 0;
      while (s.done == 0 && FUN_004346ab(unk_008ce510, x_value, max_x_value) == 0)
      {
        s.only_variable_costs = 1;
        for (i = 0; i < 7; ++i)
        {
          if (unk_008ce510[i] > 0)
          {
            s.only_variable_costs = 0;
          }
        }

        FUN_0043479e(unk_00748770, unk_008ce510, x_value, max_x_value);
        s.selected_card =
            FUN_004e9c50(player, player, player, 0, 0, unk_00748770, s.only_variable_costs != 0 ? 3 : 1);

        if (unk_00742fcc == -1 && (s.selected_card == -1 || s.selected_card == -2))
        {
          if (_DAT_0074303c == -2)
          {
            if (unk_00716244 == -1 && unk_00716248 == -1)
            {
              if (s.selected_card == -1)
              {
                spell_fizzled = 1;
              }
              s.done = 1;
            }
            else if (s.only_variable_costs != 0)
            {
              s.done = 1;
            }
          }
          else if (_DAT_0074303c == -3 && player == DAT_00715fa4 && DAT_0072c8e0 != -1)
          {
            if (raw_mana_available[player][DAT_0072c8e0] > 0 && (unk_008ce510[DAT_0072c8e0] != 0 || unk_008ce510[COLOR_COLORLESS] != 0 || unk_008ce510[COLOR_ARTIFACT] != 0) && (DAT_0072c8e0 != COLOR_ARTIFACT || unk_008ce510[COLOR_ARTIFACT] != 0))
            {
              if (unk_008ce510[DAT_0072c8e0] == 0)
              {
                if (unk_008ce510[COLOR_ARTIFACT] == 0)
                {
                  s.cost_color = COLOR_COLORLESS;
                }
                else
                {
                  s.cost_color = COLOR_ARTIFACT;
                }
              }
              else
              {
                s.cost_color = DAT_0072c8e0;
              }

              s.amount_to_take =
                  FUN_00435b88(unk_008ce510, s.cost_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
              FUN_00435b18(
                  unk_008ce510, s.cost_color, s.amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
              copy_mana_pool_to_display();
            }

            if (raw_mana_available[player][DAT_0072c8e0] > 0)
            {
              s.produced_special_colors = 0;
              for (s.slot = 0; s.slot < 10 && unk_007161e0[player][s.slot] != -1; ++s.slot)
              {
                produced_color = (unsigned short)unk_007161e0[player][s.slot];
                if ((int)produced_color == DAT_0072c8e0)
                {
                  special_mana_entry = (unsigned int)unk_007161e0[player][s.slot] >> 16;
                  s.produced_special_colors |= 1 << ((unsigned char)special_mana_entry & 0x1f);
                }
              }

              s.payable_colors = 0;
              for (i = 0; i < 7; ++i)
              {
                if (unk_008ce510[i] != 0)
                {
                  s.payable_colors |= 1 << ((unsigned char)i & 0x1f);
                }
              }

              s.produced_special_colors &= s.payable_colors;
              if (s.produced_special_colors != 0)
              {
                s.color_count = 0;
                for (i = 0; i < 7; ++i)
                {
                  if ((s.produced_special_colors & (1 << ((unsigned char)i & 0x1f))) != 0)
                  {
                    ++s.color_count;
                  }
                }

                if (s.color_count == 1)
                {
                  s.chosen_color = single_color_test_bit_to_color_t(s.produced_special_colors);
                }
                else
                {
                  load_text("promptsX1.txt", "PROMPT_GRABMANA_DUALUSE");
                  s.chosen_color = choose_a_color(player, text_lines[0], 1, DAT_0072c8e0, s.produced_special_colors);
                }

                s.amount_to_take =
                    FUN_00435b88(unk_008ce510, s.chosen_color, raw_mana_available[player], DAT_0072c8e0, unk_00715fb0, max_x_value, x_value);
                FUN_00435b18(
                    unk_008ce510, s.chosen_color, s.amount_to_take, &x_value, max_x_value, player, DAT_0072c8e0, s.mana_paid_by_color, &s.mana_paid_total);
                copy_mana_pool_to_display();
              }
            }
          }
        }
        else
        {
          instance = &PLAYER_CARD_INSTANCE(player, s.selected_card);
          s.current_internal_card_id = instance->internal_card_id;
          card_data = &global_cards_data[s.current_internal_card_id];

          if ((unk_00742fcc == -1 || s.selected_card != -1) && (card_data->extra_ability & EA_MANA_SOURCE) != 0 && ((card_data->type & TYPE_INTERRUPT) != 0 || (((instance->state & STATE_IN_PLAY) != 0) && (instance->color & 0x80) == 0 && (instance->state & STATE_TAPPED) == 0 && (((instance->color & 3) == 0) || (card_data->type & TYPE_CREATURE) == 0))))
          {
            for (i = 0; i < 8; ++i)
            {
              s.saved_raw_mana[i] = raw_mana_available[player][i];
            }

            s.saved_x_value = x_value;
            x_value = 0;
            s.saved_max_x_value = max_x_value;
            max_x_value = -1;
            for (i = 0; i < 7; ++i)
            {
              s.saved_mana_cost[i] = unk_008ce510[i];
              unk_008ce510[i] = 0;
            }

            if ((instance->state & 0x800002) == STATE_IN_PLAY)
            {
              if ((card_data->type & TYPE_LAND) != 0 || dispatch_event_to_single_card(player, s.selected_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
              {
                FUN_00443ee2(player, s.selected_card, EVENT_ACTIVATE, player, 0);
                unk_00938e2c = FUN_00434746(s.saved_mana_cost);
                unk_0092664c[6] = -1;
                s.old_tapped_state = instance->state & STATE_TAPPED;
                dispatch_event_to_single_card(player, s.selected_card, EVENT_ACTIVATE, 1 - player, -1);
                unk_00938e2c = 0;
                if (spell_fizzled == 1)
                {
                  spell_fizzled = 0;
                  obliterate_top_card_of_stack();
                }
                else
                {
                  if (s.old_tapped_state == 0 && (PLAYER_CARD_INSTANCE(player, s.selected_card).state & STATE_TAPPED) != 0)
                  {
                    dispatch_event(player, s.selected_card, EVENT_TAP_CARD);
                  }
                  if (unk_008a9000 != 1)
                  {
                    play_sound_effect(WAV_TAP);
                  }
                  resolve_top_card_on_stack();
                  TENTATIVE_reassess_all_cards();
                }
              }
            }
            else
            {
              FUN_0043ece1(player, s.selected_card);
              TENTATIVE_reassess_all_cards();
            }

            x_value = s.saved_x_value;
            max_x_value = s.saved_max_x_value;
            for (i = 0; i < 7; ++i)
            {
              unk_008ce510[i] = s.saved_mana_cost[i];
            }

            for (i = 0; i < 8; ++i)
            {
              raw_mana_available[player][i] -= s.saved_raw_mana[i];
            }

            s.max_colorless = 0;
            FUN_0043410a(player, s.mana_paid_by_color, &s.mana_paid_total, 0);
            FUN_004342b3(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);
            FUN_004344b0(player, s.mana_paid_by_color, &s.mana_paid_total, s.max_colorless, &x_value, max_x_value);

            for (i = 0; i < 8; ++i)
            {
              raw_mana_available[player][i] += s.saved_raw_mana[i];
            }

            copy_mana_pool_to_display();
          }
        }
      }
    }
  }

  if (spell_fizzled == 1)
  {
    FUN_00435db0(s.mana_paid_by_color);
    for (i = 0; i < 7; ++i)
    {
      produce_mana(player, i, s.mana_paid_by_color[i]);
      s.mana_paid_by_color[i] = 0;
    }
    s.mana_paid_total = 0;
    x_value = 0;
  }

  for (i = 0; i < 8; ++i)
  {
    unk_008ce510[i] = 0;
  }
  max_x_value = -1;

  return s.mana_paid_total;
}

// FUNCTION: MAGIC 0x004346ab
int FUN_004346ab(int *mana_cost, int required_colorless, int max_x)
{
  int color;

  for (color = 0; color < 7; color++)
  {
    if (mana_cost[color] > 0)
    {
      return 0;
    }

    if (mana_cost[color] == -1 && max_x != -1 && required_colorless < max_x)
    {
      return 0;
    }

    if (mana_cost[color] == -1 && max_x == -1)
    {
      return 0;
    }
  }

  return 1;
}

// FUNCTION: MAGIC 0x00434746
unsigned int FUN_00434746(int *mana)
{
  int color;
  unsigned int result;

  result = 0;
  for (color = 0; color < 7; ++color)
  {
    if (mana[color] != 0)
    {
      result = result | (1 << (unsigned char)color);
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0043479e
int FUN_0043479e(char *prompt, int *mana_cost, int x_paid, int max_x)
{
  char symbol[8];
  char mana_text[100];
  int i;
  int total_generic;

  if (prompt == NULL)
  {
    prompt = unk_00748770;
  }

  strcpy(prompt, "Select mana to pay ");
  mana_text[0] = '\0';

  if (mana_cost[COLOR_COLORLESS] == -1 || mana_cost[COLOR_ARTIFACT] == -1)
  {
    if (max_x == -1)
    {
      sprintf(mana_text + strlen(mana_text), "|X%d", x_paid);
    }
    else if (x_paid < max_x)
    {
      sprintf(mana_text + strlen(mana_text), "|X%d/%d", x_paid, max_x);
    }
  }
  else if (mana_cost[COLOR_COLORLESS] != 0 || mana_cost[COLOR_ARTIFACT] != 0)
  {
    total_generic = mana_cost[COLOR_COLORLESS] + mana_cost[COLOR_ARTIFACT];
    while (total_generic > 9)
    {
      strcat(mana_text, "|10");
      total_generic -= 10;
    }
    if (total_generic != 0)
    {
      sprintf(mana_text + strlen(mana_text), "|%d", total_generic);
    }
  }

  for (i = COLOR_BLACK; i <= COLOR_WHITE; ++i)
  {
    symbol[0] = '|';
    if (i == COLOR_BLACK)
    {
      symbol[1] = 'B';
    }
    else if (i == COLOR_BLUE)
    {
      symbol[1] = 'U';
    }
    else if (i == COLOR_GREEN)
    {
      symbol[1] = 'G';
    }
    else if (i == COLOR_RED)
    {
      symbol[1] = 'R';
    }
    else
    {
      symbol[1] = 'W';
    }
    symbol[2] = '\0';

    if (mana_cost[i] == -1)
    {
      if (max_x == -1)
      {
        sprintf(mana_text + strlen(mana_text), "%s%d", symbol, x_paid);
      }
      else if (x_paid < max_x)
      {
        sprintf(mana_text + strlen(mana_text), "%s%d/%d", symbol, x_paid, max_x);
      }
    }
    else if (mana_cost[i] > 0)
    {
      total_generic = mana_cost[i];
      while (total_generic > 0)
      {
        strcat(mana_text, symbol);
        --total_generic;
      }
    }
  }

  strcat(prompt, mana_text);
  return 0;
}

// FUNCTION: MAGIC 0x00435c91
int FUN_00435c91(int color, int amount)
{
  int result;

  result = unk_005607e0;
  if (0 < unk_005607e0)
  {
    unk_00637558[unk_005607e0 - 1][color] += amount;
  }
  return 0 < result;
}

// FUNCTION: MAGIC 0x004eae77
int unproduce_mana(int player, int color, int amount)
{
  raw_mana_available[player][color] -= amount;
  total_raw_mana_available[player] -= amount;
  return raw_mana_available[player][color];
}

// FUNCTION: MAGIC 0x00435b18
void FUN_00435b18(int *mana_cost,
                  int cost_color,
                  int amount,
                  int *special_mana,
                  int max_x,
                  int player,
                  int mana_color,
                  int *mana_paid_by_color,
                  int *total_mana_paid)
{
  (void)max_x;

  unproduce_mana(player, mana_color, amount);
  if (mana_cost[cost_color] == -1)
  {
    *special_mana += amount;
  }
  else
  {
    mana_cost[cost_color] -= amount;
  }
  mana_paid_by_color[mana_color] += amount;
  FUN_00435c91(mana_color, amount);
  *total_mana_paid += amount;
}

// FUNCTION: MAGIC 0x0043410a
void FUN_0043410a(int player, int *mana_paid_by_color, int *total_mana_paid, int max_colorless)
{
  unsigned short produced_color;
  unsigned int special_mana;
  int slot;
  int color;

  for (color = 0; color < 7; ++color)
  {
    while (unk_008ce510[color] > 0 && raw_mana_available[player][color] > 0)
    {
      FUN_00435b18(unk_008ce510, color, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
    }
  }

  while (unk_008ce510[6] > 0 && raw_mana_available[player][0] > 0 && max_colorless < unk_008ce510[6])
  {
    FUN_00435b18(unk_008ce510, 6, 1, (int *)0, 0, player, 0, mana_paid_by_color, total_mana_paid);
  }

  slot = 0;
  while (slot < 10 && unk_007161e0[player][slot] != -1)
  {
    produced_color = (unsigned short)unk_007161e0[player][slot];
    special_mana = (unsigned int)unk_007161e0[player][slot];
    while (raw_mana_available[player][produced_color] > 0 && unk_008ce510[special_mana >> 16] > 0)
    {
      FUN_00435b18(
          unk_008ce510, special_mana >> 16, 1, (int *)0, 0, player, produced_color, mana_paid_by_color, total_mana_paid);
    }
    slot = slot + 1;
  }
}

// FUNCTION: MAGIC 0x004342b3
void FUN_004342b3(int player,
                  int *mana_paid_by_color,
                  int *total_mana_paid,
                  int max_colorless,
                  int *special_mana_override,
                  int max_x)
{
  unsigned short produced_color;
  unsigned int special_mana;
  int slot;
  int color;

  for (color = 0; color < 7; ++color)
  {
    if (unk_008ce510[color] == -1)
    {
      while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
      {
        FUN_00435b18(
            unk_008ce510, color, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
      }
    }
  }

  if (unk_008ce510[6] == -1)
  {
    while (raw_mana_available[player][0] > 0 && max_colorless < unk_008ce510[6] &&
           (x_value < max_x || max_x == -1))
    {
      FUN_00435b18(
          unk_008ce510, 6, 1, special_mana_override, max_x, player, 0, mana_paid_by_color, total_mana_paid);
    }
  }

  slot = 0;
  while (slot < 10 && unk_007161e0[player][slot] != -1)
  {
    produced_color = (unsigned short)unk_007161e0[player][slot];
    special_mana = (unsigned int)unk_007161e0[player][slot];
    if (unk_008ce510[special_mana >> 16] == -1)
    {
      while (raw_mana_available[player][produced_color] > 0 && (x_value < max_x || max_x == -1))
      {
        FUN_00435b18(unk_008ce510,
                     special_mana >> 16,
                     1,
                     special_mana_override,
                     max_x,
                     player,
                     produced_color,
                     mana_paid_by_color,
                     total_mana_paid);
      }
    }
    slot = slot + 1;
  }
}

// FUNCTION: MAGIC 0x004344b0
void FUN_004344b0(int player,
                  int *mana_paid_by_color,
                  int *total_mana_paid,
                  int max_colorless,
                  int *special_mana_override,
                  int max_x)
{
  int color;

  if (unk_008ce510[6] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (unk_008ce510[6] < 1)
      {
        while (raw_mana_available[player][color] > 0 && unk_008ce510[6] == -1 &&
               (max_x == -1 || max_colorless < max_x - x_value))
        {
          FUN_00435b18(
              unk_008ce510, 6, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
      else
      {
        while (raw_mana_available[player][color] > 0 && max_colorless < unk_008ce510[6])
        {
          FUN_00435b18(unk_008ce510, 6, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
        }
      }
    }
  }

  if (unk_008ce510[0] != 0)
  {
    for (color = 0; color < 7; ++color)
    {
      if (color != 6)
      {
        if (unk_008ce510[0] == -1)
        {
          while (raw_mana_available[player][color] > 0 && (x_value < max_x || max_x == -1))
          {
            FUN_00435b18(
                unk_008ce510, 0, 1, special_mana_override, max_x, player, color, mana_paid_by_color, total_mana_paid);
          }
        }
        else
        {
          while (unk_008ce510[0] > 0 && raw_mana_available[player][color] > 0)
          {
            FUN_00435b18(unk_008ce510, 0, 1, (int *)0, 0, player, color, mana_paid_by_color, total_mana_paid);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x00435b88
int FUN_00435b88(int *mana_cost,
                 int cost_color,
                 int *mana_available,
                 int mana_color,
                 int allow_multi_pay,
                 int max_x,
                 int x_paid)
{
  int amount;

  if (allow_multi_pay == 0)
  {
    amount = 1;
  }
  else if (mana_cost[cost_color] == -1)
  {
    if (max_x == -1)
    {
      amount = mana_available[mana_color];
    }
    else
    {
      amount = mana_available[mana_color];
      if (max_x - x_paid <= amount)
      {
        amount = max_x - x_paid;
      }
    }
  }
  else
  {
    amount = mana_cost[cost_color];
    if (mana_available[mana_color] <= amount)
    {
      amount = mana_available[mana_color];
    }
  }

  return amount;
}

// FUNCTION: MAGIC 0x00435db0
int FUN_00435db0(int *mana_paid_by_color)
{
  int color;

  if (unk_005607e0 > 0)
  {
    for (color = 0; color < 7; ++color)
    {
      unk_00637558[unk_005607e0 - 1][color] -= mana_paid_by_color[color];
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00435643
int FUN_00435643(int player, int card, unsigned char activation_flags)
{
  struct
  {
    int internal_card_id;
    int result;
    int csvid;
  } s;

  s.internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  s.csvid = *(int *)&global_cards_data[s.internal_card_id].id;
  s.result = 1;

  if ((PLAYER_CARD_INSTANCE(player, card).state & 0x800002) == 2 && (global_cards_data[s.internal_card_id].extra_ability & 0x1000) != 0 && (global_cards_data[s.internal_card_id].extra_ability & 0x10000) == 0)
  {
    if ((activation_flags & 1) != 0 && (s.internal_card_id < 5 || s.csvid == 0x366 || PLAYER_CARD_INSTANCE(player, card).mana_color == '@'))
    {
      s.result = 0;
    }
    if ((activation_flags & 2) != 0 && (global_cards_data[s.internal_card_id].type & TYPE_LAND) != 0 && s.internal_card_id > 4 && s.csvid != 0x366 && PLAYER_CARD_INSTANCE(player, card).mana_color != '@')
    {
      s.result = 0;
    }
    if ((activation_flags & 4) != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x40000) != 0)
    {
      s.result = 0;
    }
    if ((activation_flags & 8) != 0 && (global_cards_data[s.internal_card_id].type & TYPE_ENCHANTMENT) != 0)
    {
      s.result = 0;
    }
    if ((activation_flags & 0x10) != 0 && (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0)
    {
      s.result = 0;
    }
  }
  else
  {
    s.result = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00435881
int FUN_00435881(int player, int card)
{
  struct
  {
    int old_max_x_value;
    int result;
    int saved_mana_cost[8];
    int old_x_value;
    int color;
  } s;
  unsigned int original_state;

  s.old_x_value = x_value;
  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 || ((((unsigned int)PLAYER_CARD_INSTANCE(player, card).untap_status & 3) != 0) && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0))
  {
    s.result = 0;
  }
  else
  {
    x_value = 0;
    s.old_max_x_value = max_x_value;
    max_x_value = -1;
    for (s.color = 0; s.color < 7; ++s.color)
    {
      s.saved_mana_cost[s.color] = unk_008ce510[s.color];
      unk_008ce510[s.color] = 0;
    }
    FUN_00443ee2(player, card, 0x72, player, 0);
    unk_00938e2c = FUN_00434746(s.saved_mana_cost);
    unk_00926664 = -1;
    original_state = PLAYER_CARD_INSTANCE(player, card).state;
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    unk_00938e2c = 0;
    if (spell_fizzled == 1)
    {
      spell_fizzled = 0;
      obliterate_top_card_of_stack();
      s.result = 0;
    }
    else
    {
      if ((original_state & STATE_TAPPED) == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
      {
        dispatch_event(player, card, EVENT_TAP_CARD);
      }
      if (unk_008a9000 != 1)
      {
        play_sound_effect(WAV_TAP);
      }
      resolve_top_card_on_stack();
      s.result = 1;
    }
    max_x_value = s.old_max_x_value;
    x_value = s.old_x_value;
    for (s.color = 0; s.color < 7; ++s.color)
    {
      unk_008ce510[s.color] = s.saved_mana_cost[s.color];
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x00434af9
int FUN_00434af9(int player,
                 int *mana_paid_by_color,
                 int *total_mana_paid,
                 unsigned char activation_flags,
                 int include_special_mana)
{
  struct
  {
    int current_card;
    int color;
    int dual_mana_slot;
    int max_colorless;
    int can_activate;
    unsigned char produced_color;
    unsigned char special_color;
    int any_variable_costs;
  } s;
  int result;

  s.any_variable_costs = 0;
  for (s.color = 0; s.color < 7; ++s.color)
  {
    if (unk_008ce510[s.color] == -1)
    {
      s.any_variable_costs = 1;
    }
  }

  if (player == active_player && (unk_00926804 & 2) != 0)
  {
    result = 0;
    while (result != -1)
    {
      TENTATIVE_wait_for_network_result(player, 0x10);
      result = unk_007a7d0c;
      if (unk_007a7d0c != -1 && FUN_00435881(player, unk_007a7d0c) != 0)
      {
        s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
        s.current_card = has_mana(player, COLOR_ANY, 1);
        s.max_colorless -= s.current_card;
        FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
        FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
        FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
      }
    }
    copy_mana_pool_to_display();
  }
  else
  {
    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          for (s.color = 0; s.color < 7; ++s.color)
          {
            if (unk_008ce510[s.color] > 0 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
            else if (unk_008ce510[s.color] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.can_activate = 1;
            }
          }
          if (include_special_mana != 0 && !s.can_activate)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && unk_007161e0[player][s.dual_mana_slot] != -1)
            {
              s.produced_color = (unsigned char)unk_007161e0[player][s.dual_mana_slot];
              if (unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] > 0 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              else if (unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] == -1 && x_value < max_x_value && max_x_value != -1 && (((unsigned int)(1 << s.produced_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.can_activate = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.can_activate)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0 && (unk_008ce510[0] != 0 || unk_008ce510[6] != 0))
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.can_activate = 0;
          if (unk_008ce510[0] < 1)
          {
            if (unk_008ce510[0] == -1 && x_value < max_x_value && max_x_value != -1)
            {
              s.can_activate = 1;
            }
            else if (unk_008ce510[6] < 1)
            {
              if (unk_008ce510[6] == -1 && x_value < max_x_value && max_x_value != -1)
              {
                s.can_activate = 1;
              }
            }
            else
            {
              s.can_activate = 1;
            }
          }
          else
          {
            s.can_activate = 1;
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && unk_008ce510[6] == 0)
          {
            s.can_activate = 0;
          }
          if (s.can_activate)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    result = FUN_004346ab(unk_008ce510, x_value, max_x_value);
    if (result == 0 && s.any_variable_costs && max_x_value == -1)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[player]; ++s.current_card)
      {
        result = FUN_00435643(player, s.current_card, activation_flags);
        if (result != 0 && dispatch_event_to_single_card(player, s.current_card, EVENT_CAN_ACTIVATE, 1 - player, -1) != 0)
        {
          s.any_variable_costs = 0;
          for (s.color = 1; s.color < 7; ++s.color)
          {
            if (max_x_value == -1 && unk_008ce510[s.color] == -1 && (((unsigned int)(1 << (unsigned char)s.color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && unk_008ce510[0] == -1)
            {
              s.any_variable_costs = 1;
            }
            else if (max_x_value == -1 && unk_008ce510[6] == -1)
            {
              s.any_variable_costs = 1;
            }
          }
          if (PLAYER_CARD_INSTANCE(player, s.current_card).mana_color == '@' && unk_008ce510[6] != -1)
          {
            s.any_variable_costs = 0;
          }
          if (include_special_mana != 0 && !s.any_variable_costs)
          {
            s.dual_mana_slot = 0;
            while (s.dual_mana_slot < 10 && unk_007161e0[player][s.dual_mana_slot] != -1)
            {
              s.special_color = (unsigned char)unk_007161e0[player][s.dual_mana_slot];
              if (max_x_value == -1 && unk_008ce510[unk_007161e0[player][s.dual_mana_slot] >> 16] == -1 && (((unsigned int)(1 << s.special_color) & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(player, s.current_card).mana_color) != 0))
              {
                s.any_variable_costs = 1;
              }
              ++s.dual_mana_slot;
            }
          }
          if (s.any_variable_costs)
          {
            if ((unk_00926804 & 2) != 0)
            {
              unk_007a7d08 = 0x10;
              unk_007a7d0c = s.current_card;
              TENTATIVE_send_network_result(player, 0x10);
              Sleep(0x32);
            }
            if (FUN_00435881(player, s.current_card) != 0)
            {
              s.max_colorless = has_mana(player, COLOR_ARTIFACT, 1);
              result = has_mana(player, COLOR_ANY, 1);
              s.max_colorless -= result;
              FUN_0043410a(player, mana_paid_by_color, total_mana_paid, s.max_colorless);
              FUN_004342b3(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
              FUN_004344b0(player, mana_paid_by_color, total_mana_paid, s.max_colorless, &x_value, max_x_value);
            }
          }
        }
      }
    }

    if ((unk_00926804 & 2) != 0)
    {
      unk_007a7d08 = 0x10;
      unk_007a7d0c = -1;
      TENTATIVE_send_network_result(player, 0x10);
    }
    copy_mana_pool_to_display();
  }

  return 1;
}

// FUNCTION: MAGIC 0x004e9c50
int FUN_004e9c50(int player,
                 int arg_2,
                 int player_to_check,
                 unsigned int required_type,
                 unsigned int required_color,
                 char *prompt,
                 int dialog_mode)
{
  struct
  {
    int candidate_player[60];
    int candidate_card[60];
    int num_candidates;
    int controller;
    int card;
    int selected_target[2];
    int selection_code;
    int choice;
  } s;
  card_instance_t *instance;
  card_data_t *card_data;

  if (spell_fizzled == 1 || (unk_008a9000 == 1 && unk_008b35ec == player))
  {
    return -1;
  }

  if ((DAT_00777854 & 1) != 0)
  {
    player_to_check = arg_2;
  }

  if ((active_player == player && (unk_00926804 & 2) == 0) || unk_008a9000 == 1 || unk_009252e0 != 0)
  {
    s.num_candidates = 0;
    for (s.controller = 0; s.controller < 2; ++s.controller)
    {
      if (player_to_check == -1 || player_to_check == s.controller)
      {
        for (s.card = 0; s.card < active_cards_count[s.controller]; ++s.card)
        {
          instance = &PLAYER_CARD_INSTANCE(s.controller, s.card);
          if (instance->internal_card_id != -1 && (((instance->state & 0x800002) == STATE_IN_PLAY) || (unk_009252e0 != 0 && unk_008b35ec == player)))
          {
            card_data = &global_cards_data[instance->internal_card_id];
            if ((required_type == 0xfffffffe || required_type == 0 || (required_type & card_data->type) != 0) && (required_color == 0 || required_color == 1 || (required_color & (unsigned char)instance->color) != 0))
            {
              s.candidate_player[s.num_candidates] = s.controller;
              s.candidate_card[s.num_candidates] = s.card;
              ++s.num_candidates;
            }
          }
        }

        if ((required_type == 0 || required_type == 0xfffffffe) && (required_color == 0 || required_color == 1))
        {
          s.candidate_player[s.num_candidates] = s.controller;
          s.candidate_card[s.num_candidates] = -1;
          ++s.num_candidates;
        }
      }
    }

    if (s.num_candidates == 0)
    {
      return -1;
    }

    if (unk_008a9000 == 1)
    {
      unk_00939340 = internal_rand(s.num_candidates);
      FUN_004e4f11();
      s.choice = unk_00939340;
    }
    else
    {
      s.choice = internal_rand(s.num_candidates);
    }

    unk_00742fcc = s.candidate_player[s.choice];
    return s.candidate_card[s.choice];
  }

  s.selection_code = -1;
  s.selected_target[0] = -1;
  s.selected_target[1] = -1;
  if (required_type == 0 || required_type == 0xff || required_type == 0xfffffffe)
  {
    s.choice = -1;
  }
  else
  {
    s.choice = required_type;
  }
  FUN_004466b5(player, arg_2, prompt, dialog_mode, s.choice, required_color, -1, -1, &_DAT_0074303c, s.selected_target, 0, 0);
  text_lines[0][0] = '\0';
  if (s.selected_target[0] != -1)
  {
    DAT_0072c8e4 = 0;
  }
  unk_00742fcc = s.selected_target[0];
  return s.selected_target[1];
}

// FUNCTION: MAGIC 0x004e4ff3
void FUN_004e4ff3(int a1)
{
  (void)a1;
}

// FUNCTION: MOK 0x00499010
// FUNCTION: MAGIC 0x004e503e
void FUN_004e503e(int a1)
{
  (void)a1;
}

// FUNCTION: MOK 0x0049d710
// FUNCTION: MAGIC 0x004eb23d
int has_mana_w_global_cost_mod(int player, int card, color_t color, int amount)
{
  int result;

  result = 0;

  if (amount == 0)
  {
    if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
    {
      return has_mana(
          player, 7, unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)]);
    }

    return 1;
  }
  else
  {
    result = has_mana(player, color, amount);
    if (result != 0)
    {
      if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0)
      {
        return has_mana(
            player,
            7,
            unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] + amount);
      }
      else
      {
        return result;
      }
    }
  }

  return 0;
}

// FUNCTION: MOK 0x004a09a0
// FUNCTION: MAGIC 0x004ef850
int create_legacy_effect(int player, int card, int legacy_iid, int target_player, int target_card)
{
  int legacy_card;
  int display_pic_num;
  int source_internal_card_id;

  legacy_card = add_card_to_hand(player, legacy_iid);
  if (legacy_card != -1 && card != -1)
  {
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 || PLAYER_CARD_INSTANCE(player, card).internal_card_id == unk_0091a80c)
    {
      source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    }
    else
    {
      source_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    }

    PLAYER_CARD_INSTANCE(player, legacy_card).state = player == 0 ? 2 : 0x1002;
    PLAYER_CARD_INSTANCE(player, legacy_card).mana_color = PLAYER_CARD_INSTANCE(player, card).mana_color;
    PLAYER_CARD_INSTANCE(player, legacy_card).color = PLAYER_CARD_INSTANCE(player, card).color;
    PLAYER_CARD_INSTANCE(player, legacy_card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(player, legacy_card).damage_source_card = card;
    display_pic_num = FUN_004964dd(*(int *)&global_cards_data[source_internal_card_id].id, player, card);
    *(int *)&PLAYER_CARD_INSTANCE(player, legacy_card).display_pic_csv_id =
        (display_pic_num << 16) | *(unsigned int *)&global_cards_data[source_internal_card_id].id;
    PLAYER_CARD_INSTANCE(player, legacy_card).damage_target_player = (char)target_player;
    PLAYER_CARD_INSTANCE(player, legacy_card).damage_target_card = target_card;

    if (target_player != -1 && target_card != -1)
    {
      PLAYER_CARD_INSTANCE(target_player, target_card).regen_status |= 0x0f000000;
    }

    PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= PLAYER_CARD_INSTANCE(player, card).token_status & 6;
    memcpy(PLAYER_CARD_INSTANCE(player, legacy_card).color_id, PLAYER_CARD_INSTANCE(player, card).color_id, 6);
    memcpy(PLAYER_CARD_INSTANCE(player, legacy_card).hack_mode, PLAYER_CARD_INSTANCE(player, card).hack_mode, 6);
  }

  return legacy_card;
}

// FUNCTION: MAGIC 0x0043ece1
int FUN_0043ece1(int player, int card)
{
  (void)player;
  (void)card;
  return 1;
}

// FUNCTION: MAGIC 0x0043ec38
int FUN_0043ec38(int player, int card)
{
  dispatch_event_to_single_card(player, card, EVENT_DISCARD, 1 - player, -1);
  FUN_004b14f5(player, card);
  PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  FUN_004a61a1(player, card, 11, 1);
  if (unk_008a9000 != 1)
  {
    play_sound_effect(WAV_DISCARD);
  }
  --hand_count[player];
  return 0;
}

// FUNCTION: MAGIC 0x00446c95
void FUN_00446c95(void)
{
  return;
}

// FUNCTION: MAGIC 0x004a61a1
void FUN_004a61a1(int player, int card, int unk1, int unk2)
{
  if (unk_008a9000 != 1)
  {
    FUN_00446c95();
  }
}

// FUNCTION: MAGIC 0x004b14f5
void FUN_004b14f5(int player, int card)
{
  int graveyard_index;
  int original_internal_card_id;
  unsigned int graveyard_player;

  original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  graveyard_player = (PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0;
  unk_007a7c58[graveyard_player] |= global_cards_data[original_internal_card_id].type;

  graveyard_index = 0;
  while (graveyard_index < 500)
  {
    if (global_graveyard_slots[graveyard_player][graveyard_index] == -1)
    {
      break;
    }
    ++graveyard_index;
  }

  if (graveyard_index < 500)
  {
    global_graveyard_slots[graveyard_player][graveyard_index] = original_internal_card_id;
  }
}

// FUNCTION: MAGIC 0x004b5cf5
void FUN_004b5cf5(int player, int internal_card_id)
{
  int deck_position;

  deck_position = 0;
  while (deck_position < 500 && global_library[player][deck_position] != -1)
  {
    ++deck_position;
  }
  if (deck_position < 500)
  {
    global_library[player][deck_position] = internal_card_id;
  }
}

// FUNCTION: MAGIC 0x0052460c
int FUN_0052460c(int blocker_player, int blocker_card, int attacker_player, int attacker_card)
{
  int result;
  int saved_event_result;

  saved_event_result = event_result;
  event_result = 0;
  dispatch_event_to_single_card(blocker_player,
                                blocker_card,
                                EVENT_BLOCK_LEGALITY,
                                attacker_player,
                                attacker_card);
  result = event_result == 0;
  event_result = saved_event_result;
  return result;
}

// FUNCTION: MAGIC 0x00441f16
int C_get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  card_instance_t *instance;
  card_instance_t *test_instance;
  unsigned int base_abilities;
  unsigned int current_abilities;
  unsigned int hidden_abilities;
  unsigned int result;
  int current_color;
  int raw_card_data;
  int saved_event_state;
  int test_card;
  int test_player;
  int token_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  saved_event_state = unk_00712938;
  ++unk_0093f9c0;
  affected_card_controller = player;
  affected_card = card;
  unk_008b4dd0 = instance->internal_card_id;
  unk_008cf6d4 = (int)(char)global_cards_data[unk_008b4dd0].color;
  attacking_card = new_attacking_card;

  switch (event)
  {
  case EVENT_POWER:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[unk_008b4dd0].power & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[unk_008b4dd0].power;
    }
    result += instance->counter_power;
    if ((instance->regen_status & 0x04000000) != 0)
    {
      instance->regen_status &= 0xfbffffff;
      event_result = result;
    }
    else
    {
      event_result = (unsigned int)instance->power;
    }
    if ((int)event_result < 0)
    {
      event_result = 0;
    }
    if ((instance->token_status & 0x4000) != 0)
    {
      event_result <<= 1;
    }
    break;

  case EVENT_TOUGHNESS:
    if ((instance->state & 0x800002) == 2)
    {
      result = (int)global_cards_data[unk_008b4dd0].toughness & 0xffffbfff;
    }
    else
    {
      result = (unsigned int)global_cards_data[unk_008b4dd0].toughness;
    }
    result += instance->counter_toughness;
    if ((instance->regen_status & 0x02000000) != 0)
    {
      instance->regen_status &= 0xfdffffff;
      event_result = result;
    }
    else
    {
      event_result = (unsigned int)instance->toughness;
    }
    break;

  case EVENT_ABILITIES:
    base_abilities = global_cards_data[unk_008b4dd0].static_ability;
    hidden_abilities = instance->regen_status & 0x07000000;
    result = base_abilities | hidden_abilities;
    if ((base_abilities & 0x1ff81f) != 0)
    {
      current_abilities = 0;
      for (current_color = 0; current_color < 5; ++current_color)
      {
        if ((result & (1 << current_color)) != 0)
        {
          current_abilities |= 1 << ((char)get_hacked_color(player, card, current_color + 1) - 1);
        }
        if ((result & (0x800 << current_color)) != 0)
        {
          current_abilities |= 0x800 << ((char)get_sleighted_color(player, card, current_color + 1) - 1);
        }
      }
      result = (base_abilities & 0xffe007e0) | hidden_abilities | current_abilities;
    }
    if ((instance->regen_status & 0x08000000) != 0)
    {
      instance->regen_status &= 0xf7ffffff;
      event_result = result;
    }
    else
    {
      event_result = instance->regen_status;
    }
    break;

  case EVENT_RECALC_DAMAGE:
    event_result = (unsigned int)(short)instance->damage_on_card;
    break;

  case EVENT_SET_COLOR:
    event_result = (unsigned int)(char)global_cards_data[unk_008b4dd0].color;
    break;

  case EVENT_CHANGE_TYPE:
    if (((instance->internal_card_id < unk_009266a4) || (unk_009266a4 + 0x2d <= instance->internal_card_id)) && instance->internal_card_id != -1)
    {
      result = instance->original_internal_card_id;
      if ((instance->regen_status & 0x01000000) != 0)
      {
        instance->internal_card_id = instance->original_internal_card_id;
        instance->regen_status &= 0xfeffffff;
        instance->mana_color = global_cards_data[instance->original_internal_card_id].color;
        instance->destroys_if_blocked = 0;
        event_result = result;
      }
      else
      {
        event_result = instance->internal_card_id;
      }
    }
    else
    {
      event_result = instance->internal_card_id;
    }
    break;

  default:
    event_result = 0;
    break;
  }

  result = event_result;
  if (is_in_play(player, card) && event == EVENT_TOUGHNESS && (global_cards_data[unk_008b4dd0].type & TYPE_CREATURE) != 0 && ((int)result < 1 || (int)result <= (int)(short)instance->damage_on_card) && ((*((char *)instance + 0x1b) & 4) == 0) && trigger_condition == -1 && (unk_008b4278 & 0x204) == 0)
  {
    kill_card(player, card, KILL_DESTROY);
    regenerate_or_graveyard_triggers();
  }

  if (event == EVENT_POWER)
  {
    instance->power = (short)result;
  }
  if (event == EVENT_TOUGHNESS)
  {
    instance->toughness = (short)result;
  }
  if (event == EVENT_ABILITIES)
  {
    instance->regen_status = result;
  }
  if (event != EVENT_CHANGE_TYPE)
  {
    unk_00712938 = saved_event_state;
    return result;
  }

  instance->internal_card_id = result;
  if ((global_cards_data[result].extra_ability & 0x1000) != 0)
  {
    raw_card_data = *(int *)&global_cards_data[result].id;
    if (raw_card_data < 0x12d)
    {
      if (raw_card_data == 0x12c)
      {
        instance->mana_color = 1;
      }
      else if (raw_card_data == 0xf)
      {
        instance->mana_color = 0x3e;
      }
      else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
      {
        token_color = global_cards_data[result].color;
        instance->mana_color = (unsigned char)token_color;
      }
      else
      {
        instance->mana_color = global_cards_data[result].color;
      }
    }
    else if (raw_card_data < 0x13a)
    {
      if (raw_card_data == 0x139)
      {
        instance->mana_color = 2;
      }
      else if (raw_card_data == 0x134)
      {
        instance->mana_color = 0x10;
      }
      else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
      {
        token_color = global_cards_data[result].color;
        instance->mana_color = (unsigned char)token_color;
      }
      else
      {
        instance->mana_color = global_cards_data[result].color;
      }
    }
    else if (raw_card_data < 0x194)
    {
      if (raw_card_data == 0x193)
      {
        instance->mana_color = 0x3e;
      }
      else if (raw_card_data != 0x13e)
      {
        if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
        {
          token_color = global_cards_data[result].color;
          instance->mana_color = (unsigned char)token_color;
        }
        else
        {
          instance->mana_color = global_cards_data[result].color;
        }
      }
    }
    else if (raw_card_data == 0x272)
    {
      instance->mana_color = 1;
    }
    else if (raw_card_data == 0x27b)
    {
      instance->mana_color = 0x10;
    }
    else if (raw_card_data == 0x366)
    {
      instance->mana_color = (unsigned char)instance->info_slot;
    }
    else if (global_cards_data[result].subtype == 0x0c && (instance->token_status & 2) != 0)
    {
      token_color = global_cards_data[result].color;
      instance->mana_color = (unsigned char)token_color;
    }
    else
    {
      instance->mana_color = global_cards_data[result].color;
    }
  }

  if ((instance->token_status & 0x40) != 0 && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)
  {
    for (test_player = 0; test_player < 2; ++test_player)
    {
      test_instance = global_card_instances[test_player];
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
      {
        if (test_instance->internal_card_id != -1 && (test_instance->state & 0x800002) == 2 && test_instance->damage_target_player == player && test_instance->damage_target_card == card && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && *(int *)&global_raw_cards_storage[*(int *)&global_cards_data[test_instance->internal_card_id].id].subtype == 0x2c)
        {
          kill_card(test_player, test_card, KILL_SACRIFICE);
        }
        ++test_instance;
      }
    }
  }

  if ((instance->state & 2) != 0)
  {
    unk_007abc78 |= global_cards_data[instance->internal_card_id].extra_ability & 0x1ffc0000;
  }

  unk_00712938 = saved_event_state;
  return result;
}

// FUNCTION: MAGIC 0x00500a13
int damage_player(int target_player, int amount, int source_player, int source_card)
{
  (void)target_player;
  (void)amount;
  (void)source_player;
  (void)source_card;
  return 0;
}

// FUNCTION: MOK 0x004302c0
// FUNCTION: MAGIC 0x00435e27
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount)
{
  int color_index;
  int result;

  color_index = single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color);
  unk_008ce510[color_index] += unk_0072c440[color_index];
  result = charge_mana(player, color, amount) - unk_0072c440[color_index];
  if (spell_fizzled == 1)
  {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0044331f
void play_sound_effect(wav_t sound_id)
{
  static const char *sound_filenames[] = {
      "artifact.wav", "buried.wav", "draw.wav", "enchant.wav", "endphase.wav",
      "endturn.wav", "instant.wav", "interupt.wav", "grey.wav", "black.wav",
      "blue.wav", "green.wav", "red.wav", "white.wav", "lifeloss.wav",
      "sacrfice.wav", "sorcery.wav", "summon.wav", "tap.wav", "untap.wav",
      "attack2.wav", "block2.wav", "damage.wav", "destroy.wav", "discard.wav",
      "kill.wav", "regen.wav", "blackred.wav", "greenblack.wav", "whitered.wav",
      "whitegreen.wav", "blackwhite.wav", "greenred.wav", "greenblue.wav", "whiteblue.wav",
      "blueblack.wav", "redblue.wav", "counter.wav", "fastfx.wav", "changec.wav",
      "changet.wav", "control.wav", "manaburn.wav", "shuffle.wav", "shell_loseduel.wav",
      "shell_winduel.wav", "aswanjag.wav", "callgrav.wav", "faerdrag.wav", "gembazar.wav",
      "necrazar.wav", "polkamix.wav", "pandora.wav", "prsmdrag.wav", "pwrstrgl.wav",
      "catatap.wav", "orcart.wav", "whimsy.wav", "rainbowk.wav", "toss.wav",
      "shell_shandalar.wav", "shell_tooltime.wav", "shell_helpme.wav", "shell_hallofrecords.wav", "shell_duelmenow.wav",
      "exp1_openfoil.wav", "exp1_openbox.wav", "exp1_outofpack.wav", "exp1_backinpack.wav"};
  struct
  {
    char path[264];
    int replacement_result;
    int sound_num;
    Sound sound;
  } s;
  int found_sound;

  s.sound_num = sound_id;
  s.sound.volume = 300;
  s.sound.sampleRate = 0;
  s.sound.pan = 0;
  s.sound.field_C = 0;
  s.sound.field_10 = 0;
  s.sound.field_14 = 0;
  s.sound.loadId = sound_id;
  s.sound.flags = 0;

  if (unk_008a9000 == 1)
  {
    return;
  }

  if (sound_id < 0x14)
  {
    sound_play(sound_id, 0);
    return;
  }

  if (sound_id < 0x27)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x14, 0x16);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x2e)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x27, 0x27);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x3c)
  {
    s.sound.volume = 400;
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (sound_id == WAV_CATATAP)
    {
      s.sound.field_14 = -1;
    }
    else
    {
      s.sound.flags |= 4;
    }

    if (found_sound == 0)
    {
      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, &s.sound);
    return;
  }

  if (sound_id < 0x41)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x3c, 0x40);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
    return;
  }

  if (sound_id < 0x45)
  {
    found_sound = sound_is_loaded(sound_id, &s.sound_num);
    if (found_sound == 0)
    {
      s.replacement_result = sound_get_lru(&s.sound_num, 0x41, 0x44);
      if (s.replacement_result == 0)
      {
        sound_unload(s.sound_num);
      }
      else if (s.replacement_result != 1)
      {
        return;
      }

      strcpy(s.path, global_base_directory);
      strcat(s.path, "\\");
      strcat(s.path, "DuelSounds\\");
      strcat(s.path, sound_filenames[sound_id]);
      sound_load(s.path, s.sound_num, &s.sound);
    }

    sound_play(s.sound_num, 0);
  }
}

// FUNCTION: MAGIC 0x004faee2
void FUN_004faee2(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  for (current_color = 1; current_color < 6; ++current_color)
  {
    if ((unsigned char)instance->hack_mode[current_color] == (unsigned char)color_from)
    {
      instance->hack_mode[current_color] = (char)color_to;
    }
  }
  if (instance->hack_mode[color_from] == 0)
  {
    instance->hack_mode[color_from] = (char)color_to;
  }
}

// FUNCTION: MAGIC 0x004fb8fd
void FUN_004fb8fd(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  for (current_color = 1; current_color < 6; ++current_color)
  {
    if (instance->color_id[current_color] == (unsigned char)color_from)
    {
      instance->color_id[current_color] = color_to;
    }
  }
  if (instance->color_id[color_from] == 0)
  {
    instance->color_id[color_from] = color_to;
  }
}

// FUNCTION: MAGIC 0x00437375
int FUN_00437375(int player, int card, int internal_card_id)
{
  card_instance_t *aura_being_cast;
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  aura_being_cast = &PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack);
  if (instance->damage_target_player == aura_being_cast->damage_target_player && instance->damage_target_card == aura_being_cast->damage_target_card && (global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0 && (card != card_on_stack || player != card_on_stack_controller))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043c7ab
int FUN_0043c7ab(int who_is_being_divided, int player, int card)
{
  int bank;
  int current_card;
  int legacy_card;
  int power_total[3];

  power_total[0] = 0;
  power_total[1] = 0;
  bank = internal_rand(2);
  for (current_card = 0; current_card < active_cards_count[who_is_being_divided]; ++current_card)
  {
    if (is_in_play(who_is_being_divided, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).internal_card_id].type & TYPE_CREATURE) != 0 && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0x24) & 0x20) == 0))
    {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, who_is_being_divided, current_card);
      if (legacy_card != -1)
      {
        if (power_total[1 - bank] < power_total[bank])
        {
          bank ^= 1;
        }
        power_total[bank] += *(short *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0xa);
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = bank == 0 ? 1 : 2;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0044125c
int FUN_0044125c(int player, int card)
{
  card_instance_t *instance;
  int result;
  int saved_affected_card;
  int saved_affected_card_controller;
  int saved_event_result;
  int saved_spell_fizzled;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (((global_cards_data[instance->internal_card_id].subtype == 0) && ((*(unsigned char *)((char *)instance + 0x19) & 8) == 0)) || (((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0) && ((*(unsigned char *)((char *)instance + 0xb) & 1) == 0)) || ((instance->state & 0x810010) != 0) || ((*(unsigned char *)((char *)instance + 0x19) & 0x80) != 0))
  {
    return 0;
  }

  saved_affected_card = affected_card;
  saved_affected_card_controller = affected_card_controller;
  saved_spell_fizzled = spell_fizzled;
  saved_event_result = event_result;
  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  global_cards_data[instance->internal_card_id].code_pointer(player, card, 0x79);
  result = event_result == 0;
  event_result = saved_event_result;
  spell_fizzled = saved_spell_fizzled;
  affected_card_controller = saved_affected_card_controller;
  affected_card = saved_affected_card;
  if (!result)
  {
    return 0;
  }

  return dispatch_event(player, card, EVENT_ATTACK_LEGALITY) == 0;
}

// FUNCTION: MAGIC 0x004f7783
int FUN_004f7783(int player, int card)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((instance->state & STATE_TAPPED) == 0)
  {
    instance->state |= STATE_TAPPED;
    if ((global_cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0)
    {
      produced_mana_color = -1;
    }
    dispatch_event(player, card, EVENT_TAP_CARD);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005001c4
void FUN_005001c4(int internal_card_id)
{
  *(int *)&global_cards_data[internal_card_id].id = -1;
}

// FUNCTION: MAGIC 0x0052d7a5
int FUN_0052d7a5(int player, int card, int event, unsigned int trigger_flag)
{
  card_instance_t *instance;
  keyword_t illegal_abilities;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
  }

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    spell_fizzled = !FUN_00551638(player, player, card);
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    illegal_abilities = get_protections_from(player, card);
    if (!C_real_validate_target(instance->targets[0].player,
                                instance->targets[0].card,
                                (char *)0,
                                player,
                                2,
                                2,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                illegal_abilities,
                                COLOR_TEST_0,
                                COLOR_TEST_0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }
  if (instance->damage_target_card == card_on_stack && instance->damage_target_player == card_on_stack_controller && card_on_stack != -1 && (instance->state & 0x20) == 0 && event == 0x34)
  {
    event_result |= trigger_flag;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00483190
int FUN_00483190(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int current_data;
  int source_data;

  source_data = *(int *)&global_cards_data[internal_card_id].id;
  current_data = *(int *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
  if (current_data == source_data && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x17) & 1) != 0))
  {
    event_result = (source_player << 16) | source_card;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00483242
int FUN_00483242(int player, int card, int source_player, int source_card, int internal_card_id)
{
  int current_data;
  int source_data;

  source_data = *(int *)&global_cards_data[internal_card_id].id;
  current_data = *(int *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
  if (current_data == source_data && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x17) & 1) == 0))
  {
    event_result = (source_player << 16) | source_card;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b413c
int FUN_004b413c(int player,
                 int *internal_card_ids,
                 int *damage_amounts,
                 int count,
                 int title,
                 int allow_cancel,
                 char *prompt)
{
  (void)player;
  (void)internal_card_ids;
  (void)damage_amounts;
  (void)title;
  (void)allow_cancel;
  (void)prompt;

  if (count <= 0)
  {
    return -1;
  }

  return 0;
}

int tap_card(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

int validate_target(int player, int card, target_definition_t *td, int target_number)
{
  (void)player;
  (void)card;
  (void)td;
  (void)target_number;
  return 0;
}

void vigilance(int player, int card, event_t event)
{
  (void)player;
  (void)card;
  (void)event;
}

// FUNCTION: MAGIC 0x00542a2a
void FUN_00542a2a(int player, int card)
{
  struct
  {
    int found_linked_card;
    int test_player;
    int test_card;
  } s;
  card_instance_t *instance;
  card_instance_t *test_instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  s.test_player = 0;
  s.found_linked_card = 0;
  while (s.test_player < 2 && !s.found_linked_card)
  {
    s.test_card = 0;
    while (s.test_card < active_cards_count[s.test_player] && !s.found_linked_card)
    {
      test_instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (is_in_play(s.test_player, s.test_card) &&
          test_instance->damage_target_player == player &&
          test_instance->damage_target_card == card &&
          ((test_instance->internal_card_id == unk_008b49c4 && (((unsigned char *)test_instance)[0x1a] & 0x80) != 0) ||
           test_instance->internal_card_id == unk_008b3d10))
      {
        s.found_linked_card = 1;
      }
      else
      {
        ++s.test_card;
      }
    }
    if (!s.found_linked_card)
    {
      ++s.test_player;
    }
  }

  if (!s.found_linked_card)
  {
    ((unsigned char *)instance)[0x68] = 0;
    *(int *)((char *)instance + 0x14) = 0;
    *(short *)((char *)instance + 0x10) = 0;
    FUN_004f7783(player, card);
    if (unk_008a9000 != 1)
    {
      play_sound_effect(0x1a);
    }
    *(unsigned int *)((char *)instance + 0x18) &= 0xffffff7f;
    ((unsigned char *)instance)[0x24] = 0xff;
    *(unsigned int *)((char *)instance + 8) &= 0xfffffff3;
  }
}

// FUNCTION: MAGIC 0x0054ac4d
int FUN_0054ac4d(int player, int card)
{
  struct
  {
    char (*prompt)[300];
    int target_card;
    target_t target;
    int unused;
  } s;

  s.unused = 0;
  if (unk_008b35ec == player || (unk_00926804 & 2) != 0)
  {
    if (unk_008a9000 == 1)
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
    }
    else
    {
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0,
                               text_lines[0],
                               1,
                               &s.target) == 0)
      {
        spell_fizzled = 1;
        s.target_card = -1;
        unk_00742fcc = -1;
      }
      else
      {
        s.target_card = s.target.card;
        unk_00742fcc = s.target.player;
      }
    }
  }
  else
  {
    if (unk_008a9000 == 1)
    {
      unk_00939340 = internal_rand(3) == 0;
      FUN_004e4f11();
    }
    else
    {
      FUN_004e5089();
    }

    if (unk_00939340 == 0)
    {
      s.prompt = text_lines;

      C_real_select_target(player,
                           2,
                           1 - player,
                           TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                           TYPE_CREATURE,
                           TYPE_NONE,
                           0,
                           get_protections_from(player, card),
                           COLOR_TEST_0,
                           COLOR_TEST_0,
                           -1,
                           ~SUB_WALL,
                           -1,
                           -1,
                           0,
                           0,
                           0,
                           *s.prompt,
                           1,
                           &s.target);
      s.target_card = s.target.card;
      unk_00742fcc = s.target.player;
    }
    else
    {
      s.target_card = -1;
      unk_00742fcc = 1 - player;
      if (unk_008a9000 == 1)
      {
        unk_00939340 = 0;
        unk_00925bb8 = ((unk_00742fcc == 0) ? 0 : 0x01000000) | 0xff;
        FUN_004e4f11();
      }
      else
      {
        FUN_004e5089();
      }
    }
  }

  if (spell_fizzled != 1)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = s.target_card;
    PLAYER_CARD_INSTANCE(player, card).damage_target_player = (char)unk_00742fcc;
    PLAYER_CARD_INSTANCE(player, card).upkeep_blue = 1;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054af10
int FUN_0054af10(int player, int card, event_t event, int amount)
{
  struct
  {
    int source_player;
    int source_card;
  } s;
  card_instance_t *instance;
  int target_player;
  int target_card;
  keyword_t illegal_abilities;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (instance->number_of_targets == 0)
  {
    return 0;
  }

  if (event == 0x72)
  {
    s.source_player = card_on_stack_controller;
    s.source_card = card_on_stack;
  }
  else
  {
    s.source_player = player;
    s.source_card = card;
  }

  target_player = instance->targets[0].player;
  target_card = instance->targets[0].card;
  if (target_player == -1 && target_card == -1)
  {
    return 0;
  }

  illegal_abilities = get_protections_from(player, card);
  if (!C_real_validate_target(target_player,
                              target_card,
                              NULL,
                              player,
                              2,
                              2,
                              TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                              TYPE_CREATURE,
                              TYPE_NONE,
                              0,
                              illegal_abilities,
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0))
  {
    spell_fizzled = 1;
    return 0;
  }

  if (target_card == -1)
  {
    damage_player(target_player, amount, s.source_player, s.source_card);
  }
  else
  {
    damage_creature(target_player, target_card, amount, s.source_player, s.source_card);
  }

  return 1;
}

// FUNCTION: MAGIC 0x0054e470
void FUN_0054e470(int player, int card, int color)
{
  int test_player;
  int test_card;
  int sleighted_color;
  card_instance_t *instance;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
    {
      if (is_in_play(test_player, test_card))
      {
        instance = &PLAYER_CARD_INSTANCE(test_player, test_card);
        if (instance->damage_target_player == player && instance->damage_target_card == card)
        {
          sleighted_color = get_sleighted_color(player, card, color);
          if (((1 << ((unsigned char)sleighted_color & 0x1f)) & (char)instance->toughness) != 0 && (global_cards_data[instance->internal_card_id].type & TYPE_ARTIFACT) != 0)
          {
            kill_card(test_player, test_card, 1);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0055d802
void FUN_0055d802(char *out, char *in, int choice)
{
  struct
  {
    char *write_ptr;
    int done;
    int matched;
    char *scan;
  } s;

  if (out == NULL || in == NULL)
  {
    return;
  }

  s.done = 0;
  s.matched = 0;
  s.scan = in;
  while (s.done == 0)
  {
    while (*s.scan != '\0' && strncmp(s.scan, unk_0057f758, 2) != 0)
    {
      ++s.scan;
    }

    if (*s.scan == '\0')
    {
      s.done = 1;
    }
    else
    {
      ++s.scan;
      ++s.scan;
      if (atoi(s.scan) == choice)
      {
        s.matched = 1;
        ++s.scan;
        s.write_ptr = out;
        while (*s.scan != '\0' && strncmp(s.scan, unk_0057f75c, 2) != 0)
        {
          *s.write_ptr = *s.scan;
          ++s.scan;
          ++s.write_ptr;
        }
        *s.write_ptr = '\0';
        s.done = 1;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00495311
char *FUN_00495311(int value)
{
  if (value < 0 || value >= 0xd4)
  {
    return "";
  }
  else
    return unk_00926930[value];
}

// FUNCTION: MAGIC 0x004a61fe
int FUN_004a61fe(int *cards, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (unk_008a9000 == 1)
  {
    return 1;
  }

  return show_cardlist(cards, 0, 0, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x004a59ad
int CardIDFromType(unsigned int type)
{
  if (type == -1)
  {
    return -1;
  }
  else
  {
    type &= 0xfff;
    return *(int *)&global_cards_data[type].id;
  }
}

// FUNCTION: MAGIC 0x004a5929
int CardTypeFromID(int csvid)
{
  int result;
  int internal_card_id;

  if (csvid == -1)
  {
    return -1;
  }
  else
  {
    result = -1;
    for (internal_card_id = 0; global_cards_data[internal_card_id].id != -1 && result == -1; ++internal_card_id)
    {
      if (global_cards_data[internal_card_id].id == csvid)
      {
        result = internal_card_id;
      }
    }
    return result;
  }
}

// FUNCTION: MAGIC 0x004a61d6
void FUN_004a61d6(char *text)
{
  (void)text;
}

// FUNCTION: MAGIC 0x00482a97
int FUN_00482a97(int player, int card, unsigned int flags)
{
  int source_card;
  int source_internal_card_id;
  int found;
  int test_card;
  int source_player;
  int test_player;

  if (player == -1 || flags == 0)
  {
    found = 0;
  }
  else
  {
    test_player = 0;
    found = 0;

    while (test_player < 2 && found == 0)
    {
      test_card = 0;
      while (test_card < active_cards_count[test_player] && found == 0)
      {
        if (global_card_instances[test_player][test_card].internal_card_id == unk_009266a4 && is_in_play(test_player, test_card) && global_card_instances[test_player][test_card].damage_target_player == player && global_card_instances[test_player][test_card].damage_target_card == card && global_card_instances[test_player][test_card].info_slot != 0)
        {
          source_player = (int)global_card_instances[test_player][test_card].damage_source_player;
          source_card = global_card_instances[test_player][test_card].damage_source_card;

          if (global_card_instances[source_player][source_card].internal_card_id != -1 && *(int *)&global_cards_data[global_card_instances[source_player][source_card]
                                                                                                                         .internal_card_id]
                                                                                                  .id == unk_008a8de8)
          {
            source_player =
                global_card_instances[(char)global_card_instances[source_player][source_card]
                                          .damage_source_player][source_card]
                    .damage_source_card;
          }

          if (global_card_instances[source_player][source_card].internal_card_id == -1)
          {
            source_internal_card_id =
                global_card_instances[source_player][source_card].original_internal_card_id;
          }
          else
          {
            source_internal_card_id = global_card_instances[source_player][source_card].internal_card_id;
          }

          if ((flags & global_cards_data[source_internal_card_id].type) != 0)
          {
            found = 1;
          }
        }
        ++test_card;
      }
      ++test_player;
    }
  }

  return found;
}

// FUNCTION: MAGIC 0x00551b60
int FUN_00551b60(int player, unsigned int preferred_controller, int card)
{
  int result;
  unsigned char target_number;
  target_t selected_target;

  if (preferred_controller == -1)
  {
    preferred_controller = 2;
  }

  result = C_real_select_target(player,
                                2,
                                preferred_controller,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
                                TYPE_NONE,
                                0,
                                get_protections_from(player, card),
                                0,
                                0,
                                -1,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0,
                                text_lines[0],
                                1,
                                &selected_target);
  if (result != 0)
  {
    target_number = global_card_instances[player][card].number_of_targets;
    global_card_instances[player][card].targets[target_number] = selected_target;
    ++global_card_instances[player][card].number_of_targets;
  }

  return result != 0;
}

// FUNCTION: MAGIC 0x004bd7d0
/* target_source_mode:
 *   0 = scan direct player/card targets
 *   1 = scan damage cards by damage_target_player/card
 *   2 = scan damage cards by damage_source_player/card
 */
int real_target_available(int *num_valid_targets,
                          target_scan_mode_t target_source_mode,
                          int who_chooses,
                          unsigned int allowed_controller,
                          unsigned int preferred_controller,
                          target_zone_t zone,
                          type_t required_type,
                          type_t illegal_type,
                          keyword_t required_abilities,
                          keyword_t illegal_abilities,
                          color_test_t required_color,
                          color_test_t illegal_color,
                          int extra,
                          subtype_in_card_data_t required_subtype,
                          int required_power,
                          int required_toughness,
                          target_special_t special,
                          target_state_t required_state,
                          target_state_t illegal_state)
{
  struct
  {
    int target_card;
    int player_index;
    int target_is_valid;
    int found_any;
    int stop_on_first;
    int count;
    int done;
    int target_player;
    int current_card;
    int scan_player;
  } s;

  s.stop_on_first = num_valid_targets == NULL;
  s.count = 0;
  s.found_any = 0;

  if (target_source_mode != 0 && target_source_mode != 1 && target_source_mode != 2)
  {
    return 0;
  }

  s.done = 0;
  for (s.player_index = 0; s.player_index < 2; ++s.player_index)
  {
    if (C_real_validate_target(s.player_index,
                               -1,
                               (char *)0,
                               who_chooses,
                               allowed_controller,
                               preferred_controller,
                               zone,
                               required_type,
                               illegal_type,
                               required_abilities,
                               illegal_abilities,
                               required_color,
                               illegal_color,
                               extra,
                               required_subtype,
                               required_power,
                               required_toughness,
                               special,
                               required_state,
                               illegal_state) != 0)
    {
      s.found_any = 1;
      ++s.count;
      if (s.stop_on_first)
      {
        s.done = 1;
      }
    }
  }

  if (unk_008b35ec == who_chooses || (unk_00926804 & 2) != 0)
  {
    if ((allowed_controller & 2) == 0)
    {
      s.scan_player = 1;
    }
    else
    {
      s.scan_player = 0;
    }
  }
  else if ((preferred_controller & 2) != 0 || (preferred_controller & 1) != 0)
  {
    s.scan_player = 1;
  }
  else
  {
    s.scan_player = 0;
  }

  s.player_index = 0;
  while (s.player_index < 2)
  {
    if (!s.done)
    {
      for (s.current_card = 0;
           s.current_card < (active_cards_count[1] > active_cards_count[0] ? active_cards_count[1]
                                                                           : active_cards_count[0]);
           ++s.current_card)
      {
        if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id != -1)
        {
          if (target_source_mode == 0)
          {
            s.target_player = s.scan_player;
            s.target_card = s.current_card;
            s.target_is_valid = 1;
          }
          else if (target_source_mode == 1)
          {
            if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4)
            {
              s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_player;
              s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_card;
              s.target_is_valid = 1;
            }
            else
            {
              s.target_is_valid = 0;
            }
          }
          else if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4)
          {
            s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_player;
            s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_card;
            s.target_is_valid = 1;
          }
          else
          {
            s.target_is_valid = 0;
          }

          if (s.target_is_valid && C_real_validate_target(s.target_player,
                                                          s.target_card,
                                                          (char *)0,
                                                          who_chooses,
                                                          allowed_controller,
                                                          preferred_controller,
                                                          zone,
                                                          required_type,
                                                          illegal_type,
                                                          required_abilities,
                                                          illegal_abilities,
                                                          required_color,
                                                          illegal_color,
                                                          extra,
                                                          required_subtype,
                                                          required_power,
                                                          required_toughness,
                                                          special,
                                                          required_state,
                                                          illegal_state) != 0)
          {
            s.found_any = 1;
            ++s.count;
            if (s.stop_on_first)
            {
              s.done = 1;
            }
          }
        }
      }
    }

    ++s.player_index;
    s.scan_player = 1 - s.scan_player;
  }

  if (num_valid_targets != NULL)
  {
    *num_valid_targets = s.count;
  }
  return s.found_any;
}

// FUNCTION: MAGIC 0x004bdc06
unsigned int C_real_validate_target(int tgt_player,
                                    int tgt_card,
                                    char *return_error_str,
                                    int who_chooses,
                                    int allowed_controller,
                                    int preferred_controller,
                                    target_zone_t zone,
                                    type_t required_type,
                                    type_t illegal_type,
                                    keyword_t required_abilities,
                                    keyword_t illegal_abilities,
                                    color_test_t required_color,
                                    color_test_t illegal_color,
                                    int extra,
                                    subtype_in_card_data_t required_subtype,
                                    int power_requirement,
                                    int toughness_requirement,
                                    target_special_t special,
                                    target_state_t required_state,
                                    target_state_t illegal_state)
{
  static const char empty_error[] = "";
  static const char error_player[] = ",player";
  static const char error_target[] = ",target";
  static const char error_zone[] = ",zone";
  static const char error_controller[] = ",controller";
  static const char error_owner[] = ",owner";
  static const char error_type[] = ",type";
  static const char error_ability[] = ",ability";
  static const char error_color[] = ",color";
  static const char error_extra[] = ",extra";
  static const char error_subtype[] = ",subtype";
  static const char error_power[] = ",power";
  static const char error_toughness[] = ",toughness";
  static const char error_wall[] = ",wall";
  static const char error_stack[] = ",spell on stack";
  static const char error_basic_land[] = ",basic land";
  static const char error_artifact_creature[] = ",artifact creature";
  static const char error_damage_player[] = ",damage player";
  static const char error_djinn_efreet[] = ",djinn/efreet";
  static const char error_tapped[] = ",tapped";
  static const char error_attacking[] = ",attacking";
  static const char error_attacked[] = ",attacked";
  static const char error_blocked[] = ",blocked";
  static const char error_blocking[] = ",blocking";
  static const char error_in_combat[] = ",combat";
  static const char error_enchanted[] = ",enchanted";
  static const char error_just_cast[] = ",just cast";
  static const char error_spell_resolved[] = ",spell resolved";
  static const char error_damaged[] = ",damaged";
  static const char error_could_untap[] = ",could untap";
  static const char error_will_untap[] = ",will untap";
  static const char error_summoning_sickness[] = ",summoning sickness";
  card_instance_t *instance;
  card_instance_t *test_instance;
  int bVar19;
  int bVar20;
  int local_124;
  int local_120;
  int local_118;
  int local_114;
  subtype_in_card_data_t local_f8;
  int local_f0;
  int local_e8;
  int local_e4;
  int local_e0;
  int local_dc;
  int local_d8;
  unsigned int uVar17;
  unsigned int uVar18;
  char local_d0[200];
  unsigned int local_8;

  if (tgt_player == -1)
  {
    if (return_error_str != NULL)
    {
      strcpy(return_error_str, empty_error);
    }
    local_8 = 0;
  }
  else if (tgt_card == -1 || global_card_instances[tgt_player][tgt_card].internal_card_id != -1)
  {
    bVar20 = 0;
    strcpy(local_d0, empty_error);

    if (tgt_card == -1)
    {
      if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
      {
        if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0)) && ((allowed_controller & ANYBODY) != 0)) || (who_chooses == AI && ((preferred_controller & ANYBODY) != 0)))
        {
          local_d8 = AI;
          local_dc = 1;
        }
        else if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0)) && ((allowed_controller & HUMAN) != 0)) || (who_chooses == AI && ((preferred_controller & HUMAN) != 0)))
        {
          local_d8 = HUMAN;
          local_dc = 1;
        }
      }
      else
      {
        local_d8 = HUMAN;
        local_dc = 0;
      }

      if ((tgt_player == HUMAN && local_d8 == 0) || (tgt_player == AI && local_dc == 0))
      {
        bVar20 = 1;
        strcat(local_d0, error_player);
      }
    }
    else
    {
      instance = &global_card_instances[tgt_player][tgt_card];

      if ((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0))
      {
        if ((allowed_controller & TARGET_PLAYER_OWNER) == 0)
        {
          if ((allowed_controller & ANYBODY) == 0)
          {
            if ((allowed_controller & HUMAN) == 0)
            {
              local_e4 = HUMAN;
            }
            else
            {
              local_e4 = AI;
            }
          }
          else
          {
            local_e4 = -1;
          }

          if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
          {
            local_e0 = local_e4;
          }
          else
          {
            local_e0 = -1;
          }
        }
        else
        {
          if ((allowed_controller & ANYBODY) == 0)
          {
            if ((allowed_controller & HUMAN) == 0)
            {
              local_e0 = HUMAN;
            }
            else
            {
              local_e0 = AI;
            }
          }
          else
          {
            local_e0 = -1;
          }

          if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
          {
            local_e4 = local_e0;
          }
          else
          {
            local_e4 = -1;
          }
        }
      }
      else if ((preferred_controller & TARGET_PLAYER_OWNER) == 0)
      {
        if ((preferred_controller & ANYBODY) == 0)
        {
          if ((preferred_controller & HUMAN) == 0)
          {
            local_e4 = HUMAN;
          }
          else
          {
            local_e4 = AI;
          }
        }
        else
        {
          local_e4 = -1;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          local_e0 = local_e4;
        }
        else
        {
          local_e0 = -1;
        }
      }
      else
      {
        if ((preferred_controller & ANYBODY) == 0)
        {
          if ((preferred_controller & HUMAN) == 0)
          {
            local_e0 = HUMAN;
          }
          else
          {
            local_e0 = AI;
          }
        }
        else
        {
          local_e0 = -1;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER)
        {
          local_e4 = local_e0;
        }
        else
        {
          local_e4 = -1;
        }
      }

      bVar19 = (instance->state & STATE_OUBLIETTED) != 0;
      if (bVar19)
      {
        strcat(local_d0, error_target);
      }

      if ((instance->state & STATE_CANNOT_TARGET) != 0)
      {
        bVar20 = 1;
        strcat(local_d0, error_target);
      }

      bVar20 = bVar20 || bVar19;
      if (zone != 0 && (((instance->state & STATE_IN_PLAY) != 0 && (zone & TARGET_ZONE_IN_PLAY) == 0) || ((instance->state & STATE_IN_PLAY) == 0 && (zone & TARGET_ZONE_HAND) == 0)))
      {
        bVar20 = 1;
        strcat(local_d0, error_zone);
      }

      if (local_e4 != -1 && tgt_player != local_e4)
      {
        bVar20 = 1;
        strcat(local_d0, error_controller);
      }

      if (local_e0 != -1)
      {
        local_114 = (instance->state & STATE_OWNED_BY_OPPONENT) != 0 ? 1 - tgt_player : tgt_player;
        if (local_114 != local_e0)
        {
          bVar20 = 1;
          strcat(local_d0, error_owner);
        }
      }

      if (required_type != TYPE_NONE)
      {
        if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0)
        {
          local_e8 = instance->internal_card_id;
        }
        else
        {
          local_e8 = instance->original_internal_card_id;
        }

        bVar19 = 0;
        if (((required_type & global_cards_data[local_e8].type) != TYPE_NONE) || ((required_type & TARGET_TYPE_NONCREATURE_CAN_BLOCK) != TYPE_NONE && (instance->state & STATE_NONCREATURE_CAN_BLOCK) != 0))
        {
          bVar19 = 1;
        }
        if ((required_type & TYPE_EFFECT) != TYPE_NONE && (*(int *)&global_cards_data[local_e8].id == unk_00789734 || *(int *)&global_cards_data[instance->internal_card_id].id == unk_00789b80))
        {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (instance->token_status & STATUS_TOKEN) != 0)
        {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_e8].id == unk_007a7d64)
        {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_e8].id == unk_008a8de8)
        {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_e8].id == unk_009266ac)
        {
          bVar19 = 1;
        }
        if (!bVar19)
        {
          bVar20 = 1;
          strcat(local_d0, error_type);
        }
      }

      if (illegal_type != TYPE_NONE)
      {
        if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0)
        {
          local_f0 = instance->internal_card_id;
        }
        else
        {
          local_f0 = instance->original_internal_card_id;
        }

        bVar19 = (illegal_type & global_cards_data[local_f0].type) != TYPE_NONE;
        if ((illegal_type & TYPE_EFFECT) != TYPE_NONE && (*(int *)&global_cards_data[local_f0].id == unk_00789734 || *(int *)&global_cards_data[local_f0].id == unk_00789b80))
        {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (instance->token_status & STATUS_TOKEN) != 0)
        {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_f0].id == unk_007a7d64)
        {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_f0].id == unk_008a8de8)
        {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE && *(int *)&global_cards_data[local_f0].id == unk_009266ac)
        {
          bVar19 = 1;
        }
        if (bVar19)
        {
          bVar20 = 1;
          strcat(local_d0, error_type);
        }
      }

      if (required_abilities != 0 && (instance->regen_status & required_abilities) != required_abilities)
      {
        bVar20 = 1;
        strcat(local_d0, error_ability);
      }
      if (illegal_abilities != 0 && (instance->regen_status & illegal_abilities) != 0)
      {
        bVar20 = 1;
        strcat(local_d0, error_ability);
      }
      if (required_color != COLOR_TEST_0 && (required_color & instance->color) == COLOR_TEST_0)
      {
        bVar20 = 1;
        strcat(local_d0, error_color);
      }
      if (illegal_color != COLOR_TEST_0 && (illegal_color & instance->color) != COLOR_TEST_0)
      {
        bVar20 = 1;
        strcat(local_d0, error_color);
      }

      if (extra != -1)
      {
        if ((special & TARGET_SPECIAL_NOT_LAND_SUBTYPE) == 0)
        {
          if ((special & TARGET_SPECIAL_BASIC_LAND) == 0)
          {
            if (extra < 5)
            {
              if (FUN_0048463d(tgt_player, tgt_card, extra + 1) == 0)
              {
                bVar20 = 1;
                strcat(local_d0, error_extra);
              }
            }
            else if (instance->internal_card_id != extra && *(int *)&global_cards_data[extra].id != *(int *)&global_cards_data[instance->internal_card_id].id)
            {
              bVar20 = 1;
              strcat(local_d0, error_extra);
            }
          }
          else if (FUN_004c0880(instance->internal_card_id, extra) == 0)
          {
            bVar20 = 1;
            strcat(local_d0, error_extra);
          }
        }
        else if (FUN_00483489(tgt_player, tgt_card, extra) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_extra);
        }
      }

      if (required_subtype != ~SUB_WALL)
      {
        if ((special & TARGET_SPECIAL_0x10) == 0)
        {
          local_f8 = required_subtype;
        }
        else if (required_subtype == (SUB_DJINN | SUB_MERFOLK))
        {
          local_f8 = SUB_RAT;
        }
        else if (required_subtype == SUB_RAT)
        {
          local_f8 = SUB_DJINN | SUB_MERFOLK;
        }
        else if (required_subtype == SUB_ENCHANT_WORLD)
        {
          local_f8 = SUB_ENCHANT_WORLD | SUB_MERFOLK;
        }
        else if (required_subtype == (SUB_ENCHANT_WORLD | SUB_MERFOLK))
        {
          local_f8 = SUB_ENCHANT_WORLD;
        }
        else if (required_subtype == 0x49)
        {
          local_f8 = 0x4a;
        }
        else if (required_subtype == 0x4a)
        {
          local_f8 = 0x49;
        }
        else if (required_subtype == 0x56)
        {
          local_f8 = 0x57;
        }
        else if (required_subtype == 0x57)
        {
          local_f8 = 0x56;
        }
        else if (required_subtype == 0x59)
        {
          local_f8 = 0x5a;
        }
        else if (required_subtype == 0x5a)
        {
          local_f8 = 0x59;
        }
        else if (required_subtype == 0x69)
        {
          local_f8 = 0x6a;
        }
        else if (required_subtype == 0x6a)
        {
          local_f8 = 0x69;
        }
        else
        {
          local_f8 = required_subtype;
        }

        local_114 = global_cards_data[instance->internal_card_id].id;
        if ((subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != required_subtype && (subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != local_f8)
        {
          bVar20 = 1;
          strcat(local_d0, error_subtype);
        }
      }

      if (power_requirement != -1)
      {
        uVar17 = power_requirement & TARGET_PT_MASK;
        uVar18 = power_requirement & 0xf000;
        if ((uVar18 == 0 && instance->power != (int)uVar17) || (uVar18 == TARGET_PT_GREATER_OR_EQUAL && instance->power < (int)uVar17) || (uVar18 == TARGET_PT_LESSER_OR_EQUAL && instance->power > (int)uVar17))
        {
          bVar20 = 1;
          strcat(local_d0, error_power);
        }
      }

      if (toughness_requirement != -1)
      {
        uVar17 = toughness_requirement & TARGET_PT_MASK;
        uVar18 = toughness_requirement & 0xf000;
        if ((uVar18 == 0 && instance->toughness != (int)uVar17) || (uVar18 == TARGET_PT_GREATER_OR_EQUAL && instance->toughness < (int)uVar17) || (uVar18 == TARGET_PT_LESSER_OR_EQUAL && instance->toughness > (int)uVar17))
        {
          bVar20 = 1;
          strcat(local_d0, error_toughness);
        }
      }

      if (special != 0)
      {
        local_114 = global_cards_data[instance->internal_card_id].id;
        if ((special & TARGET_SPECIAL_WALL) != 0 && (subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != 0xc5 && global_cards_data[instance->internal_card_id].subtype != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_wall);
        }
        if ((special & TARGET_SPECIAL_NON_WALL) != 0 && ((subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype == 0xc5 || global_cards_data[instance->internal_card_id].subtype == 0))
        {
          bVar20 = 1;
          strcat(local_d0, error_wall);
        }
        if ((special & TARGET_SPECIAL_SPELL_ON_STACK) != 0 && ((unk_008ce508 == -1 || tgt_player != unk_008ce508) || unk_008ce4f4 != tgt_card || (instance->state & STATE_SUMMONSICK) != 0))
        {
          bVar20 = 1;
          strcat(local_d0, error_stack);
        }
        if ((special & TARGET_SPECIAL_BASIC_LAND) != 0 && FUN_004c081a(tgt_player, tgt_card) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_basic_land);
        }
        if ((special & TARGET_SPECIAL_ARTIFACT_CREATURE) != 0 && (global_cards_data[instance->internal_card_id].type & (TYPE_ARTIFACT | TYPE_CREATURE)) != (TYPE_ARTIFACT | TYPE_CREATURE))
        {
          bVar20 = 1;
          strcat(local_d0, error_artifact_creature);
        }
        if ((special & TARGET_SPECIAL_DAMAGE_PLAYER) != 0 && (instance->damage_source_player != who_chooses || instance->damage_target_card != -1))
        {
          bVar20 = 1;
          strcat(local_d0, error_damage_player);
        }
        if ((special & TARGET_SPECIAL_DJINN_OR_EFREET) != 0 && global_cards_data[instance->internal_card_id].subtype != 6 && global_cards_data[instance->internal_card_id].subtype != 5)
        {
          bVar20 = 1;
          strcat(local_d0, error_djinn_efreet);
        }
      }

      if (required_state != 0)
      {
        if ((required_state & TARGET_STATE_TAPPED) != 0 && (instance->state & STATE_TAPPED) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_tapped);
        }
        if ((required_state & TARGET_STATE_ATTACKING) != 0 && (instance->state & STATE_ATTACKING) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_attacking);
        }
        if ((required_state & TARGET_STATE_ATTACKED) != 0 && (instance->state & STATE_ATTACKED) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_attacked);
        }
        if ((required_state & TARGET_STATE_ISBLOCKED) != 0 && (instance->state & STATE_ISBLOCKED) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_blocked);
        }
        if ((required_state & TARGET_STATE_BLOCKING) != 0 && (tgt_player == human_player || instance->damage_target_player == -1))
        {
          bVar20 = 1;
          strcat(local_d0, error_blocking);
        }
        if ((required_state & TARGET_STATE_IN_COMBAT) != 0)
        {
          bVar19 = 0;
          if (current_phase < PHASE_DECLARE_ATTACKERS || current_phase > 0x1d || tgt_player == human_player)
          {
            bVar19 = 1;
          }
          else if (tgt_player == human_player || instance->damage_target_player == -1)
          {
            bVar19 = 1;
          }
          if ((instance->state & STATE_ATTACKING) == 0 && bVar19)
          {
            bVar20 = 1;
            strcat(local_d0, error_in_combat);
          }
        }
        if ((required_state & TARGET_STATE_ENCHANTED) != 0)
        {
          bVar19 = 0;
          for (local_114 = 0; local_114 < 2; ++local_114)
          {
            for (local_118 = 0; local_118 < active_cards_count[local_114]; ++local_118)
            {
              test_instance = &global_card_instances[local_114][local_118];
              if (test_instance->internal_card_id != -1 && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && test_instance->damage_target_player == tgt_player && test_instance->damage_target_card == tgt_card)
              {
                bVar19 = 1;
              }
            }
          }
          if (!bVar19)
          {
            bVar20 = 1;
            strcat(local_d0, error_enchanted);
          }
        }
        if ((required_state & TARGET_STATE_JUST_CAST) != 0 && (instance->state & STATE_SUMMONSICK) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_just_cast);
        }
        if ((required_state & TARGET_STATE_SPELL_RESOLVED) != 0 && ((instance->state & STATE_SUMMONSICK) == 0 || (instance->state & STATE_INVISIBLE) != 0))
        {
          bVar20 = 1;
          strcat(local_d0, error_spell_resolved);
        }
        if ((required_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_damaged);
        }
        if ((required_state & TARGET_STATE_COULD_UNTAP) != 0 && (instance->untap_status & UNTAP_STATUS_COULD_UNTAP) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_could_untap);
        }
        if ((required_state & TARGET_STATE_WILL_UNTAP) != 0 && (instance->untap_status & UNTAP_STATUS_WILL_UNTAP) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_will_untap);
        }
        if ((required_state & TARGET_STATE_SUMMONING_SICK) != 0 && (instance->state & 0x30000) == 0x30000)
        {
          bVar20 = 1;
          strcat(local_d0, error_summoning_sickness);
        }
      }

      if (illegal_state != 0)
      {
        if ((illegal_state & TARGET_STATE_TAPPED) != 0 && (instance->state & STATE_TAPPED) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_tapped);
        }
        if ((((illegal_state & TARGET_STATE_ATTACKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0)) && (instance->state & STATE_ATTACKING) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_attacking);
        }
        if ((illegal_state & TARGET_STATE_ATTACKED) != 0 && (instance->state & STATE_ATTACKED) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_attacked);
        }
        if ((illegal_state & TARGET_STATE_ISBLOCKED) != 0 && (instance->state & STATE_ISBLOCKED) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_blocked);
        }
        if ((((illegal_state & TARGET_STATE_BLOCKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0)) && instance->damage_target_player != -1 && tgt_player != human_player)
        {
          bVar20 = 1;
          strcat(local_d0, error_blocking);
        }
        if ((illegal_state & TARGET_STATE_ENCHANTED) != 0)
        {
          bVar19 = 0;
          for (local_120 = 0; local_120 < 2; ++local_120)
          {
            for (local_124 = 0; local_124 < active_cards_count[local_120]; ++local_124)
            {
              test_instance = &global_card_instances[local_120][local_124];
              if (test_instance->internal_card_id != -1 && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0 && test_instance->damage_target_player == tgt_player && test_instance->damage_target_card == tgt_card)
              {
                bVar19 = 1;
              }
            }
          }
          if (bVar19)
          {
            bVar20 = 1;
            strcat(local_d0, error_enchanted);
          }
        }
        if ((illegal_state & TARGET_STATE_JUST_CAST) != 0 && (instance->state & STATE_SUMMONSICK) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_just_cast);
        }
        if ((illegal_state & TARGET_STATE_SPELL_RESOLVED) != 0 && (instance->state & STATE_SUMMONSICK) != 0 && (instance->state & STATE_INVISIBLE) == 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_spell_resolved);
        }
        if ((illegal_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_damaged);
        }
        if ((illegal_state & TARGET_STATE_COULD_UNTAP) != 0 && (instance->untap_status & UNTAP_STATUS_COULD_UNTAP) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_could_untap);
        }
        if ((illegal_state & TARGET_STATE_WILL_UNTAP) != 0 && (instance->untap_status & UNTAP_STATUS_WILL_UNTAP) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_will_untap);
        }
        if ((illegal_state & TARGET_STATE_SUMMONING_SICK) != 0 && (instance->state & 0x30000) != 0)
        {
          bVar20 = 1;
          strcat(local_d0, error_summoning_sickness);
        }
      }
    }

    local_8 = !bVar20;
    if (return_error_str != NULL)
    {
      if (local_d0[0] == '\0')
      {
        *return_error_str = '\0';
      }
      else
      {
        strcpy(return_error_str, local_d0 + 1);
      }
    }
  }
  else
  {
    if (return_error_str != NULL)
    {
      strcpy(return_error_str, empty_error);
    }
    local_8 = 0;
  }

  return local_8;
}

// FUNCTION: MAGIC 0x004e51bb
void FUN_004e51bb(void)
{
  if (unk_00743094 < 1)
  {
    unk_00743094 = 0;
  }
  else
  {
    --unk_00743094;
  }
}

// FUNCTION: MAGIC 0x00551240
void add_special_counter(int player, int card)
{
  if (global_card_instances[player][card].special_counters != 0xff)
  {
    ++global_card_instances[player][card].special_counters;
  }
}

// FUNCTION: MAGIC 0x005514cd
void FUN_005514cd(int player, int card, int amount)
{
  global_card_instances[player][card].special_counters =
      (unsigned char)(global_card_instances[player][card].special_counters - amount);
}

// FUNCTION: MAGIC 0x00551572
void FUN_00551572(int player, int card, int amount)
{
  if (amount > 0xff)
  {
    amount = 0xff;
  }

  global_card_instances[player][card].special_counters = (unsigned char)amount;
}

// FUNCTION: MAGIC 0x005515f5
unsigned int C_get_special_counters(int player, int card)
{
  return *(int *)&global_card_instances[player][card].special_counters & 0xff;
}

// FUNCTION: MAGIC 0x00419667
void FUN_00419667(int target_player, int target_card, int damage_target_player)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card)
    {
      if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == unk_009266a4 && (PLAYER_CARD_INSTANCE(current_player, current_card).state & 0x800002) == 2 && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
      {
        PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id = -1;
        damage_player(damage_target_player,
                      PLAYER_CARD_INSTANCE(current_player, current_card).info_slot,
                      *(char *)((char *)&PLAYER_CARD_INSTANCE(current_player, current_card) + 0xc),
                      *(int *)((char *)&PLAYER_CARD_INSTANCE(current_player, current_card) + 0x48));
        PLAYER_CARD_INSTANCE(current_player, current_card).info_slot = 0;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00441514
int has_vigilance(int player, int card)
{
  if (PLAYER_CARD_INSTANCE(player, card).state & STATE_VIGILANCE)
    return 1;

  return 0;
}

// FUNCTION: MAGIC 0x00443ee2
int FUN_00443ee2(int player, int card, int event, int extra, int prompt)
{
  card_instance_t *stack_card_instance;
  card_instance_t *original_instance;
  int stack_card;
  int original_display_pic_num;

  if (unk_008b2934 < 32)
  {
    unk_00939180[unk_008b2934] = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    unk_00939180[unk_008b2934] |= event << 16;
    unk_00939180[unk_008b2934] |= extra << 24;
    if (event == EVENT_RESOLVE_SPELL || event == EVENT_RESOLVE_TRIGGER || PLAYER_CARD_INSTANCE(player, card).internal_card_id < 5)
    {
      stack_card = card;
    }
    else
    {
      stack_card = add_card_to_hand(player, unk_0091a80c);
      if (stack_card == -1)
      {
        return 0;
      }

      stack_card_instance = &PLAYER_CARD_INSTANCE(player, stack_card);
      original_instance = &PLAYER_CARD_INSTANCE(player, card);
      original_display_pic_num = *(int *)((char *)stack_card_instance + 0x54);
      memcpy(stack_card_instance, original_instance, sizeof(card_instance_t));
      stack_card_instance->internal_card_id = unk_0091a80c;
      *(int *)((char *)stack_card_instance + 0x14) = 0;
      stack_card_instance->kill_code = 0;
      if (original_instance->internal_card_id == -1)
      {
        *(int *)((char *)stack_card_instance + 0x3c) = *(int *)((char *)original_instance + 0x3c);
      }
      else
      {
        *(int *)((char *)stack_card_instance + 0x3c) = original_instance->internal_card_id;
      }
      *(int *)((char *)stack_card_instance + 0x64) = *(int *)((char *)original_instance + 0x64);
      stack_card_instance->state |= 2;
      stack_card_instance->parent_controller = player;
      stack_card_instance->parent_card = card;
      *(int *)((char *)stack_card_instance + 0x54) = original_display_pic_num;
    }

    global_stack_cards[unk_008b2934].player = player;
    global_stack_cards[unk_008b2934].card = stack_card;
    global_stack_damage_targets[unk_008b2934].player = PLAYER_CARD_INSTANCE(player, card).damage_target_player;
    global_stack_damage_targets[unk_008b2934].card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;
    if (trigger_condition == -1)
    {
      unk_00895030[unk_008b2934] = current_phase;
    }
    else
    {
      unk_00895030[unk_008b2934] = trigger_condition;
    }
    if (unk_008a9000 != 1)
    {
      unk_007a7750[unk_008b2934] = prompt;
    }
    ++unk_008b2934;
    global_stack_cards[unk_008b2934].player = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004eaceb
void FUN_004eaceb(int player, unsigned int color_to_produce, int color_to_consume)
{
  int slot;

  if ((int)color_to_produce > 0 && color_to_consume > 0)
  {
    for (slot = 0; slot < 10; ++slot)
    {
      if (unk_007161e0[player][slot] == -1)
      {
        unk_007161e0[player][slot] = (color_to_consume << 16) | (color_to_produce & 0xffff);
        unk_007161e0[player][slot + 1] = -1;
        break;
      }
    }
  }
}

// FUNCTION: MAGIC 0x00551334
void FUN_00551334(int player, int card)
{
  --global_card_instances[player][card].special_counters;
}

// FUNCTION: MAGIC 0x0041c752
int FUN_0041c752(int player, int card, int event, int amount)
{
  unsigned int special_counters;
  int old_max_x_value;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player)
  {
    FUN_00551572(player, card, amount);
  }

  if (trigger_condition == 0xcc && (special_counters = C_get_special_counters(player, card)) != 0 && affected_card == card && affected_card_controller == player && player == current_turn && ((PLAYER_CARD_INSTANCE(player, card).state & 4) != 0 || (PLAYER_CARD_INSTANCE(player, card).untap_status != -1 && human_player != player)))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      FUN_00551334(player, card);
    }
  }

  if (event == EVENT_TOUGHNESS && affected_card == card && affected_card_controller == player)
  {
    event_result += C_get_special_counters(player, card);
  }

  old_max_x_value = max_x_value;
  if (event == EVENT_CAN_ACTIVATE && current_phase == PHASE_MAIN1 && human_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && unk_00742f60 == player)
  {
    special_counters = C_get_special_counters(player, card);
    if ((int)special_counters < amount && has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0))
    {
      if (active_player == player && (unk_00926804 & 2) == 0 && *(short *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0xe) < life[player])
      {
        unk_008b3270 |= 3;
      }
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    special_counters = C_get_special_counters(player, card);
    unk_00715fa8 = FUN_004c0a36(amount - special_counters, 0, has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0));
  }
  else
  {
    if (event == EVENT_ACTIVATE && affected_card == card && affected_card_controller == player)
    {
      special_counters = C_get_special_counters(player, card);
      max_x_value = amount - special_counters;
      if (unk_008b35ec == player || (unk_00926804 & 2) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = charge_mana_for_activated_ability(player, card, -1, 0, 0, 0, 0, 0);
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            charge_mana_for_activated_ability(player, card, unk_00715fa8, 0, 0, 0, 0, 0);
      }
      max_x_value = old_max_x_value;
      if (spell_fizzled == 1)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION && PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                                                  PLAYER_CARD_INSTANCE(player, card).parent_card)
                                                     .internal_card_id != -1)
    {
      int new_counter_total;

      new_counter_total =
          FUN_004c0a36(PLAYER_CARD_INSTANCE(player, card).info_slot + C_get_special_counters(card_on_stack_controller, card_on_stack),
                       0,
                       amount);
      FUN_00551572(card_on_stack_controller, card_on_stack, new_counter_total);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00444b74
int obliterate_top_card_of_stack(void)
{
  if (unk_008b2934 > 0)
  {
    --unk_008b2934;
    if (unk_0091a80c == PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
                            .internal_card_id)
    {
      PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
          .internal_card_id = -1;
    }
    global_stack_cards[unk_008b2934].player = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00444505
int resolve_top_card_on_stack(void)
{
  int player;
  int card;
  int current_internal_id;

  if (unk_008b2934 > 0)
  {
    --unk_008b2934;
    player = global_stack_cards[unk_008b2934].player;
    card = global_stack_cards[unk_008b2934].card;
    current_internal_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    if (current_internal_id == unk_0091a80c)
    {
      current_internal_id = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
    }
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1)
    {
      if ((char)(unk_00939180[unk_008b2934] >> 0x10) == '~')
      {
        FUN_004b082f(player, card, (unk_00939180[unk_008b2934] >> 0x10) & 0xff, unk_00939180[unk_008b2934] >> 0x18);
      }
      else if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 8) == 0)
      {
        if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x80) == 0)
        {
          dispatch_event_to_single_card(player,
                                        card,
                                        (unk_00939180[unk_008b2934] >> 0x10) & 0xff,
                                        1 - player,
                                        -1);
        }
        else
        {
          if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x40) != 0)
          {
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .untap_status &= ~0x10;
            dispatch_event_to_single_card(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                          PLAYER_CARD_INSTANCE(player, card).parent_card,
                                          EVENT_UNTAP_CARD,
                                          1 - player,
                                          -1);
          }
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .state &= ~0x80;
        }
      }
      else
      {
        if ((PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 0x200) == 0)
        {
          dispatch_event_to_single_card(player, card, EVENT_UPKEEP_COSTS_UNPAID, 1 - player, -1);
          if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1 && (PLAYER_CARD_INSTANCE(player, card).upkeep_flags & 2) != 0)
          {
            kill_card(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                      PLAYER_CARD_INSTANCE(player, card).parent_card,
                      KILL_BURY);
          }
        }
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .state &= ~0x208;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .state |= 4;
      }
      if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == unk_0091a80c)
      {
        kill_card(player, card, KILL_REMOVE);
      }
    }
    global_stack_cards[unk_008b2934].player = -1;
    FUN_00441d78();
    if (unk_0091a6d0 < 2 && ((unk_008b4278 & 0x200) == 0 || unk_008b2934 == 0))
    {
      FUN_004afa4b();
      regenerate_or_graveyard_triggers();
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004482f3
int copy_mana_pool_to_display(void)
{
  if (unk_008a9000 != 1)
  {
    EnterCriticalSection((void *)&unk_00789110);
    memcpy(unk_00896670, raw_mana_available[0], 0x1c);
    memcpy(unk_007ab290, raw_mana_available[1], 0x1c);
    LeaveCriticalSection((void *)&unk_00789110);
    SendMessageA((void *)unk_008ce534, 0x432, 0, 0);
    SendMessageA((void *)unk_00939344, 0x432, 0, 0);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00441d78
void FUN_00441d78(void)
{
  int player;
  int card;
  card_instance_t *instance;

  for (player = 0; player < 2; ++player)
  {
    instance = global_card_instances[player];
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      if ((instance->state & 0x800002) == 2 && instance->internal_card_id != -1)
      {
        instance->regen_status |= 0x0f000000;
        C_get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
      }
      ++instance;
    }
  }

  C_count_colors_of_lands_in_play();
  count_mana();

  for (player = 0; player < 2; ++player)
  {
    instance = global_card_instances[player];
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      if (is_in_play(player, card) && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) && (short)instance->damage_on_card < C_get_abilities(player, card, EVENT_TOUGHNESS, -1))
      {
        C_get_abilities(player, card, EVENT_POWER, -1);
      }
      ++instance;
    }
  }
}

// FUNCTION: MAGIC 0x004afa4b
void FUN_004afa4b()
{
  struct
  {
    int test_player;
    int test_card;
  } s;
  card_instance_t *instance;

  if ((unk_008b4278 & 2) == 0)
  {
    return;
  }
  unk_008b4278 &= ~2;
  unk_008b4278 |= 4;
  TENTATIVE_reassess_all_cards(0, 0xff);

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (instance->internal_card_id == unk_009266a4 &&
          is_in_play(s.test_player, s.test_card) &&
          (instance->state & 0x10) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x21);
      }
    }
  }

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (instance->internal_card_id == unk_009266a4 &&
          is_in_play(s.test_player, s.test_card) &&
          (instance->state & 0x10) == 0)
      {
        dispatch_event(s.test_player, s.test_card, 0x6e);
      }
    }
  }

  for (s.test_player = 0; s.test_player < 2; ++s.test_player)
  {
    for (s.test_card = 0; s.test_card < active_cards_count[s.test_player]; ++s.test_card)
    {
      instance = &PLAYER_CARD_INSTANCE(s.test_player, s.test_card);
      if (instance->internal_card_id == unk_009266a4 && is_in_play(s.test_player, s.test_card))
      {
        if ((instance->state & 0x10) == 0)
        {
          unk_008b4278 |= 2;
        }
        else
        {
          kill_card(s.test_player, s.test_card, 1);
        }
      }
    }
  }

  FUN_004aff25();
  unk_008b4278 &= ~4;
}

// FUNCTION: MAGIC 0x004aff25
void FUN_004aff25(void)
{
  int player;
  int card;
  int toughness;
  card_instance_t *instance;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, card);
      if (is_in_play(player, card) && (global_cards_data[instance->internal_card_id].type & TYPE_CREATURE))
      {
        toughness = (short)instance->damage_on_card;
        if (C_get_abilities(player, card, EVENT_TOUGHNESS, -1) <= toughness)
        {
          if (unk_008a9000 != 1)
          {
            play_sound_effect(0x19);
          }
          kill_card(player, card, 2);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0044aa01
int FUN_0044aa01(void)
{
  if ((unk_00926804 & 2) == 0)
  {
    if (unk_00716248 != -1 && unk_008a9000 != 1)
    {
      if (current_phase < unk_00716248 || human_player != unk_00716244)
      {
        return 1;
      }
      if (trigger_condition != -1 && unk_00716248 == current_phase)
      {
        return 1;
      }
      if (unk_00716248 < current_phase && human_player == unk_00716244)
      {
        unk_00716244 = -1;
        unk_00716248 = -1;
      }
    }
  }
  else if ((unk_00716248 != -1 || unk_007161d4 != -1) && unk_008a9000 != 1)
  {
    return 0;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0044541f
int FUN_0044541f(int param_1)
{
  int can_respond;

  can_respond = 0;
  if (param_1 == 0)
  {
    if ((unk_008b35ec == human_player && (DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0) || (active_player == human_player && (DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0))
    {
      can_respond = 1;
    }
    if (active_player == human_player)
    {
      if (unk_00716244 == human_player && current_phase == unk_00716248)
      {
        can_respond = 1;
      }
    }
    else if (_DAT_00743034 == human_player && unk_007161d4 == current_phase)
    {
      can_respond = 1;
    }
  }
  else
  {
    if ((unk_008b35ec == human_player && (DAT_007abc90[human_player * 0x26 + current_phase] & 1) != 0) || (active_player == human_player && (DAT_007abc90[human_player * 0x26 + current_phase] & 4) != 0))
    {
      can_respond = 1;
    }
    if (unk_008b35ec == human_player)
    {
      if (unk_00716244 == human_player && current_phase == unk_00716248)
      {
        can_respond = 1;
      }
    }
    else if (_DAT_00743034 == human_player && unk_007161d4 == current_phase)
    {
      can_respond = 1;
    }
  }

  if (FUN_0044aa01() == 0 && can_respond)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004460d3
void FUN_004460d3(void)
{
  int player;
  int card;
  card_instance_t *instance;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, card);
      if ((instance->timestamp & 4) == 0 && is_in_play(player, card))
      {
        instance->blocking = 0;
        instance->initial_color = 0;
        instance->unused0x26 = 0;
        instance->regen_status &= 0xff000000;
        instance->untap_status = 0;
        instance->counters = 0;
        instance->counters5 = 0;
        instance->unknown0x122 &= 0xff00;
        instance->timestamp = 0;
        dispatch_event(player, card, 0x85);
        dispatch_event(player, card, 0x84);
      }
    }
  }
}

// FUNCTION: MAGIC 0x00447f80
unsigned int FUN_00447f80(void)
{
  return (GetTickCount() - unk_0093f4b4 - unk_007abc84) / 0x37;
}

// FUNCTION: MAGIC 0x004e1b6f
unsigned int FUN_004e1b6f(void)
{
  return FUN_00447f80();
}

// FUNCTION: MAGIC 0x004b082f
int FUN_004b082f(int player, int card, event_t event, int extra)
{
  if (event == EVENT_TRIGGER && ((PLAYER_CARD_INSTANCE(player, card).state & 0x100) != 0 || unk_00777850 != 0))
  {
    return 0;
  }

  if (trigger_condition < 200)
  {
    return 0;
  }

  event_result = 0;
  affected_card_controller = player;
  affected_card = card;
  unk_008cfd20 = extra;
  attacking_card = -1;
  C_dispatch_event_raw(event);
  return event_result;
}

// FUNCTION: MAGIC 0x0044295b
void C_dispatch_event_raw(event_t event)
{
  struct
  {
    card_instance_t *instance;
    unsigned int timestamp_slot;
    int card;
    unsigned int saved_trigger_condition;
    int player;
  } locals;

  locals.saved_trigger_condition = unk_00712938;
  unk_008cc844 = event;
  for (locals.timestamp_slot = 0; (int)locals.timestamp_slot < 500; ++locals.timestamp_slot)
  {
    if (TENTATIVE_timestamp_player[locals.timestamp_slot] == -1)
    {
      break;
    }

    locals.player = TENTATIVE_timestamp_player[locals.timestamp_slot];
    locals.card = TENTATIVE_timestamp_card[locals.timestamp_slot];
    locals.instance = &PLAYER_CARD_INSTANCE(locals.player, locals.card);
    if (locals.instance->timestamp != (int)locals.timestamp_slot)
    {
      continue;
    }
    if (locals.instance->internal_card_id == -1)
    {
      continue;
    }
    if ((locals.instance->state & 0x800000) != 0)
    {
      continue;
    }
    if ((locals.instance->state & 2) != 0 || (locals.instance->state & 0x20) != 0)
    {

      unk_00789a48 = locals.player * 0x80 + locals.card;
      (*global_cards_data[locals.instance->internal_card_id].code_pointer)(locals.player, locals.card, event);
      if (event == EVENT_DECLARE_ATTACKERS && locals.player == human_player && (locals.instance->state & 0x14) == 4 && !has_vigilance(locals.player, locals.card))
      {
        locals.instance->state |= 0x10;
        produced_mana_color = -1;
        dispatch_event(locals.player, locals.card, EVENT_TAP_CARD);
        FUN_004afa4b(human_player);
        regenerate_or_graveyard_triggers();
      }
    }
  }
  if (event == EVENT_DECLARE_ATTACKERS && locals.player == human_player)
  {
    FUN_00441d78();
  }
  if (unk_00789308 != -1)
  {
    (*global_cards_data[unk_00789308].code_pointer)(0, 0x94, event);
  }
  unk_00712938 = locals.saved_trigger_condition;
}

// FUNCTION: MAGIC 0x004a6d0e
void count_mana(void)
{
  struct
  {
    card_instance_t *instance;
    int current_card;
    int player;
  } locals;

  for (locals.current_card = 0; locals.current_card <= 7; ++locals.current_card)
  {
    unk_0072c440[locals.current_card] = 0;
    unk_00742f90[locals.current_card] = unk_0072c440[locals.current_card];
    unk_00742f70[locals.current_card] = unk_00742f90[locals.current_card];
  }
  unk_007160fc = -1;
  raw_mana_available_hex[0][0] = unk_007160fc;
  unk_0071620c = -1;
  unk_007161e0[0][0] = unk_0071620c;
  for (locals.player = 0; locals.player < 2; ++locals.player)
  {
    locals.instance = global_card_instances[locals.player];
    for (locals.current_card = 0; locals.current_card < active_cards_count[locals.player];
         ++locals.current_card, ++locals.instance)
    {
      if (locals.instance->internal_card_id == -1)
      {
        continue;
      }

      if ((global_cards_data[locals.instance->internal_card_id].extra_ability & 0x1000) != 0 && (locals.instance->state & 0x800012) == 2)
      {
        dispatch_event(locals.player, locals.current_card, EVENT_COUNT_MANA);
      }
      if (*(int *)&global_cards_data[locals.instance->internal_card_id].id == 0xee && (locals.instance->state & 0x800012) == 2)
      {
        dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_COUNT_MANA, -1, -1);
      }
      if (*(int *)&global_cards_data[locals.instance->internal_card_id].id == 100 && (locals.instance->state & 0x800002) == 2)
      {
        dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_COUNT_MANA, -1, -1);
      }
    }
  }
  if ((unk_007abc78 & 0x08000000) != 0)
  {
    for (locals.player = 0; locals.player < 2; ++locals.player)
    {
      locals.instance = global_card_instances[locals.player];
      for (locals.current_card = 0; locals.current_card < active_cards_count[locals.player];
           ++locals.current_card, ++locals.instance)
      {
        if (locals.instance->internal_card_id == -1)
        {
          continue;
        }

        if (global_cards_data[locals.instance->internal_card_id].code_pointer == card_fellwar_stone && (locals.instance->state & 0x800012) == 2)
        {
          dispatch_event_to_single_card(locals.player, locals.current_card, EVENT_VARIABLE_MANA_SRC, -1, -1);
        }
      }
    }
  }
}
// FUNCTION: MAGIC 0x004a6faa
void C_count_colors_of_lands_in_play(void)
{
  int player;
  int card;
  int internal_card_id;
  int csvid;

  for (card = 0; card < 8; ++card)
  {
    unk_00743000[0][card] = 0;
    basiclandtypes_controlled[0][card] = unk_00743000[0][card];
    unk_00743000[1][card] = 0;
    basiclandtypes_controlled[1][card] = unk_00743000[1][card];
  }
  unk_00743080[0] = 0;
  unk_00743080[1] = 0;

  for (player = 0; player < 2; ++player)
  {
    for (card = 0; card < active_cards_count[player]; ++card)
    {
      internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      if (is_in_play(player, card) && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
      {
        unk_00743080[player] |= (unsigned char)PLAYER_CARD_INSTANCE(player, card).mana_color;
        if (internal_card_id < 5)
        {
          ++basiclandtypes_controlled[player][internal_card_id];
        }
        else if (internal_card_id < 0xf || unk_0093f4b8 <= internal_card_id)
        {
          csvid = *(int *)&global_cards_data[internal_card_id].id;
          if (csvid < 0xd)
          {
            if (csvid == 0xc)
            {
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
            }
            else if (csvid == 9)
            {
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
            }
            else
            {
              ++basiclandtypes_controlled[player][0];
            }
          }
          else
          {
            switch (csvid)
            {
            case 0x5b:
              ++basiclandtypes_controlled[player][3];
              break;
            case 0x7e:
              ++basiclandtypes_controlled[player][2];
              break;
            case 0xa4:
              ++basiclandtypes_controlled[player][4];
              break;
            case 0xbc:
              ++basiclandtypes_controlled[player][5];
              break;
            case 0xbd:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xd4:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              break;
            case 0xd8:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              break;
            case 0xef:
              ++basiclandtypes_controlled[player][1];
              break;
            case 0xf1:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            case 0xfc:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 3)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0xfe:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 5)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x102:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 1)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              break;
            case 0x10a:
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 2)];
              ++basiclandtypes_controlled[player][get_hacked_color(player, card, 4)];
              break;
            default:
              ++basiclandtypes_controlled[player][0];
              break;
            }
          }
        }
        else
        {
          ++basiclandtypes_controlled[player][0];
        }
        ++basiclandtypes_controlled[player][7];
      }
    }
  }
}

// FUNCTION: MAGIC 0x00534ddb
int FUN_00534ddb(int player, int mode)
{
  int best_card;
  int best_score;
  int current_card;
  int count;
  int current_internal_id;
  int current_score;
  unsigned int power;
  unsigned int toughness;
  unsigned int abilities;
  unsigned int best_power;
  unsigned int best_toughness;
  int candidate_cards[50];
  int candidate_scores[50];
  unsigned int candidate_powers[50];
  unsigned int candidate_toughnesses[50];
  unsigned int candidate_abilities[50];

  if (player == -1)
  {
    return -1;
  }

  if (mode == 1)
  {
    best_card = -1;
    best_score = -10;
    count = 0;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_LAND) && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED))
      {
        candidate_cards[count] = current_card;
        candidate_scores[count] = 0;
        ++count;
      }
    }

    for (current_card = 0; current_card < count; ++current_card)
    {
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++candidate_scores[current_card];
      }
      if ((PLAYER_CARD_INSTANCE(player, current_card).token_status & 0x400) != 0)
      {
        candidate_scores[current_card] = -1;
      }
    }

    for (current_card = 0; current_card < count; ++current_card)
    {
      if (best_score < candidate_scores[current_card])
      {
        best_score = candidate_scores[current_card];
        best_card = candidate_cards[current_card];
      }
    }

    return best_card;
  }

  if (mode == 2)
  {
    best_card = -1;
    best_score = -1;
    best_power = 0;
    best_toughness = 0;
    count = 0;

    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED))
      {
        candidate_cards[count] = current_card;
        candidate_powers[count] = C_get_abilities(player, current_card, EVENT_POWER, -1);
        if (best_power < candidate_powers[count])
        {
          best_power = candidate_powers[count];
        }
        candidate_toughnesses[count] = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        if (best_toughness < candidate_toughnesses[count])
        {
          best_toughness = candidate_toughnesses[count];
        }
        candidate_abilities[count] = C_get_abilities(player, current_card, EVENT_ABILITIES, -1);
        candidate_scores[count] = 0;
        ++count;
      }
    }

    for (current_card = 0; current_card < count; ++current_card)
    {
      if (candidate_powers[current_card] == best_power)
      {
        candidate_scores[current_card] += 3;
      }
      if (candidate_toughnesses[current_card] == best_toughness)
      {
        candidate_scores[current_card] += 2;
      }
      if ((candidate_abilities[current_card] & 0x20) != 0)
      {
        ++candidate_scores[current_card];
      }
      if ((candidate_abilities[current_card] & 0x100) != 0)
      {
        ++candidate_scores[current_card];
      }
      for (abilities = candidate_abilities[current_card]; abilities != 0; abilities >>= 1)
      {
        if ((abilities & 1) != 0)
        {
          ++candidate_scores[current_card];
        }
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability & 0x1000) != 0)
      {
        ++candidate_scores[current_card];
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability & 1) != 0)
      {
        ++candidate_scores[current_card];
      }
    }

    for (current_card = 0; current_card < count; ++current_card)
    {
      if (best_score < candidate_scores[current_card])
      {
        best_score = candidate_scores[current_card];
        best_card = candidate_cards[current_card];
      }
    }

    return best_card;
  }

  if (mode == 0x40)
  {
    best_score = -1;
    best_card = -1;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) && (PLAYER_CARD_INSTANCE(player, current_card).dummy3 & 1) != 0)
      {
        current_internal_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
        power = C_get_abilities(player, current_card, EVENT_POWER, -1);
        toughness = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        current_score = power + toughness;
        abilities = C_get_abilities(player, current_card, EVENT_ABILITIES, -1);
        if ((abilities & 0x20) != 0)
        {
          ++current_score;
        }
        if ((abilities & 0x100) != 0)
        {
          ++current_score;
        }
        while (abilities != 0)
        {
          if ((abilities & 1) != 0)
          {
            ++current_score;
          }
          abilities >>= 1;
        }
        if ((global_cards_data[current_internal_id].extra_ability & 0x1000) != 0)
        {
          ++current_score;
        }
        if ((global_cards_data[current_internal_id].extra_ability & 1) != 0)
        {
          ++current_score;
        }
        if (global_cards_data[current_internal_id].cc[1] != 0xff)
        {
          current_score += (char)global_cards_data[current_internal_id].cc[1];
        }
        if (best_score < current_score)
        {
          best_score = current_score;
          best_card = current_card;
        }
      }
    }

    return best_card;
  }

  return -1;
}

// FUNCTION: MAGIC 0x00551e49
int sacrifice_a_land(int player)
{
  target_t target;

  if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0,
                           COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                           text_lines[0], 0, &target))
  {

    if (unk_008a9000 != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
    kill_card(target.player, target.card, KILL_SACRIFICE);

    return 1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00551921
int FUN_00551921(int player)
{
  target_t target;
  int current_card;
  int best_card;
  int best_value;
  int internal_card_id;
  unsigned int power;
  unsigned int toughness;
  int card_value;
  card_instance_t *instance;

  if ((player == unk_008b35ec || (unk_00926804 & 2) != 0) && unk_008a9000 != 1)
  {
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              text_lines[0], 0, &target))
    {
      best_card = -1;
    }
    else
    {
      best_card = target.card;
    }
  }
  else
  {
    best_card = -1;
    best_value = 0x7fff;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card)
    {
      instance = &PLAYER_CARD_INSTANCE(player, current_card);
      internal_card_id = instance->internal_card_id;
      if (internal_card_id != -1 && (instance->state & 0x800002) == 2 && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 && instance->kill_code != 3 && (instance->state & 0x1000) == 0)
      {
        power = C_get_abilities(player, current_card, EVENT_POWER, -1);
        toughness = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        card_value = (power + 2) * (toughness + 2);
        if (card_value < best_value)
        {
          best_value = card_value;
          best_card = current_card;
        }
      }
    }
  }

  if (best_card != -1 && unk_008a9000 != 1)
  {
    play_sound_effect(WAV_SACRFICE);
  }
  return best_card;
}

// FUNCTION: MAGIC 0x00466e6d
int FUN_00466e6d(int player, int card, int target_player)
{
  card_instance_t *instance;
  int land_type;
  int preferred_basic;
  int smallest_count;
  int current_type;
  int current_card;
  int found;

  found = 0;
  land_type = get_hacked_color(player, card, 1);
  if (target_player == -1)
  {
    target_player = 1 - player;
  }

  preferred_basic = -1;
  smallest_count = 999;
  for (current_type = 1; current_type < 6; ++current_type)
  {
    if (current_type != land_type && basiclandtypes_controlled[target_player][current_type] != 0 && basiclandtypes_controlled[target_player][current_type] < smallest_count)
    {
      smallest_count = basiclandtypes_controlled[target_player][current_type];
      preferred_basic = current_type - 1;
    }
  }

  for (current_card = 0; current_card < active_cards_count[target_player] && !found; ++current_card)
  {
    instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
    if (is_in_play(target_player, current_card) && (global_cards_data[instance->internal_card_id].type & TYPE_LAND) != 0)
    {
      if (preferred_basic != -1 && global_cards_data[instance->internal_card_id].subtype == preferred_basic)
      {
        preferred_basic = -1;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        found = 1;
      }
    }
  }

  return PLAYER_CARD_INSTANCE(player, card).number_of_targets;
}

// FUNCTION: MAGIC 0x004823a5
int FUN_004823a5(int player, int card)
{
  card_instance_t *aura;
  card_instance_t *instance;
  card_instance_t *source;
  int target_player;
  int target_card;
  int test_player;
  int current_card;
  int saved_player;
  int saved_card;
  int found_legacy;

  aura = &PLAYER_CARD_INSTANCE(player, card);
  target_player = aura->damage_target_player;
  target_card = aura->damage_target_card;
  saved_player = -1;
  saved_card = -1;
  found_legacy = 0;

  for (current_card = 0; current_card < active_cards_count[target_player]; ++current_card)
  {
    instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
    if (is_in_play(target_player, current_card) && instance->internal_card_id == unk_008cefb8 && instance->damage_target_player == target_player && instance->damage_target_card == target_card)
    {
      source = &PLAYER_CARD_INSTANCE(instance->damage_source_player, instance->damage_source_card);
      if ((source->state & STATE_TAPPED) == 0)
      {
        found_legacy = 1;
        saved_player = instance->targets[0].player;
        saved_card = instance->targets[0].card;
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->info_slot = 1;
      }
    }
  }

  for (test_player = 0; test_player < 2; ++test_player)
  {
    for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card)
    {
      instance = &PLAYER_CARD_INSTANCE(test_player, current_card);
      if (is_in_play(test_player, current_card) && instance->internal_card_id == unk_008d0340 && instance->damage_target_player == target_player && instance->damage_target_card == target_card && (instance->token_status & 0x1000000) != 0)
      {
        if (!found_legacy)
        {
          instance->token_status &= ~0x1000000;
        }
        if (saved_player == -1)
        {
          aura->damage_source_player = test_player;
          aura->damage_source_card = current_card;
        }
        else
        {
          aura->damage_source_player = saved_player;
          aura->damage_source_card = saved_card;
        }
      }
    }
  }

  return !found_legacy;
}

// FUNCTION: MAGIC 0x0051c73d
int FUN_0051c73d(int player, int card, int internal_card_id)
{
  card_instance_t *instance;
  card_instance_t *callback_instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (internal_card_id == unk_008d0340 && instance->damage_source_player == affected_card_controller && instance->damage_source_card == affected_card)
  {
    callback_instance = &PLAYER_CARD_INSTANCE(affected_card_controller, affected_card);
    instance->damage_source_player = callback_instance->damage_source_player;
    instance->damage_source_card = callback_instance->damage_source_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051bcf0
int FUN_0051bcf0(int player, int card, event_t event, unsigned int required_type)
{
  card_instance_t *instance;
  target_t target;
  int new_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, required_type, 0, 0,
                                 get_protections_from(player, card), 0, 0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player)
  {
    if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                              ~SUB_WALL, -1, -1, 0, 0, 0, text_lines[0], 1, &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char *)0,
                                player, 2, 2, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                                ~SUB_WALL, -1, -1, 0, 0, 0))
    {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    }
    else
    {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      if (FUN_004823a5(player, card))
      {
        instance->token_status |= 0x1000000;
        if (instance->targets[0].player != player)
        {
          if (unk_008a9000 != 1)
          {
            play_sound_effect(WAV_CONTROL);
          }
          new_card = gain_control(instance->damage_target_player, instance->damage_target_card, 0, 0);
          instance->damage_target_player = player;
          instance->damage_target_card = new_card;
        }
      }
    }
    instance->number_of_targets = 0;
    return 0;
  }

  return 0;
}
// FUNCTION: MAGIC 0x004a67bd
int choose_a_number(int player, const char *prompt, int maxnum)
{
  player = player;
  prompt = prompt;
  return maxnum;
}

// FUNCTION: MAGIC 0x0043b4f3
int FUN_0043b4f3(int player, int amount)
{
  card_instance_t *instance;
  int max_targets;
  int sacrifices_to_make;
  int sacrifices_made;
  char prompt[300];
  target_t target;

  real_target_available(&max_targets,
                        TARGET_SCAN_DIRECT,
                        player,
                        (unsigned char)player,
                        (unsigned char)player,
                        TARGET_ZONE_IN_PLAY,
                        TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                        TARGET_TYPE_TOKEN,
                        0,
                        0,
                        0,
                        0,
                        -1,
                        -1,
                        0xffffffff,
                        0xffffffff,
                        0,
                        0,
                        0);

  sacrifices_to_make = max_targets;
  if (amount <= max_targets)
  {
    sacrifices_to_make = amount;
  }

  sacrifices_made = 0;
  while (sacrifices_made < sacrifices_to_make && life[player] != -99)
  {
    if (unk_008a9000 == 1)
    {
      target.player = player;
      do
      {
        do
        {
          unk_00939340 = internal_rand(active_cards_count[player]);
          target.card = unk_00939340;
        } while (!is_in_play(target.player, target.card));
        instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
      } while ((global_cards_data[instance->internal_card_id].type & 0x7f) == 0 || (instance->token_status & 0x10) != 0);
      FUN_004e4f11();
    }
    else if (player == active_player && (unk_00926804 & 2) == 0)
    {
      FUN_004e5089();
      target.player = player;
      target.card = unk_00939340;
    }
    else
    {
      load_text((int)"promptsX1.txt", "LICH");
      sprintf(prompt, text_lines[0], sacrifices_made + 1, sacrifices_to_make);
      C_real_select_target(player,
                           player,
                           player,
                           TARGET_ZONE_IN_PLAY,
                           TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                           TARGET_TYPE_TOKEN,
                           0,
                           0,
                           COLOR_TEST_0,
                           COLOR_TEST_0,
                           -1,
                           ~SUB_WALL,
                           -1,
                           -1,
                           0,
                           0,
                           0,
                           prompt,
                           0,
                           &target);
      if (unk_008a9000 != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
    }

    kill_card(target.player, target.card, KILL_SACRIFICE);
    ++sacrifices_made;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b4110
void FUN_004b4110(int player)
{
  PostMessageA((void *)unk_008cf1b4, 0x401, (unsigned int)player, 0);
  ExitThread((unsigned int)player);
}

// FUNCTION: MAGIC 0x0051c8c0
int gain_control(int player, int card)
{
  card_instance_t *instance;
  int opponent;
  int timestamp_slot;
  int new_card;
  int test_player;
  int test_card;
  int target_index;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  opponent = 1 - player;
  timestamp_slot = *(int *)((char *)instance + 0x50);
  new_card = add_card_to_hand(opponent, instance->internal_card_id);
  if (new_card != -1)
  {
    memcpy(global_card_instances[opponent] + new_card, global_card_instances[player] + card, 300);
    PLAYER_CARD_INSTANCE(opponent, new_card).state |= 0x30000;
    PLAYER_CARD_INSTANCE(opponent, new_card).state &= 0xfffffff3;
    TENTATIVE_timestamp_player[timestamp_slot] = opponent;
    TENTATIVE_timestamp_card[timestamp_slot] = new_card;

    for (test_player = 0; test_player < 2; ++test_player)
    {
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card)
      {
        if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == player && PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == card)
        {
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player = opponent;
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card = new_card;
        }
        if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_player == player && PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_card == card)
        {
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_player = opponent;
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_card = new_card;
        }
        if (PLAYER_CARD_INSTANCE(test_player, test_card).number_of_targets != 0)
        {
          for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(test_player, test_card).number_of_targets; ++target_index)
          {
            if (PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].player == player && PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].card == card)
            {
              PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].player = opponent;
              PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].card = new_card;
            }
          }
        }
      }
    }
  }

  instance->token_status |= 8;
  kill_card(player, card, KILL_REMOVE);
  if ((unk_007abc78 & 0x10000000) != 0)
  {
    dispatch_event(opponent, new_card, EVENT_CARDCONTROLLED);
  }
  return new_card;
}

// FUNCTION: MAGIC 0x004b5d72
void real_put_on_top_of_deck(int player, int internal_card_id)
{
  int index;

  for (index = 499; index > 0; --index)
  {
    global_library[player][index] = global_library[player][index - 1];
  }
  global_library[player][0] = internal_card_id;
}

// FUNCTION: MAGIC 0x005180ed
int FUN_005180ed(int a1, int a2, int player, int card, int internal_card_id)
{
  if (internal_card_id == unk_0093933c && PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller && PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
  {
    event_result = 1;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051819c
int FUN_0051819c(int parent_player, int parent_card, int player, int card, int internal_card_id)
{
  if (internal_card_id == unk_0093933c && PLAYER_CARD_INSTANCE(player, card).damage_source_player == parent_player && PLAYER_CARD_INSTANCE(player, card).blocking == parent_card)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052adf2
int FUN_0052adf2(int player, int card)
{
  card_instance_t *instance;
  int max_cards;
  int current_card;
  int test_player;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  max_cards = active_cards_count[1];
  if (active_cards_count[1] <= active_cards_count[0])
  {
    max_cards = active_cards_count[0];
  }

  for (current_card = 0; current_card < max_cards; ++current_card)
  {
    for (test_player = 0; test_player < 2; ++test_player)
    {
      if (is_in_play(test_player, current_card) && instance->targets[0].player == PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_player && instance->targets[0].card == PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_card)
      {
        return 1;
      }
    }
  }

  return 0;
}
