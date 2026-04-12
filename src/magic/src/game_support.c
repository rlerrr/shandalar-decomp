#include <string.h>
#include "defs.h"
#include "global_state.h"

extern card_data_t global_cards_data[];

typedef int (__cdecl *in_play_card_callback_t)(int source_player,
                                               int source_card,
                                               int player,
                                               int card,
                                               int internal_card_id);

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

int basiclandtypes_controlled[2][8];
int human_player;
int unk_00896534;
int unk_008a9000;
int unk_009266a4;
int unk_00926804;
int unk_0093d848;
int unk_00939520[2][8];
int unk_00939530[2][8];
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

int get_sleighted_color_test(int player, int card, int orig_color_test)
{
  (void)player;
  (void)card;
  return orig_color_test;
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

int regenerate(int player, int card, event_t event, int color, int amount)
{
  (void)player;
  (void)card;
  (void)event;
  (void)color;
  (void)amount;
  return 0;
}

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

void FUN_00432f00(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
}

void FUN_004e4ff3(int a1)
{
  (void)a1;
}

void FUN_004e503e(int a1)
{
  (void)a1;
}

int FUN_004eaf09(int player, unsigned int color, int amount)
{
  (void)player;
  (void)color;
  (void)amount;
  return 0;
}

int FUN_004ef850(int a1, int a2, int a3, int a4, int a5)
{
  (void)a1;
  (void)a2;
  (void)a3;
  (void)a4;
  (void)a5;
  return -1;
}

int get_abilities(int player, int card, event_t event, int new_attacking_card)
{
  (void)player;
  (void)card;
  (void)event;
  (void)new_attacking_card;
  return 0;
}

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

void FUN_0054ac4d(int player, int card)
{
  (void)player;
  (void)card;
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
