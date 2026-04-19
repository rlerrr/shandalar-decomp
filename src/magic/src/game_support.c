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

int create_a_card_type(int internal_card_id)
{
  return internal_card_id;
}

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

int dispatch_event(int player, int card, event_t event)
{
  return dispatch_event_to_single_card(player, card, event, 1 - player, -1);
}

int FUN_0043e18b(int player)
{
  (void)player;
  return 0;
}

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

void declare_mana_available(int player, color_t color, int amount)
{
  raw_mana_available[player][color] += amount;
}

void undeclare_mana_available_and_produce_it(int player, color_t color, int amount)
{
  if (raw_mana_available[player][color] >= amount) {
    raw_mana_available[player][color] -= amount;
  } else {
    raw_mana_available[player][color] = 0;
  }
}

void FUN_0051a41c(int player, int card)
{
  dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
  dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
}

void FUN_005513d7(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
}

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

int FUN_00464a57(int maximum)
{
  (void)maximum;
  return 0;
}

void FUN_004e4f11(void)
{
}

void FUN_004e5089(void)
{
}

int FUN_0048463d(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
  return 0;
}

int FUN_00483489(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
  return 0;
}

int FUN_004bff5a(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

int FUN_004c081a(int player, int card)
{
  (void)player;
  (void)card;
  return 0;
}

int FUN_004c0880(int internal_card_id, int extra)
{
  (void)internal_card_id;
  (void)extra;
  return 0;
}

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
void FUN_00432f00(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
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

void FUN_00542a2a(int player, int card)
{
  (void)player;
  (void)card;
}

void FUN_0054ac4d(int player, int card, int amount)
{
  (void)player;
  (void)card;
  (void)amount;
}

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
