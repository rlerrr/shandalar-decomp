#ifndef __GLOBAL_STRINGS_H__
#define __GLOBAL_STRINGS_H__

// Single header library style
#ifdef GLOBAL_STRINGS_IMPL
#define GLOBAL_STRINGS_EXTERN
#define GLOBAL_STRINGS_INIT(value) = value
#else
#define GLOBAL_STRINGS_EXTERN extern
#define GLOBAL_STRINGS_INIT(value)
#endif

int FUN_004eca6d(char *filename, char *section_name);
int load_text(const char *file_name, const char *section_name);
void FUN_00491f1e(char *filename);

typedef char text_20_t[0x14];
typedef char text_50_t[0x32];
typedef char text_75_t[0x4b];
typedef char text_100_t[100];
typedef char text_150_t[0x96];
typedef char text_300_t[300];

// GLOBAL: MAGIC 0x00896720
// GLOBAL: SHANDALAR 0x008aa920
GLOBAL_STRINGS_EXTERN char text_lines[249][300];

// GLOBAL: MAGIC 0x00789460
GLOBAL_STRINGS_EXTERN char gs_magic_the_gathering_title_00789460[300];

// GLOBAL: MAGIC 0x008962f0
GLOBAL_STRINGS_EXTERN char global_ui_strings_filename[300];

// GLOBAL: MAGIC 0x00709190
GLOBAL_STRINGS_EXTERN text_20_t gs_color_word_upper_00709190[6];

// GLOBAL: MAGIC 0x00709210
GLOBAL_STRINGS_EXTERN text_20_t gs_color_word_lower_00709210[6];

// GLOBAL: MAGIC 0x00709290
GLOBAL_STRINGS_EXTERN text_20_t gs_land_word_capitalized_00709290[6];

// GLOBAL: MAGIC 0x00709310
GLOBAL_STRINGS_EXTERN text_20_t gs_land_word_lower_00709310[6];

// GLOBAL: MAGIC 0x00709390
GLOBAL_STRINGS_EXTERN text_20_t gs_color_word_capitalized_00709390[6];

// GLOBAL: MAGIC 0x00709410
GLOBAL_STRINGS_EXTERN text_20_t gs_land_word_upper_00709410[6];

// GLOBAL: MAGIC 0x00777970
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_main_phase_precombat_cast_spells_play_land_00777970;

// GLOBAL: MAGIC 0x00777ab0
GLOBAL_STRINGS_EXTERN text_300_t gs_damage_dealing_00777ab0;

// GLOBAL: MAGIC 0x00777bf0
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_your_hand_00777bf0;

// GLOBAL: MAGIC 0x00777d30
GLOBAL_STRINGS_EXTERN text_300_t gs_upkeep_phase_00777d30;

// GLOBAL: MAGIC 0x00781aa0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_power_00781aa0;

// GLOBAL: MAGIC 0x00789130
GLOBAL_STRINGS_EXTERN text_300_t gs_illus_00789130;

// GLOBAL: MAGIC 0x00789320
GLOBAL_STRINGS_EXTERN text_300_t gs_process_00789320;

// GLOBAL: MAGIC 0x007894b0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_toughness_007894b0;

// GLOBAL: MAGIC 0x007895e0
GLOBAL_STRINGS_EXTERN text_300_t gs_gain_life_007895e0;

// GLOBAL: MAGIC 0x00789740
GLOBAL_STRINGS_EXTERN text_300_t gs_end_damage_prevention_00789740;

// GLOBAL: MAGIC 0x00789870
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_main_phase_postcombat_00789870;

// GLOBAL: MAGIC 0x007898b0
GLOBAL_STRINGS_EXTERN text_300_t DAT_007898b0;

// GLOBAL: MAGIC 0x00789a50
GLOBAL_STRINGS_EXTERN text_300_t gs_choose_defenders_00789a50;

// GLOBAL: MAGIC 0x00789b90
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_cleanup_00789b90;

// GLOBAL: MAGIC 0x00789bd0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_library_00789bd0;

// GLOBAL: MAGIC 0x00789c34
GLOBAL_STRINGS_EXTERN int gs_words_to_sleight_count_00789c34;

// GLOBAL: MAGIC 0x00789c40
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_is_a_target_00789c40;

// GLOBAL: MAGIC 0x007911b0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_damaged_007911b0;

// GLOBAL: MAGIC 0x007912e0
GLOBAL_STRINGS_EXTERN text_300_t gs_activating_007912e0;

// GLOBAL: MAGIC 0x00791420
GLOBAL_STRINGS_EXTERN text_300_t gs_mana_so_far_max_00791420;

// GLOBAL: MAGIC 0x007a77d0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_damage_to_player_007a77d0;

// GLOBAL: MAGIC 0x007a7810
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_life_points_lich_007a7810;

// GLOBAL: MAGIC 0x007a7880
GLOBAL_STRINGS_EXTERN text_300_t gs_pay_for_attacker_007a7880;

// GLOBAL: MAGIC 0x007a79c0
GLOBAL_STRINGS_EXTERN text_300_t gs_trying_to_cast_007a79c0;

// GLOBAL: MAGIC 0x007a7af0
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_band_with_other_attacker_007a7af0;

// GLOBAL: MAGIC 0x007a7c20
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_upkeep_007a7c20;

// GLOBAL: MAGIC 0x007a7cd0
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_main_declare_combat_007a7cd0;

// GLOBAL: MAGIC 0x007a7d30
GLOBAL_STRINGS_EXTERN text_300_t gs_waiting_for_opponent_007a7d30;

// GLOBAL: MAGIC 0x007aa930
GLOBAL_STRINGS_EXTERN text_300_t gs_aboutbox_text_007aa930;

// GLOBAL: MAGIC 0x007aacc0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_owner_007aacc0;

// GLOBAL: MAGIC 0x007aae00
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_phased_007aae00;

// GLOBAL: MAGIC 0x007aae40
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_cleanup_007aae40;

// GLOBAL: MAGIC 0x007aaeb0
GLOBAL_STRINGS_EXTERN text_300_t gs_cardtitle_activation_007aaeb0;

// GLOBAL: MAGIC 0x007ab020
GLOBAL_STRINGS_EXTERN text_300_t gs_end_discard_007ab020;

// GLOBAL: MAGIC 0x007ab160
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_blocking_007ab160;

// GLOBAL: MAGIC 0x007ab2d0
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_main_phase_postcombat_cast_spells_007ab2d0;

// GLOBAL: MAGIC 0x008951d0
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_save_game_008951d0;

// GLOBAL: MAGIC 0x008961c0
GLOBAL_STRINGS_EXTERN text_300_t gs_draw_a_card_008961c0;

// GLOBAL: MAGIC 0x00896400
GLOBAL_STRINGS_EXTERN text_300_t gs_casting_00896400;

// GLOBAL: MAGIC 0x00896540
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_attacking_00896540;

// GLOBAL: MAGIC 0x008966e0
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_choose_attackers_008966e0;

// GLOBAL: MAGIC 0x008a8c40
GLOBAL_STRINGS_EXTERN text_300_t gs_assign_blockers_008a8c40;

// GLOBAL: MAGIC 0x008a8d80
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_untap_008a8d80;

// GLOBAL: MAGIC 0x008a8e00
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_summoning_sickness_008a8e00;

// GLOBAL: MAGIC 0x008a8e40
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_not_controlled_by_owner_008a8e40;

// GLOBAL: MAGIC 0x008a9000
GLOBAL_STRINGS_EXTERN int unk_008a9000;

// GLOBAL: MAGIC 0x008a9010
GLOBAL_STRINGS_EXTERN text_300_t gs_mana_so_far_008a9010;

// GLOBAL: MAGIC 0x008a9a00
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_type_008a9a00;

// GLOBAL: MAGIC 0x008a9b30
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_upkeep_008a9b30;

// GLOBAL: MAGIC 0x008a9ba0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_life_points_and_poison_008a9ba0;

// GLOBAL: MAGIC 0x008b26c0
GLOBAL_STRINGS_EXTERN text_300_t gs_end_main_008b26c0;

// GLOBAL: MAGIC 0x008b2840
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_main_precombat_008b2840;

// GLOBAL: MAGIC 0x008b2890
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_life_points_and_poison_lich_008b2890;

// GLOBAL: MAGIC 0x008b2900
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_scrollbar_008b2900;

// GLOBAL: MAGIC 0x008b30a0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_life_points_008b30a0;

// GLOBAL: MAGIC 0x008b3110
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_player_008b3110;

// GLOBAL: MAGIC 0x008b3280
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_untap_008b3280;

// GLOBAL: MAGIC 0x008b32d0
GLOBAL_STRINGS_EXTERN text_300_t gs_attacker_selected_008b32d0;

// GLOBAL: MAGIC 0x008b34c0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_walls_008b34c0;

// GLOBAL: MAGIC 0x008b35f0
GLOBAL_STRINGS_EXTERN text_75_t gs_duel_option_008b35f0[0x13];

// GLOBAL: MAGIC 0x008b3be0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_can_untap_008b3be0;

// GLOBAL: MAGIC 0x008b3d20
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_illegal_attacker_008b3d20;

// GLOBAL: MAGIC 0x008b3e50
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_008b3e50;

// GLOBAL: MAGIC 0x008b3f80
GLOBAL_STRINGS_EXTERN text_300_t gs_enchantment_008b3f80;

// GLOBAL: MAGIC 0x008b4100
GLOBAL_STRINGS_EXTERN text_300_t gs_cardtitle_upkeep_008b4100;

// GLOBAL: MAGIC 0x008b4140
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_combat_choose_blockers_008b4140;

// GLOBAL: MAGIC 0x008b4280
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_minimized_attack_window_008b4280;

// GLOBAL: MAGIC 0x008b42f0
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_spell_chain_008b42f0;

// GLOBAL: MAGIC 0x008b45f0
GLOBAL_STRINGS_EXTERN text_300_t gs_assign_attackers_008b45f0;

// GLOBAL: MAGIC 0x008b4720
GLOBAL_STRINGS_EXTERN text_300_t gs_cast_008b4720;

// GLOBAL: MAGIC 0x008b4850
GLOBAL_STRINGS_EXTERN text_300_t gs_casting_008b4850;

// GLOBAL: MAGIC 0x008b4990
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_cant_target_this_008b4990;

// GLOBAL: MAGIC 0x008b4de0
GLOBAL_STRINGS_EXTERN text_50_t gs_ability_word_008b4de0[0x11];

// GLOBAL: MAGIC 0x008cc2e0
GLOBAL_STRINGS_EXTERN text_150_t gs_cuecard_your_mana_pool_008cc2e0[7];

// GLOBAL: MAGIC 0x008cc710
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_main_phase_precombat_cast_spells_008cc710;

// GLOBAL: MAGIC 0x008cc850
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_attacking_blocking_008cc850;

// GLOBAL: MAGIC 0x008cd924
GLOBAL_STRINGS_EXTERN int gs_words_to_hack_count_008cd924;

// GLOBAL: MAGIC 0x008cd980
GLOBAL_STRINGS_EXTERN text_300_t gs_end_of_turn_008cd980;

// GLOBAL: MAGIC 0x008ce290
GLOBAL_STRINGS_EXTERN text_300_t gs_processing_008ce290;

// GLOBAL: MAGIC 0x008ce3c0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_color_008ce3c0;

// GLOBAL: MAGIC 0x008ce6a0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_cast_resolved_008ce6a0;

// GLOBAL: MAGIC 0x008ce7e0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_with_reason_008ce7e0;

// GLOBAL: MAGIC 0x008cea20
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_subtype_008cea20;

// GLOBAL: MAGIC 0x008ceb50
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_will_untap_008ceb50;

// GLOBAL: MAGIC 0x008cec80
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_resolve_normal_damage_008cec80;

// GLOBAL: MAGIC 0x008ced40
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_artifact_creature_008ced40;

// GLOBAL: MAGIC 0x008cee80
GLOBAL_STRINGS_EXTERN text_300_t gs_main_phase_008cee80;

// GLOBAL: MAGIC 0x008cf230
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_is_a_target_cant_target_again_008cf230;

// GLOBAL: MAGIC 0x008cf3c0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_enchanted_008cf3c0;

// GLOBAL: MAGIC 0x008cf4f0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_life_points_and_poison_lich_008cf4f0;

// GLOBAL: MAGIC 0x008cf560
GLOBAL_STRINGS_EXTERN text_300_t gs_draw_phase_008cf560;

// GLOBAL: MAGIC 0x008cff20
GLOBAL_STRINGS_EXTERN text_150_t gs_cuecard_opponent_mana_pool_008cff20[7];

// GLOBAL: MAGIC 0x0091a6e0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_cant_target_this_0091a6e0;

// GLOBAL: MAGIC 0x0091a810
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_abilities_0091a810;

// GLOBAL: MAGIC 0x0091b110
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_draw_0091b110;

// GLOBAL: MAGIC 0x0091b290
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_damage_n_0091b290;

// GLOBAL: MAGIC 0x0091b2d0
GLOBAL_STRINGS_EXTERN text_300_t gs_activate_0091b2d0;

// GLOBAL: MAGIC 0x0091bbe0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_controller_0091bbe0;

// GLOBAL: MAGIC 0x0091bd10
GLOBAL_STRINGS_EXTERN text_300_t gs_still_thinking_0091bd10;

// GLOBAL: MAGIC 0x0091be40
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_spell_0091be40;

// GLOBAL: MAGIC 0x0091bfc0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_attacked_0091bfc0;

// GLOBAL: MAGIC 0x0091c100
GLOBAL_STRINGS_EXTERN text_300_t gs_draw_phase_0091c100;

// GLOBAL: MAGIC 0x0091c230
GLOBAL_STRINGS_EXTERN text_300_t gs_tap_for_mana_0091c230;

// GLOBAL: MAGIC 0x0091c360
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_target_player_0091c360;

// GLOBAL: MAGIC 0x0091c490
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_draw_0091c490;

// GLOBAL: MAGIC 0x0091c510
GLOBAL_STRINGS_EXTERN text_300_t gs_end_draw_0091c510;

// GLOBAL: MAGIC 0x0091c640
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_dying_0091c640;

// GLOBAL: MAGIC 0x0091c680
GLOBAL_STRINGS_EXTERN text_300_t gs_use_regeneration_effects_0091c680;

// GLOBAL: MAGIC 0x0091c7b0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_graveyard_0091c7b0;

// GLOBAL: MAGIC 0x0091c820
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_opponent_0091c820;

// GLOBAL: MAGIC 0x0091c9b0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_graveyard_0091c9b0;

// GLOBAL: MAGIC 0x0091ca20
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_library_0091ca20;

// GLOBAL: MAGIC 0x0091caa0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_blocked_0091caa0;

// GLOBAL: MAGIC 0x0091cbd0
GLOBAL_STRINGS_EXTERN text_300_t gs_graveyard_order_0091cbd0;

// GLOBAL: MAGIC 0x0091cd00
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_where_0091cd00;

// GLOBAL: MAGIC 0x0091cf50
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_main_phase_postcombat_cast_spells_play_land_0091cf50;

// GLOBAL: MAGIC 0x0091d080
GLOBAL_STRINGS_EXTERN text_300_t gs_discard_phase_0091d080;

// GLOBAL: MAGIC 0x0091d1b0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_life_points_lich_0091d1b0;

// GLOBAL: MAGIC 0x0091d220
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_resolve_first_strike_damage_0091d220;



// GLOBAL: MAGIC 0x00924790
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_main_precombat_00924790;

// GLOBAL: MAGIC 0x00924800
GLOBAL_STRINGS_EXTERN text_300_t gs_ok_00924800;

// GLOBAL: MAGIC 0x00925040
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_this_card_will_untap_00925040;

// GLOBAL: MAGIC 0x00925080
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_tapped_00925080;

// GLOBAL: MAGIC 0x009252f0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_opponent_life_points_and_poison_009252f0;

// GLOBAL: MAGIC 0x00925a90
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_assign_defenders_00925a90;

// GLOBAL: MAGIC 0x00925bc0
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_blocker_fast_effects_00925bc0;

// GLOBAL: MAGIC 0x00925c00
GLOBAL_STRINGS_EXTERN text_300_t gs_end_upkeep_00925c00;

// GLOBAL: MAGIC 0x00925d40
GLOBAL_STRINGS_EXTERN text_300_t gs_tapping_00925d40;

// GLOBAL: MAGIC 0x00925e70
GLOBAL_STRINGS_EXTERN text_300_t gs_multiblock_shadow_blocker_00925e70;

// GLOBAL: MAGIC 0x00925fa0
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_main_postcombat_00925fa0;

// GLOBAL: MAGIC 0x00926010
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_your_attack_00926010;

// GLOBAL: MAGIC 0x00926090
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_minimized_spell_chain_00926090;

// GLOBAL: MAGIC 0x00926210
GLOBAL_STRINGS_EXTERN text_300_t gs_blocker_selected_00926210;

// GLOBAL: MAGIC 0x00926380
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_main_combat_00926380;

// GLOBAL: MAGIC 0x009263f0
GLOBAL_STRINGS_EXTERN text_300_t gs_pay_for_blocker_009263f0;

// GLOBAL: MAGIC 0x00926520
GLOBAL_STRINGS_EXTERN text_300_t gs_fast_effects_00926520;

// GLOBAL: MAGIC 0x00926670
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_main_postcombat_00926670;

// GLOBAL: MAGIC 0x00926750
GLOBAL_STRINGS_EXTERN text_300_t gs_cardtitle_hunting_00926750;

// GLOBAL: MAGIC 0x009267a0
GLOBAL_STRINGS_EXTERN text_300_t gs_window_title_opponent_attack_009267a0;

// GLOBAL: MAGIC 0x00926930
GLOBAL_STRINGS_EXTERN text_300_t gs_hunting_subtype_name_00926930[212];

// GLOBAL: MAGIC 0x00938e40
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_opponent_discard_00938e40;

// GLOBAL: MAGIC 0x00939050
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_casted_00939050;

// GLOBAL: MAGIC 0x00939200
GLOBAL_STRINGS_EXTERN text_300_t gs_damage_prevention_00939200;

// GLOBAL: MAGIC 0x00939350
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_your_discard_00939350;

// GLOBAL: MAGIC 0x009394a0
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_your_life_points_009394a0;

// GLOBAL: MAGIC 0x00939570
GLOBAL_STRINGS_EXTERN text_300_t gs_choose_attackers_00939570;

// GLOBAL: MAGIC 0x009396a0
GLOBAL_STRINGS_EXTERN text_300_t gs_triggered_effects_009396a0;

// GLOBAL: MAGIC 0x0093a7c0
GLOBAL_STRINGS_EXTERN text_300_t gs_phasebar_attacker_fast_effects_0093a7c0;

// GLOBAL: MAGIC 0x0093a810
GLOBAL_STRINGS_EXTERN text_300_t gs_cuecard_scroll_thumb_0093a810;

// GLOBAL: MAGIC 0x0093a850
GLOBAL_STRINGS_EXTERN text_300_t gs_interrupts_0093a850;

// GLOBAL: MAGIC 0x0093d860
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_basic_land_0093d860;

// GLOBAL: MAGIC 0x0093d990
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_combat_choose_attackers_0093d990;

// GLOBAL: MAGIC 0x008a8c20
GLOBAL_STRINGS_EXTERN text_300_t gs_cancel_008a8c20;

// GLOBAL: MAGIC 0x008b40e0
GLOBAL_STRINGS_EXTERN text_300_t gs_done_008b40e0;

// GLOBAL: MAGIC 0x008cfd30
GLOBAL_STRINGS_EXTERN text_300_t gs_cardtitle_damage_008cfd30;

// GLOBAL: MAGIC 0x008cfde0
GLOBAL_STRINGS_EXTERN text_300_t gs_illegal_target_why_card_type_008cfde0;

// GLOBAL: MAGIC 0x008b4330
GLOBAL_STRINGS_EXTERN text_300_t gs_cardtitle_draw_a_card_008b4330;

// GLOBAL: MAGIC 0x008b43a0
GLOBAL_STRINGS_EXTERN text_300_t gs_end_of_combat_008b43a0;

// GLOBAL: MAGIC 0x008cf040
GLOBAL_STRINGS_EXTERN text_300_t gs_multiblock_creature_008cf040;

// GLOBAL: MAGIC 0x008cf080
GLOBAL_STRINGS_EXTERN text_300_t gs_begin_upkeep_008cf080;

// GLOBAL: MAGIC 0x0091b150
GLOBAL_STRINGS_EXTERN text_300_t gs_prompt_new_full_card_0091b150;

// GLOBAL: MAGIC 0x0091c840
GLOBAL_STRINGS_EXTERN text_300_t gs_card_into_play_0091c840;

// GLOBAL: MAGIC 0x007aaef0
GLOBAL_STRINGS_EXTERN text_300_t gs_card_leaving_play_007aaef0;

// GLOBAL: MAGIC 0x008a8ed0
GLOBAL_STRINGS_EXTERN text_300_t gs_cards_to_graveyard_008a8ed0;

// GLOBAL: MAGIC 0x00781bd0
GLOBAL_STRINGS_EXTERN text_100_t gs_words_to_sleight_00781bd0[6][50];

// GLOBAL: MAGIC 0x00789c80
GLOBAL_STRINGS_EXTERN text_100_t gs_words_to_hack_00789c80[6][50];

// GLOBAL: MAGIC 0x008a9c10
GLOBAL_STRINGS_EXTERN text_100_t gs_words_sleighted_008a9c10[6][50];

// GLOBAL: MAGIC 0x0091d260
GLOBAL_STRINGS_EXTERN text_100_t gs_words_hacked_0091d260[6][50];


#undef GLOBAL_STRINGS_EXTERN
#undef GLOBAL_STRINGS_INIT
#endif
