#include <stdlib.h>
#include <string.h>
#include "game_support.h"

typedef int (__cdecl *in_play_card_callback_t)(int source_player,
                                               int source_card,
                                               int player,
                                               int card,
                                               int internal_card_id);

extern card_ptr_t global_raw_cards_storage[2000];

int can_target(target_definition_t *td)
{
  return td != NULL;
}

// FUNCTION: MAGIC 0x00404c4c
int FUN_00404c4c(int player, int internal_card_id)
{
  int card;
  int result;

  result = 1;
  for (card = 0; card < active_cards_count[player]; ++card) {
    if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id
        && (PLAYER_CARD_INSTANCE(player, card).state & 2) == 0) {
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
  for (player = 0; player < 2; ++player) {
    if (who_to_check == -1 || who_to_check == player) {
      for (card = 0; card < active_cards_count[player]; ++card) {
        if (is_in_play(player, card) && PLAYER_CARD_INSTANCE(player, card).internal_card_id == internal_card_id) {
          ++result;
        }
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00435e27
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
  return player >= 0
         && player < 2
         && card >= 0
         && card < active_cards_count[player]
         && global_card_instances[player][card].internal_card_id != -1
         && (global_card_instances[player][card].state & F08_INPLAY) != 0;
}

int get_hacked_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].hack_mode[orig_color] != 0) {
    orig_color = (char)global_card_instances[player][card].hack_mode[orig_color];
  }

  return orig_color;
}

// FUNCTION: MAGIC 0x0050026d
int get_sleighted_color(int player, int card, int orig_color)
{
  if ((char)global_card_instances[player][card].color_id[orig_color] != 0) {
    orig_color = (char)global_card_instances[player][card].color_id[orig_color];
  }

  return orig_color;
}

// FUNCTION: MAGIC 0x00442763
int get_color_from_color_test(char color_test)
{
  if ((color_test & COLOR_TEST_BLACK) != 0) {
    return COLOR_BLACK;
  }
  if ((color_test & COLOR_TEST_BLUE) != 0) {
    return COLOR_BLUE;
  }
  if ((color_test & COLOR_TEST_GREEN) != 0) {
    return COLOR_GREEN;
  }
  if ((color_test & COLOR_TEST_RED) != 0) {
    return COLOR_RED;
  }
  if ((color_test & COLOR_TEST_WHITE) != 0) {
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
  if (player == -1) {
    return -1;
  }

  for (graveyard_index = 0; graveyard_index < 500 && global_graveyard_slots[player][graveyard_index] != -1;
       ++graveyard_index) {
    internal_card_id = global_graveyard_slots[player][graveyard_index];
    if (type == -1 || (type & global_cards_data[internal_card_id].type) != 0) {
      score = (char)global_cards_data[internal_card_id].cc[0] * 3
              + abs((int)(char)global_cards_data[internal_card_id].cc[1]) * 2;
      if (best_score < score) {
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
  if (player == -1 || player == unk_008b35ec) {
    graveyard_index = 0;
    while (graveyard_index < 500) {
      if (global_graveyard_slots[unk_008b35ec][graveyard_index] == -1) {
        break;
      } else if ((type & global_cards_data[global_graveyard_slots[unk_008b35ec][graveyard_index]].type) != 0) {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  if ((player == -1 || player == active_player) && result == 0) {
    graveyard_index = 0;
    while (graveyard_index < 500) {
      if (global_graveyard_slots[active_player][graveyard_index] == -1) {
        break;
      } else if ((type & global_cards_data[global_graveyard_slots[active_player][graveyard_index]].type) != 0) {
        result = 1;
        break;
      }
      ++graveyard_index;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00484581
int FUN_00484581(int internal_card_id, int color)
{
  int result;

  result = 0;
  if ((char)global_cards_data[internal_card_id].subtype == 13
      && *(int *)&global_cards_data[internal_card_id].id == unk_0092664c[color]) {
    result = 1;
  }
  if ((char)global_cards_data[internal_card_id].subtype == 12
      && ((1 << ((unsigned char)color & 0x1f))
          & (int)*(char *)&PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).mana_color) != 0) {
    result = 1;
  }

  return result;
}

// FUNCTION: MAGIC 0x0053aa74
unsigned int FUN_0053aa74(int player, int card)
{
  unsigned char type;
  unsigned int illegal_abilities;
  int internal_card_id;

  illegal_abilities = 0;
  internal_card_id = global_card_instances[player][card].internal_card_id;
  if (internal_card_id == -1 || internal_card_id == unk_0091a80c) {
    type = global_cards_data[global_card_instances[player][card].original_internal_card_id].type;
  } else {
    type = global_cards_data[internal_card_id].type;
  }

  if ((type & TYPE_SORCERY) != 0) {
    illegal_abilities = 0x100000;
  }
  if ((type & TYPE_INSTANT) != 0) {
    illegal_abilities |= 0x40000;
  }
  if ((type & TYPE_INTERRUPT) != 0) {
    illegal_abilities |= 0x80000;
  }
  if ((type & TYPE_ENCHANTMENT) != 0) {
    illegal_abilities |= 0x20000;
  }
  if ((type & TYPE_ARTIFACT) != 0) {
    illegal_abilities |= 0x10000;
  }

  return (0x800 << (((char)get_color_from_color_test(global_card_instances[player][card].color) - 1U)
                    & 0x1f))
         | illegal_abilities;
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
  int choice;
  int *selected_ptr;
  int *available_array;

  (void)prompt;
  (void)a6;
  (void)a8;
  (void)a9;

  if (selected == 0 || count < 1 || graveyard == NULL || graveyard[0] == -1) {
    return 0;
  }

  selected_ptr = (int *)selected;
  available_array = (int *)available;
  if (((active_player == player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1) {
    *selected_ptr = FUN_004087cc(player, TYPE_CREATURE);
    return *selected_ptr != -1;
  }

  for (choice = 0; choice < count && graveyard[choice] != -1; ++choice) {
    if (available_array == NULL || available_array[choice] != 0) {
      *selected_ptr = choice;
      return 1;
    }
  }

  return 0;
}

int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  (void)player;
  (void)card;
  return orig_color_test;
}

// FUNCTION: MAGIC 0x005001e0
int FUN_005001e0(int player, int card, int value)
{
  char *mapping;

  mapping = (char *)&global_card_instances[player][card].counters;
  if (mapping[value] != 0) {
    value = mapping[value];
  }

  return value;
}

// FUNCTION: MAGIC 0x004b5f03
int FUN_004b5f03(int card_id)
{
  int internal_card_id;

  if (card_id != -1) {
    for (internal_card_id = 0; internal_card_id < unk_0093f4b8 + 0x10; ++internal_card_id) {
      if (global_cards_data[internal_card_id].id == card_id) {
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

  for (current_card = 0; current_card < count && graveyard[current_card] != -1; ++current_card) {
    if (allowed_cards[current_card] != 0) {
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

  if (player == -1) {
    return -1;
  }

  best_score = -999;
  best_slot = -1;

  for (current_slot = 0; current_slot < 500 && global_library[player][current_slot] != -1; ++current_slot) {
    current_internal_id = global_library[player][current_slot];
    if (type_mask == 0xffffffff || (type_mask & global_cards_data[current_internal_id].type) != 0) {
      current_score = global_cards_data[current_internal_id].power + global_cards_data[current_internal_id].toughness;
      if ((global_cards_data[current_internal_id].type & TYPE_ENCHANTMENT) != 0) {
        ++current_score;
      }
      if ((global_cards_data[current_internal_id].type & TYPE_ARTIFACT) != 0) {
        ++current_score;
      }
      abilities = global_cards_data[current_internal_id].static_ability;
      while (abilities != 0) {
        if ((abilities & 1) != 0) {
          ++current_score;
        }
        abilities >>= 1;
      }
      if ((global_cards_data[current_internal_id].extra_ability & 0x1000) != 0) {
        current_score += 2;
      }
      if ((global_cards_data[current_internal_id].extra_ability & 1) != 0) {
        current_score += 2;
      }
      if (best_score < current_score) {
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
  produced_mana_color = 0x3e;
  unk_00926664 = -1;

  if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0
      && (global_cards_data[internal_card_id].type & TYPE_LAND) != 0
      && (global_cards_data[internal_card_id].extra_ability & 0x1000) != 0) {
    dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
    if (spell_fizzled == 1) {
      spell_fizzled = -1;
      produced_mana_color = 1;
      dispatch_event_to_single_card(player, card, EVENT_ACTIVATE, 1 - player, -1);
      produced_mana_color = 0x3e;
    }
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) {
      dispatch_event(player, card, EVENT_TAP_CARD);
    }
  }

  produced_mana_color = 0;
  return 0;
}

// FUNCTION: MAGIC 0x00481e25
int FUN_00481e25(int player, int card, int event)
{
  int parent_player;
  int parent_card;

  parent_player = *((char *)&PLAYER_CARD_INSTANCE(player, card) + 8);
  parent_card = *((int *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0x44));

  if (event == 0x78
      && card == affected_card
      && player == affected_card_controller
      && (*((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x20) == attacking_card
          || *((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x20)
                 == *((char *)&PLAYER_CARD_INSTANCE(attacking_card_controller, attacking_card) + 0x20))) {
    event_result = 1;
  }

  FUN_00485060(player, card, FUN_004821f5, -1);

  if (trigger_condition == 0xdf
      && card == affected_card
      && player == affected_card_controller
      && current_turn == player
      && player != human_player
      && player == trigger_cause_controller
      && card == trigger_cause) {
    if (event == 0x7d) {
      event_result |= 2;
    }
    if (event == 0x7e) {
      unk_007abc78 |= 4;
      *((unsigned int *)((char *)&PLAYER_CARD_INSTANCE(parent_player, parent_card) + 0x14)) |= 0x8000000;
    }
  }

  if ((trigger_condition == 0xcc || event == 199)
      && card == affected_card
      && player == affected_card_controller
      && current_turn == player) {
    if (event == 0x7d) {
      event_result |= 2;
    }
    if (event == 0x7e || event == 199) {
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

  for (color = 0; color < 6; ++color) {
    card_colors[color] = 0;
    land_scores[color] = 0;
    mana_colors[color] = 0;
  }

  for (player = 0; player < 2; ++player) {
    if (only_player == -1 || player == only_player) {
      for (card = 0; card < active_cards_count[player]; ++card) {
        if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1) {
          if ((PLAYER_CARD_INSTANCE(player, card).state & 0x800022) == 2) {
            if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0) {
              for (color = 1; color < 6; ++color) {
                if (((int)(char)PLAYER_CARD_INSTANCE(player, card).mana_color
                     & (1 << (((unsigned char)color) & 0x1f))) != 0) {
                  ++mana_colors[color];
                }
              }
            }
            if ((PLAYER_CARD_INSTANCE(player, card).color & 0x3e) != 0) {
              for (color = 1; color < 6; ++color) {
                if ((PLAYER_CARD_INSTANCE(player, card).color & (1 << (((unsigned char)color) & 0x1f))) != 0) {
                  ++land_scores[color];
                }
              }
            }
          } else if (preferred_player == only_player || only_player == -1) {
            colors = (unsigned int)(char)global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color;
            for (color = 1; color < 6; ++color) {
              if ((colors & (1 << (((unsigned char)color) & 0x1f))) != 0) {
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
  for (color = 1; color < 6; ++color) {
    if (land_scores[best_land_color] < land_scores[color]) {
      best_land_color = color;
    }
    if (card_colors[best_card_color] < card_colors[color]) {
      best_card_color = color;
    }
  }

  best_score = -1;
  for (player = 0; player < 2; ++player) {
    if (only_player == -1 || player == only_player) {
      for (card = 0; card < active_cards_count[player]; ++card) {
        if (is_in_play(player, card)
            && (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_LAND) != 0) {
          current_score = 0;
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 1) != 0) {
            current_score = 2;
            if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0) {
              current_score = 5;
            }
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].extra_ability & 0x1000) != 0) {
            current_score += 1;
            colors = (unsigned int)(char)PLAYER_CARD_INSTANCE(player, card).mana_color;
            for (color = 1; color < 6; ++color) {
              if ((colors & (1 << (((unsigned char)color) & 0x1f))) != 0) {
                internal_card_id = 4 - mana_colors[color];
                if (internal_card_id < 1) {
                  internal_card_id = 0;
                }
                current_score += internal_card_id;
              }
            }
            if (best_land_color != 0 && (colors & (1 << (((unsigned char)best_land_color) & 0x1f))) != 0) {
              ++current_score;
            }
            if ((preferred_player == only_player || only_player == -1)
                && best_card_color != 0
                && (colors & (1 << (((unsigned char)best_card_color) & 0x1f))) != 0) {
              ++current_score;
            }
          }
          if ((global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) != 0) {
            ++current_score;
          }
          if (best_score < current_score) {
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

  if (*((char *)&PLAYER_CARD_INSTANCE(test_player, test_card) + 0x4c) == source_player
      && PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == source_card) {
    *((char *)&PLAYER_CARD_INSTANCE(test_player, test_card) + 0x4c) =
        *((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 8);
    PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card =
        *((int *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x44));
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

  if (unk_008a9000 != 1) {
    play_sound_effect(WAV_SHUFFLE);
  }

  deck_size = 0;
  while (deck_size < 500 && global_library[deck_owner][deck_size] != -1) {
    ++deck_size;
  }

  for (current_slot = 0; current_slot < deck_size; ++current_slot) {
    swap_slot = current_slot + FUN_00464a57(deck_size - current_slot);
    if (swap_slot < deck_size && global_library[deck_owner][swap_slot] != -1) {
      temp = global_library[deck_owner][swap_slot];
      global_library[deck_owner][swap_slot] = global_library[deck_owner][current_slot];
      global_library[deck_owner][current_slot] = temp;
    }
  }

  for (current_slot = deck_size; current_slot < 500; ++current_slot) {
    global_library[deck_owner][current_slot] = -1;
  }
}

// FUNCTION: MAGIC 0x004b5c94
void remove_card_from_deck(int player, int position)
{
  int current_slot;

  for (current_slot = position; current_slot < 499; ++current_slot) {
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
  if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0) {
    damage_creature(test_player, test_card, 1, source_player, source_card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dccd
int FUN_0054dccd(int player, int card)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
    if (current_card != card
        && is_in_play(player, current_card)
        && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE)
               != 0) {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005510dc
int FUN_005510dc(int player, unsigned char type_mask)
{
  int current_card;

  for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
    if (is_in_play(player, current_card)
        && (type_mask & global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type) != 0) {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b15f7
void FUN_004b15f7(int player, int graveyard_index)
{
  for (; graveyard_index < 499; ++graveyard_index) {
    global_graveyard_slots[player][graveyard_index] = global_graveyard_slots[player][graveyard_index + 1];
  }
  global_graveyard_slots[player][499] = -1;
}

// FUNCTION: MAGIC 0x004b42aa
int FUN_004b42aa(int player, int *graveyard, int count, char (*lines)[300], int a5, int *a6)
{
  int graveyard_index;

  (void)graveyard;
  (void)count;
  (void)lines;
  (void)a5;
  (void)a6;

  if (((active_player == player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1) {
    return FUN_004087cc(player, -1);
  }

  for (graveyard_index = 0; graveyard_index < 500 && global_graveyard_slots[player][graveyard_index] != -1;
       ++graveyard_index) {
    return graveyard_index;
  }

  return -1;
}

// FUNCTION: MAGIC 0x00485060
int FUN_00485060(int player, int card, in_play_card_callback_t callback, int who_to_check)
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

  while (test_player >= 0 && test_player < 2 && callback_result == 0) {
    if (who_to_check == -1 || test_player == who_to_check) {
      test_card = 0;
      while (test_card < active_cards_count[test_player] && callback_result == 0) {
        if (is_in_play(test_player, test_card)) {
          callback_result = callback(player,
                                     card,
                                     test_player,
                                     test_card,
                                     global_card_instances[test_player][test_card].internal_card_id);
        }
        ++test_card;
      }
    }

    if (player == 0) {
      ++test_player;
    } else {
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
  if (card_id == -1) {
    return 0;
  }
  if (player == -1 || card == -1) {
    return 0;
  }
  if (global_raw_cards_storage[card_id].num_pics < 2) {
    return 0;
  }

  return (player + card) % global_raw_cards_storage[card_id].num_pics;
}

// FUNCTION: MAGIC 0x0050047c
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card)
{
  card_instance_t *damage;
  int source_internal_card_id;
  int damage_player;
  int result;

  if (target_player == -1 || source_player == -1 || amount < 1) {
    return -1;
  }

  if (target_card == -1) {
    damage_player = target_player;
  } else {
    damage_player = source_player;
  }

  result = add_card_to_hand(damage_player, unk_009266a4);
  if (result != -1) {
    damage = &PLAYER_CARD_INSTANCE(damage_player, result);
    damage->state |= 2;
    if (damage_player == 0) {
      damage->state |= 0x1000;
    }
    damage->damage_target_player = (char)target_player;
    damage->damage_target_card = target_card;
    damage->info_slot = amount;
    damage->damage_source_player = (char)source_player;
    damage->damage_source_card = source_card;
    if (source_card == -1) {
      *(int *)((char *)damage + 0x64) = 0xef;
    } else {
      if (PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == -1
          || PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id == unk_0091a80c) {
        source_internal_card_id = *(int *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x3c);
      } else {
        source_internal_card_id = PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id;
      }
      *(unsigned char *)((char *)damage + 0x1e) =
          *(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x1e);
      if ((global_cards_data[source_internal_card_id].type & TYPE_INTERRUPT) != 0) {
        *(unsigned char *)((char *)damage + 0x1e) |= 0x40;
      }
      damage->eot_toughness = (unsigned int)global_cards_data[source_internal_card_id].type;
      if ((PLAYER_CARD_INSTANCE(source_player, source_card).state & 4) != 0) {
        if (current_phase == 0x19) {
          damage->token_status |= 0x100000;
        }
        if (current_phase == 0x1a) {
          damage->token_status |= 0x40000;
        }
      }
      if (*(int *)&global_cards_data[source_internal_card_id].id == unk_00789734
          || *(int *)&global_cards_data[source_internal_card_id].id == unk_008a8de8
          || *(int *)&global_cards_data[source_internal_card_id].id == unk_008cf1ac) {
        *(int *)((char *)damage + 0x64) = *(int *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x64);
      } else {
        *(int *)((char *)damage + 0x64) =
            FUN_004964dd(*(int *)&global_cards_data[source_internal_card_id].id, source_player, source_card) << 16
            | *(unsigned short *)&global_cards_data[source_internal_card_id].id;
      }
    }
    unk_008b4278 |= 2;
  }

  return result;
}

// FUNCTION: MAGIC 0x004eb23d
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
  if (player < 0 || player >= 2 || card < 0) {
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

  if (player < 0 || player >= 2 || card < 0 || card >= active_cards_count[player]) {
    return 0;
  }
  if (global_card_instances[player][card].internal_card_id == -1) {
    return 0;
  }

  code_pointer = global_cards_data[global_card_instances[player][card].internal_card_id].code_pointer;
  if (code_pointer == 0) {
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

  for (cards_drawn = 0; cards_drawn < amount; ++cards_drawn) {
    FUN_0043e18b(player);
    if (player != 0) {
      unk_007161d8 = 0;
    }
  }
  hand_count[player] = amount;
}

// FUNCTION: MAGIC 0x004a6968
void TENTATIVE_reassess_all_cards(void)
{
}

// FUNCTION: MAGIC 0x00445918
int TENTATIVE_set_timestamps(int player, int card)
{
  int slot;

  slot = 0;
  while (slot < 500 && TENTATIVE_timestamp_player[slot] != -1) {
    ++slot;
  }
  if (slot >= 500) {
    return -1;
  }

  TENTATIVE_timestamp_player[slot] = player;
  TENTATIVE_timestamp_card[slot] = card;
  global_card_instances[player][card].timestamp = slot;
  return slot;
}

// FUNCTION: MAGIC 0x005493a6
int FUN_005493a6(int player, int card, int internal_card_id)
{
  unsigned int *recalc_abilities_ptr;

  if (global_cards_data[internal_card_id].subtype == SUB_ZOMBIE) {
    recalc_abilities_ptr =
        (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x28);
    *recalc_abilities_ptr |= 0x8000000;
  }

  return 1;
}

// FUNCTION: MAGIC 0x0055117d
void FUN_0055117d(int (__cdecl *callback)(int, int, int), int who_to_check)
{
  int test_card;
  int test_player;

  for (test_player = 0; test_player < 2; ++test_player) {
    if (who_to_check == -1 || test_player == who_to_check) {
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card) {
        if (is_in_play(test_player, test_card)) {
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

  if (preferred_controller == -1) {
    preferred_controller = 2;
  }

  result = C_real_select_target(player,
                                2,
                                preferred_controller,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_CREATURE,
                                TYPE_NONE,
                                0,
                                FUN_0053aa74(player, card),
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
  if (result != 0) {
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

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        2,
                        0,
                        0,
                        FUN_0053aa74(player, card),
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

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (power_modifier + toughness_modifier < 0) {
      preferred_controller = 1 - player;
    } else {
      preferred_controller = player;
    }

    if (FUN_00551638(player, preferred_controller, card) == 0) {
      spell_fizzled = 1;
    } else {
      ai_modifier += -0x18;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL) {
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
                               FUN_0053aa74(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0)
        == 0) {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    } else {
      global_card_instances[player][card].damage_target_player =
          (char)global_card_instances[player][card].targets[0].player;
      global_card_instances[player][card].damage_target_card =
          global_card_instances[player][card].targets[0].card;
    }
    global_card_instances[player][card].number_of_targets = 0;
    return 0;
  }

  if (((event == EVENT_POWER)
       && (global_card_instances[player][card].damage_target_card == affected_card))
      && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller)
          && (affected_card != -1))) {
    if (is_in_play(player, card)) {
      event_result += power_modifier;
    }
  }
  if (((event == EVENT_TOUGHNESS)
       && (global_card_instances[player][card].damage_target_card == affected_card))
      && (((int)global_card_instances[player][card].damage_target_player == affected_card_controller)
          && (affected_card != -1))) {
    if (is_in_play(player, card)) {
      event_result += toughness_modifier;
    }
  }

  return 0;
}

int do_dialog(int who_chooses,
              int bigcard_player,
              int bigcard_card,
              int smallcard_player,
              int smallcard_card,
              const char *options,
              int ai_choice)
{
  (void)who_chooses;
  (void)bigcard_player;
  (void)bigcard_card;
  (void)smallcard_player;
  (void)smallcard_card;
  (void)options;
  return ai_choice;
}

// FUNCTION: MAGIC 0x004eab5c
void declare_mana_available_hex(int player, color_test_t colors, int amount)
{
  int i;

  if ((int)colors > 0) {
    for (i = 0; i < 0x32; ++i) {
      if (raw_mana_available_hex[player][i] == -1) {
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
  for (i = 0; i < 0x32 && raw_mana_available_hex[player][i] != -1; ++i) {
    if (raw_mana_available_hex[player][i] == target) {
      for (j = i; j < 0x31; ++j) {
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
  if (raw_mana_available[player][color] >= amount) {
    raw_mana_available[player][color] -= amount;
  } else {
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

  if ((available_colors & COLOR_TEST_WHITE) != 0) {
    return COLOR_WHITE;
  }
  if ((available_colors & COLOR_TEST_BLUE) != 0) {
    return COLOR_BLUE;
  }
  if ((available_colors & COLOR_TEST_BLACK) != 0) {
    return COLOR_BLACK;
  }
  if ((available_colors & COLOR_TEST_RED) != 0) {
    return COLOR_RED;
  }
  if ((available_colors & COLOR_TEST_GREEN) != 0) {
    return COLOR_GREEN;
  }

  return -1;
}

// FUNCTION: MAGIC 0x00464a57
int FUN_00464a57(int maximum)
{
  (void)maximum;
  return 0;
}

// FUNCTION: MAGIC 0x004e4f11
void FUN_004e4f11(void)
{
}

// FUNCTION: MAGIC 0x004e5089
void FUN_004e5089(void)
{
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
  (void)player;
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
  (void)player;
  (void)card;
  (void)event;
  (void)color;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x004b08e6
void kill_card(int player, int card, kill_t kill_mode)
{
  (void)player;
  (void)card;
  (void)kill_mode;
}

// FUNCTION: MAGIC 0x004ec830
void load_text(int file_name, const char *section_name)
{
  (void)file_name;
  (void)section_name;
  text_lines[0][0] = 0;
}

// FUNCTION: MAGIC 0x0043e668
void discard(int player, int flags, int player_who_controls_effect)
{
  (void)player;
  (void)flags;
  (void)player_who_controls_effect;
}

// FUNCTION: MAGIC 0x004b99d0
int mana_producer_sound_on_resolve(int player, int card, event_t event, color_t color)
{
  (void)player;
  (void)card;
  (void)event;
  (void)color;
  return 0;
}

// FUNCTION: MAGIC 0x004eae35
int produce_mana(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
  total_raw_mana_available[player] += amount;
  return raw_mana_available[player][color];
}

// FUNCTION: MAGIC 0x005058b1
int FUN_005058b1(int player, int card, event_t event, color_test_t available_colors)
{
  (void)player;
  (void)card;
  (void)event;
  (void)available_colors;
  return 0;
}

int select_target(int player, int card, target_definition_t *td, const char *prompt, target_t *ret_location)
{
  (void)player;
  (void)card;
  (void)td;
  (void)prompt;
  if (ret_location != NULL) {
    ret_location->player = -1;
    ret_location->card = -1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004c0a36
int FUN_004c0a36(int a1, int a2, int a3)
{
  if (a1 < a2) {
    return a2;
  }
  if (a1 > a3) {
    return a3;
  }
  return a1;
}

// FUNCTION: MAGIC 0x00432f00
int FUN_00432f00(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x004e4ff3
void FUN_004e4ff3(int a1)
{
  (void)a1;
}

// FUNCTION: MAGIC 0x004e503e
void FUN_004e503e(int a1)
{
  (void)a1;
}

// FUNCTION: MAGIC 0x004eaf09
int FUN_004eaf09(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x004eb23d
int FUN_004eb23d(int player, int card, unsigned int color, int amount)
{
  int color_index;

  if (amount == 0) {
    color_index = FUN_00442763(PLAYER_CARD_INSTANCE(player, card).color);
    if (unk_0072c440[color_index] < 1) {
      return 1;
    }
    return FUN_004eaf09(player, 7, unk_0072c440[color_index]);
  }

  if (FUN_004eaf09(player, color, amount) == 0) {
    return 0;
  }

  color_index = FUN_00442763(PLAYER_CARD_INSTANCE(player, card).color);
  if (unk_0072c440[color_index] > 0) {
    return FUN_004eaf09(player, 7, unk_0072c440[color_index] + amount);
  }

  return 1;
}

// FUNCTION: MAGIC 0x004ef850
int FUN_004ef850(int a1, int a2, int a3, int a4, int a5)
{
  (void)a1;
  (void)a2;
  (void)a3;
  (void)a4;
  (void)a5;
  return -1;
}

// FUNCTION: MAGIC 0x004b5cf5
void FUN_004b5cf5(int player, int internal_card_id)
{
  int deck_position;

  deck_position = 0;
  while (deck_position < 500 && global_library[player][deck_position] != -1) {
    ++deck_position;
  }
  if (deck_position < 500) {
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
  (void)player;
  (void)card;
  (void)event;
  (void)new_attacking_card;
  return 0;
}

// FUNCTION: MAGIC 0x00442763
int FUN_00442763(int color_test)
{
  if ((color_test & 2) != 0) {
    return 1;
  }
  if ((color_test & 4) != 0) {
    return 2;
  }
  if ((color_test & 8) != 0) {
    return 3;
  }
  if ((color_test & 0x10) != 0) {
    return 4;
  }
  if ((color_test & 0x20) != 0) {
    return 5;
  }
  return 0;
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

// FUNCTION: MAGIC 0x0041f4c0
int FUN_0041f4c0(int player, int card, event_t event, int color)
{
  card_instance_t* instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    if ((((instance->token_status & 3) == 0)
         || ((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0))
        && ((instance->state & 0x10) == 0)) {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE) {
    ai_modifier -= 0xc;
    undeclare_mana_available_and_produce_it(player, color, 1);
    instance->state |= 0x10;
    produced_mana_color = color;
  }

  if ((event == EVENT_COUNT_MANA) && (card == card_on_stack) && (player == card_on_stack_controller)
      && ((instance->state & 0x10) == 0)) {
    declare_mana_available(player, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00435e27
int FUN_00435e27(int player, int card, int color, int amount)
{
  int color_index;
  int result;

  color_index = FUN_00442763(PLAYER_CARD_INSTANCE(player, card).color);
  unk_008ce510 += unk_0072c440[color_index];
  result = FUN_00432f00(player, color, amount) - unk_0072c440[color_index];
  if (spell_fizzled == 1) {
    result = 0;
  }
  return result;
}

// FUNCTION: MAGIC 0x0044331f
void play_sound_effect(int sound_id)
{
  (void)sound_id;
}

// FUNCTION: MAGIC 0x004faee2
void FUN_004faee2(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  card_instance_t* instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  for (current_color = 1; current_color < 6; ++current_color) {
    if ((unsigned char)instance->hack_mode[current_color] == (unsigned char)color_from) {
      instance->hack_mode[current_color] = (char)color_to;
    }
  }
  if (instance->hack_mode[color_from] == 0) {
    instance->hack_mode[color_from] = (char)color_to;
  }
}

// FUNCTION: MAGIC 0x004fb8fd
void FUN_004fb8fd(int player, int card, int color_from, unsigned char color_to)
{
  int current_color;
  card_instance_t* instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  for (current_color = 1; current_color < 6; ++current_color) {
    if (instance->color_id[current_color] == (unsigned char)color_from) {
      instance->color_id[current_color] = color_to;
    }
  }
  if (instance->color_id[color_from] == 0) {
    instance->color_id[color_from] = color_to;
  }
}

// FUNCTION: MAGIC 0x00437375
int FUN_00437375(int player, int card, int internal_card_id)
{
  card_instance_t* aura_being_cast;
  card_instance_t* instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  aura_being_cast = &PLAYER_CARD_INSTANCE(card_on_stack_controller, card_on_stack);
  if (instance->damage_target_player == aura_being_cast->damage_target_player
      && instance->damage_target_card == aura_being_cast->damage_target_card
      && (global_cards_data[internal_card_id].type & TYPE_ENCHANTMENT) != 0
      && (card != card_on_stack || player != card_on_stack_controller)) {
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
  bank = FUN_00464a57(2);
  for (current_card = 0; current_card < active_cards_count[who_is_being_divided]; ++current_card) {
    if (is_in_play(who_is_being_divided, current_card)
        && (global_cards_data[PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).internal_card_id].type
            & TYPE_CREATURE) != 0
        && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0x24) & 0x20)
            == 0)) {
      legacy_card = FUN_004ef850(player, card, unk_008b3bd4, who_is_being_divided, current_card);
      if (legacy_card != -1) {
        if (power_total[1 - bank] < power_total[bank]) {
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
  card_instance_t* instance;
  int result;
  int saved_affected_card;
  int saved_affected_card_controller;
  int saved_event_result;
  int saved_spell_fizzled;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (((global_cards_data[instance->internal_card_id].subtype == 0)
       && ((*(unsigned char *)((char *)instance + 0x19) & 8) == 0))
      || (((global_cards_data[instance->internal_card_id].type & TYPE_CREATURE) == 0)
          && ((*(unsigned char *)((char *)instance + 0xb) & 1) == 0))
      || ((instance->state & 0x810010) != 0)
      || ((*(unsigned char *)((char *)instance + 0x19) & 0x80) != 0)) {
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
  if (!result) {
    return 0;
  }

  return dispatch_event(player, card, EVENT_ATTACK_LEGALITY) == 0;
}

// FUNCTION: MAGIC 0x004f7783
int FUN_004f7783(int player, int card)
{
  card_instance_t* instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if ((instance->state & STATE_TAPPED) == 0) {
    instance->state |= STATE_TAPPED;
    if ((global_cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0) {
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
  card_instance_t* instance;
  keyword_t illegal_abilities;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        TARGET_ZONE_IN_PLAY,
                        TYPE_CREATURE,
                        TYPE_NONE,
                        0,
                        FUN_0053aa74(player, card),
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
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller)) {
    spell_fizzled = !FUN_00551638(player, player, card);
  }
  if (event == EVENT_RESOLVE_SPELL) {
    illegal_abilities = FUN_0053aa74(player, card);
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
                                0)) {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    } else {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
    }
    instance->number_of_targets = 0;
  }
  if (instance->damage_target_card == card_on_stack
      && instance->damage_target_player == card_on_stack_controller
      && card_on_stack != -1
      && (instance->state & 0x20) == 0
      && event == 0x34) {
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
  if (current_data == source_data
      && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x17) & 1) != 0)) {
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
  if (current_data == source_data
      && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(source_player, source_card) + 0x17) & 1) == 0)) {
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

  if (count <= 0) {
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
  (void)player;
  (void)card;
}

// FUNCTION: MAGIC 0x0054ac4d
void FUN_0054ac4d(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
}

// FUNCTION: MAGIC 0x0054af10
int FUN_0054af10(int player, int card, event_t event, int amount)
{
  (void)player;
  (void)card;
  (void)event;
  (void)amount;
  return 0;
}

// FUNCTION: MAGIC 0x0054e470
void FUN_0054e470(int player, int card, int color)
{
  (void)player;
  (void)card;
  (void)color;
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

  if (player == -1 || flags == 0) {
    found = 0;
  } else {
    test_player = 0;
    found = 0;

    while (test_player < 2 && found == 0) {
      test_card = 0;
      while (test_card < active_cards_count[test_player] && found == 0) {
        if (global_card_instances[test_player][test_card].internal_card_id == unk_009266a4
            && is_in_play(test_player, test_card)
            && global_card_instances[test_player][test_card].damage_target_player == player
            && global_card_instances[test_player][test_card].damage_target_card == card
            && global_card_instances[test_player][test_card].info_slot != 0) {
          source_player = (int)global_card_instances[test_player][test_card].damage_source_player;
          source_card = global_card_instances[test_player][test_card].damage_source_card;

          if (global_card_instances[source_player][source_card].internal_card_id != -1
              && *(int *)&global_cards_data[global_card_instances[source_player][source_card]
                                               .internal_card_id]
                               .id
                     == unk_008a8de8) {
            source_player =
                global_card_instances[(char)global_card_instances[source_player][source_card]
                                                .damage_source_player][source_card]
                    .damage_source_card;
          }

          if (global_card_instances[source_player][source_card].internal_card_id == -1) {
            source_internal_card_id =
                global_card_instances[source_player][source_card].original_internal_card_id;
          } else {
            source_internal_card_id = global_card_instances[source_player][source_card].internal_card_id;
          }

          if ((flags & global_cards_data[source_internal_card_id].type) != 0) {
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

// FUNCTION: MAGIC 0x004c0080
int C_real_select_target(int who_chooses,
                         int allowed_controller,
                         int preferred_controller,
                         int zone,
                         int required_type,
                         int illegal_type,
                         unsigned int required_abilities,
                         unsigned int illegal_abilities,
                         unsigned int required_color,
                         unsigned int illegal_color,
                         int extra,
                         int required_subtype,
                         int power_requirement,
                         int toughness_requirement,
                         unsigned int special,
                         unsigned int required_state,
                         unsigned int illegal_state,
                         char *prompt,
                         int allow_cancel,
                         target_t *ret_tgt)
{
  (void)who_chooses;
  (void)allowed_controller;
  (void)preferred_controller;
  (void)zone;
  (void)required_type;
  (void)illegal_type;
  (void)required_abilities;
  (void)illegal_abilities;
  (void)required_color;
  (void)illegal_color;
  (void)extra;
  (void)required_subtype;
  (void)power_requirement;
  (void)toughness_requirement;
  (void)special;
  (void)required_state;
  (void)illegal_state;
  (void)prompt;
  (void)allow_cancel;
  if (ret_tgt != NULL) {
    ret_tgt->player = -1;
    ret_tgt->card = -1;
  }
  return 0;
}

// FUNCTION: MAGIC 0x00551b60
int FUN_00551b60(int player, unsigned int preferred_controller, int card)
{
  int result;
  unsigned char target_number;
  target_t selected_target;

  if (preferred_controller == -1) {
    preferred_controller = 2;
  }

  result = C_real_select_target(player,
                                2,
                                preferred_controller,
                                TARGET_ZONE_IN_PLAY,
                                TYPE_LAND,
                                TYPE_NONE,
                                0,
                                FUN_0053aa74(player, card),
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
  if (result != 0) {
    target_number = global_card_instances[player][card].number_of_targets;
    global_card_instances[player][card].targets[target_number] = selected_target;
    ++global_card_instances[player][card].number_of_targets;
  }

  return result != 0;
}

// FUNCTION: MAGIC 0x004bd7d0
int FUN_004bd7d0(int *param_1,
                 int param_2,
                 int param_3,
                 unsigned int param_4,
                 unsigned int param_5,
                 unsigned int param_6,
                 unsigned int param_7,
                 unsigned int param_8,
                 unsigned int param_9,
                 unsigned int param_10,
                 unsigned int param_11,
                 unsigned int param_12,
                 int param_13,
                 int param_14,
                 unsigned int param_15,
                 unsigned int param_16,
                 unsigned int param_17,
                 unsigned int param_18,
                 unsigned int param_19)
{
  (void)param_2;
  (void)param_3;
  (void)param_4;
  (void)param_5;
  (void)param_6;
  (void)param_7;
  (void)param_8;
  (void)param_9;
  (void)param_10;
  (void)param_11;
  (void)param_12;
  (void)param_13;
  (void)param_14;
  (void)param_15;
  (void)param_16;
  (void)param_17;
  (void)param_18;
  (void)param_19;
  if (param_1 != NULL) {
    *param_1 = 0;
  }
  return 0;
}

// FUNCTION: MAGIC 0x004bdc06
unsigned int __stdcall C_real_validate_target(int tgt_player,
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

  if (tgt_player == -1) {
    if (return_error_str != NULL) {
      strcpy(return_error_str, empty_error);
    }
    local_8 = 0;
  } else if (tgt_card == -1 || global_card_instances[tgt_player][tgt_card].internal_card_id != -1) {
    bVar20 = 0;
    strcpy(local_d0, empty_error);

    if (tgt_card == -1) {
      if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0) {
        if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0))
             && ((allowed_controller & ANYBODY) != 0))
            || (who_chooses == AI && ((preferred_controller & ANYBODY) != 0))) {
          local_d8 = AI;
          local_dc = 1;
        } else if ((((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0))
                    && ((allowed_controller & HUMAN) != 0))
                   || (who_chooses == AI && ((preferred_controller & HUMAN) != 0))) {
          local_d8 = HUMAN;
          local_dc = 1;
        }
      } else {
        local_d8 = HUMAN;
        local_dc = 0;
      }

      if ((tgt_player == HUMAN && local_d8 == 0) || (tgt_player == AI && local_dc == 0)) {
        bVar20 = 1;
        strcat(local_d0, error_player);
      }
    } else {
      instance = &global_card_instances[tgt_player][tgt_card];

      if ((unk_008b35ec == who_chooses) || ((unk_00926804 & 2) != 0)) {
        if ((allowed_controller & TARGET_PLAYER_OWNER) == 0) {
          if ((allowed_controller & ANYBODY) == 0) {
            if ((allowed_controller & HUMAN) == 0) {
              local_e4 = HUMAN;
            } else {
              local_e4 = AI;
            }
          } else {
            local_e4 = -1;
          }

          if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER) {
            local_e0 = local_e4;
          } else {
            local_e0 = -1;
          }
        } else {
          if ((allowed_controller & ANYBODY) == 0) {
            if ((allowed_controller & HUMAN) == 0) {
              local_e0 = HUMAN;
            } else {
              local_e0 = AI;
            }
          } else {
            local_e0 = -1;
          }

          if ((allowed_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER) {
            local_e4 = local_e0;
          } else {
            local_e4 = -1;
          }
        }
      } else if ((preferred_controller & TARGET_PLAYER_OWNER) == 0) {
        if ((preferred_controller & ANYBODY) == 0) {
          if ((preferred_controller & HUMAN) == 0) {
            local_e4 = HUMAN;
          } else {
            local_e4 = AI;
          }
        } else {
          local_e4 = -1;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER) {
          local_e0 = local_e4;
        } else {
          local_e0 = -1;
        }
      } else {
        if ((preferred_controller & ANYBODY) == 0) {
          if ((preferred_controller & HUMAN) == 0) {
            local_e0 = HUMAN;
          } else {
            local_e0 = AI;
          }
        } else {
          local_e0 = -1;
        }

        if ((preferred_controller & TARGET_PLAYER_OWNER_AND_CONTROLLER) == TARGET_PLAYER_OWNER_AND_CONTROLLER) {
          local_e4 = local_e0;
        } else {
          local_e4 = -1;
        }
      }

      bVar19 = (instance->state & STATE_OUBLIETTED) != 0;
      if (bVar19) {
        strcat(local_d0, error_target);
      }

      if ((instance->state & STATE_CANNOT_TARGET) != 0) {
        bVar20 = 1;
        strcat(local_d0, error_target);
      }

      bVar20 = bVar20 || bVar19;
      if (zone != 0
          && (((instance->state & STATE_IN_PLAY) != 0 && (zone & TARGET_ZONE_IN_PLAY) == 0)
              || ((instance->state & STATE_IN_PLAY) == 0 && (zone & TARGET_ZONE_HAND) == 0))) {
        bVar20 = 1;
        strcat(local_d0, error_zone);
      }

      if (local_e4 != -1 && tgt_player != local_e4) {
        bVar20 = 1;
        strcat(local_d0, error_controller);
      }

      if (local_e0 != -1) {
        local_114 = (instance->state & STATE_OWNED_BY_OPPONENT) != 0 ? 1 - tgt_player : tgt_player;
        if (local_114 != local_e0) {
          bVar20 = 1;
          strcat(local_d0, error_owner);
        }
      }

      if (required_type != TYPE_NONE) {
        if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0) {
          local_e8 = instance->internal_card_id;
        } else {
          local_e8 = instance->original_internal_card_id;
        }

        bVar19 = 0;
        if (((required_type & global_cards_data[local_e8].type) != TYPE_NONE)
            || ((required_type & TARGET_TYPE_NONCREATURE_CAN_BLOCK) != TYPE_NONE
                && (instance->state & STATE_NONCREATURE_CAN_BLOCK) != 0)) {
          bVar19 = 1;
        }
        if ((required_type & TYPE_EFFECT) != TYPE_NONE
            && (*(int *)&global_cards_data[local_e8].id == unk_00789734
                || *(int *)&global_cards_data[instance->internal_card_id].id == unk_00789b80)) {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (instance->token_status & STATUS_TOKEN) != 0) {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_e8].id == unk_007a7d64) {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_e8].id == unk_008a8de8) {
          bVar19 = 1;
        }
        if ((required_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_e8].id == unk_009266ac) {
          bVar19 = 1;
        }
        if (!bVar19) {
          bVar20 = 1;
          strcat(local_d0, error_type);
        }
      }

      if (illegal_type != TYPE_NONE) {
        if ((special & TARGET_SPECIAL_USE_ORIGINAL_TYPE) == 0) {
          local_f0 = instance->internal_card_id;
        } else {
          local_f0 = instance->original_internal_card_id;
        }

        bVar19 = (illegal_type & global_cards_data[local_f0].type) != TYPE_NONE;
        if ((illegal_type & TYPE_EFFECT) != TYPE_NONE
            && (*(int *)&global_cards_data[local_f0].id == unk_00789734
                || *(int *)&global_cards_data[local_f0].id == unk_00789b80)) {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_TOKEN) != TYPE_NONE && (instance->token_status & STATUS_TOKEN) != 0) {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_DAMAGE_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_f0].id == unk_007a7d64) {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_HACK_SLEIGHT_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_f0].id == unk_008a8de8) {
          bVar19 = 1;
        }
        if ((illegal_type & TARGET_TYPE_DRAW_CARD_LEGACY) != TYPE_NONE
            && *(int *)&global_cards_data[local_f0].id == unk_009266ac) {
          bVar19 = 1;
        }
        if (bVar19) {
          bVar20 = 1;
          strcat(local_d0, error_type);
        }
      }

      if (required_abilities != 0 && (instance->regen_status & required_abilities) != required_abilities) {
        bVar20 = 1;
        strcat(local_d0, error_ability);
      }
      if (illegal_abilities != 0 && (instance->regen_status & illegal_abilities) != 0) {
        bVar20 = 1;
        strcat(local_d0, error_ability);
      }
      if (required_color != COLOR_TEST_0 && (required_color & instance->color) == COLOR_TEST_0) {
        bVar20 = 1;
        strcat(local_d0, error_color);
      }
      if (illegal_color != COLOR_TEST_0 && (illegal_color & instance->color) != COLOR_TEST_0) {
        bVar20 = 1;
        strcat(local_d0, error_color);
      }

      if (extra != -1) {
        if ((special & TARGET_SPECIAL_NOT_LAND_SUBTYPE) == 0) {
          if ((special & TARGET_SPECIAL_BASIC_LAND) == 0) {
            if (extra < 5) {
              if (FUN_0048463d(tgt_player, tgt_card, extra + 1) == 0) {
                bVar20 = 1;
                strcat(local_d0, error_extra);
              }
            } else if (instance->internal_card_id != extra
                       && *(int *)&global_cards_data[extra].id != *(int *)&global_cards_data[instance->internal_card_id].id) {
              bVar20 = 1;
              strcat(local_d0, error_extra);
            }
          } else if (FUN_004c0880(instance->internal_card_id, extra) == 0) {
            bVar20 = 1;
            strcat(local_d0, error_extra);
          }
        } else if (FUN_00483489(tgt_player, tgt_card, extra) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_extra);
        }
      }

      if (required_subtype != ~SUB_WALL) {
        if ((special & TARGET_SPECIAL_0x10) == 0) {
          local_f8 = required_subtype;
        } else if (required_subtype == (SUB_DJINN | SUB_MERFOLK)) {
          local_f8 = SUB_RAT;
        } else if (required_subtype == SUB_RAT) {
          local_f8 = SUB_DJINN | SUB_MERFOLK;
        } else if (required_subtype == SUB_ENCHANT_WORLD) {
          local_f8 = SUB_ENCHANT_WORLD | SUB_MERFOLK;
        } else if (required_subtype == (SUB_ENCHANT_WORLD | SUB_MERFOLK)) {
          local_f8 = SUB_ENCHANT_WORLD;
        } else if (required_subtype == 0x49) {
          local_f8 = 0x4a;
        } else if (required_subtype == 0x4a) {
          local_f8 = 0x49;
        } else if (required_subtype == 0x56) {
          local_f8 = 0x57;
        } else if (required_subtype == 0x57) {
          local_f8 = 0x56;
        } else if (required_subtype == 0x59) {
          local_f8 = 0x5a;
        } else if (required_subtype == 0x5a) {
          local_f8 = 0x59;
        } else if (required_subtype == 0x69) {
          local_f8 = 0x6a;
        } else if (required_subtype == 0x6a) {
          local_f8 = 0x69;
        } else {
          local_f8 = required_subtype;
        }

        local_114 = global_cards_data[instance->internal_card_id].id;
        if ((subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != required_subtype
            && (subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != local_f8) {
          bVar20 = 1;
          strcat(local_d0, error_subtype);
        }
      }

      if (power_requirement != -1) {
        uVar17 = power_requirement & TARGET_PT_MASK;
        uVar18 = power_requirement & 0xf000;
        if ((uVar18 == 0 && instance->power != (int)uVar17)
            || (uVar18 == TARGET_PT_GREATER_OR_EQUAL && instance->power < (int)uVar17)
            || (uVar18 == TARGET_PT_LESSER_OR_EQUAL && instance->power > (int)uVar17)) {
          bVar20 = 1;
          strcat(local_d0, error_power);
        }
      }

      if (toughness_requirement != -1) {
        uVar17 = toughness_requirement & TARGET_PT_MASK;
        uVar18 = toughness_requirement & 0xf000;
        if ((uVar18 == 0 && instance->toughness != (int)uVar17)
            || (uVar18 == TARGET_PT_GREATER_OR_EQUAL && instance->toughness < (int)uVar17)
            || (uVar18 == TARGET_PT_LESSER_OR_EQUAL && instance->toughness > (int)uVar17)) {
          bVar20 = 1;
          strcat(local_d0, error_toughness);
        }
      }

      if (special != 0) {
        local_114 = global_cards_data[instance->internal_card_id].id;
        if ((special & TARGET_SPECIAL_WALL) != 0
            && (subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype != 0xc5
            && global_cards_data[instance->internal_card_id].subtype != 0) {
          bVar20 = 1;
          strcat(local_d0, error_wall);
        }
        if ((special & TARGET_SPECIAL_NON_WALL) != 0
            && ((subtype_in_card_data_t)global_raw_cards_storage[local_114].subtype == 0xc5
                || global_cards_data[instance->internal_card_id].subtype == 0)) {
          bVar20 = 1;
          strcat(local_d0, error_wall);
        }
        if ((special & TARGET_SPECIAL_SPELL_ON_STACK) != 0
            && ((unk_008ce508 == -1 || tgt_player != unk_008ce508)
                || unk_008ce4f4 != tgt_card
                || (instance->state & STATE_JUST_CAST) != 0)) {
          bVar20 = 1;
          strcat(local_d0, error_stack);
        }
        if ((special & TARGET_SPECIAL_BASIC_LAND) != 0 && FUN_004c081a(tgt_player, tgt_card) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_basic_land);
        }
        if ((special & TARGET_SPECIAL_ARTIFACT_CREATURE) != 0
            && (global_cards_data[instance->internal_card_id].type & (TYPE_ARTIFACT | TYPE_CREATURE))
                   != (TYPE_ARTIFACT | TYPE_CREATURE)) {
          bVar20 = 1;
          strcat(local_d0, error_artifact_creature);
        }
        if ((special & TARGET_SPECIAL_DAMAGE_PLAYER) != 0
            && (instance->damage_source_player != who_chooses || instance->damage_target_card != -1)) {
          bVar20 = 1;
          strcat(local_d0, error_damage_player);
        }
        if ((special & TARGET_SPECIAL_DJINN_OR_EFREET) != 0
            && global_cards_data[instance->internal_card_id].subtype != 6
            && global_cards_data[instance->internal_card_id].subtype != 5) {
          bVar20 = 1;
          strcat(local_d0, error_djinn_efreet);
        }
      }

      if (required_state != 0) {
        if ((required_state & TARGET_STATE_TAPPED) != 0 && (instance->state & STATE_TAPPED) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_tapped);
        }
        if ((required_state & TARGET_STATE_ATTACKING) != 0 && (instance->state & STATE_ATTACKING) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_attacking);
        }
        if ((required_state & TARGET_STATE_ATTACKED) != 0 && (instance->state & STATE_ATTACKED) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_attacked);
        }
        if ((required_state & TARGET_STATE_ISBLOCKED) != 0 && (instance->state & STATE_ISBLOCKED) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_blocked);
        }
        if ((required_state & TARGET_STATE_BLOCKING) != 0
            && (tgt_player == human_player || instance->damage_target_player == -1)) {
          bVar20 = 1;
          strcat(local_d0, error_blocking);
        }
        if ((required_state & TARGET_STATE_IN_COMBAT) != 0) {
          bVar19 = 0;
          if (current_phase < PHASE_DECLARE_ATTACKERS || current_phase > 0x1d || tgt_player == human_player) {
            bVar19 = 1;
          } else if (tgt_player == human_player || instance->damage_target_player == -1) {
            bVar19 = 1;
          }
          if ((instance->state & STATE_ATTACKING) == 0 && bVar19) {
            bVar20 = 1;
            strcat(local_d0, error_in_combat);
          }
        }
        if ((required_state & TARGET_STATE_ENCHANTED) != 0) {
          bVar19 = 0;
          for (local_114 = 0; local_114 < 2; ++local_114) {
            for (local_118 = 0; local_118 < active_cards_count[local_114]; ++local_118) {
              test_instance = &global_card_instances[local_114][local_118];
              if (test_instance->internal_card_id != -1
                  && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0
                  && test_instance->damage_target_player == tgt_player
                  && test_instance->damage_target_card == tgt_card) {
                bVar19 = 1;
              }
            }
          }
          if (!bVar19) {
            bVar20 = 1;
            strcat(local_d0, error_enchanted);
          }
        }
        if ((required_state & TARGET_STATE_JUST_CAST) != 0 && (instance->state & STATE_JUST_CAST) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_just_cast);
        }
        if ((required_state & TARGET_STATE_SPELL_RESOLVED) != 0
            && ((instance->state & STATE_JUST_CAST) == 0 || (instance->state & STATE_INVISIBLE) != 0)) {
          bVar20 = 1;
          strcat(local_d0, error_spell_resolved);
        }
        if ((required_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_damaged);
        }
        if ((required_state & TARGET_STATE_COULD_UNTAP) != 0
            && (instance->untap_status & UNTAP_STATUS_COULD_UNTAP) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_could_untap);
        }
        if ((required_state & TARGET_STATE_WILL_UNTAP) != 0
            && (instance->untap_status & UNTAP_STATUS_WILL_UNTAP) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_will_untap);
        }
        if ((required_state & TARGET_STATE_SUMMONING_SICK) != 0 && (instance->state & 0x30000) == 0x30000) {
          bVar20 = 1;
          strcat(local_d0, error_summoning_sickness);
        }
      }

      if (illegal_state != 0) {
        if ((illegal_state & TARGET_STATE_TAPPED) != 0 && (instance->state & STATE_TAPPED) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_tapped);
        }
        if ((((illegal_state & TARGET_STATE_ATTACKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0))
            && (instance->state & STATE_ATTACKING) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_attacking);
        }
        if ((illegal_state & TARGET_STATE_ATTACKED) != 0 && (instance->state & STATE_ATTACKED) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_attacked);
        }
        if ((illegal_state & TARGET_STATE_ISBLOCKED) != 0 && (instance->state & STATE_ISBLOCKED) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_blocked);
        }
        if ((((illegal_state & TARGET_STATE_BLOCKING) != 0) || ((illegal_state & TARGET_STATE_IN_COMBAT) != 0))
            && instance->damage_target_player != -1 && tgt_player != human_player) {
          bVar20 = 1;
          strcat(local_d0, error_blocking);
        }
        if ((illegal_state & TARGET_STATE_ENCHANTED) != 0) {
          bVar19 = 0;
          for (local_120 = 0; local_120 < 2; ++local_120) {
            for (local_124 = 0; local_124 < active_cards_count[local_120]; ++local_124) {
              test_instance = &global_card_instances[local_120][local_124];
              if (test_instance->internal_card_id != -1
                  && (global_cards_data[test_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0
                  && test_instance->damage_target_player == tgt_player
                  && test_instance->damage_target_card == tgt_card) {
                bVar19 = 1;
              }
            }
          }
          if (bVar19) {
            bVar20 = 1;
            strcat(local_d0, error_enchanted);
          }
        }
        if ((illegal_state & TARGET_STATE_JUST_CAST) != 0 && (instance->state & STATE_JUST_CAST) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_just_cast);
        }
        if ((illegal_state & TARGET_STATE_SPELL_RESOLVED) != 0
            && (instance->state & STATE_JUST_CAST) != 0
            && (instance->state & STATE_INVISIBLE) == 0) {
          bVar20 = 1;
          strcat(local_d0, error_spell_resolved);
        }
        if ((illegal_state & TARGET_STATE_DAMAGED) != 0 && FUN_004bff5a(tgt_player, tgt_card) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_damaged);
        }
        if ((illegal_state & TARGET_STATE_COULD_UNTAP) != 0
            && (instance->untap_status & UNTAP_STATUS_COULD_UNTAP) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_could_untap);
        }
        if ((illegal_state & TARGET_STATE_WILL_UNTAP) != 0
            && (instance->untap_status & UNTAP_STATUS_WILL_UNTAP) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_will_untap);
        }
        if ((illegal_state & TARGET_STATE_SUMMONING_SICK) != 0 && (instance->state & 0x30000) != 0) {
          bVar20 = 1;
          strcat(local_d0, error_summoning_sickness);
        }
      }
    }

    local_8 = !bVar20;
    if (return_error_str != NULL) {
      if (local_d0[0] == '\0') {
        *return_error_str = '\0';
      } else {
        strcpy(return_error_str, local_d0 + 1);
      }
    }
  } else {
    if (return_error_str != NULL) {
      strcpy(return_error_str, empty_error);
    }
    local_8 = 0;
  }

  return local_8;
}

// FUNCTION: MAGIC 0x004e51bb
void FUN_004e51bb(void)
{
  if (unk_00743094 < 1) {
    unk_00743094 = 0;
  } else {
    --unk_00743094;
  }
}

// FUNCTION: MAGIC 0x00551240
void add_special_counter(int player, int card)
{
  if (global_card_instances[player][card].special_counters != 0xff) {
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
  if (amount > 0xff) {
    amount = 0xff;
  }

  global_card_instances[player][card].special_counters = (unsigned char)amount;
}

// FUNCTION: MAGIC 0x005515f5
unsigned int C_get_special_counters(int player, int card)
{
  return global_card_instances[player][card].special_counters;
}

// FUNCTION: MAGIC 0x00419667
void FUN_00419667(int target_player, int target_card, int damage_target_player)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player) {
    for (current_card = 0; current_card < active_cards_count[current_player]; ++current_card) {
      if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == unk_009266a4
          && (PLAYER_CARD_INSTANCE(current_player, current_card).state & 0x800002) == 2
          && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player
          && PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card) {
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
int FUN_00441514(int player, int card)
{
  return (PLAYER_CARD_INSTANCE(player, card).state & 0x2000) != 0;
}

// FUNCTION: MAGIC 0x00443ee2
int FUN_00443ee2(int player, int card, int event, int extra, int prompt)
{
  card_instance_t *stack_card_instance;
  card_instance_t *original_instance;
  int stack_card;
  int original_display_pic_num;

  if (unk_008b2934 < 32) {
    unk_00939180[unk_008b2934] = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
    unk_00939180[unk_008b2934] |= event << 16;
    unk_00939180[unk_008b2934] |= extra << 24;
    if (event == EVENT_RESOLVE_SPELL || event == EVENT_RESOLVE_TRIGGER
        || PLAYER_CARD_INSTANCE(player, card).internal_card_id < 5) {
      stack_card = card;
    } else {
      stack_card = add_card_to_hand(player, unk_0091a80c);
      if (stack_card == -1) {
        return 0;
      }

      stack_card_instance = &PLAYER_CARD_INSTANCE(player, stack_card);
      original_instance = &PLAYER_CARD_INSTANCE(player, card);
      original_display_pic_num = *(int *)((char *)stack_card_instance + 0x54);
      memcpy(stack_card_instance, original_instance, sizeof(card_instance_t));
      stack_card_instance->internal_card_id = unk_0091a80c;
      *(int *)((char *)stack_card_instance + 0x14) = 0;
      stack_card_instance->kill_code = 0;
      if (original_instance->internal_card_id == -1) {
        *(int *)((char *)stack_card_instance + 0x3c) = *(int *)((char *)original_instance + 0x3c);
      } else {
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
    if (trigger_condition == -1) {
      unk_00895030[unk_008b2934] = current_phase;
    } else {
      unk_00895030[unk_008b2934] = trigger_condition;
    }
    if (unk_008a9000 != 1) {
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

  if ((int)color_to_produce > 0 && color_to_consume > 0) {
    for (slot = 0; slot < 10; ++slot) {
      if (unk_007161e0[player][slot] == -1) {
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

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player) {
    FUN_00551572(player, card, amount);
  }

  if (trigger_condition == 0xcc && (special_counters = C_get_special_counters(player, card)) != 0
      && affected_card == card && affected_card_controller == player && player == current_turn
      && ((PLAYER_CARD_INSTANCE(player, card).state & 4) != 0
          || (PLAYER_CARD_INSTANCE(player, card).untap_status != -1 && human_player != player))) {
    if (event == EVENT_TRIGGER) {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER) {
      FUN_00551334(player, card);
    }
  }

  if (event == EVENT_TOUGHNESS && affected_card == card && affected_card_controller == player) {
    event_result += C_get_special_counters(player, card);
  }

  old_max_x_value = max_x_value;
  if (event == EVENT_CAN_ACTIVATE && current_phase == PHASE_MAIN1 && human_player == player
      && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && unk_00742f60 == player) {
    special_counters = C_get_special_counters(player, card);
    if ((int)special_counters < amount && has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0)) {
      if (active_player == player && (unk_00926804 & 2) == 0
          && *(short *)((char *)&PLAYER_CARD_INSTANCE(player, card) + 0xe) < life[player]) {
        unk_008b3270 |= 3;
      }
      return 1;
    }
  } else if (event == EVENT_GET_SELECTED_CARD) {
    special_counters = C_get_special_counters(player, card);
    unk_00715fa8 = FUN_004c0a36(amount - special_counters, 0, has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0));
  } else {
    if (event == EVENT_ACTIVATE && affected_card == card && affected_card_controller == player) {
      special_counters = C_get_special_counters(player, card);
      max_x_value = amount - special_counters;
      if (unk_008b35ec == player || (unk_00926804 & 2) != 0) {
        PLAYER_CARD_INSTANCE(player, card).info_slot = charge_mana_for_activated_ability(player, card, -1, 0, 0, 0, 0, 0);
      } else {
        PLAYER_CARD_INSTANCE(player, card).info_slot =
            charge_mana_for_activated_ability(player, card, unk_00715fa8, 0, 0, 0, 0, 0);
      }
      max_x_value = old_max_x_value;
      if (spell_fizzled == 1) {
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      } else {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    if (event == EVENT_RESOLVE_ACTIVATION
        && PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
                   .internal_card_id != -1) {
      int new_counter_total;

      new_counter_total =
          FUN_004c0a36(PLAYER_CARD_INSTANCE(player, card).info_slot
                           + C_get_special_counters(card_on_stack_controller, card_on_stack),
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
  if (unk_008b2934 > 0) {
    --unk_008b2934;
    if (unk_0091a80c
        == PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
               .internal_card_id) {
      PLAYER_CARD_INSTANCE(global_stack_cards[unk_008b2934].player, global_stack_cards[unk_008b2934].card)
          .internal_card_id = -1;
    }
    global_stack_cards[unk_008b2934].player = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0041626b
// FUNCTION: SHANDALAR 0x00518a81
int FUN_0041626b(int player, int card, int event, int color)
{
  int target_color;

  if (event == EVENT_SHOULD_AI_PLAY && affected_card == card && affected_card_controller == player) {
    ai_modifier += basiclandtypes_controlled[active_player][color] * 0xc;
  }

  if (trigger_condition == 0xd3 && affected_card == card && affected_card_controller == player && player == current_turn
      && (((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
          || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE))
      && is_in_play(player, card) && has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0)) {
    target_color = FUN_0050026d(player, card, color);
    if (((1 << ((unsigned char)target_color & 0x1f))
         & (unsigned int)(unsigned char)PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).color) != 0
        && PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id != -1
        && global_cards_data[PLAYER_CARD_INSTANCE(trigger_cause_controller, trigger_cause).internal_card_id].type
               != TYPE_LAND) {
      if (event == EVENT_TRIGGER) {
        if (active_player == player && (unk_00926804 & 2) == 0) {
          event_result |= 2;
        } else {
          event_result |= 1;
        }
      }
      if (event == EVENT_RESOLVE_TRIGGER) {
        FUN_00443ee2(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        FUN_00432f00(player, 0, 1);
        obliterate_top_card_of_stack();
        if (spell_fizzled != 1) {
          dispatch_event(player, card, EVENT_PLAY_ABILITY);
          gain_life(player, 1);
          if (active_player == player) {
            ai_modifier -= 0x18;
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0050026d
int FUN_0050026d(int player, int card, int value)
{
  if (*((char *)&PLAYER_CARD_INSTANCE(player, card).token_status + value) != 0) {
    value = *((char *)&PLAYER_CARD_INSTANCE(player, card).token_status + value);
  }

  return value;
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

  if (player == -1) {
    return -1;
  }

  if (mode == 1) {
    best_card = -1;
    best_score = -10;
    count = 0;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
      if (is_in_play(player, current_card)
          && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_LAND)
          && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED)) {
        candidate_cards[count] = current_card;
        candidate_scores[count] = 0;
        ++count;
      }
    }

    for (current_card = 0; current_card < count; ++current_card) {
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability
           & 1)
          != 0) {
        ++candidate_scores[current_card];
      }
      if ((PLAYER_CARD_INSTANCE(player, current_card).token_status & 0x400) != 0) {
        candidate_scores[current_card] = -1;
      }
    }

    for (current_card = 0; current_card < count; ++current_card) {
      if (best_score < candidate_scores[current_card]) {
        best_score = candidate_scores[current_card];
        best_card = candidate_cards[current_card];
      }
    }

    return best_card;
  }

  if (mode == 2) {
    best_card = -1;
    best_score = -1;
    best_power = 0;
    best_toughness = 0;
    count = 0;

    for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
      if (is_in_play(player, current_card)
          && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE)
          && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED)) {
        candidate_cards[count] = current_card;
        candidate_powers[count] = C_get_abilities(player, current_card, EVENT_POWER, -1);
        if (best_power < candidate_powers[count]) {
          best_power = candidate_powers[count];
        }
        candidate_toughnesses[count] = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        if (best_toughness < candidate_toughnesses[count]) {
          best_toughness = candidate_toughnesses[count];
        }
        candidate_abilities[count] = C_get_abilities(player, current_card, EVENT_ABILITIES, -1);
        candidate_scores[count] = 0;
        ++count;
      }
    }

    for (current_card = 0; current_card < count; ++current_card) {
      if (candidate_powers[current_card] == best_power) {
        candidate_scores[current_card] += 3;
      }
      if (candidate_toughnesses[current_card] == best_toughness) {
        candidate_scores[current_card] += 2;
      }
      if ((candidate_abilities[current_card] & 0x20) != 0) {
        ++candidate_scores[current_card];
      }
      if ((candidate_abilities[current_card] & 0x100) != 0) {
        ++candidate_scores[current_card];
      }
      for (abilities = candidate_abilities[current_card]; abilities != 0; abilities >>= 1) {
        if ((abilities & 1) != 0) {
          ++candidate_scores[current_card];
        }
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability
           & 0x1000)
          != 0) {
        ++candidate_scores[current_card];
      }
      if ((global_cards_data[PLAYER_CARD_INSTANCE(player, candidate_cards[current_card]).internal_card_id].extra_ability
           & 1)
          != 0) {
        ++candidate_scores[current_card];
      }
    }

    for (current_card = 0; current_card < count; ++current_card) {
      if (best_score < candidate_scores[current_card]) {
        best_score = candidate_scores[current_card];
        best_card = candidate_cards[current_card];
      }
    }

    return best_card;
  }

  if (mode == 0x40) {
    best_score = -1;
    best_card = -1;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
      if (is_in_play(player, current_card)
          && (PLAYER_CARD_INSTANCE(player, current_card).state & STATE_TAPPED)
          && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_ENCHANTMENT)
          && (PLAYER_CARD_INSTANCE(player, current_card).dummy3 & 1) != 0) {
        current_internal_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
        power = C_get_abilities(player, current_card, EVENT_POWER, -1);
        toughness = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        current_score = power + toughness;
        abilities = C_get_abilities(player, current_card, EVENT_ABILITIES, -1);
        if ((abilities & 0x20) != 0) {
          ++current_score;
        }
        if ((abilities & 0x100) != 0) {
          ++current_score;
        }
        while (abilities != 0) {
          if ((abilities & 1) != 0) {
            ++current_score;
          }
          abilities >>= 1;
        }
        if ((global_cards_data[current_internal_id].extra_ability & 0x1000) != 0) {
          ++current_score;
        }
        if ((global_cards_data[current_internal_id].extra_ability & 1) != 0) {
          ++current_score;
        }
        if (global_cards_data[current_internal_id].cc[1] != 0xff) {
          current_score += (char)global_cards_data[current_internal_id].cc[1];
        }
        if (best_score < current_score) {
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

  if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_LAND, TYPE_NONE, 0, 0,
                            COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                            text_lines[0], 0, &target)) {
    return 0;
  }

  if (unk_008a9000 != 1) {
    play_sound_effect(WAV_SACRFICE);
  }
  kill_card(target.player, target.card, KILL_SACRIFICE);
  return 1;
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
  card_instance_t* instance;

  if ((player == unk_008b35ec || (unk_00926804 & 2) != 0) && unk_008a9000 != 1) {
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, 0,
                              COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL, -1, -1, 0, 0, 0,
                              text_lines[0], 0, &target)) {
      best_card = -1;
    } else {
      best_card = target.card;
    }
  } else {
    best_card = -1;
    best_value = 0x7fff;
    for (current_card = 0; current_card < active_cards_count[player]; ++current_card) {
      instance = &PLAYER_CARD_INSTANCE(player, current_card);
      internal_card_id = instance->internal_card_id;
      if (internal_card_id != -1
          && (instance->state & 0x800002) == 2
          && (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0
          && instance->kill_code != 3
          && (instance->state & 0x1000) == 0) {
        power = C_get_abilities(player, current_card, EVENT_POWER, -1);
        toughness = C_get_abilities(player, current_card, EVENT_TOUGHNESS, -1);
        card_value = (power + 2) * (toughness + 2);
        if (card_value < best_value) {
          best_value = card_value;
          best_card = current_card;
        }
      }
    }
  }

  if (best_card != -1 && unk_008a9000 != 1) {
    play_sound_effect(WAV_SACRFICE);
  }
  return best_card;
}

// FUNCTION: MAGIC 0x00466e6d
int FUN_00466e6d(int player, int card, int target_player)
{
  card_instance_t* instance;
  int land_type;
  int preferred_basic;
  int smallest_count;
  int current_type;
  int current_card;
  int found;

  found = 0;
  land_type = FUN_005001e0(player, card, 1);
  if (target_player == -1) {
    target_player = 1 - player;
  }

  preferred_basic = -1;
  smallest_count = 999;
  for (current_type = 1; current_type < 6; ++current_type) {
    if (current_type != land_type
        && basiclandtypes_controlled[target_player][current_type] != 0
        && basiclandtypes_controlled[target_player][current_type] < smallest_count) {
      smallest_count = basiclandtypes_controlled[target_player][current_type];
      preferred_basic = current_type - 1;
    }
  }

  for (current_card = 0; current_card < active_cards_count[target_player] && !found; ++current_card) {
    instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
    if (is_in_play(target_player, current_card)
        && (global_cards_data[instance->internal_card_id].type & TYPE_LAND) != 0) {
      if (preferred_basic != -1 && global_cards_data[instance->internal_card_id].subtype == preferred_basic) {
        preferred_basic = -1;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target_player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = current_card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      } else {
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
  card_instance_t* aura;
  card_instance_t* instance;
  card_instance_t* source;
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

  for (current_card = 0; current_card < active_cards_count[target_player]; ++current_card) {
    instance = &PLAYER_CARD_INSTANCE(target_player, current_card);
    if (is_in_play(target_player, current_card)
        && instance->internal_card_id == unk_008cefb8
        && instance->damage_target_player == target_player
        && instance->damage_target_card == target_card) {
      source = &PLAYER_CARD_INSTANCE(instance->damage_source_player, instance->damage_source_card);
      if ((source->state & STATE_TAPPED) == 0) {
        found_legacy = 1;
        saved_player = instance->targets[0].player;
        saved_card = instance->targets[0].card;
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->info_slot = 1;
      }
    }
  }

  for (test_player = 0; test_player < 2; ++test_player) {
    for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card) {
      instance = &PLAYER_CARD_INSTANCE(test_player, current_card);
      if (is_in_play(test_player, current_card)
          && instance->internal_card_id == unk_008d0340
          && instance->damage_target_player == target_player
          && instance->damage_target_card == target_card
          && (instance->token_status & 0x1000000) != 0) {
        if (!found_legacy) {
          instance->token_status &= ~0x1000000;
        }
        if (saved_player == -1) {
          aura->damage_source_player = test_player;
          aura->damage_source_card = current_card;
        } else {
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
  card_instance_t* instance;
  card_instance_t* callback_instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  if (internal_card_id == unk_008d0340
      && instance->damage_source_player == affected_card_controller
      && instance->damage_source_card == affected_card) {
    callback_instance = &PLAYER_CARD_INSTANCE(affected_card_controller, affected_card);
    instance->damage_source_player = callback_instance->damage_source_player;
    instance->damage_source_card = callback_instance->damage_source_card;
  }
  return 0;
}

// FUNCTION: MAGIC 0x0051bcf0
int FUN_0051bcf0(int player, int card, event_t event, unsigned int required_type)
{
  card_instance_t* instance;
  target_t target;
  int new_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int*)0, 0, player, 2, 2, 0x200, required_type, 0, 0,
                        FUN_0053aa74(player, card), 0, 0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player) {
    if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                              FUN_0053aa74(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                              ~SUB_WALL, -1, -1, 0, 0, 0, text_lines[0], 1, &target)) {
      spell_fizzled = 1;
    } else {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL) {
    if (!C_real_validate_target(instance->targets[0].player, instance->targets[0].card, (char*)0,
                                player, 2, 2, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                                FUN_0053aa74(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
                                ~SUB_WALL, -1, -1, 0, 0, 0)) {
      kill_card(player, card, KILL_BURY);
      spell_fizzled = 1;
    } else {
      instance->damage_target_player = instance->targets[0].player;
      instance->damage_target_card = instance->targets[0].card;
      if (FUN_004823a5(player, card)) {
        instance->token_status |= 0x1000000;
        if (instance->targets[0].player != player) {
          if (unk_008a9000 != 1) {
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


extern int unk_008b2938;
extern int unk_008b293c;

// FUNCTION: MAGIC 0x004a67bd
int choose_a_number(int player, const char* prompt, int maxnum)
{
  player = player;
  prompt = prompt;
  return maxnum;
}

// FUNCTION: MAGIC 0x0051c8c0
int gain_control(int player, int card)
{
  card_instance_t* instance;
  int opponent;
  int timestamp_slot;
  int new_card;
  int test_player;
  int test_card;
  int target_index;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  opponent = 1 - player;
  timestamp_slot = *(int*)((char*)instance + 0x50);
  new_card = add_card_to_hand(opponent, instance->internal_card_id);
  if (new_card != -1) {
    memcpy(global_card_instances[opponent] + new_card, global_card_instances[player] + card, 300);
    PLAYER_CARD_INSTANCE(opponent, new_card).state |= 0x30000;
    PLAYER_CARD_INSTANCE(opponent, new_card).state &= 0xfffffff3;
    TENTATIVE_timestamp_player[timestamp_slot] = opponent;
    TENTATIVE_timestamp_card[timestamp_slot] = new_card;

    for (test_player = 0; test_player < 2; ++test_player) {
      for (test_card = 0; test_card < active_cards_count[test_player]; ++test_card) {
        if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == player
            && PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == card) {
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player = opponent;
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card = new_card;
        }
        if (PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_player == player
            && PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_card == card) {
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_player = opponent;
          PLAYER_CARD_INSTANCE(test_player, test_card).damage_source_card = new_card;
        }
        if (PLAYER_CARD_INSTANCE(test_player, test_card).number_of_targets != 0) {
          for (target_index = 0; target_index < PLAYER_CARD_INSTANCE(test_player, test_card).number_of_targets; ++target_index) {
            if (PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].player == player
                && PLAYER_CARD_INSTANCE(test_player, test_card).targets[target_index].card == card) {
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
  if ((unk_007abc78 & 0x10000000) != 0) {
    dispatch_event(opponent, new_card, EVENT_CARDCONTROLLED);
  }
  return new_card;
}






// FUNCTION: MAGIC 0x004b5d72
void real_put_on_top_of_deck(int player, int internal_card_id)
{
  int index;

  for (index = 499; index > 0; --index) {
    global_library[player][index] = global_library[player][index - 1];
  }
  global_library[player][0] = internal_card_id;
}

// FUNCTION: MAGIC 0x005180ed
int FUN_005180ed(int a1, int a2, int player, int card, int internal_card_id)
{
  if (internal_card_id == unk_0093933c
      && PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller
      && PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card) {
    event_result = 1;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0051819c
int FUN_0051819c(int parent_player, int parent_card, int player, int card, int internal_card_id)
{
  if (internal_card_id == unk_0093933c
      && PLAYER_CARD_INSTANCE(player, card).damage_source_player == parent_player
      && PLAYER_CARD_INSTANCE(player, card).blocking == parent_card) {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}
