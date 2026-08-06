#ifndef __GAME_SUPPORT_H__
#define __GAME_SUPPORT_H__

#include "defs.h"
#include "global_state.h"
#include "global_other.h"
#include "network.h"

extern card_data_t global_cards_data[];

typedef struct
{
  int player;
  int type_flags;
  int color_flags;
  int owner;
  int zone_flags;
  int allow_cancel;
  char prompt[200];
  int allow_ai_player;
  int allow_human_player;
} target_selection_request_t;
STATIC_ASSERT(sizeof(target_selection_request_t) == 0xe8, target_selection_request_t_wrong_size);

typedef struct
{
  int selection_code;
  int target_player;
  int target_card;
  int unused;
} target_selection_result_t;

/* Helpers that are inlined in the original executable (no function calls). */
#define is_tapped(player_, card_) ((PLAYER_CARD_INSTANCE((player_), (card_)).state & STATE_TAPPED) != 0)
#define is_animated_and_sick(player_, card_) \
  (((PLAYER_CARD_INSTANCE((player_), (card_)).state & STATE_SUMMONSICK_BOTH) != 0) && \
   ((global_cards_data[PLAYER_CARD_INSTANCE((player_), (card_)).internal_card_id].type & TYPE_CREATURE) != 0))

int can_target(target_definition_t *td);
char *get_displayed_card_name(int player, int card);
void draw_cards_and_set_hand_count(int player, int amount);
int add_card_to_hand(int player, int internal_card_id);
int create_a_card_type(int internal_card_id);
void default_target_definition(int player, int card, target_definition_t *td, int type);
int damage_creature(int target_player, int target_card, int amount, int source_player, int source_card);
void damage_player(int target_player, int amount, int source_player, int source_card);
int dispatch_event(int player, int card, event_t event);
int draw_card_for_player(int player);
color_t single_color_test_bit_to_color_t(color_test_t color_test);
int count_active_card_instances_plus_one(int player, int internal_card_id);
int count_permanents_by_internal_card_id(int player, int internal_card_id, int who_to_check);
void copy_opponent_name_prefix(char *name);
int generic_clockwork_creature(int player, int card, int event, int amount);
int charge_mana_w_global_cost_mod(int player, int card, int color, int amount);
int FUN_0042d790(int player, int card, event_t event, int color);
int handle_button_palette_message(int window, unsigned int message, int other_window, int data);
int helper_destroy_basiclandtype(int source_player,
                                        int source_card,
                                        int test_player,
                                        int test_card,
                                        int internal_card_id);
int destroy_other_auras_on_same_permanent(int player, int card, int internal_card_id);
int divide_creatures_into_two_piles(int who_is_being_divided, int player, int card);
int sacrifice_permanents_for_lich_damage(int player, int amount);
int can_attack(int player, int card);
int push_card_onto_stack(int player, int card, int event, int extra, int prompt);
void redirect_pending_damage_to_player(int target_player, int target_card, int damage_target_player);
int has_vigilance(int player, int card);
char *get_hunting_subtype_name(int value);
int CardIDFromType(unsigned int type);
int CardTypeFromID(int csvid);
int find_highest_value_graveyard_card_by_type(int player, unsigned int type);
int internal_rand(int maximum);
int should_skip_phase(int player);
int can_stop_for_phase(int use_current_player_stops);
int can_pay_untap_cost(int player, int card);
int is_card_pending_resolution(int player, int card);
int network_random(int player, int maximum);
void start_ai_decision_search(int decision_code, int time_scale);
void reset_upkeep_costs(void);
unsigned int get_current_stack_action(void);
char *format_stack_action_text(char *out, int msg, int player, int card);
void __stdcall reset_ai_search_trial_state(void);
void __stdcall save_recorded_ai_actions(void);
int ai_opinion_of_gamestate(int player);
int run_target_selection_modal(int who_chooses,
                               int player,
                               char *prompt,
                               int allow_cancel,
                               int type_flags,
                               int color_flags,
                               int owner,
                               int zone_flags,
                               int *out_selection_code,
                               int *out_target_player,
                               int allow_ai_player,
                               int allow_human_player);
int can_pay_card_mana_cost(int player, int target_player, int target_card);
int check_attached_aura_can_pay_cost(int player, int card, int internal_card_id);
unsigned int get_duel_time_units(void);
int get_available_card_action(int player, int card);
void redraw_shandalar_duel_screen(int view_player, int present_after_draw);
int graveyard_has_type(int player, int type);
int ClampIntToRange(int a1, int a2, int a3);
int DrawRandomCardFromInitialLibrary(int library_index);
int is_basic_land_internal_card_id_of_color(int internal_card_id, int color);
int card_has_basic_land_type(int player, int card, int land_color);
int ante_drawn_card(int player);
void remove_card_from_graveyard(int player, int graveyard_index);
void clear_card_attachments(int player, int card);
void exit_duel_thread(int player);
int show_deck(int player, int *cards, int count, void *context, int suppress_done_txt, char *prompt);
int discard_card_from_hand(int player, int card);
void request_duel_display_refresh(int player, int card, int reason, int flags);
void request_duel_display_refresh_if_human(int player, int card, int reason, int flags);
void move_card_to_graveyard(int player, int card);
void put_card_on_bottom_of_library(int player, int internal_card_id);
int find_internal_card_id_by_csv_id(int card_id);
int charge_mana(int player, color_t color, int amount);
int begin_mana_payment_record(void);
int end_mana_payment_record(void);
int restore_recorded_mana_to_pool(int player);
int is_mana_cost_paid(int *mana_cost, int required_colorless, int max_x);
unsigned int get_required_mana_color_mask(int *mana);
int format_mana_payment_prompt(char *prompt, int *mana_cost, int x_paid, int max_x);
void auto_pay_colored_mana(int player, int *mana_paid_by_color, int *total_mana_paid, int max_colorless);
void auto_pay_variable_mana(int player,
                  int *mana_paid_by_color,
                  int *total_mana_paid,
                  int max_colorless,
                  int *special_mana,
                  int max_x);
void auto_pay_generic_mana(int player,
                           int *mana_paid_by_color,
                           int *total_mana_paid,
                           int max_colorless,
                           int *special_mana,
                           int max_x);
int can_activate_mana_source(int player, int card, int activation_flags);
int activate_mana_source_for_payment(int player, int card);
int activate_mana_sources_for_payment(int player,
                 int *mana_paid_by_color,
                 int *total_mana_paid,
                 unsigned char activation_flags,
                 int include_special_mana);
int record_paid_mana(int color, int amount);
int unproduce_mana(int player, int color, int amount);
int get_mana_payment_amount(int *mana_cost,
                            int cost_color,
                            int *mana_available,
                            int mana_color,
                            int allow_multi_pay,
                            int max_x,
                            int x_paid);
int copy_mana_pool_to_display(void);
int resolve_top_card_on_stack(void);
void reassess_all_cards_and_mana(void);
void process_damage_prevention(int player);
int dispatch_card_trigger_event(int player, int card, int event, int extra);
void push_affected_card_stack(void);
void pop_affected_card_stack(void);
int card_death_ward(int player, int card, event_t event);
void spend_mana_for_cost(int *mana_cost,
                  int cost_color,
                  int amount,
                  int *special_mana,
                  int max_x,
                  int player,
                  int mana_color,
                  int *mana_paid_by_color,
                  int *total_mana_paid);
int refund_paid_mana(int *mana_paid_by_color);
int find_land_animation_legacy(int a1, int a2, int player, int card, int internal_card_id);
int destroy_land_animation_legacies(int parent_player, int parent_card, int player, int card, int internal_card_id);
int show_cardlist_if_human(int *graveyard,
                 int count,
                 void *context,
                 unsigned int big_card_mode,
                 char *prompt);
int show_cardlist(int *graveyard,
                  int *card_counts,
                  int *available,
                  int count,
                  void *context,
                  unsigned int big_card_mode,
                  char *prompt);
void create_card_list_gdi_objects(int *brush1, int *pen1, int *pen2, int *pen3, int *brush2, int *text_color);
void delete_card_list_gdi_objects(HGDIOBJ brush1, HGDIOBJ pen1, HGDIOBJ pen2, HGDIOBJ pen3, HGDIOBJ brush2);
int select_from_graveyard_with_dialog(int player,
                 int *graveyard,
                 void *available,
                 int count,
                 int prompt,
                 int a6,
                 int selected,
                 int a8,
                 int a9);
int dispatch_event_to_single_card(int player,
                                  int card,
                                  event_t event,
                                  int new_attacking_card_controller,
                                  int new_attacking_card);
int is_nonactivated_mana_source(int player, int card);
int get_hacked_color(int player, int card, int value);
void invalidate_dynamic_card_type(int internal_card_id);
int is_selected_target_already_attached(int player, int card);
int choose_best_tapped_permanent_for_ai(int player, int mode);
/* target_source_mode:
 *   0 = check direct player/card targets
 *   1 = check damage-card target player/card
 *   2 = check damage-card source player/card
 */
typedef enum
{
  TARGET_SCAN_DIRECT = 0,
  TARGET_SCAN_DAMAGE_TARGET = 1,
  TARGET_SCAN_DAMAGE_SOURCE = 2
} target_scan_mode_t;

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
                          target_state_t illegal_state);
void set_duel_prompt_text(char *text);
int gain_life(int player, int amount, ...);
void discard(int player, int flags, int player_who_controls_effect);
void declare_mana_available_hex(int player, color_test_t colors, int amount);
int do_dialog(int who_chooses,
              int bigcard_player,
              int bigcard_card,
              int smallcard_player,
              int smallcard_card,
              const char *options,
              int ai_choice);
void undeclare_mana_available_hex(int player, color_test_t color, int amount);
int play_sound_effect(int sound_id);
int load_recorded_action_target(int offset);
void record_ai_action_selection(void);
int load_recorded_action_code(int offset);
void replay_ai_action_selection(void);
void rewind_recorded_action(void);
void kill_creatures_with_lethal_damage(void);
unsigned int get_duel_time_units_if_available(void);
void extract_numbered_text_choice(char *out, char *in, int choice);
void declare_mana_color_conversion(int player, int color_to_produce, int color_to_consume);
int has_mana(int player, color_t color, int amount);
int has_mana_w_global_cost_mod(int player, int card, color_t color, int amount);
void count_mana(void);
void C_count_colors_of_lands_in_play(void);
void C_dispatch_event_raw(event_t event);
int undeclare_mana_available(int player, color_t color, int amount);
int create_legacy_effect(int player, int card, int legacy_iid, int target_player, int target_card);
int tap_card_and_dispatch_event(int player, int card);
int has_effect_source_type(int player, int card, unsigned int flags);
int find_matching_active_control_effect(int player, int card, int source_player, int source_card, int internal_card_id);
int find_matching_inactive_control_effect(int player, int card, int source_player, int source_card, int internal_card_id);
int dispatch_function_to_all_cards_in_play(int player, int card, int(__cdecl *callback)(int, int, int, int, int), int who_to_check);
int card_fellwar_stone(int player, int card, event_t event);
unsigned int get_protections_from(int player, int card);
void regenerate_card(int player, int card);
int generic_creature_stat_aura(int player, int card, event_t event, int power_modifier, int toughness_modifier);
int mark_zombie_as_nonregenerable(int player, int card, int internal_card_id);
int select_target_creature_and_store(int player, unsigned int preferred_controller, int card);
int select_target_land_and_store(int player, int preferred_controller, int card);
int select_target_artifact_and_store(int player, unsigned int preferred_controller, int card);
void dispatch_three_arg_callback_to_cards_in_play(int(__cdecl *callback)(int, int, int), int who_to_check);
void remove_special_counter(int player, int card);
void set_special_counters(int player, int card, int amount);
void remove_special_counters(int player, int card, int amount);
void TENTATIVE_reassess_all_cards(int view_player, int present_after_draw);

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
                                    target_state_t illegal_state);
int get_hacked_color(int player, int card, int orig_color);
int get_sleighted_color(int player, int card, int orig_color);
int get_sleighted_color_test(int player, int card, int orig_color_test);
int is_in_play(int player, int card);
void kill_card(int player, int card, kill_t kill_mode);
int mana_producer_sound_on_resolve(int player, int card, event_t event, color_t color);
int produce_mana(int player, color_t color, int amount);
int resolve_cast_card(int player, int card);
int generic_creature_ability_aura(int player, int card, int event, unsigned int trigger_flag);
int process_card_enters_play(int player, int card);
int obliterate_top_card_of_stack(void);
int choose_a_color(int player, const char *prompt, int unused1, int unused2, unsigned int available_colors);
int select_damage_target(int player, int card, int damage_unused);
int deal_damage_to_selected_target(int player, int card, event_t event, int amount);
int generic_regeneration_ability(int player, int card, event_t event, unsigned int color, int amount);
void add_special_counters(int player, int card, int amount);
int select_target(int player, int card, target_definition_t *td, const char *prompt, target_t *ret_location);
int declare_mana_available(int player, color_t color, int amount);
int undeclare_mana_available_and_produce_it(int player, color_t color, int amount);
void destroy_attached_auras_of_color(int player, int card, int color);
void add_special_counter(int player, int card);
int TENTATIVE_set_timestamps(int player, int card);
int sacrifice_a_land(int player);
int choose_creature_to_sacrifice(int player);
int select_card_for_action(int player,
                 int arg_2,
                 int player_to_check,
                 unsigned int required_type,
                 unsigned int required_color,
                 char *prompt,
                 int dialog_mode);
int select_land_for_cyclopean_tomb_ai(int player, int card, int target_player);
int resolve_control_aura_conflict(int player, int card);
int rewire_control_aura_source(int player, int card, int internal_card_id);
int generic_control_aura(int player, int card, event_t event, unsigned int required_type);
int choose_a_number(int player, char *prompt, int maxnum);
void real_put_on_top_of_deck(int player, int internal_card_id);
int select_damage_card_from_list(int player,
                 int *internal_card_ids,
                 int *damage_amounts,
                 int count,
                 int title,
                 int allow_cancel,
                 char *prompt);
int select_card_from_graveyard(int player,
                 int *graveyard,
                 int *allowed_cards,
                 int count,
                 const char *prompt,
                 int allow_cancel,
                 int *title);
int choose_best_card_from_library(int player, unsigned int type_mask);
int select_best_land_target_by_score(int preferred_player, int only_player, int *target_data);
int drain_power_draw_mana_from_land(int player, int card, int internal_card_id);
int card_two_headed_giant_of_foriys_legacy(int player, int card, int event);
int reattach_if_attached_to_source(int source_player, int source_card, int test_player, int test_card, int internal_card_id);
void remove_card_from_deck(int player, int position);
void shuffle_duel_library(int player, int deck_owner);
int damage_creature_callback(int source_player, int source_card, int test_player, int test_card, int internal_card_id);
int has_other_creature_in_play(int player, int card);
int has_permanent_of_type(int player, int type_mask);
int regenerate_or_graveyard_triggers(void);
int can_block_with_landwalk_masks(int blocker_player, int blocker_card, int attacker_player, int attacker_card);
void replace_card_hack_color(int player, int card, int color_from, unsigned char color_to);
void replace_card_sleight_color(int player, int card, int color_from, unsigned char color_to);

#endif
