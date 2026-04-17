#include <string.h>
#include "defs.h"
#include "global_state.h"

extern card_data_t global_cards_data[];

typedef int (__cdecl *in_play_card_callback_t)(int source_player,
                                               int source_card,
                                               int player,
                                               int card,
                                               int internal_card_id);

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
                         target_t *ret_tgt);
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
                 unsigned int param_19);
unsigned int __stdcall FUN_004bdc06(int param_1,
                                    int param_2,
                                    char *param_3,
                                    int param_4,
                                    unsigned int param_5,
                                    unsigned int param_6,
                                    unsigned int param_7,
                                    unsigned int param_8,
                                    unsigned int param_9,
                                    unsigned int param_10,
                                    unsigned int param_11,
                                    unsigned int param_12,
                                    unsigned int param_13,
                                    int param_14,
                                    int param_15,
                                    unsigned int param_16,
                                    unsigned int param_17,
                                    unsigned int param_18,
                                    unsigned int param_19,
                                    unsigned int param_20);
void kill_card(int player, int card, kill_t kill_mode);

// GLOBAL: MAGIC 0x0093dabc
int affected_card = -1;

// GLOBAL: MAGIC 0x008cd928
int affected_card_controller = -1;

// GLOBAL: MAGIC 0x00789450
int event_result;

// GLOBAL: MAGIC 0x008b426c
int spell_fizzled;

// GLOBAL: MAGIC 0x00777be0
int active_cards_count[2];

// GLOBAL: MAGIC 0x00939048
trigger_t trigger_condition;

// GLOBAL: MAGIC 0x00939338
int current_phase;

// GLOBAL: MAGIC 0x00939340
int unk_00939340;

// GLOBAL: MAGIC 0x00939348
int unk_00939348;

// GLOBAL: MAGIC 0x008b497c
int current_turn;

// GLOBAL: MAGIC 0x008b32b8
int active_player;

// GLOBAL: MAGIC 0x0093a808
int attacking_card_controller = -1;

// GLOBAL: MAGIC 0x008ce530
int attacking_card = -1;

// GLOBAL: MAGIC 0x0093b160
int card_on_stack_controller = -1;

// GLOBAL: MAGIC 0x0091ce2c
int card_on_stack = -1;

// GLOBAL: MAGIC 0x00742fe0
int basiclandtypes_controlled[2][8];

// GLOBAL: MAGIC 0x00743040
int raw_mana_available[2][8];

// GLOBAL: MAGIC 0x00715fb0
int unk_00715fb0;

// GLOBAL: MAGIC 0x00791554
int human_player;

// GLOBAL: MAGIC 0x008cfd78
int hand_count[2];

// GLOBAL: MAGIC 0x007ab2b8
int trigger_cause_controller;

// GLOBAL: MAGIC 0x008cee74
int trigger_cause;

// GLOBAL: MAGIC 0x008cfd88
int artifact_cards_in_play[2];

// GLOBAL: MAGIC 0x008cfd90
int enchantments_in_play[2];

// GLOBAL: MAGIC 0x00742f60
int unk_00742f60;

// GLOBAL: MAGIC 0x007a7c1c
int unk_007a7c1c;

// GLOBAL: MAGIC 0x007a7d88
int unk_007a7d88[2];

// GLOBAL: MAGIC 0x008b3270
int unk_008b3270;

// GLOBAL: MAGIC 0x008b35ec
int unk_008b35ec;

// GLOBAL: MAGIC 0x008b4270
int life[2];

// GLOBAL: MAGIC 0x008b4278
int unk_008b4278;

// GLOBAL: MAGIC 0x00743094
int unk_00743094;

int unk_00896534;
int unk_008a9000;
int unk_008a8de8;
// GLOBAL: MAGIC 0x008a8c38
unsigned int card_types_in_play[2];
// GLOBAL: MAGIC 0x0091a80c
int unk_0091a80c;
// GLOBAL: MAGIC 0x00925d3c
int unk_00925d3c;
// GLOBAL: MAGIC 0x00926664
int produced_mana_color;
int unk_009266a4;
int unk_00926804;
int unk_0093d848;
// GLOBAL: MAGIC 0x0093f4b8
int unk_0093f4b8;

// GLOBAL: MAGIC 0x008a8df8
int LEGACY_EFFECT_PUMP;

// GLOBAL: MAGIC 0x008b40f4
int x_value;

// GLOBAL: MAGIC 0x008ce568
int max_x_value;

// GLOBAL: MAGIC 0x00939520
int unk_00939520[2][8];

// GLOBAL: MAGIC 0x00939530
int unk_00939530[2][8];

// GLOBAL: MAGIC 0x0093950c
int ai_modifier;
char text_lines[249][300];

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
    if (FUN_004bdc06(global_card_instances[player][card].targets[0].player,
                     global_card_instances[player][card].targets[0].card,
                     (char *)0,
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
unsigned int __stdcall FUN_004bdc06(int param_1,
                                    int param_2,
                                    char *param_3,
                                    int param_4,
                                    unsigned int param_5,
                                    unsigned int param_6,
                                    unsigned int param_7,
                                    unsigned int param_8,
                                    unsigned int param_9,
                                    unsigned int param_10,
                                    unsigned int param_11,
                                    unsigned int param_12,
                                    unsigned int param_13,
                                    int param_14,
                                    int param_15,
                                    unsigned int param_16,
                                    unsigned int param_17,
                                    unsigned int param_18,
                                    unsigned int param_19,
                                    unsigned int param_20)
{
  (void)param_1;
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
  (void)param_20;
  return 0;
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
