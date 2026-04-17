#include "defs.h"
#include "../global_state.h"

extern int affected_card;
extern int affected_card_controller;
extern int active_player;
extern int attacking_card;
extern int attacking_card_controller;
extern int ai_modifier;
extern int artifact_cards_in_play[2];
extern int basiclandtypes_controlled[2][8];
extern int card_on_stack;
extern int card_on_stack_controller;
extern unsigned int card_types_in_play[2];
extern int current_phase;
extern int current_turn;
extern int enchantments_in_play[2];
extern int event_result;
extern int hand_count[2];
extern int human_player;
extern int LEGACY_EFFECT_PUMP;
extern int life[2];
extern int max_x_value;
extern int produced_mana_color;
extern int raw_mana_available[2][8];
extern int spell_fizzled;
extern int trigger_condition;
extern int trigger_cause;
extern int trigger_cause_controller;
extern int unk_00742f60;
extern int unk_007a7c1c;
extern int unk_007a7d88[2];
extern int unk_00715fb0;
extern int unk_008b35ec;
extern int unk_008b3270;
extern int unk_008b4278;
extern int unk_00896534;
extern int unk_008a9000;
extern int unk_00925d3c;
extern int unk_009266a4;
extern int unk_00926804;
extern int unk_0093d848;
extern int unk_00939340;
extern int unk_00939348;
extern int unk_00939520[2][8];
extern int unk_00939530[2][8];
extern int x_value;
extern char text_lines[249][300];
extern card_data_t global_cards_data[];

int can_target(target_definition_t *td);
int add_card_to_hand(int player, int internal_card_id);
int charge_mana_for_activated_ability(int player, int card, int colorless, int black, int blue, int green, int red, int white);
int create_a_card_type(int internal_card_id);
void default_target_definition(int player, int card, target_definition_t *td, int type);
int damage_player(int target_player, int amount, int source_player, int source_card);
int dispatch_event(int player, int card, event_t event);
int FUN_0043e18b(int player);
int FUN_00442763(int color_test);
int FUN_00464a57(int maximum);
int FUN_004c0a36(int a1, int a2, int a3);
int FUN_0048463d(int player, int card, int amount);
int FUN_004b5f03(int card_id);
void FUN_00432f00(int player, unsigned int color, int amount);
int dispatch_event_to_single_card(int player,
                                  int card,
                                  event_t event,
                                  int new_attacking_card_controller,
                                  int new_attacking_card);
int FUN_005001e0(int player, int card, int value);
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
int get_color_from_color_test(char color_test);
void FUN_004a61d6(char *text);
void gain_life(int player, int amount);
void discard(int player, int flags, int player_who_controls_effect);
int do_dialog(int who_chooses,
              int bigcard_player,
              int bigcard_card,
              int smallcard_player,
              int smallcard_card,
              const char *options,
              int ai_choice);
void FUN_004e4ff3(int a1);
void FUN_004e4f11(void);
void FUN_004e503e(int a1);
void FUN_004e5089(void);
void FUN_004e51bb(void);
int FUN_004eaf09(int player, unsigned int color, int amount);
int FUN_004ef850(int a1, int a2, int a3, int a4, int a5);
int FUN_00482a97(int player, int card, unsigned int flags);
int FUN_00485060(int player, int card, int (__cdecl *callback)(int, int, int, int, int), int who_to_check);
unsigned int FUN_0053aa74(int player, int card);
void FUN_00542a2a(int player, int card);
int FUN_0052dd74(int player, int card, event_t event, int power_modifier, int toughness_modifier);
int FUN_005493a6(int player, int card, int internal_card_id);
int FUN_00551638(int player, unsigned int preferred_controller, int card);
int FUN_00551b60(int player, unsigned int preferred_controller, int card);
void FUN_0055117d(int (__cdecl *callback)(int, int, int), int who_to_check);
void FUN_00551572(int player, int card, int amount);
void FUN_005514cd(int player, int card, int amount);
void TENTATIVE_reassess_all_cards(void);

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
int C_get_abilities(int player, int card, event_t event, int new_attacking_card);
unsigned int C_get_special_counters(int player, int card);
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
int get_hacked_color(int player, int card, int orig_color);
int get_sleighted_color(int player, int card, int orig_color);
int get_sleighted_color_test(int player, int card, int orig_color_test);
int has_mana_for_activated_ability(int player, int card, int colorless, int black, int blue, int green, int red, int white);
int is_animated_and_sick(int player, int card);
int is_in_play(int player, int card);
int is_tapped(int player, int card);
void kill_card(int player, int card, kill_t kill_mode);
void load_text(int file_name, const char *section_name);
int mana_producer_sound_on_resolve(int player, int card, event_t event, color_t color);
int FUN_005058b1(int player, int card, event_t event, color_test_t available_colors);
void FUN_0051a41c(int player, int card);
int choose_a_color(int player, const char *prompt, int unused1, int unused2, unsigned int available_colors);
void FUN_0054ac4d();
int FUN_0054af10(int player, int card, event_t event, int amount);
int FUN_0054276d(int player, int card, event_t event, unsigned int color, int amount);
void FUN_005513d7(int player, int card, int amount);
int select_target(int player, int card, target_definition_t *td, const char *prompt, target_t *ret_location);
int tap_card(int player, int card);
void declare_mana_available(int player, color_t color, int amount);
void undeclare_mana_available_and_produce_it(int player, color_t color, int amount);
int validate_target(int player, int card, target_definition_t *td, int target_number);
void vigilance(int player, int card, event_t event);
void FUN_0054e470(int player, int card, int color);
void add_special_counter(int player, int card);
int TENTATIVE_set_timestamps(int player, int card);
void __stdcall Sleep(unsigned long dwMilliseconds);


card_instance_t __inline *get_card_instance(int player, int card)
{
  return &global_card_instances[player][card];
}

#define PLAYER_CARD_INSTANCE(player_, card_) \
  (((card_instance_t *)((char *)global_card_instances + (player_) * 0xb0f4))[card_])


// FUNCTION: MAGIC 0x004b6120
// FUNCTION: SHANDALAR 0x00429ee0
int card_swamp(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_BLACK);
}

// FUNCTION: MAGIC 0x004b6146
// FUNCTION: SHANDALAR 0x00429f06
int card_island(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_BLUE);
}

// FUNCTION: MAGIC 0x004b616c
// FUNCTION: SHANDALAR 0x00429f2c
int card_forest(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_GREEN);
}

// FUNCTION: MAGIC 0x004b6192
// FUNCTION: SHANDALAR 0x00429f52
int card_mountain(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_RED);
}

// FUNCTION: MAGIC 0x004b61b8
// FUNCTION: SHANDALAR 0x00429f78
int card_plains(int player, int card, event_t event)
{
  return mana_producer_sound_on_resolve(player, card, event, COLOR_WHITE);
}

// FUNCTION: MAGIC 0x00505dd2
// FUNCTION: SHANDALAR 0x00555e68
int card_badlands(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "BADLANDS");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00505e42
// FUNCTION: SHANDALAR 0x00555ed8
int card_bayou(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "BAYOU");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00505eb2
// FUNCTION: SHANDALAR 0x00555f48
int card_plateau(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "PLATEAU");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00505f22
// FUNCTION: SHANDALAR 0x00555fb8
int card_savannah(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "SAVANNAH");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00505f92
// FUNCTION: SHANDALAR 0x00556028
int card_scrubland(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "SCRUBLAND");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00506002
// FUNCTION: SHANDALAR 0x00556098
int card_taiga(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "TAIGA");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00506072
// FUNCTION: SHANDALAR 0x00556108
int card_tropical_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "TROPICAL_ISLAND");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x005060e2
// FUNCTION: SHANDALAR 0x00556178
int card_tundra(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "TUNDRA");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x00506152
// FUNCTION: SHANDALAR 0x005561e8
int card_underground_sea(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "UNDERGROUND_SEA");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x005061c2
// FUNCTION: SHANDALAR 0x00556258
int card_volcanic_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE) {
    load_text((int)"promptsX1.txt", "VOLCANIC_ISLAND");
  }

  return FUN_005058b1(player, card, event, *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x58));
}

// FUNCTION: MAGIC 0x0053aa50
// FUNCTION: SHANDALAR 0x00488ca0
int card_dummy(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x00545c2e
// FUNCTION: SHANDALAR 0x00493e75
int card_northern_paladin(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_definition_t td;

  instance = get_card_instance(player, card);

  default_target_definition(player, card, &td, TYPE_PERMANENT);
  td.zone = TARGET_ZONE_IN_PLAY;
  td.required_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);
  td.required_subtype = ~SUBTYPE_WALL;

  if (event == EVENT_CAN_ACTIVATE) {
    return !is_tapped(player, card)
           && !is_animated_and_sick(player, card)
           && has_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 2)
           && can_target(&td);
  }

  if (event == EVENT_ACTIVATE) {
    charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 2);
    if (spell_fizzled) {
      return 0;
    }

    load_text((int)"prompts.txt", "NORTHERN_PALADIN");
    instance->number_of_targets = 0;
    if (!select_target(player, card, &td, text_lines[0], &instance->targets[0])) {
      spell_fizzled = 1;
      return 0;
    }

    instance->number_of_targets = 1;
    tap_card(player, card);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION) {
    default_target_definition(player, card, &td, TYPE_PERMANENT);
    td.zone = TARGET_ZONE_IN_PLAY;
    td.required_color = get_sleighted_color_test(instance->parent_controller,
                                                 instance->parent_card,
                                                 COLOR_TEST_BLACK);
    td.required_subtype = ~SUBTYPE_WALL;
    if (validate_target(player, card, &td, 0)) {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054920c
// FUNCTION: SHANDALAR 0x0049744a
int card_serra_angel(int player, int card, event_t event)
{
  if (affected_card == card && affected_card_controller == player) {
    *(unsigned int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 8) |= STATE_VIGILANCE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c736
static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id)
{
  int card_id;
  int pending_event;
  int result;

  result = 0;
  card_id = global_cards_data[global_card_instances[source_player][source_card].internal_card_id].id;

  if (unk_009266a4 == internal_card_id
      && global_card_instances[player][card].info_slot != 0
      && global_card_instances[player][card].damage_target_player == source_player
      && global_card_instances[player][card].damage_target_card == -1
      && ((card_id == CARD_ID_VETERAN_BODYGUARD
           && (global_card_instances[player][card].token_status & 0x140000) != 0)
          || (card_id == CARD_ID_MARTYRS_OF_KORLIS
              && (global_card_instances[player][card].eot_toughness & 0x40) != 0))) {
    pending_event = global_card_instances[source_player][source_card].info_slot;
    if (pending_event == EVENT_TRIGGER) {
      result = 1;
      event_result = 2;
    } else if (pending_event == EVENT_RESOLVE_TRIGGER) {
      global_card_instances[player][card].damage_target_player = (char)source_player;
      global_card_instances[player][card].damage_target_card = source_card;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c8ca
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id)
{
  (void)unused;

  if ((global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD
       || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS)
      && card != source_card) {
    global_card_instances[player][card].eot_toughness = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c948
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id)
{
  int result;

  (void)unused1;
  (void)unused2;
  (void)unused3;
  (void)unused4;

  result = 0;
  if (global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD
      || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS) {
    result = 1;
    event_result = result;
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c4e9
// FUNCTION: SHANDALAR 0x0047ffee
int card_veteran_bodyguard(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_00485060(player, card, veteran_bodyguard_or_martyrs_present, player) != -1) {
      global_card_instances[player][card].eot_toughness = 1;
    }
  }

  if (trigger_condition == TRIGGER_END_DAMAGE_PREV
      && global_card_instances[player][card].eot_toughness == 0
      && (global_card_instances[player][card].state & F08_TAPPED) == 0
      && affected_card == card
      && affected_card_controller == player
      && current_turn == player
      && player != human_player
      && is_in_play(player, card)
      && (current_phase == 0x1a || current_phase == 0x19)) {
    global_card_instances[player][card].info_slot = event;
    if (event == EVENT_TRIGGER) {
      if (FUN_00485060(player, card, veteran_bodyguard_prevent_damage_callback, -1) != -1) {
        event_result |= 2;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER) {
      FUN_00485060(player, card, veteran_bodyguard_prevent_damage_callback, -1);
      FUN_00485060(player, card, veteran_bodyguard_mark_other_protectors, player);
    }
  }

  if (event == EVENT_CLEANUP && affected_card == card && affected_card_controller == player) {
    global_card_instances[player][card].eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00548bd9
// FUNCTION: SHANDALAR 0x00496e1d
int card_samite_healer(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *target;
  target_definition_t td;

  instance = get_card_instance(player, card);

  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.zone = TARGET_ZONE_IN_PLAY;
  td.required_state = TARGET_STATE_DAMAGED;
  td.required_subtype = ~SUBTYPE_WALL;

  if (event == EVENT_CAN_ACTIVATE) {
    return !is_tapped(player, card)
           && !is_animated_and_sick(player, card)
           && can_target(&td);
  }

  if (event == EVENT_ACTIVATE) {
    load_text((int)"prompts.txt", "SAMITE_HEALER");
    instance->number_of_targets = 0;
    if (!select_target(player, card, &td, text_lines[0], &instance->targets[0])) {
      spell_fizzled = 1;
      return 0;
    }

    instance->number_of_targets = 1;
    tap_card(player, card);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION) {
    default_target_definition(player, card, &td, TYPE_CREATURE);
    td.zone = TARGET_ZONE_IN_PLAY;
    td.required_state = TARGET_STATE_DAMAGED;
    td.required_subtype = ~SUBTYPE_WALL;
    if (validate_target(player, card, &td, 0)) {
      target = get_card_instance(instance->targets[0].player, instance->targets[0].card);
      if (target->damage_on_card != 0) {
        --target->damage_on_card;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e36e
// FUNCTION: SHANDALAR 0x0049c5b0
int card_white_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES
      && affected_card == card
      && affected_card_controller == player) {
    event_result |= KEYWORD_PROT_BLACK << (((char)get_sleighted_color(player, card, COLOR_BLACK) - 1U) & 0x1f);
    saved_event_result = event_result;
    FUN_0054e470(player, card, COLOR_BLACK);
    event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054baf9
// FUNCTION: SHANDALAR 0x00499d35
int card_orcish_artillery(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;
  int damage_result;

  if (event == EVENT_CAN_ACTIVATE) {
    result = (unsigned int)((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
                             & 0x20010)
                            == 0);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(1);
    result = 0;
  } else if (event == EVENT_ACTIVATE) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "ORCISH_ARTILLERY");
    }
    FUN_0054ac4d(player, card);
    if (spell_fizzled != 1) {
      state_ptr = (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8);
      *state_ptr |= 0x10;
    }
    result = 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    damage_result = FUN_0054af10(player, card, event, 2);
    if (damage_result != 0) {
      damage_player(player, 3, card_on_stack_controller, card_on_stack);
    }

    *(unsigned char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                       + (int)global_card_instances + 0x36
                       + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                             * 0xb0f4)
        = 0;
    result = 0;
  } else {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0054060c
// FUNCTION: SHANDALAR 0x0048e853
int card_dragon_whelp(int player, int card, event_t event)
{
  int result;
  unsigned int saved_max_x_value;

  if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939530[player][0];
    result = 0;
  } else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    result = 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    result = FUN_004eaf09(player, COLOR_RED, 1);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e503e(0);
    result = 0;
  } else if (event == EVENT_ACTIVATE) {
    result = FUN_004eaf09(player, COLOR_RED, 1);
    saved_max_x_value = max_x_value;
    if (result != 0) {
      x_value = 0;
      if (player == human_player) {
        if ((((player == active_player) && ((unk_00926804 & 2) == 0))
             || ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) == 0x30000))
            || unk_00715fb0 != 1) {
          max_x_value = -1;
        } else {
          max_x_value = 3 - ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) >> 16);
        }
        FUN_00432f00(player, COLOR_RED, -1);
        max_x_value = saved_max_x_value;
        if (x_value < 1) {
          spell_fizzled = 1;
        }
      } else {
        FUN_00432f00(player, COLOR_RED, 1);
        x_value = 1;
      }

      PLAYER_CARD_INSTANCE(player, card).eot_toughness &= 0xff0000;
      if (spell_fizzled == 1) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      } else {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += x_value * 0x10001;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0) {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    result = 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id
        == -1) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
                 .info_slot
           & 0x80000) != 0) {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        result = FUN_004ef850(card_on_stack_controller,
                              card_on_stack,
                              LEGACY_EFFECT_PUMP,
                              card_on_stack_controller,
                              card_on_stack);
        if (result != -1) {
          PLAYER_CARD_INSTANCE(player, result).info_slot |= 0x80000;
        }
      }
    }
    result = 0;
  } else if (event == EVENT_POW_BOOST) {
    result = FUN_004eaf09(player, COLOR_RED, 1);
    result = FUN_004c0a36(result, 0, 3);
    result -= PLAYER_CARD_INSTANCE(player, card).info_slot;
  } else {
    if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY))
         && (affected_card == card)
         && (affected_card_controller == player)
         && ((char)PLAYER_CARD_INSTANCE(player, card).info_slot != 0))
        && player == current_turn) {
      if (PLAYER_CARD_INSTANCE(player, card).info_slot < 4) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      } else {
        if (event == EVENT_TRIGGER) {
          event_result |= 2;
        }
        if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY) {
          kill_card(player, card, KILL_DESTROY);
        }
      }
    }
    if (event == EVENT_SHOULD_AI_PLAY) {
      if (player == active_player) {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * 0xc;
      } else {
        ai_modifier += basiclandtypes_controlled[player][COLOR_RED] * -0xc;
      }
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00540fc8
// FUNCTION: SHANDALAR 0x0048f20f
int card_goblin_balloon_brigade(int player, int card, event_t event)
{
  int legacy_card;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x20;
    return 0;
  } else if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939530[player][0];
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot == 0) || (FUN_004eaf09(player, COLOR_RED, 1) == 0)) {
      return 0;
    }
    return 1;
  } else if (event == EVENT_ACTIVATE) {
    if (FUN_004eaf09(player, COLOR_RED, 1) != 0) {
      FUN_00432f00(player, COLOR_RED, 1);
      if (spell_fizzled != 1) {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((active_player == player) && ((unk_00926804 & 2) == 0)) {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        }
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id
        == -1) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot = 0x20;
      legacy_card = FUN_004ef850(card_on_stack_controller,
                                 card_on_stack,
                                 unk_00896534,
                                 card_on_stack_controller,
                                 card_on_stack);
      if (legacy_card != -1) {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549451
// FUNCTION: SHANDALAR 0x0049768d
int card_goblin_king(int player, int card, event_t event)
{
  if (is_in_play(player, card)
      && is_in_play(affected_card_controller, affected_card)
      && (char)global_cards_data[global_card_instances[affected_card_controller][affected_card]
                                     .internal_card_id]
                             .subtype
             == 3) {
    if (event == EVENT_ABILITIES) {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_RED) - 1);
    }
    if (event == EVENT_POWER || event == EVENT_TOUGHNESS) {
      ++event_result;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004732e4
// FUNCTION: SHANDALAR 0x00476ddc
int card_granite_gargoyle(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939530[player][0];
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  if (event == EVENT_CAN_ACTIVATE) {
    return FUN_004eaf09(player, COLOR_RED, 1);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e503e(0);
    return 0;
  } else {
    if ((event == EVENT_ACTIVATE) && (FUN_004eaf09(player, COLOR_RED, 1) != 0)) {
      if (human_player == player) {
        FUN_00432f00(player, COLOR_RED, -1);
        if (x_value < 1) {
          spell_fizzled = 1;
        } else {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      } else {
        FUN_00432f00(player, COLOR_RED, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      } else {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0) {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION) {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id
          == -1) {
        spell_fizzled = 1;
      } else {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot +=
            (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .number_of_targets = 0;
        if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                  PLAYER_CARD_INSTANCE(player, card).parent_card)
                 .info_slot
             & 0x80000) != 0) {
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .info_slot &= 0xfff7ffff;
          legacy_card = FUN_004ef850(card_on_stack_controller,
                                     card_on_stack,
                                     LEGACY_EFFECT_PUMP,
                                     card_on_stack_controller,
                                     card_on_stack);
          if (legacy_card != -1) {
            PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
          }
        }
      }
    }

    if (event == EVENT_TOU_BOOST) {
      return FUN_004eaf09(player, COLOR_RED, 1);
    } else {
      if (event == EVENT_CAN_WASTE_MANA && raw_mana_available[player][COLOR_RED] != 0) {
        event_result |= 1;
      }

      if (event == EVENT_SHOULD_AI_PLAY) {
        if (active_player == player) {
          ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
        } else {
          ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
        }
      }

      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }

      return 0;
    }
  }
}

// FUNCTION: MAGIC 0x00542610
// FUNCTION: SHANDALAR 0x00490853
int card_keldon_warlord(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL) {
    legacy_card = FUN_004ef850(player, card, unk_0093d848, player, card);
    if (legacy_card != -1) {
      global_card_instances[player][legacy_card].eot_toughness = 0x80d;
      global_card_instances[player][legacy_card].token_status = 0x10020;
      global_card_instances[player][card].damage_source_player = (char)player;
      global_card_instances[player][card].damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047894c
// FUNCTION: SHANDALAR 0x0047c44c
int card_rock_hydra(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  unsigned int local_18;
  target_t local_14;
  int local_c;
  int local_8;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card))
      && (affected_card_controller == player)) {
    FUN_00551572(player, card, x_value);
    if (active_player == player) {
      ai_modifier += (x_value + x_value * 2 - 9) << 3;
    }
  }

  if (event == EVENT_RESOLVE_SPELL) {
    local_c = FUN_004ef850(player, card, unk_0093d848, player, card);
    if (local_c != -1) {
      uVar3 = C_get_special_counters(player, card);
      *(unsigned int *)(player * 0xb0f4 + (int)global_card_instances + 0x38 + local_c * 300) =
          (uVar3 << 8) | uVar3;
      *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x4c + local_c * 300) = 0x40c;
      *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x18 + local_c * 300) = 0x10020;
      *(char *)(player * 0xb0f4 + (int)global_card_instances + 0xc + card * 300) = (char)player;
      *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x48 + card * 300) = local_c;
    }
  }

  if (event == EVENT_CAN_ACTIVATE) {
    if ((((unk_008b4278 & 4) == 0) || ((iVar4 = FUN_004eaf09(player, 4, 1)) == 0))
        || (((uVar3 = C_get_special_counters(player, card)) == 0)
            || ((iVar4 = FUN_00482a97(player, card, 0xffffffff)) == 0))) {
      if (((current_phase == 4) && (player == human_player) && (player == unk_00742f60))
          && ((iVar4 = FUN_004eaf09(player, 4, 3)) != 0)) {
        if (((player == active_player) && ((unk_00926804 & 2) == 0))
            && (hand_count[player]
                    - *(int *)((char *)basiclandtypes_controlled + player * 0x20 + 0x10)
                == 3)) {
          unk_008b3270 |= 3;
        }
        iVar4 = 1;
      } else {
        iVar4 = 0;
      }
    } else {
      iVar4 = 99;
    }
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(0);
    iVar4 = 0;
  } else {
    if (event == EVENT_ACTIVATE) {
      if ((unk_008b4278 & 4) == 0) {
        FUN_00432f00(player, 4, 3);
        if (spell_fizzled != 1) {
          *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x74 + card * 300) = player;
          *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x78 + card * 300) = card;
          *(char *)(player * 0xb0f4 + (int)global_card_instances + 0x36 + card * 300) = 1;
        }
      } else {
        FUN_00432f00(player, 4, 1);
        if (spell_fizzled != 1) {
          local_8 = 0;
          do {
            load_text((int)"promptsX1.txt", "ROCK_HYDRA");
            iVar4 = C_real_select_target(player,
                                         2,
                                         2,
                                         0x200,
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
                                         unk_009266a4,
                                         -1,
                                         -1,
                                         -1,
                                         0,
                                         0,
                                         0,
                                         text_lines[0],
                                         1,
                                         &local_14);
            if (iVar4 == 0) {
              spell_fizzled = 1;
            } else if ((*(char *)(local_14.player * 0xb0f4 + (int)global_card_instances + 0xc
                                  + local_14.card * 300)
                        == player)
                       && (*(int *)(local_14.player * 0xb0f4 + (int)global_card_instances + 0x48
                                    + local_14.card * 300)
                           == card)) {
              *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x74 + card * 300) =
                  local_14.player;
              *(int *)(player * 0xb0f4 + (int)global_card_instances + 0x78 + card * 300) =
                  local_14.card;
              *(char *)(player * 0xb0f4 + (int)global_card_instances + 0x36 + card * 300) = 1;
              local_8 = 1;
            } else if (unk_008a9000 == 1) {
              FUN_004e51bb();
            } else {
              FUN_004a61d6(text_lines[1]);
              Sleep(0x9c4);
              FUN_004a61d6("");
            }
          } while ((spell_fizzled != 1) && (local_8 == 0));
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION) {
      if (*(int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                  + (int)global_card_instances + 0x6c
                  + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                        * 0xb0f4)
          != -1) {
        if ((unk_008b4278 & 4) == 0) {
          add_special_counter(card_on_stack_controller, card_on_stack);
          piVar1 = (int *)(*(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc)
                               * 0xb0f4
                           + (int)global_card_instances + 0x38
                           + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x48)
                                 * 300);
          *piVar1 += 0x101;
        } else {
          uVar3 = FUN_004bdc06(
              *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x74),
              *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x78),
              (char *)0,
              player,
              2,
              2,
              0x200,
              0,
              0,
              0,
              0,
              0,
              0,
              unk_009266a4,
              -1,
              0xffffffff,
              0xffffffff,
              0,
              0,
              0);
          if (uVar3 == 0) {
            spell_fizzled = 1;
          } else if (0 < *(int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                             + 0x78)
                                  * 300
                                  + (int)global_card_instances + 0x38
                                  + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                             + 0x74)
                                        * 0xb0f4)) {
            piVar1 = (int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x78)
                                 * 300
                             + (int)global_card_instances + 0x38
                             + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                        + 0x74)
                                   * 0xb0f4);
            *piVar1 += -1;
          }
        }
      }

      *(char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                + (int)global_card_instances + 0x36
                + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                      * 0xb0f4)
          = 0;
    }

    if ((((event == EVENT_DEAL_DAMAGE)
          && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                       + (int)global_card_instances + 0x6c)
              == unk_009266a4))
         && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                      + (int)global_card_instances + 0x48)
             == card))
        && ((*(char *)(affected_card * 300 + affected_card_controller * 0xb0f4
                       + (int)global_card_instances + 0xc)
             == player)
            && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                         + (int)global_card_instances + 0x38)
                != 0))) {
      iVar2 = affected_card * 300;
      iVar4 = affected_card_controller * 0xb0f4;
      uVar3 = C_get_special_counters(player, card);
      if (*(int *)(iVar2 + iVar4 + (int)global_card_instances + 0x38) < (int)uVar3) {
        local_18 = *(unsigned int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                                     + (int)global_card_instances + 0x38);
      } else {
        local_18 = C_get_special_counters(player, card);
      }
      piVar1 = (int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                       + (int)global_card_instances + 0x38);
      *piVar1 -= local_18;
      FUN_005514cd(player, card, local_18);
      piVar1 =
          (int *)(*(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc) * 0xb0f4
                  + (int)global_card_instances + 0x38
                  + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x48)
                        * 300);
      *piVar1 -= (local_18 << 8) | local_18;
    }
    iVar4 = 0;
  }
  return iVar4;
}

// FUNCTION: MAGIC 0x004796cc
// FUNCTION: SHANDALAR 0x0047d1d2
int card_sedge_troll(int player, int card, event_t event)
{
  int color;

  if (card == affected_card && player == affected_card_controller) {
    if (basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLACK)] > 0) {
      if (event == EVENT_POWER) {
        ++event_result;
      }
      if (event == EVENT_TOUGHNESS) {
        ++event_result;
      }
    }
  }

  if (event == EVENT_UNTAP_PHASE) {
    color = get_hacked_color(player, card, COLOR_BLACK);
    ((int *)((char *)unk_00939520 + player * 0x20))[color] += 2;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION) {
    return FUN_0054276d(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053fe1c
// FUNCTION: SHANDALAR 0x0048e066
int card_shivan_dragon(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939530[player][0];
    return 0;
  } else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    return FUN_004eaf09(player, COLOR_RED, 1);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e503e(0);
    return 0;
  } else if (event == EVENT_ACTIVATE) {
    if (FUN_004eaf09(player, COLOR_RED, 1) != 0) {
      if (human_player == player) {
        FUN_00432f00(player, COLOR_RED, -1);
        if (x_value < 1) {
          spell_fizzled = 1;
        } else {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      } else {
        FUN_00432f00(player, COLOR_RED, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      } else {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0) {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id
        == -1) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot
           & 0x80000) != 0) {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        legacy_card = FUN_004ef850(card_on_stack_controller,
                                   card_on_stack,
                                   LEGACY_EFFECT_PUMP,
                                   card_on_stack_controller,
                                   card_on_stack);
        if (legacy_card != -1) {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  } else if (event == EVENT_POW_BOOST) {
    return FUN_004eaf09(player, COLOR_RED, 1);
  } else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_RED] != 0) {
    event_result |= 1;
    return 0;
  } else {
    if (event == EVENT_SHOULD_AI_PLAY) {
      if (active_player == player) {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
      } else {
        ai_modifier += (basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * -4;
      }
    }

    if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0054760a
// FUNCTION: SHANDALAR 0x00495851
int card_stone_giant(int player, int card, event_t event)
{
  unsigned int target_result;
  unsigned int illegal_abilities;
  int legacy_card;
  unsigned char player_byte;
  target_t selected_target;

  player_byte = (unsigned char)player;
  if (event == EVENT_CAN_ACTIVATE) {
    legacy_card = 0;
    if ((*(unsigned int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 8) & 0x20010) == 0) {
      illegal_abilities = FUN_0053aa74(player, card);
      legacy_card = FUN_004bd7d0((int *)0,
                                 0,
                                 player,
                                 player_byte,
                                 player_byte,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 0,
                                 0,
                                 illegal_abilities,
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 (int)*(short *)(player * 0xb0f4 + card * 300 + (int)global_card_instances
                                                 + 0x124)
                                     - 1U
                                     | 0x2000,
                                 0xffffffff,
                                 0,
                                 0,
                                 0);
    }
    return legacy_card;
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(0);
    return 0;
  } else if ((event == EVENT_ACTIVATE)
             && ((*(unsigned int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 8)
                  & 0x20010)
                 == 0)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "STONE_GIANT");
    }
    legacy_card = C_real_select_target(player,
                                       player,
                                       player,
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
                                       (int)*(short *)(player * 0xb0f4 + card * 300 + (int)global_card_instances
                                                       + 0x124)
                                           - 1U
                                           | 0x2000,
                                       0,
                                       0,
                                       0,
                                       text_lines[0],
                                       1,
                                       &selected_target);
    if (legacy_card == 0) {
      spell_fizzled = 1;
    } else {
      *(int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 0x74) = selected_target.player;
      *(int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 0x78) = selected_target.card;
      *(unsigned char *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 0x36) = 1;
      if ((active_player == player)
          && ((*(unsigned char *)(selected_target.player * 0xb0f4
                                  + selected_target.card * 300
                                  + (int)global_card_instances
                                  + 0x18)
               & 0x20)
              != 0)) {
        ai_modifier += -0x60;
      }
      *(unsigned int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 8) |= 0x10;
    }
    return 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    selected_target.player = *(int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 0x74);
    selected_target.card = *(int *)(player * 0xb0f4 + card * 300 + (int)global_card_instances + 0x78);
    target_result = FUN_004bdc06(selected_target.player,
                                 selected_target.card,
                                 (char *)0,
                                 player,
                                 player_byte,
                                 player_byte,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 0,
                                 0,
                                 FUN_0053aa74(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 0xffffffff,
                                 (int)*(short *)(player * 0xb0f4 + card * 300 + (int)global_card_instances
                                                 + 0x124)
                                     - 1U
                                     | 0x2000,
                                 0,
                                 0,
                                 0);
    if (target_result == 0) {
      spell_fizzled = 1;
    } else {
      legacy_card = FUN_004ef850(card_on_stack_controller,
                                 card_on_stack,
                                 unk_00896534,
                                 selected_target.player,
                                 selected_target.card);
      if (legacy_card != -1) {
        *(unsigned char *)(player * 0xb0f4 + legacy_card * 300 + (int)global_card_instances + 0x58) = 5;
        *(int *)(player * 0xb0f4 + legacy_card * 300 + (int)global_card_instances + 0x38) = 0x20;
        *(int *)(selected_target.player * 0xb0f4
                 + selected_target.card * 300
                 + (int)global_card_instances
                 + 0x18) = 0x8000000;
      }
    }
    *(unsigned char *)(PLAYER_CARD_INSTANCE(player, card).parent_controller * 0xb0f4
                       + PLAYER_CARD_INSTANCE(player, card).parent_card * 300
                       + (int)global_card_instances
                       + 0x36) = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542d29
// FUNCTION: SHANDALAR 0x00490f6c
int card_uthden_troll(int player, int card, event_t event)
{
  unsigned int color;

  color = get_color_from_color_test(
      *(char *)&global_cards_data[*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                           + 0x6c)]
           .color);
  if (event == EVENT_UNTAP_PHASE) {
    ((int *)((char *)unk_00939520 + player * 0x20))[color] += 2;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE
             || event == EVENT_RESOLVE_ACTIVATION) {
    return FUN_0054276d(player, card, event, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d355
// FUNCTION: SHANDALAR 0x0048b5a9
int card_ironclaw_orcs(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && affected_card == card && affected_card_controller == player) {
    if (C_get_abilities(attacking_card_controller, attacking_card, EVENT_POWER, card) > 1) {
      event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b8ad
// FUNCTION: SHANDALAR 0x00499ae9
int card_lord_of_atlantis(int player, int card, event_t event)
{
  if (is_in_play(player, card)
      && is_in_play(affected_card_controller, affected_card)
      && (char)global_cards_data[global_card_instances[affected_card_controller][affected_card]
                                     .internal_card_id]
                             .subtype
             == 1) {
    if (event == EVENT_ABILITIES) {
      event_result |= 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
    } else if (event == EVENT_POWER || event == EVENT_TOUGHNESS) {
      ++event_result;
    } else if (event == EVENT_GRAVEYARD_FROM_PLAY
               && (global_card_instances[player][card].token_status & 0x80) != 0) {
      global_card_instances[affected_card_controller][affected_card].regen_status |= 0xe000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b360
// FUNCTION: SHANDALAR 0x0049959d
int card_sea_serpent(int player, int card, event_t event)
{
  if (is_in_play(player, card)) {
    if (basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == 0) {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY) {
    if (basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0) {
      event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c351
static int clone_find_data_card_callback(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id)
{
  if (global_cards_data[internal_card_id].id == CARD_ID_DATA_CARD
      && PLAYER_CARD_INSTANCE(player, card).damage_target_player == source_player
      && PLAYER_CARD_INSTANCE(player, card).damage_target_card == source_card) {
    event_result = player << 8 | card;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00470140
// FUNCTION: SHANDALAR 0x00473c2e
int card_clone(int player, int card, event_t event)
{
  target_t selected_target;
  int data_card_slot;
  int source_internal_card_id;
  int cloned_internal_card_id;
  int data_card_controller;
  unsigned int data_card_location;

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
                        0x200,
                        0,
                        0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    load_text((int)"promptsX1.txt", "CLONE");
    if (C_real_select_target(player,
                             2,
                             2,
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
                             TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                             0,
                             0,
                             text_lines[0],
                             1,
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL) {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (FUN_004bdc06(selected_target.player,
                     selected_target.card,
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
                     0x200,
                     0,
                     0)
        == 0) {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_DESTROY);
    } else if (global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id]
                    .id
               == CARD_ID_VESUVAN_DOPPELGANGER) {
      data_card_location = FUN_00485060(selected_target.player,
                                        selected_target.card,
                                        clone_find_data_card_callback,
                                        -1);
      if (data_card_location == -1) {
        source_internal_card_id = 0;
      } else {
        source_internal_card_id = PLAYER_CARD_INSTANCE((data_card_location >> 8) & 0xff,
                                                       data_card_location & 0xff)
                                      .info_slot;
      }
      data_card_controller = player;
      data_card_slot = add_card_to_hand(player, FUN_004b5f03(CARD_ID_DATA_CARD));
      if (data_card_slot != -1) {
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).state =
            ((((unsigned int)(((player == 0) - 1) >> 8)) & 0x1000) | 2);
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).token_status |= 0x10008;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_player =
            (char)player;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_card = card;
        cloned_internal_card_id = create_a_card_type(source_internal_card_id);
        if (cloned_internal_card_id != -1) {
          PLAYER_CARD_INSTANCE(player, card).internal_card_id = cloned_internal_card_id;
          PLAYER_CARD_INSTANCE(player, card).dummy3 = cloned_internal_card_id;
          PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
          PLAYER_CARD_INSTANCE(player, card).state &= 0xffffdfff;
          PLAYER_CARD_INSTANCE(player, card).color = global_cards_data[cloned_internal_card_id].color;
          if ((global_cards_data[cloned_internal_card_id].type & TYPE_ARTIFACT) != 0) {
            ++artifact_cards_in_play[player];
          }
          if ((global_cards_data[cloned_internal_card_id].type & TYPE_ENCHANTMENT) != 0) {
            ++enchantments_in_play[player];
          }
          card_types_in_play[player] |= global_cards_data[cloned_internal_card_id].type;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot =
              source_internal_card_id;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |=
              global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
          dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
          global_cards_data[cloned_internal_card_id].code_pointer =
              global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                    .internal_card_id]
                  .code_pointer;
          global_cards_data[cloned_internal_card_id].extra_ability |= 1;
        }
      }
    } else {
      PLAYER_CARD_INSTANCE(player, card).dummy3 =
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3;
      PLAYER_CARD_INSTANCE(player, card).color =
          PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).color;
      FUN_0051a41c(player, card);
    }
  }

  if (((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x20000) == 0))
      && ((card == affected_card) && (player == affected_card_controller))) {
    event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005447da
// FUNCTION: SHANDALAR 0x00492a1e
int card_wall_of_water(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939520[player][COLOR_BLUE];
    return 0;
  } else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    return FUN_004eaf09(player, COLOR_BLUE, 1);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e503e(0);
    return 0;
  } else if (event == EVENT_ACTIVATE) {
    if (FUN_004eaf09(player, COLOR_BLUE, 1) != 0) {
      if (player == human_player) {
        FUN_00432f00(player, COLOR_BLUE, -1);
        if (x_value < 1) {
          spell_fizzled = 1;
        } else {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = x_value;
        }
      } else {
        FUN_00432f00(player, COLOR_BLUE, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (spell_fizzled == 1) {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      } else {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0) {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id
        == -1) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot
           & 0x80000) != 0) {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= 0xfff7ffff;
        legacy_card = FUN_004ef850(card_on_stack_controller,
                                   card_on_stack,
                                   LEGACY_EFFECT_PUMP,
                                   card_on_stack_controller,
                                   card_on_stack);
        if (legacy_card != -1) {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  } else if (event == EVENT_POW_BOOST) {
    return FUN_004eaf09(player, COLOR_BLUE, 1);
  } else if ((event == EVENT_CAN_WASTE_MANA) && raw_mana_available[player][COLOR_BLUE] != 0) {
    event_result |= 1;
    return 0;
  } else {
    if (event == EVENT_SHOULD_AI_PLAY) {
      if (active_player == player) {
        ai_modifier += basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      } else {
        ai_modifier += basiclandtypes_controlled[player][COLOR_BLUE] * -0xc;
      }
    }

    if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }

    return 0;
  }
}

// FUNCTION: MAGIC 0x0054cc76
// FUNCTION: SHANDALAR 0x0049aeb3
int card_phantasmal_forces(int player, int card, event_t event)
{
  unsigned int *upkeep_flags_ptr;

  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (affected_card == card)) && (affected_card_controller == player)) {
    if ((human_player == player) && (player == unk_00742f60)) {
      upkeep_flags_ptr =
          (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x5c);
      *upkeep_flags_ptr |= 1;
      *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x126) =
          *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x126) + '\x01';
    }
  } else if (event == EVENT_UPKEEP_COSTS_UNPAID) {
    kill_card(card_on_stack_controller, card_on_stack, KILL_BURY);
  } else if ((event == EVENT_SHOULD_AI_PLAY)
             && (basiclandtypes_controlled[player][COLOR_BLUE] < 1)) {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b1ad
// FUNCTION: SHANDALAR 0x004993ea
int card_pirate_ship(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE) {
    result = (unsigned int)((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
                             & 0x20010)
                            == 0);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(1);
    result = 0;
  } else {
    if (event == EVENT_ACTIVATE) {
      if (unk_008a9000 != 1) {
        load_text((int)"prompts.txt", "PIRATE_SHIP");
      }
      FUN_0054ac4d(player, card, 1);
      if (spell_fizzled != 1) {
        state_ptr = (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8);
        *state_ptr |= 0x10;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION) {
      FUN_0054af10(player, card, event, 1);
      *(unsigned char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                         + (int)global_card_instances + 0x36
                         + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                               * 0xb0f4)
          = 0;
    }

    card_sea_serpent(player, card, event);
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x0054a96d
// FUNCTION: SHANDALAR 0x00498baa
int card_prodigal_sorcerer(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE) {
    result = (unsigned int)((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
                             & 0x20010)
                            == 0);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(1);
    result = 0;
  } else if (event == EVENT_ACTIVATE) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "PRODIGAL_SORCERER");
    }
    FUN_0054ac4d(player, card, 1);
    if (spell_fizzled != 1) {
      state_ptr = (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8);
      *state_ptr |= 0x10;
    }
    result = 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    FUN_0054af10(player, card, event, 1);
    *(unsigned char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                       + (int)global_card_instances + 0x36
                       + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                             * 0xb0f4)
        = 0;
    result = 0;
  } else if ((event == EVENT_CHECK_PUMP)
             && ((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
                  & 0x20014)
                 == 0)) {
    --unk_007a7d88[1 - player];
    result = 0;
  } else {
    if ((event == EVENT_SHOULD_AI_PLAY)
        && (player == human_player)
        && (active_player == player)
        && ((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
             & 0x20010)
            == 0)) {
      ai_modifier += 0x18;
    }

    if (((event == EVENT_ATTACK_RATING) && (affected_card == card)) && (affected_card_controller == player)) {
      unk_00925d3c += 0x30;
      result = 0;
    } else if (((event == EVENT_BLOCK_RATING) && (affected_card == card))
               && (affected_card_controller == player)) {
      unk_00925d3c += -0x30;
      result = 0;
    } else {
      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0047a847
// FUNCTION: SHANDALAR 0x0047e344
int card_vesuvan_doppelganger(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0053f22d
// FUNCTION: SHANDALAR 0x0048d477
int card_fungusaur(int player, int card, event_t event)
{
  short *counter_ptr;

  if (event == EVENT_UNKNOWN80) {
    if (((((unk_008b4278 & 0x200) != 0)
          && (*(char *)(affected_card * 300 + affected_card_controller * 0xb0f4
                        + (int)global_card_instances + 0x50)
                 == player))
         && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4
                      + (int)global_card_instances + 4)
                == card))
        && (0 < *(short *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10))) {
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) = 1;
    }
  } else {
    if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (card == affected_card))
         && (player == affected_card_controller))
        && (0 < *(short *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10))) {
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) = 1;
    }
    if ((((trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY))
         && ((card == affected_card)
             && ((player == affected_card_controller)
                 && (*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38)
                     != 0))))
        && (player == current_turn)) {
      if (event == EVENT_TRIGGER) {
        event_result |= 2;
      }
      if ((event == EVENT_RESOLVE_TRIGGER) || (event == EVENT_SHOULD_AI_PLAY)) {
        counter_ptr = (short *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x12);
        *counter_ptr = *counter_ptr + 1;
        counter_ptr = (short *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x1c);
        *counter_ptr = *counter_ptr + 1;
        FUN_005513d7(player, card, 1);
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) = 0;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054926e
// FUNCTION: SHANDALAR 0x004974ab
int card_giant_spider(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES) {
    if (affected_card == card) {
      if (affected_card_controller == player) {
        if (human_player == player) {
          event_result &= ~KEYWORD_FLYING;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cdc0
// FUNCTION: SHANDALAR 0x0049affe
int card_force_of_nature(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS) {
    if (affected_card == card) {
      if (affected_card_controller == player) {
        if (human_player == player) {
          if (unk_00742f60 == player) {
            *(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x5c) |=
                1;
            *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x127) =
                *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x127)
                + '\x04';
          }
        }
      }
    }
  } else if (event == EVENT_UPKEEP_COSTS_UNPAID) {
    load_text((int)"prompts.txt", "FORCE_OF_NATURE");
    do_dialog(player, player, card, -1, -1, text_lines[0], 0);
    damage_player(player, 8, card_on_stack_controller, card_on_stack);
  } else if ((event == EVENT_SHOULD_AI_PLAY)
             && (basiclandtypes_controlled[player][COLOR_GREEN] < 4)) {
    damage_player(player, 8, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cf44
// FUNCTION: SHANDALAR 0x0049b183
int card_llanowar_elves(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE) {
    return (unsigned int)((*(unsigned int *)(card * 300 + player * 0xb0f4
                                             + (int)global_card_instances + 8)
                           & 0x20010)
                          == 0);
  } else if ((event == EVENT_ACTIVATE)
             && ((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
                  & 0x10)
                 == 0)) {
    undeclare_mana_available_and_produce_it(player, COLOR_GREEN, 1);
    *(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8) |= 0x10;
    produced_mana_color = COLOR_GREEN;
  } else if ((event == EVENT_COUNT_MANA) && (affected_card == card) && (affected_card_controller == player)) {
    if ((*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 8)
         & 0x20010)
        == 0) {
      declare_mana_available(player, COLOR_GREEN, 1);
    }
  } else if ((event == EVENT_ATTACK_RATING) && (affected_card == card) && (affected_card_controller == player)) {
    unk_00925d3c += 0x18 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
  } else if ((event == EVENT_BLOCK_RATING) && (affected_card == card) && (affected_card_controller == player)) {
    unk_00925d3c -= 0x60 / (basiclandtypes_controlled[player][COLOR_GREEN] + 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549014
// FUNCTION: SHANDALAR 0x00497258
int card_verduran_enchantress(int player, int card, event_t event)
{
  int library_position;

  if ((((((trigger_condition == TRIGGER_SPELL_CAST) && (affected_card == card))
         && (affected_card_controller == player))
        && ((current_turn == player) && (human_player == player)))
       && (((*(unsigned char *)(player * 0xb0f4 + (int)global_card_instances + card * 300 + 8) & 0x20)
            == 0)
           && ((affected_card_controller == player)
               && (*(int *)(trigger_cause_controller * 0xb0f4 + (int)global_card_instances
                            + trigger_cause * 300 + 0x6c)
                   != -1))))
      && ((global_cards_data[*(int *)(trigger_cause_controller * 0xb0f4 + (int)global_card_instances
                                      + trigger_cause * 300 + 0x6c)]
               .type
           & TYPE_ENCHANTMENT)
          != 0)) {
    if (event == EVENT_TRIGGER) {
      if ((unk_008b35ec == player) || ((unk_00926804 & 2) != 0)) {
        event_result |= RESOLVE_TRIGGER_OPTIONAL;
      } else {
        library_position = 0;
        while ((library_position < 500) && (global_library[player][library_position] != -1)) {
          ++library_position;
        }

        if ((hand_count[player] < 8) && (5 < library_position)) {
          event_result |= RESOLVE_TRIGGER_MANDATORY;
        } else {
          event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER) {
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
      FUN_0043e18b(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549404
// FUNCTION: SHANDALAR 0x00497640
int card_scathe_zombies(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      || (event == EVENT_RESOLVE_ACTIVATION)) {
    result = FUN_0054276d(player, card, event, 1, 1);
  } else {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x005492be
// FUNCTION: SHANDALAR 0x004974fb
int card_zombie_master(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_ABILITIES) {
    if (is_in_play(player, card)) {
      if (global_cards_data[PLAYER_CARD_INSTANCE(affected_card_controller, affected_card)
                                .internal_card_id]
              .subtype
          == SUB_ZOMBIE) {
        color = get_hacked_color(player, card, COLOR_BLACK);
        event_result |= (1 << (((char)color - 1U) & 0x1f)) + KEYWORD_REGENERATION;
      }
    }
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card == card))
      && (player == affected_card_controller)) {
    FUN_0055117d(FUN_005493a6, -1);
    TENTATIVE_reassess_all_cards();
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543640
// FUNCTION: SHANDALAR 0x00491883
int card_frozen_shade(int player, int card, event_t event)
{
  int *info_slot_ptr;
  int result;
  unsigned int *state_ptr;

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) = 0;
    *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) =
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c);
    result = 0;
  } else if (event == EVENT_UNTAP_PHASE) {
    ++unk_00939520[player][COLOR_BLACK];
    result = 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    result = FUN_004eaf09(player, COLOR_BLACK, 1);
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e503e(0);
    result = 0;
  } else if (event == EVENT_ACTIVATE) {
    result = FUN_004eaf09(player, COLOR_BLACK, 1);
    if (result != 0) {
      if (player == human_player) {
        FUN_00432f00(player, COLOR_BLACK, -1);
        if (x_value < 1) {
          spell_fizzled = 1;
        } else {
          *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) = x_value;
        }
      } else {
        FUN_00432f00(player, COLOR_BLACK, 1);
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) = 1;
      }

      if (spell_fizzled == 1) {
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) = 0;
      } else {
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c) = player;
        *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) = card;
        *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x36) = 1;
        if (*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) == 0) {
          state_ptr = (unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38);
          *state_ptr |= 0x80000;
        }
      }
    }
    result = 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    if (*(int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                 + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                       * 0xb0f4
                 + (int)global_card_instances + 0x6c)
        == -1) {
      spell_fizzled = 1;
    } else {
      info_slot_ptr =
          (int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                  + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                        * 0xb0f4
                  + (int)global_card_instances + 0x38);
      *info_slot_ptr += *(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                          + 0x4c)
                       & 0xff;
      info_slot_ptr =
          (int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                  + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                        * 0xb0f4
                  + (int)global_card_instances + 0x38);
      *info_slot_ptr += (*(unsigned int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                           + 0x4c)
                        & 0xff)
                       * 0x100;
      *(char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110) * 300
                + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x10c)
                      * 0xb0f4
                + (int)global_card_instances + 0x36)
          = 0;
      if ((*(unsigned char *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x110)
                              * 300
                              + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                         + 0x10c)
                                    * 0xb0f4
                              + (int)global_card_instances + 0x3a)
           & 8)
          != 0) {
        state_ptr =
            (unsigned int *)(*(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                      + 0x110)
                             * 300
                             + *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances
                                        + 0x10c)
                                   * 0xb0f4
                             + (int)global_card_instances + 0x38);
        *state_ptr &= 0xfff7ffff;
        result = FUN_004ef850(card_on_stack_controller,
                              card_on_stack,
                              LEGACY_EFFECT_PUMP,
                              card_on_stack_controller,
                              card_on_stack);
        if (result != -1) {
          *(short *)(result * 300 + player * 0xb0f4 + (int)global_card_instances + 0x12) = 1;
          *(short *)(result * 300 + player * 0xb0f4 + (int)global_card_instances + 0x1c) = 1;
          state_ptr =
              (unsigned int *)(result * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38);
          *state_ptr |= 0x80000;
        }
      }
    }
    result = 0;
  } else if (event == EVENT_POW_BOOST) {
    result = FUN_004eaf09(player, COLOR_BLACK, 1);
  } else if (event == EVENT_TOU_BOOST) {
    result = FUN_004eaf09(player, COLOR_BLACK, 1);
  } else if ((event == EVENT_CAN_WASTE_MANA) && (raw_mana_available[player][COLOR_BLACK] != 0)) {
    event_result |= 1;
    result = 0;
  } else {
    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY)) {
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) = 0;
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) =
          *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c);
    }
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00541740
// FUNCTION: SHANDALAR 0x0048f983
int card_nightmare(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  int legacy_effect_card;

  if (event == EVENT_RESOLVE_SPELL) {
    legacy_effect_card = FUN_004ef850(player, card, unk_0093d848, player, card);
    if (legacy_effect_card != -1) {
      *(int *)(legacy_effect_card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x38) = 1;
      *(int *)(legacy_effect_card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x4c) =
          0x10d;
      *(int *)(legacy_effect_card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x18) =
          0x10020;
      state_ptr = (unsigned int *)(legacy_effect_card * 300 + player * 0xb0f4
                                   + (int)global_card_instances + 0x18);
      *state_ptr |= 0x2000000;
      *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc) = (char)player;
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x48) =
          legacy_effect_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005460cb
// FUNCTION: SHANDALAR 0x00494312
int card_royal_assassin(int player, int card, event_t event)
{
  int result;
  target_t selected_target;

  if (event == EVENT_CAN_ACTIVATE) {
    result = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0) {
      result = FUN_004bd7d0((int *)0,
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
                            1,
                            0);
    }
    return result;
  } else if (event == EVENT_ACTIVATE) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "ROYAL_ASSASSIN");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
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
                             TARGET_STATE_TAPPED,
                             0,
                             text_lines[0],
                             1,
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  } else if (event == EVENT_GET_SELECTED_CARD) {
    FUN_004e4ff3(0);
    return 0;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (FUN_004bdc06(selected_target.player,
                     selected_target.card,
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
                     1,
                     0)
        == 0) {
      spell_fizzled = 1;
    } else {
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  } else if (((event == EVENT_ATTACK_RATING) && (affected_card == card))
             && (affected_card_controller == player)) {
    unk_00925d3c += 0x30;
    return 0;
  } else if (((event == EVENT_BLOCK_RATING) && (affected_card == card))
             && (affected_card_controller == player)) {
    unk_00925d3c += -0x30;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d4c9
// FUNCTION: SHANDALAR 0x0048b71c
int card_hypnotic_specter(int player, int card, event_t event)
{
  if ((event == EVENT_DEAL_DAMAGE)
      && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4 + (int)global_card_instances
                   + 0x6c)
          == unk_009266a4)) {
    if ((*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4 + (int)global_card_instances
                  + 4)
         == -1)
        && (*(char *)(affected_card * 300 + affected_card_controller * 0xb0f4
                      + (int)global_card_instances + 0xc)
            == player)
        && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4 + (int)global_card_instances
                     + 0x48)
            == card)
        && (*(int *)(affected_card * 300 + affected_card_controller * 0xb0f4 + (int)global_card_instances
                     + 0x38)
            != 0)) {
      *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc) =
          (char)affected_card_controller;
      *(int *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0x48) = affected_card;
    }
  } else if ((((trigger_condition == TRIGGER_DEAL_DAMAGE) && (affected_card == card))
              && (affected_card_controller == player)
              && (*(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc) != -1))
             && (player == current_turn)) {
    if (event == EVENT_TRIGGER) {
      event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER) {
      discard((int)*(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc), 1, 0);
      *(char *)(card * 300 + player * 0xb0f4 + (int)global_card_instances + 0xc) = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005237dc
// FUNCTION: SHANDALAR 0x004dafcb
int card_bad_moon(int player, int card, event_t event)
{
  if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS))
      && is_in_play(player, card)
      && is_in_play(affected_card_controller, affected_card)
      && (((1 << (char)get_sleighted_color(player, card, COLOR_BLACK))
           & *(char *)(affected_card * 300 + affected_card_controller * 0xb0f4
                       + (int)global_card_instances + 0x1e))
          != 0)) {
    ++event_result;
  }

  return (unsigned int)(event == EVENT_CAN_CAST);
}

// FUNCTION: MAGIC 0x00520ab6
// FUNCTION: SHANDALAR 0x004d829b
int card_cursed_land(int player, int card, event_t event)
{
  int ai_tweak;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        1,
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
  } else if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "CURSED_LAND");
    }
    spell_fizzled = (unsigned int)(FUN_00551b60(player, 1 - player, card) == 0);
    if (spell_fizzled != 1) {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec) {
        ai_modifier += 0x30;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player) {
        ai_modifier += -0x60;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     1,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player))
        && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player)
        && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0)) {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  } else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card))
             && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  } else if (event == EVENT_UPKEEP_COSTS_UNPAID) {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  } else if (event == EVENT_CLEANUP) {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  } else {
    if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card)
        && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1)) {
      ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
      if (ai_tweak < 2) {
        ai_tweak = 1;
      }
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec) {
        ai_modifier += ai_tweak * 0x18;
      } else {
        ai_modifier += ai_tweak * -0x18;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00521465
// FUNCTION: SHANDALAR 0x004d8c4a
int card_evil_presence(int player, int card, event_t event)
{
  int land_type;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        1,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "EVIL_PRESENCE");
    }
    spell_fizzled = (unsigned int)(FUN_00551b60(player, 1 - player, card) == 0);
    if (spell_fizzled != 1) {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec) {
        ai_modifier += 0x40 / (basiclandtypes_controlled[unk_008b35ec][7] + 1);
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player) {
        ai_modifier += -0x60;
      }
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card)
              .internal_card_id
          == 0) {
        ai_modifier += -0x90;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     1,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
      land_type = FUN_005001e0(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .internal_card_id = land_type - 1;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x1000000;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x200) == 0))
         && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
             && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
                 && (affected_card != -1))))
        && is_in_play(player, card)) {
      land_type = FUN_005001e0(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
      event_result = land_type - 1;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0052dc98
// FUNCTION: SHANDALAR 0x004e5495
int card_unholy_strength(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
      && (unk_008a9000 != 1)) {
    load_text((int)"prompts.txt", "UNHOLY_STRENGTH");
  }
  return FUN_0052dd74(player, card, event, 2, 1);
}

// FUNCTION: MAGIC 0x0052dd05
// FUNCTION: SHANDALAR 0x004e5502
int card_weakness(int player, int card, event_t event)
{
  if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
      && (unk_008a9000 != 1)) {
    load_text((int)"prompts.txt", "WEAKNESS");
  }
  return FUN_0052dd74(player, card, event, -2, -1);
}

// FUNCTION: MAGIC 0x005301b4
// FUNCTION: SHANDALAR 0x004e79b0
int card_warp_artifact(int player, int card, event_t event)
{
  int ai_tweak;
  target_t selected_target;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        0x40,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "WARP_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec) {
        ai_modifier +=
            ((char)global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                         .internal_card_id]
                 .cc[1]
             * 3
             + 3)
            * 4;
      } else {
        ai_modifier += -0x60;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     0x40,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
          && ((unk_00926804 & 2) == 0)) {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state |= 0x40000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player))
        && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player)
        && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0)) {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  } else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card))
             && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  } else if (event == EVENT_UPKEEP_COSTS_UNPAID) {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  } else if (event == EVENT_CLEANUP) {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  } else {
    if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card)
        && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1)) {
      ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
      if (ai_tweak < 2) {
        ai_tweak = 1;
      }
      if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec) {
        ai_modifier += ai_tweak * 0x18;
      } else {
        ai_modifier += ai_tweak * -0x18;
      }
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x005335b5
// FUNCTION: SHANDALAR 0x004eadb0
int card_phantasmal_terrain(int player, int card, event_t event)
{
  int land_type;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        1,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "PHANTASMAL_TERRAIN");
    }
    if (FUN_00551b60(player, 1 - player, card) == 0) {
      spell_fizzled = 1;
    } else {
      if ((player == unk_008b35ec) || ((unk_00926804 & 2) != 0)) {
        if (unk_008a9000 != 1) {
          load_text((int)"prompts.txt", "PHANTASMAL_TERRAIN2");
        }
        land_type = choose_a_color(player,
                                   text_lines[1],
                                   0,
                                   -1,
                                   COLOR_TEST_WHITE | COLOR_TEST_RED | COLOR_TEST_GREEN
                                       | COLOR_TEST_BLUE | COLOR_TEST_BLACK);
        if (land_type == -1) {
          spell_fizzled = 1;
        }
      } else if (unk_008a9000 == 1) {
        land_type = FUN_00464a57(5) + 1;
        unk_00939340 = land_type;
        FUN_004e4f11();
      } else {
        FUN_004e5089();
        land_type = unk_00939340;
      }

      if (spell_fizzled != 1) {
        if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player) {
          ai_modifier += -0x30;
        }
        PLAYER_CARD_INSTANCE(player, card).info_slot = land_type;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     1,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      --PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .internal_card_id = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .regen_status |= 0x1000000;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x200) == 0))
         && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
             && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
                 && (affected_card != -1))))
        && is_in_play(player, card)) {
      event_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0052f80a
// FUNCTION: SHANDALAR 0x004e7006
int card_copy_artifact(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00534336
// FUNCTION: SHANDALAR 0x004ebb33
int card_flight(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST) {
    FUN_004e4ff3(0);
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "FLIGHT");
    }
    if (FUN_00551638(player, player, card) == 0) {
      spell_fizzled = 1;
    } else {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec) {
        ai_modifier += -0x18;
      }
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                PLAYER_CARD_INSTANCE(player, card).targets[0].card)
               .regen_status
           & KEYWORD_FLYING)
          != 0) {
        ai_modifier += -99;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if (((PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
         && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card))
        && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
            && ((affected_card != -1) && (event == EVENT_ABILITIES)))) {
      event_result |= KEYWORD_FLYING;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00522a4f
// FUNCTION: SHANDALAR 0x004da23c
int card_psychic_venom(int player, int card, event_t event)
{
  int land_color;
  target_t selected_target;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        1,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "PSYCHIC_VENOM");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
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
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec) {
        land_color = get_color_from_color_test(
            global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                  .internal_card_id]
                .color);
        ai_modifier += 0x60 / (basiclandtypes_controlled[unk_008b35ec][land_color] + 1);
      }
      if (selected_target.player == active_player) {
        ai_modifier += -0x60;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     1,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      if ((PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player)
          && ((unk_00926804 & 2) == 0)) {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card)
            .state |= 0x40000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if (((event == EVENT_TAP_CARD)
         && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card))
        && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
            && (affected_card != -1))) {
      damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player, 2, player, card);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00524db3
// FUNCTION: SHANDALAR 0x004dc5a1
int card_creature_bond(int player, int card, event_t event)
{
  int effect_card;

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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "CREATUREBOND");
    }
    spell_fizzled = (unsigned int)(FUN_00551638(player, 1 - player, card) == 0);
    if (spell_fizzled != 1) {
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == unk_008b35ec) {
        ai_modifier += 0x18;
      }
      if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == active_player) {
        ai_modifier += -0x60;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if (((event == EVENT_GRAVEYARD_FROM_PLAY)
         && (PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card))
        && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
            && ((affected_card != -1)
                && ((effect_card = add_card_to_hand(player, unk_00939348)) != -1)))) {
      PLAYER_CARD_INSTANCE(player, effect_card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, effect_card).state |= F08_INPLAY;
      *(int *)&PLAYER_CARD_INSTANCE(player, effect_card).display_pic_csv_id = 0x32;
      PLAYER_CARD_INSTANCE(player, effect_card).info_slot =
          C_get_abilities(affected_card_controller, affected_card, EVENT_TOUGHNESS, -1);
      PLAYER_CARD_INSTANCE(player, effect_card).damage_target_player = (char)affected_card_controller;
      TENTATIVE_set_timestamps(player, effect_card);
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0051a64c
// FUNCTION: SHANDALAR 0x004d1e33
int card_animate_artifact(int player, int card, event_t event)
{
  int animated_internal_card_id;
  target_t selected_target;

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        0x40,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "ANIMATE_ARTIFACT");
    }
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
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
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      ai_modifier += -0x18;
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     0x40,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
      animated_internal_card_id = create_a_card_type(
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_target_card)
              .internal_card_id);
      if (animated_internal_card_id != -1) {
        if ((global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                                    PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                                   .internal_card_id]
                 .type
             & 0x42)
            == 0x40) {
          global_cards_data[animated_internal_card_id].toughness =
              (short)(char)global_cards_data
                  [PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                                        PLAYER_CARD_INSTANCE(player, card).damage_target_card)
                       .internal_card_id]
                      .cc[1];
          global_cards_data[animated_internal_card_id].power =
              global_cards_data[animated_internal_card_id].toughness;
        }
        global_cards_data[animated_internal_card_id].type = 0x42;
        PLAYER_CARD_INSTANCE(player, card).info_slot = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .internal_card_id = animated_internal_card_id;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_target_card)
            .regen_status |= 0x1000000;
      }
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else {
    if ((((event == EVENT_CHANGE_TYPE) && ((unk_008b4278 & 0x200) == 0))
         && ((PLAYER_CARD_INSTANCE(player, card).damage_target_card == affected_card)
             && (((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player == affected_card_controller)
                 && (affected_card != -1))))
        && is_in_play(player, card)) {
      event_result = PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_target_card)
          .token_status |= 0x40;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x0051d83c
// FUNCTION: SHANDALAR 0x004d5020
int card_feedback(int player, int card, event_t event)
{
  int ai_tweak;
  target_t selected_target;

  if ((event == EVENT_SHOULD_AI_PLAY) && is_in_play(player, card)
      && (PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1)) {
    ai_tweak = 0x18 - life[PLAYER_CARD_INSTANCE(player, card).damage_target_player];
    if (ai_tweak < 2) {
      ai_tweak = 1;
    }
    if (PLAYER_CARD_INSTANCE(player, card).damage_target_player == unk_008b35ec) {
      ai_modifier += ai_tweak * 0x18;
    } else {
      ai_modifier += ai_tweak * -0x18;
    }
  }

  if (event == EVENT_CAN_CAST) {
    return FUN_004bd7d0((int *)0,
                        0,
                        player,
                        2,
                        2,
                        0x200,
                        4,
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
  } else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller)) {
    if (unk_008a9000 != 1) {
      load_text((int)"prompts.txt", "FEEDBACK");
    }
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ENCHANTMENT,
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
                             &selected_target)
        == 0) {
      spell_fizzled = 1;
    } else {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (selected_target.player == unk_008b35ec) {
        ai_modifier += 0x30;
      }
      if (selected_target.player == active_player) {
        ai_modifier += -0x60;
      }
    }
    return 0;
  } else if (event == EVENT_RESOLVE_SPELL) {
    if (FUN_004bdc06(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                     PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                     (char *)0,
                     player,
                     2,
                     2,
                     0x200,
                     4,
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
      PLAYER_CARD_INSTANCE(player, card).damage_target_player =
          (char)PLAYER_CARD_INSTANCE(player, card).targets[0].player;
      PLAYER_CARD_INSTANCE(player, card).damage_target_card =
          PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    return 0;
  } else if (event == EVENT_CAN_ACTIVATE) {
    if (((current_phase == EVENT_UPKEEP_PHASE) && (unk_00742f60 == human_player))
        && (PLAYER_CARD_INSTANCE(player, card).damage_target_player == human_player)
        && ((PLAYER_CARD_INSTANCE(player, card).state & 1) == 0)) {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 0x101;
      unk_008b3270 |= 3;
      return 1;
    }
    return 0;
  } else if (((event == EVENT_UPKEEP_PHASE) && (affected_card == card))
             && (affected_card_controller == player)) {
    PLAYER_CARD_INSTANCE(player, card).state |= 1;
    unk_007a7c1c = 1;
    event_result |= 1;
    return 0;
  } else if (event == EVENT_UPKEEP_COSTS_UNPAID) {
    damage_player((int)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                  1,
                  card_on_stack_controller,
                  card_on_stack);
    return 0;
  } else if (event == EVENT_CLEANUP) {
    PLAYER_CARD_INSTANCE(player, card).state &= 0xfffffffe;
    return 0;
  } else {
    return 0;
  }
}

// FUNCTION: MAGIC 0x00523399
// FUNCTION: SHANDALAR 0x004dab86
int card_lifetap(int player, int card, event_t event)
{
  int color;

  if (event == EVENT_CAN_CAST) {
    return 1;
  } else {
    if ((event == EVENT_TAP_CARD) && (affected_card_controller != player)) {
      color = FUN_005001e0(player, card, 3);
      if (FUN_0048463d(affected_card_controller, affected_card, color) != 0) {
        gain_life(player, 1);
        return 0;
      }
    }
    if ((((event == EVENT_CAST_SPELL) || (event == EVENT_SHOULD_AI_PLAY)) && (affected_card == card))
        && (affected_card_controller == player)) {
      color = FUN_005001e0(player, card, 3);
      ai_modifier += (basiclandtypes_controlled[unk_008b35ec][color] * 3 + 3) * 8;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00533e91
// FUNCTION: SHANDALAR 0x004eb68f
int card_wild_growth(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0053152d
// FUNCTION: SHANDALAR 0x004e8d27
int card_regeneration(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005293ba
// FUNCTION: SHANDALAR 0x004e0bab
int card_web(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052b9f0
// FUNCTION: SHANDALAR 0x004e31e9
int card_wanderlust(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052c052
// FUNCTION: SHANDALAR 0x004e384b
int card_instill_energy(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005347a7
// FUNCTION: SHANDALAR 0x004ebfa3
int card_lifeforce(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00523a60
// FUNCTION: SHANDALAR 0x004db252
int card_aspect_of_wolf(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00521abd
// FUNCTION: SHANDALAR 0x004d92a4
int card_living_artifact(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004385c3
// FUNCTION: SHANDALAR 0x004faf62
int card_earthbind(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052b842
// FUNCTION: SHANDALAR 0x004e303a
int card_burrowing(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00527e71
// FUNCTION: SHANDALAR 0x004df65e
int card_firebreathing(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00522fdc
// FUNCTION: SHANDALAR 0x004da7c9
int card_manabarbs(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005238a7
// FUNCTION: SHANDALAR 0x004db096
int card_orcish_oriflamme(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005230ea
// FUNCTION: SHANDALAR 0x004da8d7
int card_mana_flare(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00526202
// FUNCTION: SHANDALAR 0x004dd9ef
int card_holy_armor(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e233
// FUNCTION: SHANDALAR 0x004e5a30
int card_castle(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052dae0
// FUNCTION: SHANDALAR 0x004e52db
int card_holy_strength(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e2ed
// FUNCTION: SHANDALAR 0x004e5aea
int card_black_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e324
// FUNCTION: SHANDALAR 0x004e5b21
int card_green_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e35b
// FUNCTION: SHANDALAR 0x004e5b58
int card_blue_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e392
// FUNCTION: SHANDALAR 0x004e5b8f
int card_red_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052e3c9
// FUNCTION: SHANDALAR 0x004e5bc6
int card_white_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052112c
// FUNCTION: SHANDALAR 0x004d8911
int card_karma(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00438b5c
// FUNCTION: SHANDALAR 0x004fb4fd
int card_farmstead(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00532b0e
// FUNCTION: SHANDALAR 0x004ea30b
int card_co_p_white(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00532ad7
// FUNCTION: SHANDALAR 0x004ea2d4
int card_co_p_black(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00532b7c
// FUNCTION: SHANDALAR 0x004ea379
int card_co_p_blue(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00532b45
// FUNCTION: SHANDALAR 0x004ea342
int card_co_p_red(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00532bb3
// FUNCTION: SHANDALAR 0x004ea3b0
int card_co_p_green(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052398e
// FUNCTION: SHANDALAR 0x004db180
int card_crusade(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ab9c2
// FUNCTION: SHANDALAR 0x004a3232
int card_blaze_of_glory(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052700b
// FUNCTION: SHANDALAR 0x004de7f8
int card_blessing(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00533bc2
// FUNCTION: SHANDALAR 0x004eb3bf
int card_conversion(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00408b2d
// FUNCTION: SHANDALAR 0x0044e84e
int card_wrath_of_god(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00403a6b
// FUNCTION: SHANDALAR 0x00449791
int card_armageddon(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040bdc6
// FUNCTION: SHANDALAR 0x004024fc
int card_resurrection(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00406d4f
// FUNCTION: SHANDALAR 0x0044ca6b
int card_raise_dead(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004097ba
// FUNCTION: SHANDALAR 0x0044f4db
int card_drain_life(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004019b4
// FUNCTION: SHANDALAR 0x004476d4
int card_braingeyser(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040948b
// FUNCTION: SHANDALAR 0x0044f1ac
int card_disintegrate(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00409df5
// FUNCTION: SHANDALAR 0x0044fb16
int card_stone_rain(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00403557
// FUNCTION: SHANDALAR 0x0044927d
int card_earthquake(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00404dce
// FUNCTION: SHANDALAR 0x0044aaee
int card_fireball(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00408c24
// FUNCTION: SHANDALAR 0x0044e945
int card_flashfires(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004037e1
// FUNCTION: SHANDALAR 0x00449507
int card_hurricane(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00402c15
// FUNCTION: SHANDALAR 0x00448939
int card_tranquility(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040293f
// FUNCTION: SHANDALAR 0x00448662
int card_stream_of_life(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040727c
// FUNCTION: SHANDALAR 0x0044cf98
int card_regrowth(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00403b62
// FUNCTION: SHANDALAR 0x00449888
int card_tsunami(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00465b27
// FUNCTION: SHANDALAR 0x00455247
int card_basalt_monolith(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041598f
// FUNCTION: SHANDALAR 0x005181a3
int card_conservator(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00467167
// FUNCTION: SHANDALAR 0x00456897
int card_gauntlet_of_might(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004161ad
// FUNCTION: SHANDALAR 0x005189c3
int card_iron_star(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004161d3
// FUNCTION: SHANDALAR 0x005189e9
int card_ivory_cup(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004161f9
// FUNCTION: SHANDALAR 0x00518a0f
int card_crystal_rod(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041653e
// FUNCTION: SHANDALAR 0x00518d54
int card_ankh_of_mishra(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00416dd5
// FUNCTION: SHANDALAR 0x005195e9
int card_dingus_egg(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00419295
// FUNCTION: SHANDALAR 0x0051baa5
int card_jade_monolith(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00417d1f
// FUNCTION: SHANDALAR 0x0051a531
int card_jayemdae_tome(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00417f48
// FUNCTION: SHANDALAR 0x0051a759
int card_mana_vault(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004188ba
// FUNCTION: SHANDALAR 0x0051b0c8
int card_meekstone(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041b801
// FUNCTION: SHANDALAR 0x0051e01b
int card_rod_of_ruin(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004186d8
// FUNCTION: SHANDALAR 0x0051aee6
int card_sol_ring(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00417228
// FUNCTION: SHANDALAR 0x00519a3d
int card_soul_net(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041621f
// FUNCTION: SHANDALAR 0x00518a35
int card_throne_of_bone(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00416245
// FUNCTION: SHANDALAR 0x00518a5b
int card_wooden_sphere(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041ba8b
// FUNCTION: SHANDALAR 0x0051e2a5
int card_winter_orb(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041d4f9
// FUNCTION: SHANDALAR 0x0051fd11
int card_helm_of_chatzuk(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00415072
// FUNCTION: SHANDALAR 0x00517888
int card_howling_mine(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041f3d3
// FUNCTION: SHANDALAR 0x00521be8
int card_sunglasses_of_urza(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041c706
// FUNCTION: SHANDALAR 0x0051ef24
int card_clockwork_beast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00468878
// FUNCTION: SHANDALAR 0x00457f9e
int card_living_wall(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f6f2a
// FUNCTION: SHANDALAR 0x004b0fe3
int card_disenchant(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004abcd8
// FUNCTION: SHANDALAR 0x004a354a
int card_guardian_angel(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f8b77
// FUNCTION: SHANDALAR 0x004b2c3e
int card_death_ward(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f8305
// FUNCTION: SHANDALAR 0x004b23c6
int card_righteousness(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f895d
// FUNCTION: SHANDALAR 0x004b2a22
int card_swords_to_plowshares(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fdfb2
// FUNCTION: SHANDALAR 0x004b80dc
int card_healing_salve(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ac52b
// FUNCTION: SHANDALAR 0x004a3da3
int card_psionic_blast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f94f3
// FUNCTION: SHANDALAR 0x004b35bf
int card_jump(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fa4eb
// FUNCTION: SHANDALAR 0x004b45ba
int card_unsummon(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f64d5
// FUNCTION: SHANDALAR 0x004b0590
int card_ancestral_recall(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f6108
// FUNCTION: SHANDALAR 0x004b01c2
int card_mana_short(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f7c84
// FUNCTION: SHANDALAR 0x004b1d3f
int card_howl_from_beyond(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f9b91
// FUNCTION: SHANDALAR 0x004b3c5f
int card_terror(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f6d49
// FUNCTION: SHANDALAR 0x004b0e02
int card_shatter(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f7857
// FUNCTION: SHANDALAR 0x004b1910
int card_tunnel(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f9e9e
// FUNCTION: SHANDALAR 0x004b3f6b
int card_lightning_bolt(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f801d
// FUNCTION: SHANDALAR 0x004b20dc
int card_berserk(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fa213
// FUNCTION: SHANDALAR 0x004b42e2
int card_giant_growth(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f66bc
// FUNCTION: SHANDALAR 0x004b0777
int card_simulacrum(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fa7dc
// FUNCTION: SHANDALAR 0x004b48ac
int card_purelace(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0043c1a5
// FUNCTION: SHANDALAR 0x004feb50
int card_raging_river(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fc353
// FUNCTION: SHANDALAR 0x004b647a
int card_blue_elemental_blast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fc90c
// FUNCTION: SHANDALAR 0x004b6a32
int card_counterspell(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fd465
// FUNCTION: SHANDALAR 0x004b7590
int card_spell_blast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ad5b4
// FUNCTION: SHANDALAR 0x004a4e37
int card_sacrifice(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fdeec
// FUNCTION: SHANDALAR 0x004b8014
int card_dark_ritual(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fd933
// FUNCTION: SHANDALAR 0x004b7a5b
int card_red_elemental_blast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040cfa0
// FUNCTION: SHANDALAR 0x0050f7c0
int card_mox_emerald(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040cfc6
// FUNCTION: SHANDALAR 0x0050f7e6
int card_mox_jet(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040cfec
// FUNCTION: SHANDALAR 0x0050f80c
int card_mox_pearl(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040d012
// FUNCTION: SHANDALAR 0x0050f832
int card_mox_ruby(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040d038
// FUNCTION: SHANDALAR 0x0050f858
int card_mox_sapphire(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040d05e
// FUNCTION: SHANDALAR 0x0050f87e
int card_black_lotus(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00411981
// FUNCTION: SHANDALAR 0x0051419a
int card_celestial_prism(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00466435
// FUNCTION: SHANDALAR 0x00455b59
int card_copper_tablet(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00414cec
// FUNCTION: SHANDALAR 0x00517500
int card_disrupting_scepter(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004675a1
// FUNCTION: SHANDALAR 0x00456cd1
int card_icy_manipulator(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00436955
// FUNCTION: SHANDALAR 0x004f92f9
int card_consecrate_land(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0043aee2
// FUNCTION: SHANDALAR 0x004fd888
int card_lance(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004feb03
// FUNCTION: SHANDALAR 0x004b8c28
int card_reverse_damage(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00467a6c
// FUNCTION: SHANDALAR 0x00457199
int card_jade_statue(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005424a4
// FUNCTION: SHANDALAR 0x004906e7
int card_plague_rats(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054e3ef
// FUNCTION: SHANDALAR 0x0049c630
int card_black_knight(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00474f4d
// FUNCTION: SHANDALAR 0x00478a33
int card_juggernaut(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00439f56
// FUNCTION: SHANDALAR 0x004fc8fa
int card_invisibility(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00528bc0
// FUNCTION: SHANDALAR 0x004e03af
int card_fear(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00471838
// FUNCTION: SHANDALAR 0x00475328
int card_dwarven_demolition_team(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00547bb3
// FUNCTION: SHANDALAR 0x00495df9
int card_dwarven_warriors(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054d153
// FUNCTION: SHANDALAR 0x0049b392
int card_birds_of_paradise(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00475b66
// FUNCTION: SHANDALAR 0x0047964f
int card_nettling_imp(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054c131
// FUNCTION: SHANDALAR 0x0049a36e
int card_sengir_vampire(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054d8ca
// FUNCTION: SHANDALAR 0x0049bb0a
int card_lord_of_the_pit(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054df54
// FUNCTION: SHANDALAR 0x0049c195
int card_nether_shadow(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041ae2a
// FUNCTION: SHANDALAR 0x0051d63a
int card_nevinyrral_s_disk(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052a3fd
// FUNCTION: SHANDALAR 0x004e1bf2
int card_paralyze(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005195e5
// FUNCTION: SHANDALAR 0x004d0dc7
int card_animate_dead(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004076dc
// FUNCTION: SHANDALAR 0x0044d3fa
int card_demonic_tutor(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040808e
// FUNCTION: SHANDALAR 0x0044ddad
int card_mind_twist(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052d2d4
// FUNCTION: SHANDALAR 0x004e4ace
int card_pestilence(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041e45a
// FUNCTION: SHANDALAR 0x00520c71
int card_the_hive(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004146f4
// FUNCTION: SHANDALAR 0x00516f08
int card_forcefield(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0051f540
// FUNCTION: SHANDALAR 0x004d6d23
int card_power_leak(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040a06a
// FUNCTION: SHANDALAR 0x0044fd8c
int card_drain_power(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f71db
// FUNCTION: SHANDALAR 0x004b1292
int card_twiddle(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0047a22a
// FUNCTION: SHANDALAR 0x0047dd1e
int card_two_headed_giant_of_foriys(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00529afb
// FUNCTION: SHANDALAR 0x004e12ec
int card_stasis(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054bfaf
// FUNCTION: SHANDALAR 0x0049a1eb
int card_scavenging_ghoul(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052b3d4
// FUNCTION: SHANDALAR 0x004e2bc8
int card_power_surge(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0052af2f
// FUNCTION: SHANDALAR 0x004e2723
int card_smoke(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00401c46
// FUNCTION: SHANDALAR 0x00447966
int card_wheel_of_fortune(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004024c5
// FUNCTION: SHANDALAR 0x004481e7
int card_channel(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004391fe
// FUNCTION: SHANDALAR 0x004fbb9f
int card_fastbond(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054e986
// FUNCTION: SHANDALAR 0x0049cbc7
int card_ley_druid(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00523ef2
// FUNCTION: SHANDALAR 0x004db6e2
int card_lure(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0054f7fe
// FUNCTION: SHANDALAR 0x0049da3f
int card_thicket_basilisk(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fafeb
// FUNCTION: SHANDALAR 0x004b50c0
int card_magical_hack(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fba06
// FUNCTION: SHANDALAR 0x004b5b0c
int card_sleight_of_mind(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040db0e
// FUNCTION: SHANDALAR 0x0051032b
int card_black_vise(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040a8d0
// FUNCTION: SHANDALAR 0x00401000
int card_contract_from_below(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00402229
// FUNCTION: SHANDALAR 0x00447f4b
int card_timetwister(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004013ed
// FUNCTION: SHANDALAR 0x0044710d
int card_time_walk(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040d52f
// FUNCTION: SHANDALAR 0x0050fd4d
int card_time_vault(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040159b
// FUNCTION: SHANDALAR 0x004472bb
int card_balance(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0046667e
// FUNCTION: SHANDALAR 0x00455da6
int card_cyclopean_tomb(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00517382
// FUNCTION: SHANDALAR 0x004ceb62
int card_island_sanctuary(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0053e28a
// FUNCTION: SHANDALAR 0x0048c4db
int card_personal_incarnation(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0051baf5
// FUNCTION: SHANDALAR 0x004d32db
int card_control_magic(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0051bb62
// FUNCTION: SHANDALAR 0x004d3348
int card_steal_artifact(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f6456
// FUNCTION: SHANDALAR 0x004b0512
int card_siren_s_call(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00402d0c
// FUNCTION: SHANDALAR 0x00448a30
int card_volcanic_eruption(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040a9ed
// FUNCTION: SHANDALAR 0x0040111d
int card_darkpact(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00470e7e
// FUNCTION: SHANDALAR 0x00474970
int card_demonic_hordes(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0040aee0
// FUNCTION: SHANDALAR 0x00401611
int card_demonic_attorney(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f5d83
// FUNCTION: SHANDALAR 0x004afe3b
int card_fork(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00541b88
// FUNCTION: SHANDALAR 0x0048fdcb
int card_gaea_s_liege(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0043a7ae
// FUNCTION: SHANDALAR 0x004fd152
int card_kudzu(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00517dc9
// FUNCTION: SHANDALAR 0x004cf5a7
int card_living_lands(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00517a2c
// FUNCTION: SHANDALAR 0x004cf20b
int card_kormus_bell(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ac114
// FUNCTION: SHANDALAR 0x004a3988
int card_natural_selection(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041ea45
// FUNCTION: SHANDALAR 0x0052125a
int card_glasses_of_urza(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0041edf5
// FUNCTION: SHANDALAR 0x0052160a
int card_library_of_leng(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0043af43
// FUNCTION: SHANDALAR 0x004fd8eb
int card_lich(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x0051b55f
// FUNCTION: SHANDALAR 0x004d2d45
int card_animate_wall(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004f8901
// FUNCTION: SHANDALAR 0x004b29c6
int card_fog(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x00534ac2
// FUNCTION: SHANDALAR 0x004ec2be
int card_deathgrip(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004fcc3f
// FUNCTION: SHANDALAR 0x004b6d67
int card_power_sink(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x005194e4
// FUNCTION: SHANDALAR 0x004d0cc6
int card_gloom(int player, int card, event_t event)
{

}
