#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cardartlib/src/assert.h"
#include "game_support.h"

typedef int (__cdecl *in_play_card_callback_t)(int source_player,
                                               int source_card,
                                               int player,
                                               int card,
                                               int internal_card_id);

extern card_ptr_t global_raw_cards_storage[2000];
extern int __stdcall PostMessageA(void* hwnd, unsigned int msg, unsigned int wparam, int lparam);
extern void __stdcall ExitThread(unsigned int exit_code);
extern void __stdcall EnterCriticalSection(void *critical_section);
extern void __stdcall LeaveCriticalSection(void *critical_section);
extern int __stdcall LoadIconA(int hinstance, const char *icon_name);
extern int __stdcall LoadCursorA(int hinstance, const char *cursor_name);
extern unsigned short __stdcall RegisterClassA(void *wndclass);
extern long __stdcall DialogBoxParamA(int hinstance, const char *template_name, int parent, void *dialog_proc, long init_param);

int can_target(target_definition_t *td)
{
  return td != NULL;
}

// FUNCTION: MAGIC 0x00551ed7
int FUN_00551ed7(int player, unsigned int preferred_controller, int card)
{
  target_t selected_target;

  if (preferred_controller == -1) {
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
                                &selected_target)) {

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

// FUNCTION: MAGIC 0x00449bef
void FUN_00449bef(char *name)
{
  char *source;

  if (name == 0) {
    return;
  }

  source = unk_009266d0;
  while (*source != '\0' && *source != '-') {
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
  return player >= 0
         && player < 2
         && card >= 0
         && card < active_cards_count[player]
         && global_card_instances[player][card].internal_card_id != -1
         && (global_card_instances[player][card].state & F08_INPLAY) != 0;
}

// FUNCTION: MAGIC 0x005001e0
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

// FUNCTION: MOK 0x004358f0
// FUNCTION: MAGIC 0x00442763
color_t single_color_test_bit_to_color_t(color_test_t color_test)
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
unsigned int get_protections_from(int player, int card)
{
  unsigned char type;
  keyword_t illegal_abilities;
  int internal_card_id;

  illegal_abilities = 0;
  internal_card_id = global_card_instances[player][card].internal_card_id;
  if (internal_card_id == -1 || internal_card_id == unk_0091a80c) {
    type = global_cards_data[global_card_instances[player][card].original_internal_card_id].type;
  } else {
    type = global_cards_data[internal_card_id].type;
  }

  if ((type & TYPE_SORCERY) != 0) {
    illegal_abilities = KEYWORD_PROT_SORCERIES;
  }
  if ((type & TYPE_INSTANT) != 0) {
    illegal_abilities |= KEYWORD_PROT_INSTANTS;
  }
  if ((type & TYPE_INTERRUPT) != 0) {
    illegal_abilities |= KEYWORD_PROT_INTERRUPTS;
  }
  if ((type & TYPE_ENCHANTMENT) != 0) {
    illegal_abilities |= KEYWORD_PROT_ENCHANTMENTS;
  }
  if ((type & TYPE_ARTIFACT) != 0) {
    illegal_abilities |= KEYWORD_PROT_ARTIFACTS;
  }

  return (0x800 << (((char)single_color_test_bit_to_color_t(global_card_instances[player][card].color) - 1U)
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
  if (unk_008a9000 == 1) {
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

  dispatch_function_to_all_cards_in_play(player, card, FUN_004821f5, -1);

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
    swap_slot = current_slot + internal_rand(deck_size - current_slot);
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

// FUNCTION: MOK 0x0047936d
// FUNCTION: MAGIC 0x004b42aa
int show_deck(int player, int *cards, int count, void *context, int suppress_done_txt, char *prompt)
{
  struct {
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

  if (((active_player == player) && ((unk_00926804 & 2) != 0)) && unk_00742fc4 != 0) {
    TENTATIVE_wait_for_network_result(player, 0x19);
    s.result = unk_007a7d0c;
  } else if (((active_player == player) && ((unk_00926804 & 2) == 0)) || unk_008a9000 == 1) {
    s.valid_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8) {
      if (cards[s.local_8] != -1) {
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
      }
    }

    unk_00939340 = internal_rand(s.valid_count);
    if (unk_008b35ec != player) {
      if (unk_008a9000 == 1) {
        FUN_004e4f11();
      } else {
        FUN_004e5089();
      }
    }

    s.result = s.valid_graveyards[unk_00939340];
  } else if (unk_00742fc4 == 0) {
    ReadPalette(unk_00573364, (char *)0);
    FUN_004e1c81(0, 0);
    IAT_SelectPalette(unk_0074309c, unk_007463dc, 0);
    FUN_004e1da6(unk_00573370);
    FUN_004e1d91(1, 0, 0, unk_00573380, &unk_007462a0);
    FUN_004e1d6d(unk_0057a75c, 0, 0, 0x280, 0x1e0, unk_007497b0, 0, 0, unk_0057d9f0, unk_0057d9f4);

    s.display_count = 0;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8) {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8])) {
        ++s.display_count;
      }
    }

    s.valid_count = (s.display_count - 1) / 5;
    if (s.valid_count == 0) {
      s.valid_count = 1;
    }

    s.x = 0x60;
    s.valid_count = 0;
    s.y = 0x10;
    for (s.local_8 = 0; s.local_8 < count; ++s.local_8) {
      if (cards[s.local_8] != -1 && (s.local_8 == 0 || cards[s.local_8 - 1] != cards[s.local_8])) {
        s.x_positions[s.valid_count] = s.x + 4;
        s.y_positions[s.valid_count] = s.y;
        s.valid_graveyards[s.valid_count] = s.local_8;
        ++s.valid_count;
        s.x += 0x38;
        if (s.x > 0x13f) {
          s.x = 0x60;
          s.y += 0x54 / s.display_count;
        }
      }
    }

    if (s.valid_count > 0) {
      s.result = s.valid_graveyards[0];
    } else {
      s.result = -1;
    }
  } else {
    s.result = FUN_004a61fe(cards, count, context, suppress_done_txt, prompt);
    if ((unk_008b35ec == player) && ((unk_00926804 & 2) != 0)) {
      unk_007a7d08 = 0x19;
      unk_007a7d0c = s.result;
      TENTATIVE_send_network_result(player, 0x19);
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0044a3bf
char *FUN_0044a3bf(int player, int card)
{
  struct {
    int transformed_csvid;
    char temporary_name[52];
    int source_card_data;
    int source_player;
    unsigned int choice;
    char *card_name;
    unsigned int type;
    int csvid;
  } locals;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1) {
    locals.type = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  } else {
    locals.type = PLAYER_CARD_INSTANCE(player, card).original_internal_card_id;
  }

  locals.csvid = CardIDFromType(locals.type);
  locals.card_name = unk_0056e49c;
  if (locals.csvid != -1) {
    if (locals.csvid == unk_0092666c) {
      locals.csvid = CardIDFromType(FUN_004a583e(player, card));
    }

    locals.choice = (unsigned int)PLAYER_CARD_INSTANCE(player, card).display_pic_csv_id;
    locals.source_player = (int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
    locals.source_card_data = PLAYER_CARD_INSTANCE(player, card).damage_source_card;

    if (locals.csvid == unk_007a7d64) {
      strcpy(unk_00637670, unk_008cfd30);
    } else if (locals.csvid == unk_00789b80) {
      sprintf(unk_00637670, unk_00926750, FUN_00495311(PLAYER_CARD_INSTANCE(player, card).info_slot));
    } else if (locals.csvid == unk_008cf1ac) {
      strcpy(unk_00637670, unk_008cf040);
    } else if (locals.csvid == unk_00789734) {
      strcpy(unk_00637670, unk_00777e64[locals.choice].name_at_0);
    } else if (locals.csvid == unk_008a8de8) {
      strcpy(unk_00637670, unk_00777e64[locals.choice].name_at_8);
    } else if (locals.csvid == unk_009266ac) {
      strcpy(unk_00637670, unk_008b4330);
    } else {
      unk_00637670[0] = '\0';
    }

    if (locals.csvid == unk_00789734 && 0 < (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness) {
      strcpy(locals.temporary_name, unk_00637670);
      FUN_0055d802(unk_00637670, locals.temporary_name, PLAYER_CARD_INSTANCE(player, card).eot_toughness);
    }

    locals.transformed_csvid = FUN_004a58ba(locals.source_player, locals.source_card_data);
    if (locals.transformed_csvid == 0x361 || locals.transformed_csvid == 0x360) {
      strcpy(unk_00637670, unk_00777e64[locals.transformed_csvid].name_at_0);
    }

    if (unk_00637670[0] == '\0') {
      locals.card_name = global_raw_cards_storage[locals.csvid].full_name;
    } else {
      locals.card_name = unk_00637670;
    }
  }

  return locals.card_name;
}

// FUNCTION: MAGIC 0x0044a796
void FUN_0044a796(int player, int card)
{
  char *card_name;

  card_name = FUN_0044a3bf(player, card);
  if (card_name != NULL) {
    strcat(unk_00748770, card_name);
  }
}

// FUNCTION: MAGIC 0x004483be
int FUN_004483be(int player, int card)
{
  if (player != 0 && player != 1) {
    return 1;
  }

  if (card < 0 || card > 0x96) {
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004487d8
int FUN_004487d8(int player, int card)
{
  int internal_card_id;

  if (FUN_004483be(player, card) != 0) {
    return -1;
  }

  EnterCriticalSection((void *)&unk_00789110);
  internal_card_id = global_displayed_card_instances[player][card].internal_card_id;
  LeaveCriticalSection((void *)&unk_00789110);
  return internal_card_id;
}

// FUNCTION: MAGIC 0x00448857
int FUN_00448857(int player, int card)
{
  int internal_card_id;

  if (FUN_004483be(player, card) != 0) {
    return -1;
  }

  internal_card_id = FUN_004487d8(player, card);
  if (internal_card_id == -1) {
    return -1;
  }

  return *(int *)&global_cards_data[internal_card_id].id;
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

// FUNCTION: MAGIC 0x0055b9f0
void FUN_0055b9f0(int dc, int *rect, int value)
{
  (void)dc;
  (void)rect;
  (void)value;
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
                               get_protections_from(player, card),
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

// FUNCTION: MAGIC 0x004a63b8
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

  if (maximum > 1) {
    result = rand() % maximum;
  } else {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x004e4f11
void FUN_004e4f11(void)
{
  if (unk_00743094 < 0x100) {
    unk_006a21b8[unk_00743094] = unk_00925bb8;
    unk_006a8258[unk_00743094] = PLAYER_CARD_INSTANCE((unk_00925bb8 & 0x100) >> 8, unk_00925bb8 & 0xff).internal_card_id;
    unk_006a8660[unk_00743094] = unk_0057aae8;
    unk_006a6ee8[unk_00743094] = unk_00939340;
    ++unk_00743094;
    if (unk_006a6ee8[0] == 99 || unk_006a6ae8[0] == 99) {
      unk_00925bb8 = -1;
    }
  } else {
    spell_fizzled = 1;
  }

  unk_0057aae8 = 0;
}

// FUNCTION: MAGIC 0x004e5089
void FUN_004e5089(void)
{
  if (unk_006a5f18[unk_00743094] != unk_0057aae8) {
    unk_0057aae8 |= 0x100;
  }

  unk_00925bb8 = unk_006a1db8[unk_00743094];
  unk_00939340 = unk_006a6ae8[unk_00743094];
  if (unk_00939340 != 99) {
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
int load_text(const char * file_name, const char *section_name)
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
  //if (!strchr(file_name, '.'))
  //  strcat(s.path, ".txt");

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

// FUNCTION: MOK 0x004a9310
// FUNCTION: MAGIC 0x005058b1
int tap_for_multicolor_mana(int player, int card, event_t event, color_test_t available_colors)
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

// FUNCTION: MOK 0x0042de60
// FUNCTION: MAGIC 0x00432f00
int charge_mana(int player, color_t color, int amount)
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

// FUNCTION: MOK 0x00499010
// FUNCTION: MAGIC 0x004e503e
void FUN_004e503e(int a1)
{
  (void)a1;
}

// FUNCTION: MOK 0x0049d510
// FUNCTION: MAGIC 0x004eaf09
int has_mana(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
  return 0;
}

// FUNCTION: MOK 0x0049d710
// FUNCTION: MAGIC 0x004eb23d
int has_mana_w_global_cost_mod(int player, int card, color_t color, int amount)
{
  int result;

  result = 0;

  if (amount == 0) {
    if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0) {
      result = has_mana(
          player, 7, unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)]);
    } else {
      result = 1;
    }
  } else {
    result = has_mana(player, color, amount);
    if (result != 0) {
      if (unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] > 0) {
        result = has_mana(
            player,
            7,
            unk_0072c440[single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color)] + amount);
      }
    }
  }

  return result;
}

// FUNCTION: MOK 0x004a09a0
// FUNCTION: MAGIC 0x004ef850
int create_legacy_effect(int player,int card,int legacy_iid,int target_player,int target_card)
{
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

// FUNCTION: MOK 0x004302c0
// FUNCTION: MAGIC 0x00435e27
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount)
{
  int color_index;
  int result;

  color_index = single_color_test_bit_to_color_t(PLAYER_CARD_INSTANCE(player, card).color);
  unk_008ce510 += unk_0072c440[color_index];
  result = charge_mana(player, color, amount) - unk_0072c440[color_index];
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
  bank = internal_rand(2);
  for (current_card = 0; current_card < active_cards_count[who_is_being_divided]; ++current_card) {
    if (is_in_play(who_is_being_divided, current_card)
        && (global_cards_data[PLAYER_CARD_INSTANCE(who_is_being_divided, current_card).internal_card_id].type
            & TYPE_CREATURE) != 0
        && ((*(unsigned char *)((char *)&PLAYER_CARD_INSTANCE(who_is_being_divided, current_card) + 0x24) & 0x20)
            == 0)) {
      legacy_card = create_legacy_effect(player, card, unk_008b3bd4, who_is_being_divided, current_card);
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
  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller)) {
    spell_fizzled = !FUN_00551638(player, player, card);
  }
  if (event == EVENT_RESOLVE_SPELL) {
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

// FUNCTION: MAGIC 0x0055d802
void FUN_0055d802(char *out, char *in, int choice)
{
  struct {
    char *write_ptr;
    int done;
    int matched;
    char *scan;
  } s;

  if (out == NULL || in == NULL) {
    return;
  }

  s.done = 0;
  s.matched = 0;
  s.scan = in;
  while (s.done == 0) {
    while (*s.scan != '\0' && strncmp(s.scan, unk_0057f758, 2) != 0) {
      ++s.scan;
    }

    if (*s.scan == '\0') {
      s.done = 1;
    } else {
      ++s.scan;
      ++s.scan;
      if (atoi(s.scan) == choice) {
        s.matched = 1;
        ++s.scan;
        s.write_ptr = out;
        while (*s.scan != '\0' && strncmp(s.scan, unk_0057f75c, 2) != 0) {
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
  if (value < 0 || value >= 0xd4) {
    return unk_005724ac;
  }

  return unk_00926930[value];
}

// FUNCTION: MAGIC 0x0049ebde
long FUN_0049ebde(int dialog_window, unsigned int message, int wparam_dc, int *lparam_data)
{
  unsigned int command;
  int result;

  (void)dialog_window;
  (void)lparam_data;

  switch (message) {
    case 0x10:
    case 0x201:
      return 1;

    case 0x14:
      return 1;

    case 0x100:
      return 1;

    case 0x111:
      command = (unsigned int)wparam_dc & 0xffff;
      if (command == 1 || command == 2 || command >= 10) {
        return 1;
      }
      return 1;

    case 0x110:
    case 0x115:
      return 0;
  }

  if (message > 0x30e && message < 0x312) {
    result = 0;
  } else {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0049fe68
long FUN_0049fe68(int card_window, unsigned int message, int wparam_window, long *lparam_data)
{
  (void)card_window;
  (void)wparam_window;
  (void)lparam_data;

  if (message < 0x10) {
    if (message == 1) {
      return 0;
    }
  } else if (message < 0x101) {
    if (message == 0x100) {
      return 0;
    }
    if (message == 0x87) {
      return 4;
    }
  } else {
    if (message < 0x312) {
      if (message == 0x201) {
        return 0;
      }
      if (message > 0x30e) {
        return 0;
      }
    } else {
      if (message == 0x414) {
        return 0;
      }
      if (message == 0x437) {
        return 0;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0049fd0c
void FUN_0049fd0c(int *brush1, int *pen1, int *pen2, int *pen3, int *brush2, int *text_color)
{
  if (brush1 != NULL) {
    *brush1 = 2;
  }
  if (pen1 != NULL) {
    *pen1 = 6;
  }
  if (pen2 != NULL) {
    *pen2 = 6;
  }
  if (pen3 != NULL) {
    *pen3 = 7;
  }
  if (brush2 != NULL) {
    *brush2 = 2;
  }
  if (text_color != NULL) {
    *text_color = 0x1000090;
  }
}

// FUNCTION: MAGIC 0x0049fdf9
void FUN_0049fdf9(int brush1, int pen1, int pen2, int pen3, int brush2)
{
  (void)brush1;
  (void)pen1;
  (void)pen2;
  (void)pen3;
  (void)brush2;
}

// FUNCTION: MAGIC 0x004a61fe
int FUN_004a61fe(int *cards, int count, void *context, unsigned int big_card_mode, char *prompt)
{
  if (unk_008a9000 == 1) {
    return 1;
  }

  return FUN_0049e6aa(cards, 0, 0, count, context, big_card_mode, prompt);
}

// FUNCTION: MAGIC 0x0049e6aa
int FUN_0049e6aa(int *graveyard,
                 int *alternate_csvids,
                 int *available,
                 int count,
                 void *context,
                 unsigned int big_card_mode,
                 char *prompt)
{
  typedef struct {
    void *context;
    int displayed_csvids[500];
    int available_cards[500];
    int item_count;
    unsigned int copy_alternate_csvids;
    int show_bigcard;
    char title[12];
    int index;
  } dialog_box_data_t;
  typedef struct {
    unsigned int style;
    void *window_proc;
    int class_extra_bytes;
    int window_extra_bytes;
    int hinstance;
    int hicon;
    int hcursor;
    int background_brush;
    char *menu_name;
    char *class_name;
  } local_wndclass_t;
  dialog_box_data_t locals;
  local_wndclass_t wndclass;

  if (count < 1 || graveyard == NULL || graveyard[0] == -1) {
    return -1;
  }

  wndclass.style = 0;
  wndclass.window_proc = FUN_0049fe68;
  wndclass.class_extra_bytes = 0;
  wndclass.window_extra_bytes = unk_0055e0cc;
  wndclass.hinstance = unk_00925030;
  wndclass.hicon = LoadIconA(0, (const char *)0x7f00);
  wndclass.hcursor = LoadCursorA(0, (const char *)0x7f00);
  wndclass.background_brush = 6;
  wndclass.menu_name = 0;
  wndclass.class_name = unk_00572920;
  RegisterClassA(&wndclass);

  locals.context = context;
  for (locals.index = 0; locals.index < count && graveyard[locals.index] != -1; ++locals.index) {
    locals.displayed_csvids[locals.index] = CardIDFromType(graveyard[locals.index] & 0xfff);
  }

  locals.item_count = locals.index;
  locals.copy_alternate_csvids = alternate_csvids != 0;
  if (locals.copy_alternate_csvids != 0) {
    for (locals.index = 0; locals.index < locals.item_count; ++locals.index) {
      locals.displayed_csvids[locals.index] = alternate_csvids[locals.index];
    }
  }

  for (locals.index = 0; locals.index < locals.item_count; ++locals.index) {
    if (available == 0) {
      locals.available_cards[locals.index] = 1;
    } else {
      locals.available_cards[locals.index] = available[locals.index];
    }
  }

  locals.show_bigcard = big_card_mode;
  if (big_card_mode == 0) {
    strcpy(locals.title, prompt);
  } else {
    strcpy(locals.title, unk_00572930);
  }

  return DialogBoxParamA(unk_00925030, (const char *)0xe9, unk_008cf1b4, FUN_0049ebde, (long)&locals.context);
}

// FUNCTION: MAGIC 0x0049e8bb
int FUN_0049e8bb(int player,
                 int *graveyard,
                 int unused,
                 void *available,
                 int count,
                 int prompt_lines,
                 int num_prompt_lines,
                 int selected_indices,
                 int highlighted_choices,
                 int max_choices)
{
  struct {
    int stop_selection;
    int packet_card;
    int selected_count;
    char prompt[200];
    int available_cards[500];
    int index;
    unsigned int show_bigcard;
    int graveyard_copy[492];
    int unused_slot;
  } locals;
  int selection;

  (void)unused;

  if (selected_indices == 0 || max_choices < 1) {
    return 0;
  } else if (count < 1 || graveyard == NULL || graveyard[0] == -1) {
    return 0;
  }

  memcpy(locals.graveyard_copy, graveyard, count << 2);
  if (available == NULL) {
    for (locals.index = 0; locals.index < count; ++locals.index) {
      locals.available_cards[locals.index] = 1;
    }
  } else {
    memcpy(locals.available_cards, available, count << 2);
  }

  if (active_player == player && (unk_00926804 & 2) != 0) {
    locals.stop_selection = 1;
  } else {
    locals.stop_selection = 0;
  }

  locals.selected_count = 0;
  while (locals.stop_selection == 0 && locals.selected_count < max_choices) {
    if (locals.selected_count < num_prompt_lines) {
      strcpy(locals.prompt, ((char **)prompt_lines)[locals.selected_count]);
    } else {
      strcpy(locals.prompt, ((char **)prompt_lines)[num_prompt_lines - 1]);
    }

    locals.show_bigcard = (unsigned int)(locals.selected_count < highlighted_choices);
    selection = FUN_0049e6aa(locals.graveyard_copy,
                             0,
                             locals.available_cards,
                             count,
                             &unk_008b40e0,
                             locals.show_bigcard,
                             locals.prompt);
    if (selection == -1) {
      locals.stop_selection = 1;
    } else {
      ((int *)selected_indices)[locals.selected_count] = selection;
      ++locals.selected_count;
      locals.graveyard_copy[selection] = unk_008b28f8;
      locals.available_cards[selection] = 0;
    }
  }

  if ((unk_00926804 & 2) != 0) {
    if (active_player == player) {
      TENTATIVE_wait_for_network_result(player, 0x16);
      locals.packet_card = 0;
      locals.index = 0;
      while (locals.packet_card != -1) {
        locals.selected_count = locals.index;
        locals.packet_card = GetCardFromCLPacket(locals.index);
        if (locals.packet_card != -1) {
          ((int *)selected_indices)[locals.index] = locals.packet_card;
          ++locals.index;
        }
      }
    } else {
      for (locals.index = 0; locals.index < locals.selected_count; ++locals.index) {
        AddCardToCLPacket((short)((int *)selected_indices)[locals.index]);
      }
      AddCardToCLPacket(0xffff);
      TENTATIVE_send_network_result(player, 0x16);
    }
  }

  return locals.selected_count;
}

// FUNCTION: MAGIC 0x00501e96
void AddCardToCLPacket(unsigned short card_in_packet)
{
  if (unk_0091ca94 != 0
      && unk_0091ca96 - 1 < ((int)unk_0091ca94 + (((int)unk_0091ca94 >> 0x1f) & 0xfU)) >> 4) {
    ++unk_0091ca96;
    unk_0091ca98 = realloc(unk_0091ca98, unk_0091ca96 * 0x20);
  }

  *(unsigned short *)((char *)unk_0091ca98 + unk_0091ca94 * 2) = card_in_packet;
  ++unk_0091ca94;
}

// FUNCTION: MAGIC 0x00501f3f
int GetCardFromCLPacket(int packet_index)
{
  int packet_card;

  packet_card = (int)*(short *)((char *)unk_0091ca98 + packet_index * 2);
  if (packet_card == -1) {
    if (0x10 < unk_0091ca94) {
      free(unk_0091ca98);
      unk_0091ca98 = malloc(0x20);
    }
    unk_0091ca96 = 1;
    unk_0091ca94 = 0;
  }

  return packet_card;
}

// FUNCTION: MAGIC 0x00501143
int FUN_00501143(int player, char packet_type)
{
  struct {
    unsigned char packet_header[32];
    int packet_number;
    int packet_kind;
    size_t packet_size;
    void *global_packet;
    int index;
    short *source_ptr;
    int send_result;
    short *write_ptr;
  } locals;

  (void)player;

  locals.packet_size = unk_0091ca94 * 2 + 6;
  unk_0091ca90 = packet_type;
  unk_0091ca92 = (short)unk_0091d07c;
  ++unk_0091d07c;

  locals.packet_number = -1;
  locals.packet_kind = 2;
  locals.global_packet = malloc(locals.packet_size);
  locals.write_ptr = locals.global_packet;
  *locals.write_ptr = (short)unk_0091ca90;
  locals.write_ptr[1] = unk_0091ca92;
  locals.write_ptr[2] = unk_0091ca94;
  locals.write_ptr += 3;

  locals.source_ptr = unk_0091ca98;
  for (locals.index = 0; locals.index < unk_0091ca94; ++locals.index) {
    *locals.write_ptr = *locals.source_ptr;
    ++locals.source_ptr;
    ++locals.write_ptr;
  }

  locals.send_result = FamInterface_SendPacket(locals.packet_header, 1);
  free(locals.global_packet);
  if (locals.send_result == 1) {
    return 0;
  }

  if (0x10 < unk_0091ca94) {
    free(unk_0091ca98);
    unk_0091ca98 = malloc(0x20);
  }

  unk_0091ca90 = '\0';
  unk_0091ca94 = 0;
  unk_0091ca96 = 1;
  return 1;
}

// FUNCTION: MAGIC 0x00501c19
int FUN_00501c19(int player, int packet_type, unsigned char *packet)
{
  int index;
  short *read_ptr;
  short *write_ptr;

  (void)player;
  (void)packet_type;

  unk_0091ca90 = *packet;
  unk_0091ca92 = *(short *)(packet + 2);
  read_ptr = (short *)(packet + 4);
  unk_0091ca94 = *read_ptr;
  ++read_ptr;
  if (unk_0091ca94 >= 0x1f5) {
    return 0;
  }

  if (0x10 < unk_0091ca94) {
    unk_0091ca98 = realloc(unk_0091ca98, unk_0091ca94 * 2);
  }

  write_ptr = unk_0091ca98;
  for (index = 0; index < unk_0091ca94; ++index) {
    *write_ptr = *read_ptr;
    ++read_ptr;
    ++write_ptr;
  }

  return 1;
}

// FUNCTION: MAGIC 0x005012d7
int TENTATIVE_wait_for_network_result(int player, signed int packet_type)
{
  (void)player;
  (void)packet_type;
  return 1;
}

// FUNCTION: MAGIC 0x00500d74
int TENTATIVE_send_network_result(int player, signed int packet_type)
{
  if (packet_type == 3 || packet_type == 4 || packet_type == 0x16) {
    return FUN_00501143(player, (char)packet_type);
  }

  return 0;
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
  int internal_card_id;

  if (player == -1 || card == -1) {
    return -1;
  }

  internal_card_id = FUN_004a587c(player, card);
  if (internal_card_id == -1) {
    return -1;
  }

  return *(int *)&global_cards_data[internal_card_id].id;
}

// FUNCTION: MAGIC 0x004a59ad
int CardIDFromType(unsigned int type)
{
  if (type == -1) {
    return -1;
  }

  type &= 0xfff;
  return *(int *)&global_cards_data[type].id;
}

// FUNCTION: MAGIC 0x004a5929
int CardTypeFromID(int csvid)
{
  int result;
  int internal_card_id;

  if (csvid == -1) {
    return -1;
  }

  result = -1;
  for (internal_card_id = 0; *(int *)&global_cards_data[internal_card_id].id != -1; ++internal_card_id) {
    if (result == -1) {
      if (*(int *)&global_cards_data[internal_card_id].id == csvid) {
        result = internal_card_id;
      }
    }
  }

  return result;
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
  if (result != 0) {
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
  struct {
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

  if (target_source_mode != 0 && target_source_mode != 1 && target_source_mode != 2) {
    return 0;
  }

  s.done = 0;
  for (s.player_index = 0; s.player_index < 2; ++s.player_index) {
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
                               illegal_state) != 0) {
      s.found_any = 1;
      ++s.count;
      if (s.stop_on_first) {
        s.done = 1;
      }
    }
  }

  if (unk_008b35ec == who_chooses || (unk_00926804 & 2) != 0) {
    if ((allowed_controller & 2) == 0) {
      s.scan_player = 1;
    } else {
      s.scan_player = 0;
    }
  } else if ((preferred_controller & 2) != 0 || (preferred_controller & 1) != 0) {
    s.scan_player = 1;
  } else {
    s.scan_player = 0;
  }

  s.player_index = 0;
  while (s.player_index < 2) {
    if (!s.done) {
      for (s.current_card = 0;
           s.current_card < (active_cards_count[1] > active_cards_count[0] ? active_cards_count[1]
                                                                           : active_cards_count[0]);
           ++s.current_card) {
        if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id != -1) {
          if (target_source_mode == 0) {
            s.target_player = s.scan_player;
            s.target_card = s.current_card;
            s.target_is_valid = 1;
          } else if (target_source_mode == 1) {
            if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4) {
              s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_player;
              s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_target_card;
              s.target_is_valid = 1;
            } else {
              s.target_is_valid = 0;
            }
          } else if (PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).internal_card_id == unk_009266a4) {
            s.target_player = (char)PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_player;
            s.target_card = PLAYER_CARD_INSTANCE(s.scan_player, s.current_card).damage_source_card;
            s.target_is_valid = 1;
          } else {
            s.target_is_valid = 0;
          }

          if (s.target_is_valid
              && C_real_validate_target(s.target_player,
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
                                        illegal_state) != 0) {
            s.found_any = 1;
            ++s.count;
            if (s.stop_on_first) {
              s.done = 1;
            }
          }
        }
      }
    }

    ++s.player_index;
    s.scan_player = 1 - s.scan_player;
  }

  if (num_valid_targets != NULL) {
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
      && is_in_play(player, card) && has_mana_w_global_cost_mod(player, card, 1, 0, 0, 0, 0, 0)) {
    target_color = get_sleighted_color(player, card, color);
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
        charge_mana(player, 0, 1);
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
  land_type = get_hacked_color(player, card, 1);
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
    return real_target_available((int*)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, required_type, 0, 0,
                        get_protections_from(player, card), 0, 0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player) {
    if (!C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, required_type, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
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
                                get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1,
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

// FUNCTION: MAGIC 0x0043b4f3
int FUN_0043b4f3(int player, int amount)
{
  card_instance_t* instance;
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
               TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT
                   | TYPE_CREATURE | TYPE_LAND,
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
  if (amount <= max_targets) {
    sacrifices_to_make = amount;
  }

  sacrifices_made = 0;
  while (sacrifices_made < sacrifices_to_make && life[player] != -99) {
    if (unk_008a9000 == 1) {
      target.player = player;
      do {
        do {
          unk_00939340 = internal_rand(active_cards_count[player]);
          target.card = unk_00939340;
        } while (!is_in_play(target.player, target.card));
        instance = &PLAYER_CARD_INSTANCE(target.player, target.card);
      } while ((global_cards_data[instance->internal_card_id].type & 0x7f) == 0
               || (instance->token_status & 0x10) != 0);
      FUN_004e4f11();
    } else if (player == active_player && (unk_00926804 & 2) == 0) {
      FUN_004e5089();
      target.player = player;
      target.card = unk_00939340;
    } else {
      load_text((int)"promptsX1.txt", "LICH");
      sprintf(prompt, text_lines[0], sacrifices_made + 1, sacrifices_to_make);
      C_real_select_target(player,
                           player,
                           player,
                           TARGET_ZONE_IN_PLAY,
                           TYPE_ARTIFACT | TYPE_INTERRUPT | TYPE_INSTANT | TYPE_SORCERY | TYPE_ENCHANTMENT
                               | TYPE_CREATURE | TYPE_LAND,
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
      if (unk_008a9000 != 1) {
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
  PostMessageA((void*)unk_008cf1b4, 0x401, (unsigned int)player, 0);
  ExitThread((unsigned int)player);
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

// FUNCTION: MAGIC 0x00532bea
int FUN_00532bea(int player, int card, int event, int color)
{
  card_instance_t *instance;
  target_t target;
  unsigned int target_color;
  int target_player;
  int target_card;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  target_color = 1 << ((unsigned char)color & 0x1f);

  if (event == EVENT_CAN_CAST) {
    return 1;
  } else {
    if (event == EVENT_CAST_SPELL && affected_card == card && affected_card_controller == player
        && FUN_00404cff(player, instance->internal_card_id, player) == 0) {
      ai_modifier +=
          (basiclandtypes_controlled[unk_008b35ec][color] + unk_008cf1c0[unk_008b35ec][color] / 2) * 0x18;
    }

    if (event == EVENT_CAN_ACTIVATE) {
      if ((unk_008b4278 & 4) == 0 || has_mana_w_global_cost_mod(player, card, 7, 1) == 0
          || real_target_available((int *)0,
                          TARGET_SCAN_DIRECT,
                          player,
                          2,
                          2,
                          TARGET_ZONE_IN_PLAY,
                          TYPE_NONE,
                          TYPE_NONE,
                          0,
                          0,
                          target_color,
                          0,
                          unk_009266a4,
                          -1,
                          0xffffffff,
                          0xffffffff,
                          TARGET_SPECIAL_DAMAGE_PLAYER,
                          0,
                          0) == 0) {
        return 0;
      }
      return 99;
    } else {
      if (event == EVENT_ACTIVATE && (instance->state & STATE_INVISIBLE) == 0) {
        charge_mana_w_global_cost_mod(player, card, 0, 1);
        if (spell_fizzled != 1) {
          if (unk_008a9000 != 1) {
            load_text("prompts.txt", "CIRCLE_OF_PROTECTION");
          }

          if (C_real_select_target(player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
                                   COLOR_TEST_0,
                                   unk_009266a4,
                                   ~SUB_WALL,
                                   -1,
                                   -1,
                                   TARGET_SPECIAL_DAMAGE_PLAYER,
                                   0,
                                   0,
                                   text_lines[0],
                                   1,
                                   &target) == 0) {
            spell_fizzled = 1;
          } else {
            instance->targets[0].player = target.player;
            instance->targets[0].card = target.card;
            instance->number_of_targets = 1;
          }
        }
      }

      if (event == EVENT_RESOLVE_ACTIVATION) {
        if (C_real_validate_target(instance->targets[0].player,
                                   instance->targets[0].card,
                                   (char *)0,
                                   player,
                                   2,
                                   2,
                                   TARGET_ZONE_IN_PLAY,
                                   TYPE_NONE,
                                   TYPE_NONE,
                                   0,
                                   0,
                                   target_color,
                                   COLOR_TEST_0,
                                   unk_009266a4,
                                   ~SUB_WALL,
                                   -1,
                                   -1,
                                   TARGET_SPECIAL_DAMAGE_PLAYER,
                                   0,
                                   0) == 0) {
          spell_fizzled = 1;
        } else {
          target_player = instance->targets[0].player;
          target_card = instance->targets[0].card;
          if (PLAYER_CARD_INSTANCE(target_player, target_card).info_slot != 0) {
            PLAYER_CARD_INSTANCE(target_player, target_card).info_slot = 0;
          }
        }

        PLAYER_CARD_INSTANCE(instance->parent_controller, instance->parent_card).number_of_targets = 0;
      }

      return 0;
    }
  }
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
  if (active_cards_count[1] <= active_cards_count[0]) {
    max_cards = active_cards_count[0];
  }

  for (current_card = 0; current_card < max_cards; ++current_card) {
    for (test_player = 0; test_player < 2; ++test_player) {
      if (is_in_play(test_player, current_card)
          && instance->targets[0].player == PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_player
          && instance->targets[0].card == PLAYER_CARD_INSTANCE(test_player, current_card).damage_target_card) {
        return 1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0052e400
int FUN_0052e400(int player, int card, int event, int color)
{
  card_instance_t *instance;
  card_instance_t *other_instance;
  unsigned int ward_color;
  int aura_count;
  int current_card;
  int test_player;
  int old_event_result;
  int protection_color;

  instance = &PLAYER_CARD_INSTANCE(player, card);
  ward_color = 1 << ((unsigned char)color & 0x1f);

  if (event == EVENT_CAN_CAST) {
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
                        0,
                        0,
                        -1,
                        -1,
                        0xffffffff,
                        0xffffffff,
                        0,
                        0,
                        0);
  } else {
    if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) {
      if (unk_008a9000 != 1) {
        load_text((int)"prompts.txt", "ANY_WARD");
      }

      if (!FUN_00551638(player, player, card)) {
        spell_fizzled = 1;
      } else {
        if (player == active_player && (unk_00926804 & 2) == 0 && FUN_0052adf2(player, card) != 0) {
          ai_modifier -= 0x60;
        }
        if (instance->targets[0].player == active_player) {
          aura_count = unk_008cf1c0[unk_008b35ec][color];
          ai_modifier += (aura_count + 1)
                         * C_get_abilities(instance->targets[0].player, instance->targets[0].card, EVENT_POWER, -1)
                         * 3;
        }
        if (instance->targets[0].player == unk_008b35ec) {
          ai_modifier -= 0x60;
        }
      }
    }

    if (event == EVENT_RESOLVE_SPELL) {
      if (C_real_validate_target(instance->targets[0].player,
                                 instance->targets[0].card,
                                 (char *)0,
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
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0) == 0) {
        kill_card(player, card, KILL_BURY);
        spell_fizzled = 1;
      } else {
        instance->damage_target_player = instance->targets[0].player;
        instance->damage_target_card = instance->targets[0].card;
      }
      instance->number_of_targets = 0;
    }

    if (instance->damage_target_card != -1) {
      for (test_player = 0; test_player < 2; ++test_player) {
        for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card) {
          other_instance = &PLAYER_CARD_INSTANCE(test_player, current_card);
          old_event_result = event_result;
          if (is_in_play(test_player, current_card) && other_instance->damage_target_card == instance->damage_target_card
              && other_instance->damage_target_player == instance->damage_target_player
              && (unsigned char)other_instance->color == ward_color && (player != test_player || card != current_card)
              && (global_cards_data[other_instance->internal_card_id].type & TYPE_ENCHANTMENT) != 0) {
            kill_card(test_player, current_card, KILL_BURY);
          }
          event_result = old_event_result;
        }
      }
    }

    if (instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller
        && affected_card != -1 && (instance->state & STATE_INVISIBLE) == 0 && event == EVENT_ABILITIES) {
      protection_color = get_sleighted_color(player, card, color);
      event_result |= 0x800 << (((unsigned char)protection_color - 1) & 0x1f);
    }

    if (event == EVENT_CAST_SPELL
        && instance->damage_target_player
               == PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_player
        && PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).damage_target_card == instance->damage_target_card
        && (ward_color & (unsigned char)PLAYER_CARD_INSTANCE(affected_card_controller, affected_card).color) != 0
        && (instance->state & STATE_INVISIBLE) == 0) {
      event_result = 1;
    }

    if (event == EVENT_SHOULD_AI_PLAY && current_phase == EVENT_CLEANUP && card == affected_card
        && player == affected_card_controller) {
      test_player = 1 - player;
      aura_count = 0;
      for (current_card = 0; current_card < active_cards_count[test_player]; ++current_card) {
        if ((ward_color & (unsigned char)PLAYER_CARD_INSTANCE(test_player, current_card).color) != 0
            && is_in_play(test_player, current_card)) {
          aura_count += 0x18;
        }
      }

      if (aura_count != 0) {
        ai_modifier += aura_count + 0x18;
      }
    }

    return 0;
  }
}

