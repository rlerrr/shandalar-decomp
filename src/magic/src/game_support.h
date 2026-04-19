#ifndef __GAME_SUPPORT_H__
#define __GAME_SUPPORT_H__

#include "defs.h"
#include "global_state.h"

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
                                              target_state_t illegal_state);
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

#endif
